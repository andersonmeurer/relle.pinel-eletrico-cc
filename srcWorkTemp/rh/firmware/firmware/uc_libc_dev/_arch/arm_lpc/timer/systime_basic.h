#ifndef __SYSTIME_BASIC_H
#define __SYSTIME_BASIC_H

#if defined(FREE_RTOS) // Não se usa essa lib com freertos
#error Não se utiliza esta biblioteca com freeRTOS, utilize a include systime.h
#endif // SEM SISTEMA OPERACIONAL

#include "_config_cpu_.h"
#include "_config_cpu_timer.h"
// SYS TIME E UIP CLOCK
// OBS:  FREE_RTOS, UIP e SYS usam timer 1 ou timer 0. Eles não podem ser usados juntos. Para o cortexM3 utiliza um timer dedicado para systick

// Note: with a PCLK = CCLK/2 = 60MHz/2 and a Prescale divider of 3, we
// have a resolution of 100nSec.  Given the timer's counter register is
// 32-bits, we must make a call to one of the sysTime functions at least
// every ~430 sec.

#define SYS_TIME_PCLK_DIV      		3
#define SYS_TIME_TICKS_PER_SEC   	(PCLK / SYS_TIME_PCLK_DIV)

#define SYS_TIME_1MS        (u32)((  1e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_2MS        (u32)((  2e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_5MS        (u32)((  5e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_10MS       (u32)(( 10e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_20MS       (u32)(( 20e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_30MS       (u32)(( 30e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_50MS       (u32)(( 50e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_100MS      (u32)((100e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_150MS    	(u32)((150e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_250MS     	(u32)((250e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_500MS      (u32)((500e-3 * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_1SEC       (u32)(( 1.0   * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_2SEC       (u32)(( 2.0   * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_5SEC       (u32)(( 5.0   * SYS_TIME_TICKS_PER_SEC) + .5)
#define SYS_TIME_10SEC      (u32)((10.0   * SYS_TIME_TICKS_PER_SEC) + .5)

#define sec2tick(sec)	 ( ( u32 ) ( sec*SYS_TIME_1SEC) )
#define ms2tick(ms)		 ( ( u32 ) ( ms*SYS_TIME_1MS) )

void systime_Init(void);
u32 systime_GetTick(void);
u32 systime_GetElapsedTick(u32 start_time);
void systime_Pause(u32 duration);
void systime_delay_ms(u32 delayval);
void systime_delay_us(u32 delayval);
void timerout_Init(u32 val);
int timerout_Check(void);

#endif
