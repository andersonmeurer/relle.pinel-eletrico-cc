#ifndef __ATTINY85_H
#define __ATTINY85_H

#include "uc_libdefs.h"


// ########################################################################################################################################
// TIMER 0
// MASCARA DE INTERRUP��O - TIMSK
#define TIMER0_NO_INT 					0x00 		// Sem interrup��o
#define TIMER0_ENABLE_INT_OVERFLOW		0x02 		// TOIE0
//
// VETOR DE INTERRUP��O
#define TIMER0_VETOR_INT_OVERFLOW  		0			// Indece da fun��o para interrup��o de overflow no vetor de interrup��es
#define TIMER0_NUM_INTERRUPTS			1			// Quantidade de fun��es de tratamento de interrup��es no vetor de interrup��es

// PRESCALER - TCCR0
#define TCCR0	TCCR0B

#define TIMER0_PRESCALE_STOP			0x00
#define TIMER0_PRESCALE_DIV1			0x01
#define TIMER0_PRESCALE_DIV8			0x02
#define TIMER0_PRESCALE_DIV64			0x03
#define TIMER0_PRESCALE_DIV256			0x04
#define TIMER0_PRESCALE_DIV1024			0x05
#define TIMER0_PRESCALE_EXT_T0_FALL 	0x06
#define TIMER0_PRESCALE_EXT_T0_RISE		0x07
#define TIMER0_PRESCALE_MASK 			0x07

#define TIMER0_INTERRUPT_HANDLER		ISR
//#define TIMER0_OVF_vect 				TIMER0_OVF_vect

// ########################################################################################################################################
// TIMER 1
#define TCCR1B	TCCR1
#define TIMER1_PRESCALE_MASK 			0x0F

// MASCARA DE INTERRUP��O - TIMSK
#define TIMER1_NO_INT 					0x00 	// Sem interrup��o
#define TIMER1_ENABLE_INT_OVERFLOW 		0x04	// TOIE1
#define TIMER1_ENABLE_INT_COMPARE_A 	0x40	// OCIE1A
#define TIMER1_ENABLE_INT_COMPARE_B 	0x20	// OCIE1B

#define TIMER1_INTERRUPT_HANDLER		ISR		// Type of interrupt handler to use for timer interrupts
#endif
