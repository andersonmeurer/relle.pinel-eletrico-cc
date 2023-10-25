#include "uart0.h"

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializo a UART
// Parametros:	BaudRate
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void uart0_Init(u32 BaudRate) {
	UCSR0B = _BV(RXEN)|_BV(TXEN); // RXEN0 e TXEN0 habilitam TX e RX UArt
	uart0_SetBaudRate(BaudRate);

	#if defined(U2X) && (UART0_USE_DOUBLESPEED == pdON)
		sbi(UCSR0A, U2X);
	#endif	
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta baud rate da UART0 de acordo com o clock da CPU
// Parametros: 	Baudrate
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void uart0_SetBaudRate(u32 BaudRate) {
	u32 Br;
		
	// Calcular o fator de divisao de acordo com a frequencia da CPU
	#if defined(U2X) && (UART0_USE_DOUBLESPEED == pdON)
		Br = (u32)((F_CPU / (BaudRate * 8L)) - 1);
	#else
		Br = (u32)((F_CPU / (BaudRate * 16L)) - 1);	
	#endif
		
 	UBRR0L = (u8) (Br&0xFF);
	
	#ifdef UBRR0H
		UBRR0H = (u8) ((Br>>8)&0x0F);		
	#endif
}

//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Checa se recebeu algum byte na UART
// Parametros:	O endereço da variavel byte para captura do dado recebido
// Retorna:		Retona pdPASS se caractere recebido ou código do erro
// -----------------------------------------------------------------------------------------------------------------
int uart0_GetChar(u8* rxData) {
    if ( (UCSR0A & _BV(RXC0)) ) { // Checa se o registrador da recepção da UART contém byte recebido
    	*rxData = UDR0;
    	return pdPASS;
    } else
    	return errREG_EMPTY;			// no data
}

//###################################################################################################################
// UART TRANSMISSÂO
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Transmitir um byte na UART
// Parametros:	O byte a ser transmitido
// Retorna:		
// -----------------------------------------------------------------------------------------------------------------
int uart0_PutChar(n16 c) {
	while(!(UCSR0A & (1<<UDRE)))  	// Espera que o registrador da UART0 para transmissão fique vazio
		CRITICAL_WAIT;        		// Caso exista algum OS, pode trocar de contexto
		
	UDR0 = (u8)c;
	return pdPASS;
}

