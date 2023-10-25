#ifndef __AT90S2343_H
#define __AT90S2343_H

#include "uc_libdefs.h"

// ########################################################################################################################################
// SPI

#define SPI_MODE_SLAVE				0
#define SPI_MODE_MASTER				1

#define SPI_SPEED_F_DIV_4			0
#define SPI_SPEED_F_DIV_16			_BV(SPR0)
#define SPI_SPEED_F_DIV_64			_BV(SPR1)
#define SPI_SPEED_F_DIV_128			_BV(SPR1) | _BV(SPR0)

#define SPI_CPHA_1			0
#define SPI_CPHA_2			(1<<2)
#define SPI_CLK_LOW_IDLE 	0
#define SPI_CLK_HIGH_IDLE	(1<<3)
#define SPI_MSB				0
#define SPI_LSB				(1<<5)

#define SPI_INTERRUPT_HANDLER			ISR	// Type of interrupt handler to use for uart interrupts.

// ########################################################################################################################################
// TIMER 0
//u16 __attribute__ ((progmem)) TimerPrescaleFactor[] = {0,1,8,64,256,1024};

// MASCARA DE INTERRUP��O - TIMSK
#define TIMER0_NO_INT 					0x00 		// Sem interrup��o
#define TIMER0_ENABLE_INT_OVERFLOW		0x02 		// TOIE0

// VETOR DE INTERRUP��O
#define TIMER0_VETOR_INT_OVERFLOW  		0			// Indece da fun��o para interrup��o de overflow no vetor de interrup��es
#define TIMER0_NUM_INTERRUPTS			1			// Quantidade de fun��es de tratamento de interrup��es no vetor de interrup��es

// PRESCALER - TCCR0	
#define TCCR0B TCCR0

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
#define TIMER0_OVF_vect 				TIMER0_OVF0_vect 

#endif
