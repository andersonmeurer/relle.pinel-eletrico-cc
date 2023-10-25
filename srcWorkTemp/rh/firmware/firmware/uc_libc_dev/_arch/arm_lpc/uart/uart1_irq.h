#ifndef __UART1_IRQ_H
#define __UART1_IRQ_H

#include "_config_cpu_uart.h"
#include "buffer.h"

//#if defined (SIM_PROTEUS)
//#error O SIMULADOR PROTEUS CONTÉM BUGS NA INTERRUPÇÃO DA UART. Utilize serial simples
//#endif

#if ( UART1_RX_BUFFER_SIZE < 32)
#error BUFFER DE RECEPÇÃO DA UART1 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if ( UART1_TX_BUFFER_SIZE < 32)
#error BUFFER DE TRANSMISSÃO DA UART1 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if !defined (UART1_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO DA UART NO _config_cpu_.h
#endif

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################
int uart1_Init(u32 baudrate);

//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################
int uart1_GetChar(u8 *ch);
void uart1_ClearBufferRx(void);
u32 uart1_CountRxOverflow(void);
int uart1_BufferRxIsEmpty(void);
int uart1_BufferQtdRx(void);
buffer_t *uart1_GetBufferRx(void);

//###################################################################################################################
// UART TRANSMISSÂO
//###################################################################################################################
//###################################################################################################################
int uart1_PutChar(n16 ch);
int uart1_BasicPutChar(n16 ch);
int uart1_WriteTx(u8 *buffer, u16 count);
u8 *uart1_PutString(u8 *str);
u16 uart1_SpaceTx(void);	
void uart1_FlushTx(void);
#define uart1_ClearBufferTx uart1_FlushTx
int uart1_EmptyTx(void);

#if (UART1_USE_HANDSHAKING == pdON)
#define uart1_RTSon()  	UART1_RTS_CLR = UART1_RTS // Sinaliza que posso receber dados
#define uart1_RTSoff()  UART1_RTS_SET = UART1_RTS // Sinaliza que não posso receber dados
#define uart1_CTS()		(UART1_CTS_PIN & UART1_CTS)?(pdFALSE):(pdTRUE)
#endif

#endif
