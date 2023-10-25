#ifndef __UART0_POLL_H
#define __UART0_POLL_H

#include "_config_cpu_.h"
#include "_config_cpu_uart.h"

#if (UART0_USE_HANDSHAKING == pdON)
	#warning A LIB UART0.C NÃO ESTÁ PREPARADADA PARA CONTROLE DE FLUXO
#endif


//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################
void uart0_Init(u32 baudrate);


//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################
int uart0_GetChar(u8 *ch);
void uart0_ClearBufferRx(void);
int uart0_EmptyRx(void);

//###################################################################################################################
// UART TRANSMISSÂO
//###################################################################################################################
//###################################################################################################################
int uart0_PutChar(n16 ch);
u8 *uart0_PutString(u8 *str);
void uart0_FlushTx(void);
#define uart0_ClearBufferTx uart0_FlushTx
int uart0_EmptyTx(void);

#if (UART0_USE_HANDSHAKING == pdON)
#define uart0_RTSon()  	UART0_RTS_CLR = UART0_RTS // Sinaliza que posso receber dados
#define uart0_RTSoff()  UART0_RTS_SET = UART0_RTS // Sinaliza que não posso receber dados
#define uart0_CTS()		(UART0_CTS_PIN & UART0_CTS)?(pdFALSE):(pdTRUE)
#endif

#endif
