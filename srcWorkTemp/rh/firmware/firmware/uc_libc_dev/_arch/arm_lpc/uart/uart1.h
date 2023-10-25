#ifndef __UART1_POLL_H
#define __UART1_POLL_H

#include "_config_cpu_.h"
#include "_config_cpu_uart.h"

#if (UART1_USE_HANDSHAKING == pdON)
	#warning A LIB UART1.C NÃO ESTÁ PREPARADADA PARA CONTROLE DE FLUXO
#endif

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################
void uart1_Init(u32 baudrate);


//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################
int uart1_GetChar(u8 *ch);
void uart1_ClearBufferRx(void);


//###################################################################################################################
// UART TRANSMISSÂO
//###################################################################################################################
//###################################################################################################################
int uart1_PutChar(n16 ch);
u8 *uart1_PutString(u8 *str);
void uart1_FlushTx(void);
#define uart1_ClearBufferTx uart1_FlushTx
int uart1_EmptyTx(void);

#if (UART1_USE_HANDSHAKING == pdON)
#define uart1_RTSon()  	UART1_RTS_CLR = UART1_RTS // Sinaliza que posso receber dados
#define uart1_RTSoff()  UART1_RTS_SET = UART1_RTS // Sinaliza que não posso receber dados
#define uart1_CTS()		(UART1_CTS_PIN & UART1_CTS)?(pdFALSE):(pdTRUE)
#endif

#endif
