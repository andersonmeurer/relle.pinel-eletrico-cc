/*
	REELEMBRANDO QUE EXISTEM Os delays NO SYS_TIME QUE UTILIZA TICK DE SISTEMA
*/


#ifndef __DELAY_H
#define __DELAY_H

#if defined(FREE_RTOS)
#error Não se utiliza esta biblioteca com freeRTOS, utilize a include systime.h
#endif

#if defined(arm7tdmi)
#define INS_DIV 4 // 4 instruções no loop em assembler
#endif

#if defined(cortexm3)
#define INS_DIV 3 // 3 instruções no loop em assembler
#endif

//#define TUNE_FAC 0.96154
#define TUNE_FAC 1

#include "_config_cpu_.h"

#define delay_us(delayval) delay( (u32)( (CCLK) / (1000000UL/delayval) /INS_DIV * TUNE_FAC) )
#define delay_ms(delayval) delay( (u32)( (CCLK) / (1000000UL/delayval) /INS_DIV * TUNE_FAC)*1000UL )

void delay(u32 delayval);

#endif
