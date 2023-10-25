#ifndef __CONFIG_CPU_UART_H
#define __CONFIG_CPU_UART_H

#include "_config_cpu_.h"

// ###################################################################################################################
// UART0 
#define UART0_USE_HANDSHAKING pdOFF // pdON Utiliza o controle de fluxo RTS/CTS // AINDA NÃO TESTADO nesta versão de uc_libx

#define UART0_RX_BUFFER_SIZE 	300			// Somente para uart com interrupção
#define UART0_TX_BUFFER_SIZE 	300			// Somente para uart com interrupção
#define UART0_MODE				UART0_8N1
	//	UART0_8N1 / UART0_7N1 / UART0_8N2 / UART0_7N2 / UART0_8E1 / UART0_7E1 / UART0_8E2 / UART0_7E2
	//	UART0_8O1 / UART0_7O1 / UART0_8O2 / UART0_7O2
#define UART0_FIFOMODE 			UART0_FIFO_RX_TRIG1  // Simulador proteus não funciona a int CTI, então colocar FIFO RX Trigger 1
	// UART0_FIFO_OFF 
	// UART0_FIFO_RX_TRIG1 
	// UART0_FIFO_RX_TRIG4 
	// UART0_FIFO_RX_TRIG8 
	// UART0_FIFO_RX_TRIG14

// ###################################################################################################################
// UART1 
#define UART1_USE_HANDSHAKING pdOFF // pdON Utiliza o controle de fluxo RTS/CTS // AINDA NÃO TESTADO nesta versão de uc_libx

#define UART1_RX_BUFFER_SIZE 300			// Somente para uart com interrupção
#define UART1_TX_BUFFER_SIZE 300			// Somente para uart com interrupção
#define UART1_MODE				UART1_8N1
	//	UART1_8N1 / UART1_7N1 / UART1_8N2 / UART1_7N2 / UART1_8E1 / UART1_7E1 / UART1_8E2 / UART1_7E2
	//	UART1_8O1 / UART1_7O1 / UART1_8O2 / UART1_7O2
#define UART1_FIFOMODE 			UART1_FIFO_RX_TRIG1  // Simulador proteus não funciona a int CTI, então colocar FIFO RX Trigger 1
	// UART1_FIFO_OFF 
	// UART1_FIFO_RX_TRIG1 
	// UART1_FIFO_RX_TRIG4 
	// UART1_FIFO_RX_TRIG8 
	// UART1_FIFO_RX_TRIG14

// ###################################################################################################################
// UART2
#if defined(LPC2468) || defined(cortexm3)
	#define UART2_USE_HANDSHAKING pdOFF // pdON Utiliza o controle de fluxo RTS/CTS // AINDA NÃO TESTADO nesta versão de uc_libx

	#define UART2_RX_BUFFER_SIZE 300		// Somente para uart com interrupção
	#define UART2_TX_BUFFER_SIZE 300		// Somente para uart com interrupção
	#define UART2_MODE				UART2_8N1
		//	UART2_8N1 / UART2_7N1 / UART2_8N2 / UART2_7N2 / UART2_8E1 / UART2_7E1 / UART2_8E2 / UART2_7E2
		//	UART2_8O1 / UART2_7O1 / UART2_8O2 / UART2_7O2
	#define UART2_FIFOMODE 			UART2_FIFO_RX_TRIG1  // Simulador proteus não funciona a int CTI, então colocar FIFO RX Trigger 1
		// UART2_FIFO_OFF 
		// UART2_FIFO_RX_TRIG1 
		// UART2_FIFO_RX_TRIG4 
		// UART2_FIFO_RX_TRIG8 
		// UART2_FIFO_RX_TRIG14
#endif

// ###################################################################################################################
// UART3
#if defined(LPC2468) || defined(cortexm3)
	#define UART3_USE_HANDSHAKING pdOFF // pdON Utiliza o controle de fluxo RTS/CTS // AINDA NÃO TESTADO nesta versão de uc_libx

	#define UART3_RX_BUFFER_SIZE 300		// Somente para uart com interrupção
	#define UART3_TX_BUFFER_SIZE 300	// Somente para uart com interrupção

	#define UART3_MODE				UART3_8N1
		//	UART3_8N1 / UART3_7N1 / UART3_8N2 / UART3_7N2 / UART3_8E1 / UART3_7E1 / UART3_8E2 / UART3_7E2
		//	UART3_8O1 / UART3_7O1 / UART3_8O2 / UART3_7O2
	#define UART3_FIFOMODE 			UART3_FIFO_RX_TRIG1  // Simulador proteus não funciona a int CTI, então colocar FIFO RX Trigger 1
		// UART3_FIFO_OFF 
		// UART3_FIFO_RX_TRIG1 
		// UART3_FIFO_RX_TRIG4 
		// UART3_FIFO_RX_TRIG8 
		// UART3_FIFO_RX_TRIG14
#endif

#endif
