#ifndef __SYSTICK_H
#define __SYSTICK_H

#if !defined (cortexm3)
#error Este microcontrolador não suporta este tipo de timer de sistema
#endif

#if defined(FREE_RTOS) // Não se usa essa lib com freertos
#error Não se utiliza esta biblioteca com freeRTOS, utilize a include systime.h
#endif // SEM SISTEMA OPERACIONAL

#include "_config_cpu_.h"
#include "_config_cpu_timer.h"

#define SYSTIME_1MS   	(u32)(1)
#define SYSTIME_2MS   	(u32)(2)
#define SYSTIME_5MS    (u32)(5)
#define SYSTIME_10MS   (u32)(10)
#define SYSTIME_20MS   (u32)(20)
#define SYSTIME_30MS   (u32)(30)
#define SYSTIME_50MS   (u32)(50)
#define SYSTIME_100MS  (u32)(100)
#define SYSTIME_150MS  (u32)(150)
#define SYSTIME_250MS  (u32)(250)
#define SYSTIME_500MS  (u32)(500)
#define SYSTIME_1SEC   (u32)(1000)
#define SYSTIME_2SEC   (u32)(2000)
#define SYSTIME_5SEC   (u32)(5000)
#define SYSTIME_10SEC 	(u32)(10000)

#define sec2tick(sec)	 ( ( u32 ) ( sec*SYSTIME_1SEC) )
#define ms2tick(ms)		 ( ( u32 ) ( ms*SYSTIME_1MS) )

int systime_Init(void);
void systime_SetIntHandler(void (*int_func)(void), uint xticks);
void systime_ClearIntHandler(void);
u32 systime_GetTick(void);
u32 systime_GetElapsedTick(u32 start_time);
void systime_Pause(u32 duration);
#define systime_delay_ms(d) systime_Pause(d)
void timeout_Init(u32 timeout);
int timeout_Check(void);

#endif
