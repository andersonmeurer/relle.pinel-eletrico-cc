#ifndef __CONFIG_CPU_TIMER_H
#define __CONFIG_CPU_TIMER_H

#include "_config_cpu_.h"

// ###################################################################################################################
// SYSTIME usado em systick.c ou systime_irq.c

// Definir o tempo de cada tick de sistema
#define SYSTIME_BASE_TICK  SYSTIME_TICK_1ms
		//	SYSTIME_TICK_1ms 	// Cada tick � gerado a cada 1ms (1000us)
		//	SYSTIME_TICK_500us	// Cada tick � gerado a cada 0.5ms (500us)
		//	SYSTIME_TICK_250us	// Cada tick � gerado a cada 0.25ms (250us)

#define SYSTIME_USE_LED 	pdON 		// pdON habilita driver para piscar um led em tempo na interrup��o do timer
	// Definir o tempo em que o led ficar� ligado e desligado. Ideal � que a soma seja 1 segundo
	#define SYSTIME_LED_OFF_MS	900		// Tempo em ms que o led ficar� desligado
	#define SYSTIME_LED_ON_MS	100		// Tempo em ms que o led ficar� ligado


// ###################################################################################################################
// CONFIGURA��ES DO RTC
#define	RTC_USE_INT  			pdOFF // pdON habilita INT, sen�o coloque pdOFF

// ###################################################################################################################
// TIMER 0

// TIMER 0 CAPTURE
#define TIMER0_USE_CAPTURE0 	pdOFF
#define TIMER0_USE_CAPTURE1 	pdOFF

// TIMER 0 MATCH
//	Usar ou n�o o pino GPIO para o comportamento da compara��o
#define TIMER0_USE_MATCH0		pdOFF
#define TIMER0_USE_MATCH1		pdOFF
#define TIMER0_USE_MATCH2		pdOFF
#define TIMER0_USE_MATCH3		pdOFF

// ###################################################################################################################
// TIMER 1

// TIMER 1 CAPTURE
#define TIMER1_USE_CAPTURE0		pdOFF
#define TIMER1_USE_CAPTURE1		pdOFF

// TIMER 1 MATCH
//	Usar ou n�o o pino GPIO para o comportamento da compara��o
#define TIMER1_USE_MATCH0		pdON
#define TIMER1_USE_MATCH1		pdOFF
#define TIMER1_USE_MATCH2		pdOFF
#define TIMER1_USE_MATCH3		pdOFF

// ###################################################################################################################
// TIMER 2

// TIMER 2 CAPTURE
#define TIMER2_USE_CAPTURE0		pdOFF
#define TIMER2_USE_CAPTURE1		pdOFF

// TIMER 2 MATCH
//	Usar ou n�o o pino GPIO para o comportamento da compara��o
#define TIMER2_USE_MATCH0		pdOFF
#define TIMER2_USE_MATCH1		pdOFF
#define TIMER2_USE_MATCH2		pdOFF
#define TIMER2_USE_MATCH3		pdOFF

#endif
