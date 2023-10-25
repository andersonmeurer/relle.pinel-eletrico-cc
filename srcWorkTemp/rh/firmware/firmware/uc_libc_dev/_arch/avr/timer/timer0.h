#ifndef __TIMER0_H
#define __TIMER0_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "_config_cpu_.h"
#include "_config_cpu_timer.h"

void timer0_Init(u8 prescale, u8 ints);
void timer0_SetPrescaler(u8 prescale);
u16 timer0_GetPrescaler(void);
inline void timer0_ClearCount(void);
inline u8 timer0_GetCount(void);
inline u32 timer0_GetCountAll(void);
u16 timer0_GetOverflowCount(void);
void timer0_SetIntHandler(u8 interruptNum, void (*userFunc)(void) );
void timer0_ClearIntHandler(u8 interruptNum);

#if defined (OCIE0) && (TIMER0_USE_COMPARE == pdON) // se existe comparação no timer0
	void timer0_SetCompare (u8 value);
#endif

#endif
