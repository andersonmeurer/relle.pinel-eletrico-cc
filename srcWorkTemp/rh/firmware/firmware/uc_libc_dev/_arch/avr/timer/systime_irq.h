#ifndef __SYSTIME_IRQ_H
#define __SYSTIME_IRQ_H

#include "_config_cpu_.h"

#if defined(FREE_RTOS) // COM FREE RTOS

#include "FreeRTOS.h"
#include "task.h"

#define delay_us(us)		vTaskDelay (us / portTICK_RATE_MS / 1000)
#define delay_ms(ms) 		vTaskDelay (ms / portTICK_RATE_MS)
#define systime_GetTick		xTaskGetTickCount

#define sec2tick(sec)		( ( portTickType ) ( (sec*1000) / portTICK_RATE_MS ) )
#define ms2tick(ms)			( ( portTickType ) ( ms / portTICK_RATE_MS ) )


#else // SEM SISTEMA OPERACIONAL

#include "_config_cpu_timer.h"

#define SYSTIME_TICKS_PER_MS ((1e-3 * F_CPU) + .5)

//#define sec2tick(sec)	 ( ( u16 ) ( sec*ONE_SEC) )
//#define ms2tick(ms)		 ( ( u16 ) ( ms*ONE_MS) )

int systime_Init(void);
void systime_SetIntHandler(void (*int_func)(void), tTime calltime);
void systime_ClearIntHandler(void);
u32 systime_GetTick(void);
u32 systime_GetElapsedTick(u32 start_time);
void systime_Pause(u32 duration);
#define systime_delay_ms(d) systime_Pause(d)
void timerout_Init(u32 val);
int timerout_Check(void);

#endif
#endif
