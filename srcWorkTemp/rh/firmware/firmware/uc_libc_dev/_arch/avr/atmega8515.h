#ifndef __ATMEGA8515_H
#define __ATMEGA8515_H

#include "uc_libdefs.h"

// ########################################################################################################################################
// SPI
#define SPI_MODE_SLAVE				0
#define SPI_MODE_MASTER				1

#define SPI_SPEED_F_DIV_4			0
#define SPI_SPEED_F_DIV_16			_BV(SPR0)
#define SPI_SPEED_F_DIV_64			_BV(SPR1)
#define SPI_SPEED_F_DIV_128			_BV(SPR1) | _BV(SPR0)
#define SPI_SPEED_F_DIV_2 			0x80 | SPI_SPEED_F_DIV_4
#define SPI_SPEED_F_DIV_8 			0x80 | SPI_SPEED_F_DIV_16
#define SPI_SPEED_F_DIV_32			0x80 | SPI_SPEED_F_DIV_64

#define SPI_CPHA_1			0
#define SPI_CPHA_2			(1<<2)
#define SPI_CLK_LOW_IDLE 	0
#define SPI_CLK_HIGH_IDLE	(1<<3)
#define SPI_MSB				0
#define SPI_LSB				(1<<5)

#define SPI_INTERRUPT_HANDLER	ISR	// Type of interrupt handler to use for uart interrupts.

// ########################################################################################################################################
// TIMER 0
//u16 __attribute__ ((progmem)) TimerPrescaleFactor[] = {0,1,8,64,256,1024};
	
// MASCARA DE INTERRUP��O - TIMSK
#define TIMER0_NO_INT 					0x00 		// Sem interrup��o
#define TIMER0_ENABLE_INT_OVERFLOW  	0x01 		// TOIE0
#define TIMER0_ENABLE_INT_COMPARE 		0x02 		// OCIE0

// VETOR DE INTERRUP��O
#define TIMER0_VETOR_INT_OVERFLOW  		0			// Indece da fun��o para interrup��o de overflow no vetor de interrup��es
#define TIMER0_VETOR_INT_COMPARE   		1			// Indece da fun��o para interrup��o de compara��o no vetor de interrup��es
#define TIMER0_NUM_INTERRUPTS			2			// Quantidade de fun��es de tratamento de interrup��es no vetor de interrup��es

// PRESCALER - TCCR0	
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

// ########################################################################################################################################
// TIMER 1

// MASCARA DE INTERRUP��O - TIMSK
#define TIMER1_NO_INT					0x00 	// Sem interrup��o
#define TIMER1_ENABLE_INT_OVERFLOW		0x80 	// TOIE1
#define TIMER1_ENABLE_INT_COMPARE_B		0x20 	// OCIE1B
#define TIMER1_ENABLE_INT_COMPARE_A		0x40 	// OCIE1A
#define TIMER1_ENABLE_INT_CAPTURE		0x08 	// TICIE1

// VETOR DE INTERRUP��O
#define TIMER1_VETOR_INT_OVERFLOW  		0		// Indece da fun��o para interrup��o de overflow no vetor de interrup��es
#define TIMER1_VETOR_INT_COMPARE_A 		1		// Indece da fun��o para interrup��o A de compara��o no vetor de interrup��es
#define TIMER1_VETOR_INT_COMPARE_B 		2		// Indece da fun��o para interrup��o B de compara��o no vetor de interrup��es
#define TIMER1_VETOR_INT_CAPTURE   		3		// Indece da fun��o para interrup��o de captura do pino externo no vetor de interrup��es
#define TIMER1_NUM_INTERRUPTS 			4		// Quantidade de fun��es de tratamento de interrup��es no vetor de interrup��es

#define TIMER1_OC1A_OFF					0x00	// Desconecta o pino OC1A/B da compara��o
#define TIMER1_OC1A_CHANGE				0x40	// Conecta o pino OC1A/B da compara��o modo de troca
#define TIMER1_OC1A_NORMAL				0x80	// Conecta o pino OC1A/B da compara��o modo n�o invertido
#define TIMER1_OC1A_INV					0xC0	// Conecta o pino OC1A/B da compara��o modo invertido
#define TIMER1_OC1A_MASK				0xC0	// Mascara para configura��o das portas OC1A
	
#define TIMER1_OC1B_OFF					0x00	// Desconecta o pino OC1A/B da compara��o
#define TIMER1_OC1B_CHANGE				0x10	// Conecta o pino OC1A/B da compara��o modo de troca
#define TIMER1_OC1B_NORMAL				0x20	// Conecta o pino OC1A/B da compara��o modo n�o invertido
#define TIMER1_OC1B_INV					0x30	// Conecta o pino OC1A/B da compara��o modo invertido
#define TIMER1_OC1B_MASK				0x30	// Mascara para configura��o das portas OC1B
	
#define TIMER1_OC1A_DDR					DDRD 	// Porta de configura��o do pino OC1A
#define TIMER1_OC1A_PIN					PD5		// Porta de saida do pino OC1A
#define TIMER1_OC1B_DDR 				DDRE	// Porta de configura��o do pino OC1B
#define TIMER1_OC1B_PIN  				PE2		// Porta de saida do pino OC1B	
#define TIMER1_OC1C_DDR  				DDRE	// Porta de configura��o do pino OC1C
#define TIMER1_OC1C_PIN 				PE0		// Porta de saida do pino OC1C

// PRESCALER - TCCR1B
#define TIMER1_PRESCALE_STOP 			0x00
#define TIMER1_PRESCALE_DIV1 			0x01
#define TIMER1_PRESCALE_DIV8			0x02
#define TIMER1_PRESCALE_DIV64			0x03
#define TIMER1_PRESCALE_DIV256			0x04
#define TIMER1_PRESCALE_DIV1024			0x05
#define TIMER1_PRESCALE_EXT_T1_FALL		0x06
#define TIMER1_PRESCALE_EXT_T1_RISE		0x07
#define TIMER1_PRESCALE_MASK 			0x07

// CONFIGURA��O DO PINO EXTERNO DE CAPTURA
#define TIMER1_ICP_NOISE_CANCELER_ON 	0x80
#define TIMER1_ICP_NOISE_CANCELER_OFF	0x00
#define TIMER1_ICP_FALLING_EDGE 		0x00	// Captura o valor do clock para o registrador ICP durante a borda de descida do pino IC
#define TIMER1_ICP_RISING_EDGE 			0x40	// Captura o valor do clock para o registrador ICP durante a borda de subida do pino IC
#define TIMER1_ICP_MASK 				0xC0

// BITS DE RESOLU��O DO TIMER PARA PWM
#define TIMER1_PWM_MODE_8BITS 			8
#define TIMER1_PWM_MODE_9BITS 			9
#define TIMER1_PWM_MODE_10BITS 			10

#define TIMER1_INTERRUPT_HANDLER		ISR

// ########################################################################################################################################
// UART 0
#define UCSR0A				UCSRA
#define UCSR0B				UCSRB
#define UBRR0L				UBRRL
#define UBRR0H				UBRRH
#define UDR0				UDR
#define RXC0				RXC

#define UART0_INTERRUPT_HANDLER		ISR // Type of interrupt handler to use for uart interrupts.
#define USART0_RX_vect				UART_RX_vect 
#define USART0_TX_vect				UART_TX_vect 



#endif
