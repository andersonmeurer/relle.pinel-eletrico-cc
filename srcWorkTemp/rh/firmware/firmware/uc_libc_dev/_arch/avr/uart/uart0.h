#ifndef __UART0_SIMPLE_H
#define __UART0_SIMPLE_H

#include "_config_cpu_.h"
#include "_config_cpu_uart.h"

void uart0_Init(u32 BaudRate);
void uart0_SetBaudRate(u32 BaudRate);
int uart0_GetChar(u8* data);
int uart0_PutChar(n16 c);

#endif

