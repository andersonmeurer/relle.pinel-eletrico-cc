#ifndef __CONFIG_LIB_FREERTOS_H
#define __CONFIG_LIB_FREERTOS_H

#include "_config_cpu_.h"

// ###################################################################################################################
// LED TICK SYSTEM

#define SYSTIME_USE_LED 	pdON 		// pdON habilita driver para piscar um led em tempo na interrupção do timer
	// Definir o tempo em que o led ficará ligado e desligado. Ideal é que a soma seja 1 segundo
	#define SYSTIME_LED_OFF_MS	900		// Tempo em ms que o led ficará desligado
	#define SYSTIME_LED_ON_MS	100		// Tempo em ms que o led ficará ligado


// ###################################################################################################################
// CONFIG freeRTOS
// 	OBS: As muitras outras condfigurações que não estão aqui, e se estão auxentes assumem desligadas

#define configUSE_PREEMPTION						1		// 1 se deseja usar o kernel modo pre-emptivo, 0 para uso do kernel modo cooperativo
#define configUSE_TRACE_FACILITY					0
#define configMAX_TASK_NAME_LEN						( 5 )	// Tamanho máximo dos nomes dados as tarefas, esse nomes são uteis para debug
#define configIDLE_SHOULD_YIELD						1
#define configUSE_CO_ROUTINES 						0		// Ajuste para 1 caso deseja usar co-routines, senão coloque 0
#define configUSE_MUTEXES							1
#define configUSE_RECURSIVE_MUTEXES					1
#define configCHECK_FOR_STACK_OVERFLOW				0		// 0 a X, deve implementar vApplicationStackOverflowHook
#define configQUEUE_REGISTRY_SIZE					10 		// 0 a X
#define configUSE_APPLICATION_TASK_TAG				0
#define configUSE_COUNTING_SEMAPHORES				0
#define configUSE_ALTERNATIVE_API 					0
#define configGENERATE_RUN_TIME_STATS				0
#define configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES 	0



// ###################################################################################################################
// HOOK
#define configUSE_MALLOC_FAILED_HOOK		0 	// 1 se deseja usar notificação falha malloc hook, deve implementar vApplicationMallocFailedHook
#define configUSE_TICK_HOOK					1	// 1 se deseja usar o tick hook, deve implementar vApplicationTickHook
#define configUSE_IDLE_HOOK					0	// 1 se deseja usar o idle hook. A tarega IDLE é criada automaticamente e é sempre execuatda quando as outras tarefas não estão rodando.
											//		Então é possivel re-utilizar a tarefa idle (idle hook) para funções do tipo, power saving, medidor de tempo ocioso do kernel e outros processos


// ###################################################################################################################
// PRIORIDADES
#define configMAX_CO_ROUTINE_PRIORITIES 	( 2 )	// Número máximo de prioridades. Veja configMAX_PRIORITIES
#define configMAX_PRIORITIES				( 4 )	// Número máximo de prioridades. Muitas tarefas podem compartilhar a mesma prioridade.
													// 	 Nas tarefas o número menor representa a menor prioridade e o número máximo (configMAX_PRIORITIES - 1) tem maior prioridade
													//	 Quanto maior número de prioridade mais RAM é ocupada pelo kernel, então mantenha o menor número necessário

// ###################################################################################################################
// VELOCIDADE
#define configTICK_RATE_HZ					( ( portTickType ) 1000 )				// Tempo de medida para o kernel

#if defined(CPU_ARM_LPC) // FAMÍLIA ARM LPC
#define configCPU_CLOCK_HZ			( ( unsigned portLONG ) CCLK )
#elif defined(CPU_AVR) 	// FAMÍLIA AVR
#define configCPU_CLOCK_HZ			( ( unsigned portLONG ) F_CPU )
#else
#error Not defined configCPU_CLOCK_HZ to this CPU
#endif

// ###################################################################################################################
// DEFINIR O NÚMERO DE BITS PARA O CONTADOR DE TICK DO SISTEMA
// 1 Se deseja usar o ticks de 16bits , e 0 para 32bits 
// 		16 bits
//			Contagem máxima delay de 262s para TICK de 250Hz
//			Aumenta a performace para arquitetura de 8 e 16 bits
// 		32 Bits
//  		Contagem máxima delay de 17179869s para TICK de 250Hz
#if defined(CPU_ARM_LPC)
#define configUSE_16_BIT_TICKS			0
#elif defined(CPU_AVR) 	// FAMÍLIA AVR
#define configUSE_16_BIT_TICKS			1
#else
#error Not defined configCPU_CLOCK_HZ to this CPU
#endif

// ###################################################################################################################
// STACK
#define configMINIMAL_STACK_SIZE			( ( unsigned portSHORT ) 200 )			// Tamanho da pilha usado pelo idle task
#define configTOTAL_HEAP_SIZE				( ( size_t ) ( 10 * 1024 ) )			// Tamanho de RAM para que o kernel aloca RAM para as tarefas (STACK e TCB)


/* Software timer definitions.  This example uses I2C to write to the LEDs.  As
this takes a finite time, and because a timer callback writes to an LED, the
priority of the timer task is kept to a minimum to ensure it does not disrupt
test tasks that check their own execution times. */
//#define configUSE_TIMERS				1
//#define configTIMER_TASK_PRIORITY		( 0 )
//#define configTIMER_QUEUE_LENGTH		5
//#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

// INCLUSÃO DE APIs. COLOQUE 1 NAS RESPECTIVAS INCLUDES PARA SUA INCLUSÃO. ISTO É UTIL PARA ECONOMIZAR ROM
#define INCLUDE_vTaskPrioritySet			1
#define INCLUDE_uxTaskPriorityGet			1
#define INCLUDE_vTaskDelete					1
#define INCLUDE_vTaskCleanUpResources		0
#define INCLUDE_vTaskSuspend				1
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay					1
#define INCLUDE_uxTaskGetStackHighWaterMark	1
#define INCLUDE_xTaskGetSchedulerState		1

#endif
