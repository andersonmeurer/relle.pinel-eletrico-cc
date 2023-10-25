#ifndef __CONFIG_CPU_TIMER_H
#define __CONFIG_CPU_TIMER_H

#include "_config_cpu_.h"

#define SYSTIME_USE_TIMER 1 // 1 ou 3 contador deve ser de 16 bits

// ###################################################################################################################
// SYSTIME usado em systick.c ou systime_irq.c

// Definir o tempo de cada tick de sistema
#define SYSTIME_BASE_TICK  SYSTIME_TICK_1ms
		//	SYSTIME_TICK_1ms 	// Cada tick é gerado a cada 1ms (1000us)
		//	SYSTIME_TICK_500us	// Cada tick é gerado a cada 0.5ms (500us)
		//	SYSTIME_TICK_250us	// Cada tick é gerado a cada 0.25ms (250us)
#define SYSTIME_PRESCALER  6 	// Prescaler de 32. Use o muxtools para definir o prescaler

#define SYSTIME_USE_LED 	pdON 		// pdON habilita driver para piscar um led em tempo na interrupção do timer
	// Definir o tempo em que o led ficará ligado e desligado. Ideal é que a soma seja 1 segundo
	#define SYSTIME_LED_OFF_MS	900		// Tempo em ms que o led ficará desligado
	#define SYSTIME_LED_ON_MS	100		// Tempo em ms que o led ficará ligado


	#define SYSTIME_LED_DIR 	DDRB
	#define SYSTIME_LED_PORT 	PORTB
	#define SYSTIME_LED			PB1

//###################################################################################################################
//  CONFIGURAÇÕES RTC
#define RTC_USE_XTAL_EXTERNAL		pdON	// Caso a fonte de clock para o RTC seja do cristal externo

//###################################################################################################################
//  CONFIGURAÇÕES TIMER0
#define TIMER0_USE_COMPARE			pdOFF

//###################################################################################################################
//  CONFIGURAÇÕES TIMER1
#define TIMER1_USE_COMPARE			pdON	// Caso deseja utilizar comparação (OC1A, OC1B e OC1C)
#define TIMER1_USE_ICP 		 		pdOFF	// Caso deseja utilizar captura do valor do timer para o registrador ICP quando mudar o esttado do pino IC1
#define TIMER1_USE_PWM				pdOFF	// Caso deseja utilizar PWM do timer1
#define TIMER1_USE_PWM_OC1A_INV 	pdOFF	// Caso a saida do pino OC1A seja invertido no PWM do timer
#define TIMER1_USE_PWM_OC1B_INV 	pdOFF	// Caso a saida do pino OC1B seja invertido no PWM do timer
#define TIMER1_USE_PWM_OC1C_INV		pdOFF	// Caso a saida do pino OC1C seja invertido no PWM do timer (caso tenha suporte)

//###################################################################################################################
//  CONFIGURAÇÕES TIMER2
#define TIMER2_USE_COMPARE			pdOFF	//Caso deseja utilizar capture (ICP = Ic2) do timer2

//###################################################################################################################
//  CONFIGURAÇÕES TIMER3
#define TIMER3_USE_COMPARE			pdOFF	// Caso deseja utilizar comparação (OC3A, OC3B e OC3C) do timer
#define TIMER3_USE_ICP				pdOFF	// Caso deseja utilizar captura do valor do timer para o registrador ICP quando mudar o esttado do pino IC3
#define TIMER3_USE_PWM				pdOFF	// Caso deseja utilizar PWM do timer3
#define TIMER3_USE_PWM_OC1A_INV 	pdOFF	// Caso a saida do pino OC3A seja invertido no PWM do timer
#define TIMER3_USE_PWM_OC1B_INV 	pdOFF	// Caso a saida do pino OC3B seja invertido no PWM do timer
#define TIMER3_USE_PWM_OC1C_INV 	pdOFF	// Caso a saida do pino OC3C seja invertido no PWM do timer


#endif
