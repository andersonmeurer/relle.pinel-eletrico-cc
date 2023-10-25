#ifndef __UART0_IRQ_H
#define __UART0_IRQ_H

#include "_config_cpu_uart.h"
#include "buffer.h"

//#if defined (SIM_PROTEUS)
//#error O SIMULADOR PROTEUS CONT�M BUGS NA INTERRUP��O DA UART. Utilize serial simples
//#endif

#if ( UART0_RX_BUFFER_SIZE < 32)
#error BUFFER DE RECEP��O DA UART0 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if ( UART0_TX_BUFFER_SIZE < 32)
#error BUFFER DE TRANSMISS�O DA UART0 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if !defined (UART0_PRIO_LEVEL)
#error N�O FOI DEFINIDO O N�VEL DE INTERRUP��O DA UART NO _config_cpu_.h
#endif


//###################################################################################################################
// UART CONFIGURA��ES
//###################################################################################################################
//###################################################################################################################
int uart0_Init(u32 baudrate);

//###################################################################################################################
// UART RECEP��O
//###################################################################################################################
//###################################################################################################################
int uart0_GetChar(u8 *ch);
void uart0_ClearBufferRx(void);
u32 uart0_CountRxOverflow(void);
int uart0_BufferRxIsEmpty(void);
buffer_t *uart0_GetBufferRx(void);

//###################################################################################################################
// UART TRANSMISS�O
//###################################################################################################################
//###################################################################################################################
int uart0_PutChar(n16 ch);
int uart0_BasicPutChar(n16 ch);
u16 uart0_SpaceTx(void);
void uart0_FlushTx(void);
#define uart0_ClearBufferTx uart0_FlushTx
int uart0_EmptyTx(void);
int uart0_BufferQtdRx(void);

#if !defined(FREE_RTOS)
int uart0_WriteTx(u8 *buffer, u16 count);
u8 *uart0_PutString(u8 *str);
#else // #if !defined(FREE_RTOS)
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#define xUart0GetChar	uart0_GetChar

extern xSemaphoreHandle uart0_mutexTX; // sem�foro para trasmiss�o de dados
extern xSemaphoreHandle uart0_mutexRX; // sem�foro para recep��o de dados
extern xQueueHandle uart0_queueTX;		// Fila de transmiss�o

int xUart0Init(u32 baudrate, unsigned portSHORT stack_size, unsigned int priority_task, xTaskHandle *task_handle);
int xUart0PutChar (n16 ch);
int xUart0PutString(const char* s);
//int xUart0LockRX(void);
//int xUart0UnLockRX(void);
#endif // #if !defined(FREE_RTOS)

#if (UART0_USE_HANDSHAKING == pdON)
#define uart0_RTSon()  	UART0_RTS_CLR = UART0_RTS // Sinaliza que posso receber dados
#define uart0_RTSoff()  UART0_RTS_SET = UART0_RTS // Sinaliza que n�o posso receber dados
#define uart0_CTS()		(UART0_CTS_PIN & UART0_CTS)?(pdFALSE):(pdTRUE)
#endif

#endif
