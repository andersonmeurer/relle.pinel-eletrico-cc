#include "uart1.h"

//###################################################################################################################
// UART CONFIGURA��ES
//###################################################################################################################
//###################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializo a UART
// Parametros:	BaudRate
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void uart1_Init(u32 BaudRate) {
	UCSR1B = _BV(RXEN1)|_BV(TXEN1); // RXEN1 e TXEN1 habilitam TX e RX UArt
	uart1_SetBaudRate(BaudRate); 
	
	#if (UART1_USE_DOUBLESPEED == pdON)
		sbi(UCSR1A, U2X1);
	#endif
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta baud rate da UART de acordo com o clock da CPU
// Parametros: 	Baudrate
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void uart1_SetBaudRate(u32 BaudRate) {
	u32 Br;
		
	// Calcular o fator de divisao de acordo com a frequencia da CPU
	#if (UART1_USE_DOUBLESPEED == pdON)
		Br = (u32)((F_CPU / (BaudRate * 8L)) - 1);
	#else
		Br = (u32)((F_CPU / (BaudRate * 16L)) - 1);	
	#endif
		
 	UBRR1L = (u8) (Br&0xFF);
	UBRR1H = (u8) ((Br>>8)&0x0F);
}

//###################################################################################################################
// UART RECEP��O
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Checa se recebeu algum byte na UART
// Parametros:	O endere�o da variavel byte para captura do dado recebido
// Retorna:		Retona pdPASS se caractere recebido ou c�digo do erro
// -----------------------------------------------------------------------------------------------------------------
int uart1_GetChar(u8* rxData) {
    if ( (UCSR1A & _BV(RXC1)) ) { // Checa se o registrador da recep��o da UART cont�m byte recebido
    	*rxData = UDR1;
    	return pdPASS;
    } else
    	return errREG_EMPTY;			// no data
}

//###################################################################################################################
// UART TRANSMISS�O
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Transmitir um byte na UART de forma b�sica. FUN��O UTIL PARA DEBUG ONDE N�O PRECISA INICIAR IRQ ou OS
// Parametros:	O byte a ser transmitido
// Retorna:		
// -----------------------------------------------------------------------------------------------------------------
int uart1_PutChar(n16 c) {
	while(!(UCSR1A & (1<<UDRE)))	// Espera que o registrador da UART1 para transmiss�o fique vazio
		CRITICAL_WAIT;        		// Caso exista algum OS, pode trocar de contexto
	UDR1 = (u8)c;
	return pdPASS;
}
