#ifndef __UART3_POLL_H
#define __UART3_POLL_H

#include "_config_cpu_.h"
#include "_config_cpu_uart.h"

#if !defined(LPC2468) && !defined(cortexm3)
#error ESTE DISPOSITICO NÃO TEM SUPORTE A UART3
#endif

#if (UART3_USE_HANDSHAKING == pdON)
	#warning A LIB UART3.C NÃO ESTÁ PREPARADADA PARA CONTROLE DE FLUXO
#endif

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################
void uart3_Init(u32 baudrate);


//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################
int uart3_GetChar(u8 *ch);
void uart3_ClearBufferRx(void);


//###################################################################################################################
// UART TRANSMISSÂO
//###################################################################################################################
//###################################################################################################################
int uart3_PutChar(n16 ch);
u8 *uart3_PutString(u8 *string);
void uart3_FlushTx(void);
#define uart3_ClearBufferTx uart3_FlushTx
int uart3_EmptyTx(void);


#if (UART3_USE_HANDSHAKING == pdON)
#define uart3_RTSon()  	UART3_RTS_CLR = UART3_RTS // Sinaliza que posso receber dados
#define uart3_RTSoff()  UART3_RTS_SET = UART3_RTS // Sinaliza que não posso receber dados
#define uart3_CTS()		(UART3_CTS_PIN & UART3_CTS)?(pdFALSE):(pdTRUE)
#endif

#endif
