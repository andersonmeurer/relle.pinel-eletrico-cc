#ifndef __TIMER3_H
#define __TIMER3_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "_config_cpu_.h"
#include "_config_cpu_timer.h"
	
void timer3_Init(u8 prescale, u8 ints);
void timer3_SetPrescaler(u8 prescale);
u16 timer3_GetPrescaler(void);
inline void timer3_ClearCount(void);
inline u16 timer3_GetCount(void);
inline u32 timer3_GetCountAll(void);
u16 timer3_GetOverflowCount(void);
void timer3_SetIntHandler(u8 IntNum, void (*Func)(void) );
void timer3_ClearIntHandler(u8 IntNum);

#if (TIMER3_USE_COMPARE == pdON)
	void timer3_SetComparePortOC3A (u8 StsOC3A);
	void timer3_SetComparePortOC3B (u8 StsOC3B);
	void timer3_SetComparePortOC3C (u8 StsOC3C);
	void timer3_SetCompareA (u16 Value);
	void timer3_SetCompareB (u16 Value);
	void timer3_SetCompareC (u16 Value);
	void timer3_SetCompareClearOnMatch(void);
#endif

#if (TIMER3_USE_ICP == pdON)
	void timer3_IcpInit(u8 Config);
	u16 timer3_IcpGetValue(void);
#endif

#if (TIMER3_USE_PWM == pdON)
	void timer3_PwmInit(u8 bitRes);
	void timer3_PwmInitICR(u16 TopCount);
	void timer3_PwmOff(void);
	void timer3_PwmAOn(void);
	void timer3_PwmBOn(void);
	void timer3_PwmCOn(void);
	void timer3_PwmAOff(void);
	void timer3_PwmBOff(void);
	void timer3_PwmCOff(void);
	void timer3_PwmASet(u8 pwmDuty);
	void timer3_PwmBSet(u8 pwmDuty);
	void timer3_PwmCSet(u8 pwmDuty);
#endif

#endif
