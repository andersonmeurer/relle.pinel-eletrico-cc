#ifndef __RTC_H
#define __RTC_H

#include "_config_cpu_.h"
#include "_config_cpu_timer.h"
#include "vic.h"

#if (RTC_USE_INT == pdON)
#if !defined (RTC_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO PARA O RTC NO _config_cpu_.h
#endif
#endif

// VETOR PARA INTERRUPÇÕES DO RTC
#define RTC_NUM_INTERRUPTS 		2		
	#define RTC_INT_TIME		0 // Interrupção gerada para contagem do tempo
	#define RTC_INT_ALARM		1 // Interrupção gerada para o alarme

typedef struct sRTC {
  	u16 year;    		// Year value - [0, 4095]
  	u8 month;  			// Month value - [1, 24]
  	u8 mday;    		// Day of the month value - [1, 31]
  	u8 wday;    		// Day of week value - [0, 6]
  	u8 hour;    		// Hour value - [0, 23]
  	u8 min;     		// Minute value - [0, 59]
  	u8 sec;     		// Second value - [0, 59]
} tRTC, *pRTC;

int rtc_Init(void);

void rtc_Start( void );
void rtc_Stop( void );
void rtc_ResetCTC( void );
void rtc_SetTime( tRTC *rtc );
bool rtc_GetTime( tRTC *rtc );

void rtc_Wake(void);
void rtc_Sleep(void);
#if (RTC_USE_INT == pdON)
void rtc_SetIntHandler(u8 int_num, void (*int_func)(void) );
void rtc_ClearIntHandler(u8 int_num);
void rtc_RegInt( u8 reg_int );
void rtc_RegIntAlarm( u8 reg_int_alarm );
#endif
#endif
