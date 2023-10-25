#ifndef __TIMER2_H
#define __TIMER2_H

#include "_config_cpu_.h"
#include "_config_cpu_timer.h"
#include "vic.h"

#if !defined (TIMER2_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO DO TIMER2 NO _config_cpu_.h
#endif

#define TIMER2_VECTOR_INT_MATCH0		0
#define TIMER2_VECTOR_INT_MATCH1		1
#define TIMER2_VECTOR_INT_MATCH2		2
#define TIMER2_VECTOR_INT_MATCH3		3
#define TIMER2_VECTOR_INT_CAPTURE0 		4
#define TIMER2_VECTOR_INT_CAPTURE1 		5
#define TIMER2_VECTOR_INT_CAPTURE2 		6
#define TIMER2_VECTOR_INT_CAPTURE3 		7

#define TIMER2_NUM_INTERRUPTS 			8

int timer2_Init(u32 prescale, u8 sts, u16 int_macths);
void timer2_Set(u8 sts);
void timer2_SetIntHandler(u8 int_num, void (*int_func)(void) );
void timer2_ClearIntHandler(u8 int_num);
void timer2_SetPrescale(u32 prescale);
inline void timer2_ClearCount(void);
inline u32 timer2_GetCount(void);
void timer2_ClearOverflowCount(void);
u32 timer2_GetOverflowCount(void);
inline u64 timer2_GetCountAll(void);

#if (TIMER2_USE_MATCH0 == pdON)
void timer2_SetPinOutputMatch0(int action);
void timer2_SetMatch0(u32 value);
#endif

#if (TIMER2_USE_MATCH1 == pdON)
void timer2_SetPinOutputMatch1(int action);
void timer2_SetMatch1(u32 value);
#endif

#if (TIMER2_USE_MATCH2 == pdON)
void timer2_SetMatch2(u32 value);
#endif

#if (TIMER2_USE_MATCH3 == pdON)
void timer2_SetMatch3(u32 value);
#endif


#if (TIMER2_USE_CAPTURE0 == pdON)
	void timer2_InitCapture0(u16 sts);
	u32 timer2_GetCapture0(void);
	void timer2_StopCapture0(void);
#endif

#if (TIMER2_USE_CAPTURE1 == pdON)
	void timer2_InitCapture1(u16 sts);
	u32 timer2_GetCapture1(void);
	void timer2_StopCapture1(void);
#endif

#endif
