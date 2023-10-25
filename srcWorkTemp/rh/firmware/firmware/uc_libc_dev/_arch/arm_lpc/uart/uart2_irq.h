#ifndef __UART2_IRQ_H
#define __UART2_IRQ_H

#include "_config_cpu_uart.h"
#include "buffer.h"

//#if defined (SIM_PROTEUS)
//#error O SIMULADOR PROTEUS CONT�M BUGS NA INTERRUP��O DA UART. Utilize serial simples
//#endif

#if !defined(LPC2468) && !defined(cortexm3)
#error ESTE DISPOSITICO N�O TEM SUPORTE A UART2
#endif

#if ( UART2_RX_BUFFER_SIZE < 32)
#error BUFFER DE RECEP��O DA UART2 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if ( UART2_TX_BUFFER_SIZE < 32)
#error BUFFER DE TRANSMISS�O DA UART2 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if !defined (UART2_PRIO_LEVEL)
#error N�O FOI DEFINIDO O N�VEL DE INTERRUP��O DA UART NO _config_cpu_.h
#endif

//###################################################################################################################
// UART CONFIGURA��ES
//###################################################################################################################
//###################################################################################################################
int uart2_Init(u32 baudrate);

//###################################################################################################################
// UART RECEP��O
//###################################################################################################################
//###################################################################################################################
int uart2_GetChar(u8 *ch);
void uart2_ClearBufferRx(void);
u32 uart2_CountRxOverflow(void);
int uart2_BufferRxIsEmpty(void);
int uart2_BufferQtdRx(void);
buffer_t *uart2_GetBufferRx(void);

//###################################################################################################################
// UART TRANSMISS�O
//###################################################################################################################
//###################################################################################################################
int uart2_PutChar(n16 ch);
int uart2_BasicPutChar(n16 ch);
int uart2_WriteTx(u8 *buffer, u16 count);
u8 *uart2_PutString(u8 *str);
u16 uart2_SpaceTx(void);	
void uart2_FlushTx(void);
#define uart2_ClearBufferTx uart2_FlushTx
int uart2_EmptyTx(void);

#if (UART2_USE_HANDSHAKING == pdON)
#define uart2_RTSon()  	UART2_RTS_CLR = UART2_RTS // Sinaliza que posso receber dados
#define uart2_RTSoff()  UART2_RTS_SET = UART2_RTS // Sinaliza que n�o posso receber dados
#define uart2_CTS()		(UART2_CTS_PIN & UART2_CTS)?(pdFALSE):(pdTRUE)
#endif

#endif
