#ifndef __UART1_IRQ_H
#define __UART1_IRQ_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "_config_cpu_.h"
#include "_config_cpu_uart.h"
#include "buffer.h"

void uart1_Init(u32 BaudRate);
void uart1_SetBaudRate(u32 BaudRate);
#if UART1_USE_INT_FUNC == pdON
void uart1_SetRxHandler(void (*rx_func)(u8 c));
void uart1_ClearRxHandler(void);
#endif

int uart1_GetChar(u8 *ch);
u16 uart1_GetRxOverFlow(void);
void uart1_ClearRxBuffer(void);
int uart1_RxBufferIsEmpty(void);
buffer_t *uart1_GetRxBuffer(void);

int uart1_PutCharBasic(n16 c);
int uart1_PutChar(n16 c);
void uart1_ClearTxBuffer(void);
int uart1_TxBufferIsEmpty(void);
buffer_t *uart1_GetTxBuffer(void);

#endif

