#ifndef __TIMER2_H
#define __TIMER2_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "_config_cpu_.h"
#include "_config_cpu_timer.h"

void timer2_Init(u8 prescale, u8 ints);
void timer2_SetPrescaler(u8 prescale);
u16 timer2_GetPrescaler(void);
inline void timer2_ClearCount(void);
inline u8 timer2_GetCount(void);
inline u32 timer2_GetCountAll(void);
u16 timer2_GetOverflowCount(void);
void timer2_SetIntHandler(u8 interruptNum, void (*userFunc)(void) );
void timer2_ClearIntHandler(u8 interruptNum);

#if (TIMER2_USE_COMPARE == pdON)
	void timer2_SetCompare (u8 Value);
#endif

#endif
