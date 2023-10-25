#include "rtc.h"

// PROGRAM ROM CONSTANTS
static u8 __attribute__ ((progmem)) month_day_table[]   = {31,28,31,30,31,30,31,31,30,31,30,31};
tRTC rtc;

void rtc_Service(void);

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o RTC
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_Init(void) {
	// use timer for most AVRs
	// initialize timer 
	timer_Init(TIMER_RTC_CLK_DIV8, TIMER_ENABLE_INT_OVERFLOW);
	// count with 32.768KHz/8
	// switch to asynchronous input (32KHz crystal)
	#if (RTC_USE_XTAL_EXTERNAL == pdON)
	sbi(ASSR, ASSR_AS); // Habilitar o cristal externo para o clock do RTC ao inves do clock da CPU
	#endif
	// attach service to real-time clock interrupt
	// rtcService() will be called at ((32768/8)/256) = 16Hz
	timer_SetIntHandler(TIMER_VETOR_INT_OVERFLOW, rtc_Service);
}


// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta hora do sistema
// Parametros:	parametros de hora, minuto e segundos
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_SetTime(u8 hours, u8 minutes, u8 seconds)	{
	rtc.hours 	= hours;
	rtc.minutes = minutes;
	rtc.seconds = seconds;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta data
// Parametros:	Dia, mes e ano
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_SetDate(u8 day, u8 month, u16 year)	{
	rtc.day 	= day;
	rtc.month   = month;
	rtc.year 	= year;
}

void rtc_SetRtc(tRTC *t) {
	rtc.hours 	= t->hours;
	rtc.minutes = t->minutes;
	rtc.seconds = t->seconds;
	rtc.day 	= t->day;
	rtc.month   = t->month;
	rtc.year 	= t->year;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Captura o hora do RTC
// Parametros:	Ponteiros para hora, minuto e segundos
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_GetTime(u8 *hours, u8 *minutes, u8 *seconds)	{
	*hours = rtc.hours;
	*minutes = rtc.minutes;
	*seconds = rtc.seconds;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Captura data do sistema
// Parametros:	Ponteiros para dia, mes e ano
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void rtc_GetDate(u8 *day, u8 *month, u16 *year)	{
	*day = rtc.day;
	*month = rtc.month;
	*year = rtc.year;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Captura a estrutura do RTC atualizado
// Parametros:	Nenhum
// Retorna:		Ponteiro da estrutura do RTC
// -------------------------------------------------------------------------------------------------------------------
void rtc_GetRtc(tRTC *t) {
	// return the real-time clock data
	t->hours 	= rtc.hours;
	t->minutes 	= rtc.minutes;
	t->seconds 	= rtc.seconds;
	t->day 		= rtc.day;
	t->month   	= rtc.month;
	t->year 	= rtc.year;
}

//###################################################################################################################
//###################################################################################################################
// SERVIÇO DE INT
//###################################################################################################################
void rtc_Service(void) {
	u8 day;
	
	rtc.totaltics++;
	rtc.tics++;
	
	if(rtc.tics == 16) {															// check for overflows
		rtc.tics = 0;
		rtc.seconds++;																// increment seconds
		if(rtc.seconds > 59) {														// check seconds overflow
			rtc.seconds -= 60;
			rtc.minutes++;															// increment minutes
			if(rtc.minutes > 59) {													// check minutes overflow
				rtc.minutes -= 60;
				rtc.hours++;														// increment hours
				if(rtc.hours > 23) {												// check hours overflow
					rtc.hours -= 24;
					rtc.day++;														// increment days
	
					day = pgm_read_byte(&month_day_table[rtc.month-1]);
					
					// Todos os anos que são divisíveis por 400 ou são divisíveis por 4 e não divisíveis por 100 são anos bissextos
					if (  ((rtc.year % 4 == 0) && (rtc.year % 100 != 0)) || (rtc.year % 400 == 0)  )	// checa se o ano é bissexto
						day++;
						
					if(rtc.day == day) {	// check days overflow
						rtc.day = 1;
						rtc.month++;												// increment months
						if(rtc.month == 13) {										// check months overflow
							rtc.month = 1;
							rtc.year++;												// increment years
						}
					}
				}
			}
		}
	}
}
