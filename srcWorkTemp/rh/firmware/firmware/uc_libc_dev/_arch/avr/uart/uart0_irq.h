#ifndef __UART0_IRQ_H
#define __UART0_IRQ_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "_config_cpu_.h"
#include "_config_cpu_uart.h"
#include "buffer.h"

void uart0_Init(u32 BaudRate);
void uart0_SetBaudRate(u32 BaudRate);
#if UART0_USE_INT_FUNC == pdON
void uart0_SetRxHandler(void (*rx_func)(u8 c));
void uart0_ClearRxHandler(void);
#endif 

int uart0_GetChar(u8 *ch);
u16 uart0_GetRxOverFlow(void);
void uart0_ClearRxBuffer(void);
int uart0_RxBufferIsEmpty(void);
buffer_t *uart0_GetRxBuffer(void);

int uart0_PutCharBasic(n16 c);
int uart0_PutChar(n16 c);
void uart0_ClearTxBuffer(void);
int uart0_TxBufferIsEmpty(void);
buffer_t *uart0_GetTxBuffer(void);

#endif

