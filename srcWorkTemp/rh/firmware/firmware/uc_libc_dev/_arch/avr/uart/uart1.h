#ifndef __UART1_SIMPLE_H
#define __UART1_SIMPLE_H

#include "_config_cpu_.h"
#include "_config_cpu_uart.h"

void uart1_Init(u32 BaudRate);
void uart1_SetBaudRate(u32 BaudRate);
int uart1_GetChar(u8* data);
int uart1_PutChar(n16 c);

#endif

