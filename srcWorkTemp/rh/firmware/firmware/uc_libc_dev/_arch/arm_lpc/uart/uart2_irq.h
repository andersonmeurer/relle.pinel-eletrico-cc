#ifndef __UART2_IRQ_H
#define __UART2_IRQ_H

#include "_config_cpu_uart.h"
#include "buffer.h"

//#if defined (SIM_PROTEUS)
//#error O SIMULADOR PROTEUS CONTÉM BUGS NA INTERRUPÇÃO DA UART. Utilize serial simples
//#endif

#if !defined(LPC2468) && !defined(cortexm3)
#error ESTE DISPOSITICO NÃO TEM SUPORTE A UART2
#endif

#if ( UART2_RX_BUFFER_SIZE < 32)
#error BUFFER DE RECEPÇÃO DA UART2 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if ( UART2_TX_BUFFER_SIZE < 32)
#error BUFFER DE TRANSMISSÃO DA UART2 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if !defined (UART2_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO DA UART NO _config_cpu_.h
#endif

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################
int uart2_Init(u32 baudrate);

//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################
int uart2_GetChar(u8 *ch);
void uart2_ClearBufferRx(void);
u32 uart2_CountRxOverflow(void);
int uart2_BufferRxIsEmpty(void);
int uart2_BufferQtdRx(void);
buffer_t *uart2_GetBufferRx(void);

//###################################################################################################################
// UART TRANSMISSÂO
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
#define uart2_RTSoff()  UART2_RTS_SET = UART2_RTS // Sinaliza que não posso receber dados
#define uart2_CTS()		(UART2_CTS_PIN & UART2_CTS)?(pdFALSE):(pdTRUE)
#endif

#endif
