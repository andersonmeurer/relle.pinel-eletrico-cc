#ifndef __UART3_IRQ_H
#define __UART3_IRQ_H

#include "_config_cpu_uart.h"
#include "buffer.h"

//#if defined (SIM_PROTEUS)
//#error O SIMULADOR PROTEUS CONTÉM BUGS NA INTERRUPÇÃO DA UART. Utilize serial simples
//#endif

#if !defined(LPC2468) && !defined(cortexm3)
#error ESTE DISPOSITICO NÃO TEM SUPORTE A UART3
#endif

#if ( UART3_RX_BUFFER_SIZE < 32)
#error BUFFER DE RECEPÇÃO DA UART3 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if ( UART3_TX_BUFFER_SIZE < 32)
#error BUFFER DE TRANSMISSÃO DA UART3 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if !defined (UART3_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO DA UART NO _config_cpu_.h
#endif

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################
int uart3_Init(u32 baudrate);
void uart3_SetBaudrate(u32 baudrate);

//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################
int uart3_GetChar(u8 *ch);
void uart3_ClearBufferRx(void);
u32 uart3_CountRxOverflow(void);
int uart3_BufferRxIsEmpty(void);
int uart3_BufferQtdRx(void);
buffer_t *uart3_GetBufferRx(void);

//###################################################################################################################
// UART TRANSMISSÂO
//###################################################################################################################
//###################################################################################################################
int uart3_PutChar(n16 ch);
int uart3_BasicPutChar(n16 ch);
int uart3_WriteTx(u8 *buffer, u16 count);
u8 *uart3_PutString(u8 *str);
u16 uart3_SpaceTx(void);	
void uart3_FlushTx(void);
#define uart3_ClearBufferTx uart3_FlushTx
int uart3_EmptyTx(void);

#if (UART3_USE_HANDSHAKING == pdON)
#define uart3_RTSon()  	UART3_RTS_CLR = UART3_RTS // Sinaliza que posso receber dados
#define uart3_RTSoff()  UART3_RTS_SET = UART3_RTS // Sinaliza que não posso receber dados
#define uart3_CTS()		(UART3_CTS_PIN & UART3_CTS)?(pdFALSE):(pdTRUE)
#endif

#endif
