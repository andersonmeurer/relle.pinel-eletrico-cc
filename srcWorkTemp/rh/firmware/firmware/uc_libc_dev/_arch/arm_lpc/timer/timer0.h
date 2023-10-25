#ifndef __TIMER0_H
#define __TIMER0_H

#include "_config_cpu_.h"
#include "_config_cpu_timer.h"
#include "vic.h"

#if !defined (TIMER0_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO DO TIMER0 NO _config_cpu_.h
#endif

#define TIMER0_VECTOR_INT_MATCH0		0
#define TIMER0_VECTOR_INT_MATCH1		1
#define TIMER0_VECTOR_INT_MATCH2		2
#define TIMER0_VECTOR_INT_MATCH3		3
#define TIMER0_VECTOR_INT_CAPTURE0 		4
#define TIMER0_VECTOR_INT_CAPTURE1 		5
#define TIMER0_VECTOR_INT_CAPTURE2 		6
#define TIMER0_VECTOR_INT_CAPTURE3 		7

#define TIMER0_NUM_INTERRUPTS 			8

int timer0_Init(u32 prescale, u8 sts, u16 int_macths);
void timer0_Set(u8 sts);
void timer0_SetIntHandler(u8 int_num, void (*int_func)(void) );
void timer0_ClearIntHandler(u8 int_num);
void timer0_SetPrescale(u32 prescale);
inline void timer0_ClearCount(void);
inline u32 timer0_GetCount(void);
void timer0_ClearOverflowCount(void);
u32 timer0_GetOverflowCount(void);
inline u64 timer0_GetCountAll(void);

#if (TIMER0_USE_MATCH0 == pdON)
void timer0_SetPinOutputMatch0(int action);
void timer0_SetMatch0(u32 value);
#endif

#if (TIMER0_USE_MATCH1 == pdON)
void timer0_SetPinOutputMatch1(int action);
void timer0_SetMatch1(u32 value);
#endif

#if (TIMER0_USE_MATCH2 == pdON)
void timer0_SetMatch2(u32 value);
#endif

#if (TIMER0_USE_MATCH3 == pdON)
void timer0_SetMatch3(u32 value);
#endif

#if (TIMER0_USE_CAPTURE0 == pdON)
void timer0_InitCapture0(u16 sts);
u32 timer0_GetCapture0(void);
void timer0_StopCapture0(void);
#endif

#if (TIMER0_USE_CAPTURE1 == pdON)
void timer0_InitCapture1(u16 sts);
u32 timer0_GetCapture1(void);
void timer0_StopCapture1(void);
#endif

#endif
