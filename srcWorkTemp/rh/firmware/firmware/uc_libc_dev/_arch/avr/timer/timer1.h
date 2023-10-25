#ifndef __TIMER1_H
#define __TIMER1_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "_config_cpu_.h"
#include "_config_cpu_timer.h"

void timer1_Init(u8 prescale, u16 ints);
void timer1_SetPrescaler(u8 prescale);
u16 timer1_GetPrescaler(void);
inline void timer1_ClearCount(void);
inline u16 timer1_GetCount(void);
inline u32 timer1_GetCountAll(void);
u16 timer1_GetOverflowCount(void);
void timer1_SetIntHandler(u8 interruptNum, void (*userFunc)(void) );
void timer1_ClearIntHandler(u8 interruptNum);

#if (TIMER1_USE_COMPARE == pdON)
	void timer1_SetComparePortOC1A(u8 StsOC1A);
	void timer1_SetComparePortOC1B(u8 StsOC1B);
	void timer1_SetCompareA (u16 Value);
	void timer1_SetCompareB (u16 Value);
	void timer1_SetCompareClearOnMatch(void);
	
	#ifdef OCR1C
		void timer1_SetComparePortOC1C(u8 StsOC1C);
		void timer1_SetCompareC (u16 Value);
	#endif
#endif

#if (TIMER1_USE_ICP == pdON)	
	void timer1_IcpInit(u8 config);
	u16 timer1_IcpGetValue(void);
#endif

#if (TIMER1_USE_PWM == pdON)
	void timer1_PwmInit(u8 bitRes);
	void timer1_PwmInitICR(u16 TopCount);
	void timer1_PwmOff(void);
	void timer1_PwmAOn(void);
	void timer1_PwmBOn(void);
	void timer1_PwmAOff(void);
	void timer1_PwmBOff(void);
	void timer1_PwmASet(u8 pwmDuty);
	void timer1_PwmBSet(u8 pwmDuty);
	
	#ifdef OCR1C
		void timer1_PwmCSet(u8 pwmDuty);
		void timer1_PwmCOn(void);
		void timer1_PwmCOff(void);
	#endif

#endif

#endif
