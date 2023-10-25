#ifndef __CONFIG_CPU_UART_H
#define __CONFIG_CPU_UART_H

#include "_config_cpu_.h"

//###################################################################################################################
// CONFIGURA��ES UART0
#define UART0_TX_BUFFER_SIZE		50		// Numero de bytes para o buffer TX da Uart0. N�o � usado na uart_simple
#define UART0_RX_BUFFER_SIZE		5		// Numero de bytes para o buffer RX da Uart0. N�o � usado na uart_simple
#define UART0_USE_DOUBLESPEED		pdOFF 	// pdON dobra o valor do baudrate na uart. Somente para atmegas
#define UART0_USE_INT_FUNC 			pdOFF	// pdON adiciona as fun��es para trabalhar com chamadas de fun��es nas interrup��es

//###################################################################################################################
// CONFIGURA��ES UART1
#define UART1_TX_BUFFER_SIZE		10		// Numero de bytes para o buffer TX da Uart1. N�o � usado na uart_simple
#define UART1_RX_BUFFER_SIZE		1024	// Numero de bytes para o buffer RX da Uart1. N�o � usado na uart_simple
#define UART1_USE_DOUBLESPEED		pdOFF	// pdON dobra o valor do baudrate na uart. Somente para atmegas
#define UART1_USE_INT_FUNC 			pdOFF 	// pdON adiciona as fun��es para trabalhar com chamadas de fun��es nas interrup��es


#endif
