#include "uart3.h"
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
void uart3_Init(u32 baudrate) {
	u32 f_div;
	
  	PCONP |= PCUART3;											// Ligar a uart3 no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões
  	
	#if defined(arm7tdmi)
  	U3_PINSEL = (U3_PINSEL & ~U3_PINMASK) | U3_PINS;			// Configura os pinos do uC para a função UART.
	#endif //arm7tdmi

	#if defined(cortexm3)
  	uart3_ConfigPort();
	#endif

  	#if (UART3_USE_HANDSHAKING == pdON)
  	// Por padrão esses pinos já são GPIO
	U3_CTS_DIR &= ~U3_CTS; // Pino CTS como entrada
	U3_RTS_DIR |= U3_RTS; // Pino RTS como saída
	uart3_RTSon(); // Sinaliza que posso receber dados
	#endif

  	U3IER = 0;   	                      						// Desabilita todas as fonte de interrupções da uart
  	U3IIR;                                						// Limpa os identificadores de pendencia interrupções da uart
  	U3FCR = (U3FCR_TX_FIFO_RESET | U3FCR_RX_FIFO_RESET);		// Limpa TX e RX FIFOS
  	
  	U3LCR = U3LCR_DLAB_ENABLE;             						// Seleciona os latches de devisões para ajusta do baudrate
  	f_div = ( PCLK / 16 ) / baudrate ;							// Calcular divisor para o baudrate
  	U3DLL = f_div%256;              							// Ajusta o baudrate byte baixo
  	U3DLM = f_div/256;       									// Ajusta o baudrate byte alto
  	U3LCR = (UART3_MODE & ~U3LCR_DLAB_ENABLE);						// Configura o modo de trabalho da uart e desativa o acesso aos registradores do baudrate
  	
  	U3FCR = UART3_FIFOMODE;										// Configura o modo de trabalho da FIFO

	U3RBR;                                						// Limpa o registrador de recepçao de dados
  	U3LSR;                                						// Limpa o registrador de estado de linha
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
int uart3_GetChar(u8 *ch) {
  	if (U3LSR & U3LSR_RDR) {                 					// Checa se há caractere na FIFO não lido
    	*ch = U3RBR;											// Captura o caractere da FIFO
    	return pdPASS;                      						
    } else
	  	return errBUFFER_EMPTY;
}


// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa o buffer e FIFO de recepção
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart3_ClearBufferRx(void) {
	U3FCR = U3FCR_RX_FIFO_RESET;		// Limpa RX FIFO
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
int uart3_PutChar(n16 ch) {
	while (!(U3LSR & U3LSR_THRE)) 			// Espero até que o buffer TX fique vazio
		CRITICAL_WAIT;        				// Caso exista algum OS, pode trocar de contexto

	#if (UART3_USE_HANDSHAKING == pdON)
	while(!uart3_CTS()) 					// espera até que o receptor esteja pronto para eviar dados
		CRITICAL_WAIT;        				// Caso exista algum OS, pode trocar de contexto
	#endif
  	
  	U3THR = (u8)ch;							// Atualiza o registrador de TX uart com o dado
	  	
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
u8 *uart3_PutString(u8 *str) {
   	register char ch;

  	while ((ch = *str) && (uart3_PutChar(ch) >= 0))				// Adiciona para transmissão enquanto tenha dados para transmitir
    	str++;

  	return str;													// Retorna o ponteiro atual da string
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Remove todos os dados da fila de transmissão
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void uart3_FlushTx(void) {
	U3FCR |= U3FCR_TX_FIFO_RESET;          							// Limpa a FIFO de transmissão
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Retorna o status do registrador de transmissão de dados
// Parametros: 	Nenhum
// Retorna:		pdFALSE = Se o registradores U0THR ou U0TSR não estão vazios
//				pdTRUE = Se o registradores U0THR ou U0TSR estão vazios
// -------------------------------------------------------------------------------------------------------------------
int uart3_EmptyTx(void) {
  	if ((U3LSR & (U3LSR_THRE | U3LSR_TEMT)) == (U3LSR_THRE | U3LSR_TEMT))
  		return pdTRUE;
  	else
  		return pdFALSE;
}
