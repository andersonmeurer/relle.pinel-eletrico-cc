#ifndef __PNL403v4_H
#define __PNL403v4_H

#include "_config_cpu_.h"
#include "_config_cpu_ihm.h"
#if 	(PNL403_USE_UART == 0)
	extern void uart0_Init(u32 baudrate);
	extern int uart0_GetChar(u8 *ch);
	extern int uart0_PutChar(n16 ch);
	extern int uart0_EmptyTx(void);
	#define uart_Init		uart0_Init
	#define uart_GetChar	uart0_GetChar
	#define uart_PutChar	uart0_PutChar
	#define uart_EmptyTx	uart0_EmptyTx
#elif 	(PNL403_USE_UART == 1)
	extern void uart1_Init(u32 baudrate);
	extern int uart1_GetChar(u8 *ch);
	extern int uart1_PutChar(n16 ch);
	extern int uart1_EmptyTx(void);
	#define uart_Init		uart1_Init
	#define uart_GetChar	uart1_GetChar
	#define uart_PutChar	uart1_PutChar
	#define uart_EmptyTx	uart1_EmptyTx
#elif 	(PNL403_USE_UART == 2)
	extern void uart2_Init(u32 baudrate);
	extern int uart2_GetChar(u8 *ch);
	extern int uart2_PutChar(n16 ch);
	extern int uart2_EmptyTx(void);
	#define uart_Init		uart2_Init
	#define uart_GetChar	uart2_GetChar
	#define uart_PutChar	uart2_PutChar
	#define uart_EmptyTx	uart2_EmptyTx
#elif 	(PNL403_USE_UART == 3)
	extern void uart3_Init(u32 baudrate);
	extern int uart3_GetChar(u8 *ch);
	extern int uart3_PutChar(n16 ch);
	extern int uart3_EmptyTx(void);
	#define uart_Init		uart3_Init
	#define uart_GetChar	uart3_GetChar
	#define uart_PutChar	uart3_PutChar
	#define uart_EmptyTx	uart3_EmptyTx
#else
	#error NÃO FOI ESPECIFICADO QUAL UART A USAR NO _config_cpu_ihm.h
#endif


// ###################################################################################################################
// TECLAS
// ###################################################################################################################
#define KEY_NONE	200
#define KEY_UP		50
#define KEY_LEFT	52
#define KEY_RIGHT	54
#define KEY_DOWN	56
#define KEY_ENTER	42
#define KEY_ESC		35
#define KEY_MENU	53
#define KEY_ADD		64
#define KEY_SUB		52
#define KEY_1		49
#define KEY_2		50
#define KEY_3		51
#define KEY_4		52
#define KEY_5		53
#define KEY_6		54
#define KEY_7		55
#define KEY_8		56
#define KEY_9		57
#define KEY_0		48


void pnl403_Init(void);
int pnl403_Reset(void);
int pnl403_WriteLine(int line, pchar s);
int pnl403_Clear(u8 line);
int pnl403_GetKey(u8* key);
void pnl403_Tick(void);
#endif
