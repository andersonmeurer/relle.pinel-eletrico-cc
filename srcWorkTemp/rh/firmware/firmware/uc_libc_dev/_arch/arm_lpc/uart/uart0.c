#include "uart0.h"
#if defined(cortexm3)
#include "arm.h"
#endif

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa e configura o modo de trabalho da UART
// Parametros:	baudrate: 
//					1200, 2400, ... , 57600, 115200 ...
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void uart0_Init(u32 baudrate) {
  	u32 f_div;

  	PCONP |= PCUART0;												// Ligar a uart no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões

	#if defined(arm7tdmi)
  	U0_PINSEL = (U0_PINSEL & ~U0_PINMASK) | U0_PINS;				// Configura os pinos do uC para a função UART
	#endif

	#if defined(cortexm3)
  	uart0_ConfigPort();
	#endif

	#if (UART0_USE_HANDSHAKING == pdON)
	// Por padrão esses pinos já são GPIO
	U0_CTS_DIR &= ~U0_CTS; 				// Pino CTS como entrada
	U0_RTS_DIR |= U0_RTS; 				// Pino RTS como saída
	uart0_RTSon(); 						// Sinaliza que posso receber dados
	#endif

  	U0IER = 0;   	                      						// Desabilita todas as fonte de interrupções da uart
  	U0IIR;                                						// Limpa os identificadores de pendencia interrupções da uart
	U0FCR = (U0FCR_TX_FIFO_RESET | U0FCR_RX_FIFO_RESET);		// Limpa TX e RX FIFOS

  	U0LCR = U0LCR_DLAB_ENABLE;             						// Seleciona os latches de devisões para ajusta do baudrate
  	f_div = ( PCLK / 16 ) / baudrate ;							// Calcular divisor para o baudrate
  	U0DLL = f_div%256;              							// Ajusta o baudrate byte baixo
  	U0DLM = f_div/256;       									// Ajusta o baudrate byte alto
  	U0LCR = (UART0_MODE & ~U0LCR_DLAB_ENABLE);					// Configura o modo de trabalho da uart e desativa o acesso aos registradores do baudrate

  	U0FCR = UART0_FIFOMODE;										// Configura o modo de trabalho da FIFO

  	U0RBR;                                						// Limpa o registrador de recepçao de dados
  	U0LSR;                                						// Limpa o registrador de estado de linha
}

//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Captura o próximo byte recebido na UART
// Parametros: 	Ponteiro para da variavel char para o retorno do byte recebido
// Retorna:		pdPASS indicando que existe caractere recebido 
//				errBUFFER_EMPTY indicando que o buffer está vazio
// -------------------------------------------------------------------------------------------------------------------
int uart0_GetChar(u8 *ch) {
  	if (U0LSR & U0LSR_RDR) {                 					// Checa se há caractere na FIFO não lido
    	*ch = U0RBR;											// Captura o caractere da FIFO
    	return pdPASS;
    } else
    	return errBUFFER_EMPTY;
}


// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa o buffer e FIFO de recepção
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart0_ClearBufferRx(void) {
	U0FCR = U0FCR_RX_FIFO_RESET;		// Limpa RX FIFO
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Checa se a FIFO de recpeção está vazia
// Retorna:		pdTRUE se a FIFO está vazia, senão retorna pdFALSE
// -------------------------------------------------------------------------------------------------------------------
int uart0_EmptyRx(void) {
  	if (U0LSR & U0LSR_RDR) return pdFALSE; else return pdTRUE;
}


//###################################################################################################################
// UART TRANSMISSAO
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Coloca um caractere na fila de transmissão
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o código de erro
// -------------------------------------------------------------------------------------------------------------------
int uart0_PutChar(n16 ch) {
	while (!(U0LSR & U0LSR_THRE)) 			// Espero até que o buffer TX fique vazio
		CRITICAL_WAIT;        				// Caso exista algum OS, pode trocar de contexto

	#if (UART0_USE_HANDSHAKING == pdON)
	while(!uart0_CTS()) 					// espera até que o receptor esteja pronto para eviar dados
		CRITICAL_WAIT;        				// Caso exista algum OS, pode trocar de contexto
	#endif
  	
  	U0THR = (u8)ch;							// Atualiza o registrador de TX uart com o dado
	  	
  	return pdPASS;
}


// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Transmite uma string até que encontre o caractere NULO '\0' ou que o buffer estoura
//				ATENÇÂO: o Compilador já adiciona \0 na string do tipo "xyz" EX: "\r\r\nHello World!\r\r\n" 
// Parametros: 	Ponteiro da string
// Retorna:		O ponteiro da string. 
//					Retorna NULO se a string foi completamente transmitindo 
//					Ou retorna o ponteiro do próximo caractere a ser transmitido
// OBS: Se a string for maior que o espaço disponivel no buffer de TX pode ocorrer que a string não seja toda transmitida
// -------------------------------------------------------------------------------------------------------------------
u8 *uart0_PutString(u8 *str) {
  	register char ch;

	while ((ch = *str) && (uart0_PutChar(ch) >= 0))				// Adiciona para transmissão enquanto tenha dados para transmitir
    	str++;

	return str;													// Retorna o ponteiro atual da string
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Remove todos os dados da fila de transmissão
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void uart0_FlushTx(void) {
	U0FCR |= U0FCR_TX_FIFO_RESET;          							// Limpa a FIFO de transmissão
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Retorna o status do registrador de transmissão de dados
// Parametros: 	Nenhum
// Retorna:		pdFALSE = Se o registradores U0THR ou U0TSR não estão vazios
//				pdTRUE = Se o registradores U0THR ou U0TSR estão vazios
// -------------------------------------------------------------------------------------------------------------------
int uart0_EmptyTx(void) {
  	if ((U0LSR & (U0LSR_THRE | U0LSR_TEMT)) == (U0LSR_THRE | U0LSR_TEMT) )
  			return pdTRUE;
  	else	return pdFALSE;
}
