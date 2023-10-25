#ifndef __UART2_POLL_H
#define __UART2_POLL_H

#include "_config_cpu_.h"
#include "_config_cpu_uart.h"

#if !defined(LPC2468) && !defined(cortexm3)
#error ESTE DISPOSITICO N�O TEM SUPORTE A UART2
#endif

#if (UART2_USE_HANDSHAKING == pdON)
#warning A LIB UART2.C N�O EST� PREPARADADA PARA CONTROLE DE FLUXO
#endif

//###################################################################################################################
// UART CONFIGURA��ES
//###################################################################################################################
//###################################################################################################################
void uart2_Init(u32 baudrate);


//###################################################################################################################
// UART RECEP��O
//###################################################################################################################
//###################################################################################################################
int uart2_GetChar(u8 *ch);
void uart2_ClearBufferRx(void);


//###################################################################################################################
// UART TRANSMISS�O
//###################################################################################################################
//###################################################################################################################
int uart2_PutChar(n16 ch);
u8 *uart2_PutString(u8 *str);
void uart2_FlushTx(void);
#define uart2_ClearBufferTx uart2_FlushTx
int uart2_EmptyTx(void);

#if (UART2_USE_HANDSHAKING == pdON)
#define uart2_RTSon()  	UART2_RTS_CLR = UART2_RTS // Sinaliza que posso receber dados
#define uart2_RTSoff()  UART2_RTS_SET = UART2_RTS // Sinaliza que n�o posso receber dados
#define uart2_CTS()		(UART2_CTS_PIN & UART2_CTS)?(pdFALSE):(pdTRUE)
#endif


#endif
