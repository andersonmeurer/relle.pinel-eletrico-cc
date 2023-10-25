#include "uart1.h"
#if defined(cortexm3)
#include "arm.h"
#endif

//###################################################################################################################
// UART CONFIGURA��ES
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incializa e configura o modo de trabalho da UART
// Parametros:	baudrate: 
//					1200, 2400, ... , 57600, 115200 ...
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void uart1_Init(u32 baudrate) {
	u32 f_div;
	
  	PCONP |= PCUART1;											// Ligar a uart no controle de potencia. Tem que ser antes da configura��o, pois ap�s ligar o perif�ricos, seus regs assumem valores padr�es
  	
	#if defined(arm7tdmi)
  	U1_PINSEL = (U1_PINSEL & ~U1_PINMASK) | U1_PINS;			// Configura os pinos do uC para a fun��o UART.
  	#if defined(LPC2468)
  	U1_PINSEL1 = (U1_PINSEL1 & ~U1_PINMASK1) | U1_PINS1;	// Configura os pinos do uC para a fun��o UART.
  	#endif
	#endif

  	#if defined(cortexm3)
	uart1_ConfigPort();
	#endif

	#if (UART1_USE_HANDSHAKING == pdON)
	// Por padr�o esses pinos j� s�o GPIO
	U1_CTS_DIR &= ~U1_CTS; 				// Pino CTS como entrada
	U1_RTS_DIR |= U1_RTS; 				// Pino RTS como sa�da
	uart1_RTSon(); 						// Sinaliza que posso receber dados
	#endif

  	U1IER = 0;   	                      						// Desabilita todas as fonte de interrup��es da uart
  	U1IIR;                                						// Limpa os identificadores de pendencia interrup��es da uart
  	U1FCR = (U1FCR_TX_FIFO_RESET | U1FCR_RX_FIFO_RESET);		// Limpa TX e RX FIFOS
  	
  	U1LCR = U1LCR_DLAB_ENABLE;             						// Seleciona os latches de devis�es para ajusta do baudrate
  	f_div = ( PCLK / 16 ) / baudrate ;							// Calcular divisor para o baudrate
  	U1DLL = f_div%256;              							// Ajusta o baudrate byte baixo
  	U1DLM = f_div/256;       									// Ajusta o baudrate byte alto
  	U1LCR = (UART1_MODE & ~U1LCR_DLAB_ENABLE);						// Configura o modo de trabalho da uart e desativa o acesso aos registradores do baudrate
  	
  	U1FCR = UART1_FIFOMODE;										// Configura o modo de trabalho da FIFO

	U1RBR;                                						// Limpa o registrador de recep�ao de dados
  	U1LSR;                                						// Limpa o registrador de estado de linha
}

//###################################################################################################################
// UART RECEP��O
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Captura o pr�ximo byte recebido na UART
// Parametros: 	Ponteiro para da variavel char para o retorno do byte recebido
// Retorna:		pdPASS indicando que existe caractere recebido 
//				errBUFFER_EMPTY indicando que o buffer est� vazio
// -------------------------------------------------------------------------------------------------------------------
int uart1_GetChar(u8 *ch) {
  	if (U1LSR & U1LSR_RDR) {                 					// Checa se h� caractere na FIFO n�o lido
    	*ch = U1RBR;											// Captura o caractere da FIFO
    	return pdPASS;                      						
    } else
	  	return errBUFFER_EMPTY;
}


// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa o buffer e FIFO de recep��o
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart1_ClearBufferRx(void) {
	U1FCR = U1FCR_RX_FIFO_RESET;		// Limpa RX FIFO
}


//###################################################################################################################
// UART TRANSMISSAO
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Coloca um caractere na fila de transmiss�o
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o c�digo de erro
// -------------------------------------------------------------------------------------------------------------------
int uart1_PutChar(n16 ch) {
	while (!(U1LSR & U1LSR_THRE)) 			// Espero at� que o buffer TX fique vazio
		CRITICAL_WAIT;        				// Caso exista algum OS, pode trocar de contexto

	#if (UART1_USE_HANDSHAKING == pdON)
	while(!uart1_CTS()) 					// espera at� que o receptor esteja pronto para eviar dados
		CRITICAL_WAIT;        				// Caso exista algum OS, pode trocar de contexto
	#endif
  	
  	U1THR = (u8)ch;							// Atualiza o registrador de TX uart com o dado
	  	
  	return pdPASS;
}


// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Transmite uma string at� que encontre o caractere NULO '\0' ou que o buffer estoura
//				ATEN��O: o Compilador j� adiciona \0 na string do tipo "xyz" EX: "\r\r\nHello World!\r\r\n" 
// Parametros: 	Ponteiro da string
// Retorna:		O ponteiro da string. 
//					Retorna NULO se a string foi completamente transmitindo 
//					Ou retorna o ponteiro do pr�ximo caractere a ser transmitido
// OBS: Se a string for maior que o espa�o disponivel no buffer de TX pode ocorrer que a string n�o seja toda transmitida
// -------------------------------------------------------------------------------------------------------------------
u8 *uart1_PutString(u8 *str) {
   	register char ch;

  	while ((ch = *str) && (uart1_PutChar(ch) >= 0))				// Adiciona para transmiss�o enquanto tenha dados para transmitir
    	str++;

  	return str;													// Retorna o ponteiro atual da string
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Remove todos os dados da fila de transmiss�o
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void uart1_FlushTx(void) {
	U1FCR |= U1FCR_TX_FIFO_RESET;          							// Limpa a FIFO de transmiss�o
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Retorna o status do registrador de transmiss�o de dados
// Parametros: 	Nenhum
// Retorna:		pdFALSE = Se o registradores U0THR ou U0TSR n�o est�o vazios
//				pdTRUE = Se o registradores U0THR ou U0TSR est�o vazios
// -------------------------------------------------------------------------------------------------------------------
int uart1_EmptyTx(void) {
  	if ((U1LSR & (U1LSR_THRE | U1LSR_TEMT)) == (U1LSR_THRE | U1LSR_TEMT))
  		return pdTRUE;
  	else
  		return pdFALSE;
}
