#ifndef __UART0_IRQ_H
#define __UART0_IRQ_H

#include "_config_cpu_uart.h"
#include "buffer.h"

//#if defined (SIM_PROTEUS)
//#error O SIMULADOR PROTEUS CONTÉM BUGS NA INTERRUPÇÃO DA UART. Utilize serial simples
//#endif

#if ( UART0_RX_BUFFER_SIZE < 32)
#error BUFFER DE RECEPÇÃO DA UART0 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if ( UART0_TX_BUFFER_SIZE < 32)
#error BUFFER DE TRANSMISSÃO DA UART0 TEM QUE SER MAIOR QUE 32 BYTES. AJUSTE CORRETAMENTE O _config_cpu_uart.h
#endif

#if !defined (UART0_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO DA UART NO _config_cpu_.h
#endif


//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################
int uart0_Init(u32 baudrate);

//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################
int uart0_GetChar(u8 *ch);
void uart0_ClearBufferRx(void);
u32 uart0_CountRxOverflow(void);
int uart0_BufferRxIsEmpty(void);
buffer_t *uart0_GetBufferRx(void);

//###################################################################################################################
// UART TRANSMISSÂO
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

extern xSemaphoreHandle uart0_mutexTX; // semáforo para trasmissão de dados
extern xSemaphoreHandle uart0_mutexRX; // semáforo para recepção de dados
extern xQueueHandle uart0_queueTX;		// Fila de transmissão

int xUart0Init(u32 baudrate, unsigned portSHORT stack_size, unsigned int priority_task, xTaskHandle *task_handle);
int xUart0PutChar (n16 ch);
int xUart0PutString(const char* s);
//int xUart0LockRX(void);
//int xUart0UnLockRX(void);
#endif // #if !defined(FREE_RTOS)

#if (UART0_USE_HANDSHAKING == pdON)
#define uart0_RTSon()  	UART0_RTS_CLR = UART0_RTS // Sinaliza que posso receber dados
#define uart0_RTSoff()  UART0_RTS_SET = UART0_RTS // Sinaliza que não posso receber dados
#define uart0_CTS()		(UART0_CTS_PIN & UART0_CTS)?(pdFALSE):(pdTRUE)
#endif

#endif
