#ifndef __TIMER1_H
#define __TIMER1_H

#include "_config_cpu_.h"
#include "_config_cpu_timer.h"
#include "vic.h"

#if !defined (TIMER1_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO DO TIMER1 NO _config_cpu_.h
#endif

#define TIMER1_VECTOR_INT_MATCH0		0
#define TIMER1_VECTOR_INT_MATCH1		1
#define TIMER1_VECTOR_INT_MATCH2		2
#define TIMER1_VECTOR_INT_MATCH3		3
#define TIMER1_VECTOR_INT_CAPTURE0 		4
#define TIMER1_VECTOR_INT_CAPTURE1 		5
#define TIMER1_VECTOR_INT_CAPTURE2 		6
#define TIMER1_VECTOR_INT_CAPTURE3 		7

#define TIMER1_NUM_INTERRUPTS 			8

int timer1_Init(u32 prescale, u8 sts, u16 int_macths);
void timer1_Set(u8 sts);
void timer1_SetIntHandler(u8 int_num, void (*int_func)(void) );
void timer1_ClearIntHandler(u8 int_num);
void timer1_SetPrescale(u32 prescale);
inline void timer1_ClearCount(void);
inline u32 timer1_GetCount(void);
void timer1_ClearOverflowCount(void);
u32 timer1_GetOverflowCount(void);
inline u64 timer1_GetCountAll(void);

#if (TIMER1_USE_MATCH0 == pdON)
void timer1_SetPinOutputMatch0(int action);
void timer1_SetMatch0(u32 value);
#endif

#if (TIMER1_USE_MATCH1 == pdON)
void timer1_SetPinOutputMatch1(int action);
void timer1_SetMatch1(u32 value);
#endif

#if (TIMER1_USE_MATCH2 == pdON)
void timer1_SetMatch2(u32 value);
#endif

#if (TIMER1_USE_MATCH3 == pdON)
void timer1_SetMatch3(u32 value);
#endif

#if (TIMER1_USE_CAPTURE0 == pdON)
void timer1_InitCapture0(u16 sts);
u32 timer1_GetCapture0(void);
void timer1_StopCapture0(void);
#endif

#if (TIMER1_USE_CAPTURE1 == pdON)
void timer1_InitCapture1(u16 sts);
u32 timer1_GetCapture1(void);
void timer1_StopCapture1(void);
#endif

#endif
