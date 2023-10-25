#ifndef __RTC_H
#define __RTC_H
	
#include "_config_cpu_.h"
#include "_config_cpu_timer.h"

#if defined (AS2) // atmega8, atmega16 e atmega8535 utilizam o timer 2 para o rtc
	#include "timer2.h"	
	
	#define TIMER_RTC_CLK_STOP 			TIMER2_RTC_CLK_STOP				// RTC Timer Stopped
	#define TIMER_RTC_CLK_DIV1 			TIMER2_RTC_CLK_DIV1				// RTC Timer clocked at CPU
	#define TIMER_RTC_CLK_DIV8 			TIMER2_RTC_CLK_DIV8				// RTC Timer clocked at CPU/8
	#define TIMER_RTC_CLK_DIV32 		TIMER2_RTC_CLK_DIV32			// RTC Timer clocked at CPU/32
	#define TIMER_RTC_CLK_DIV64 		TIMER2_RTC_CLK_DIV64			// RTC Timer clocked at CPU/64
	#define TIMER_RTC_CLK_DIV128 		TIMER2_RTC_CLK_DIV128			// RTC Timer clocked at CPU/128
	#define TIMER_RTC_CLK_DIV256		TIMER2_RTC_CLK_DIV256			// RTC Timer clocked at CPU/256
	#define TIMER_RTC_CLK_DIV1024 		TIMER2_RTC_CLK_DIV1024			// RTC Timer clocked at CPU/1024

	#define TIMER_ENABLE_INT_OVERFLOW	TIMER2_ENABLE_INT_OVERFLOW
	#define TIMER_VETOR_INT_OVERFLOW 	TIMER2_VETOR_INT_OVERFLOW
	#define ASSR_AS						AS2
	#define timer_Init 					timer2_Init
	#define timer_SetIntHandler 		timer2_SetIntHandler
#else // atmega128 utiliza o timer 0 para o rtc
	#include "timer0.h"
	#define TIMER_RTC_CLK_STOP 			TIMER0_RTC_CLK_STOP				// RTC Timer Stopped
	#define TIMER_RTC_CLK_DIV1 			TIMER0_RTC_CLK_DIV1				// RTC Timer clocked at CPU
	#define TIMER_RTC_CLK_DIV8 			TIMER0_RTC_CLK_DIV8				// RTC Timer clocked at CPU/8
	#define TIMER_RTC_CLK_DIV32 		TIMER0_RTC_CLK_DIV32			// RTC Timer clocked at CPU/32
	#define TIMER_RTC_CLK_DIV64 		TIMER0_RTC_CLK_DIV64			// RTC Timer clocked at CPU/64
	#define TIMER_RTC_CLK_DIV128 		TIMER0_RTC_CLK_DIV128			// RTC Timer clocked at CPU/128
	#define TIMER_RTC_CLK_DIV256		TIMER0_RTC_CLK_DIV256			// RTC Timer clocked at CPU/256
	#define TIMER_RTC_CLK_DIV1024 		TIMER0_RTC_CLK_DIV1024			// RTC Timer clocked at CPU/1024

	#define TIMER_ENABLE_INT_OVERFLOW	TIMER0_ENABLE_INT_OVERFLOW
	#define TIMER_VETOR_INT_OVERFLOW 	TIMER0_VETOR_INT_OVERFLOW
	#define ASSR_AS						AS0
	#define timer_Init 					timer0_Init
	#define timer_SetIntHandler 		timer0_SetIntHandler
#endif

typedef struct rtc_s {	
	// HARDWARE
	u8 tics;
	u64  totaltics;
	// HORARIO
	u8 hours;
	u8 minutes;
	u8 seconds;
	// DATA
	u8 day;
	u8 month;
	u16 year;
} tRTC;


void rtc_Init(void);
void rtc_SetTime(u8 hours, u8 minutes, u8 seconds);
void rtc_SetDate(u8 day, u8 month, u16 year);
void rtc_SetRtc(tRTC *t);
void rtc_GetTime(u8 *hours, u8 *minutes, u8 *seconds);
void rtc_GetDate(u8 *day, u8 *month, u16 *year);
void rtc_GetRtc(tRTC *t);

#endif
