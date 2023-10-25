#ifndef __ARCH_LPC22XX_H
#define __ARCH_LPC22XX_H

#include "lpc22xx.h"

// ###################################################################################################################
// AJUSTE DE CLOCKS
// FREQUENCIA DESEJADA DO PROCESSADOR (CCLK = F_CPU * PLL_M)

// DEFINIÇÕES PARA AJUSTES
#define F_CPU 		14745600							// Freqüência de entrada (10MHz a 25MHz).
#define PLL_M 		4	        						// Multiplicador M do PLL (1 a 32, na prática entre 1 a 6)
#define PLL_P 		2	        						// Divisor P do PLL para ajustar corretamete o Fcco (1, 2, 4 ou 8)
#define PBSD       	PBSD_1_1_CCLK						// Fator para o ajuste da freqüência dos dispositivos internos do ARM.
															// PBSD_1_4_CCLK = 4 = Pclk roda a 1/4 da velocidade do CCLK
															// PBSD_1_2_CCLK = 2 = Pclk roda a 1/2 da velocidade do CCLK
															// PBSD_1_1_CCLK = 1 = Pclk roda a mesma velocidade do CCLK

// FÓRMULAS PARA AJUSTES DO CLOCK
#define CCLK (F_CPU * PLL_M) 							// Freqüência da CPU
#define PCLK (CCLK / PBSD) 								// Freqüência do barramemto dos dispositivos internos do ARM
#define FCCO (F_CPU * PLL_M * 2 * PLL_P)				// Freqüência do oscilador controlado por corrente



#define PLLCON_PLLE   (1 << 0)          						// Habilita PLL
#define PLLCON_PLLC   (1 << 1)          						// Conecta o PLL a CPU

// ###################################################################################################################
// DEFINIÇÕES DE MACROS PARA ACESSO A PORTAS GPIO
//				GPIO0		GPIO1		GPIO2		GPIO3
// LPC2214		SLOW		SLOW		SLOW		SLOW
// LPC2294		SLOW		SLOW		SLOW		SLOW

#ifdef IO0PIN
#define GPIO0_PIN IO0PIN
#define GPIO0_SET IO0SET
#define GPIO0_DIR IO0DIR
#define GPIO0_CLR IO0CLR
#define GPIO0 GPIO0_DIR
#endif
	
#ifdef IO1PIN
#define GPIO1_PIN IO1PIN
#define GPIO1_SET IO1SET
#define GPIO1_DIR IO1DIR
#define GPIO1_CLR IO1CLR
#define GPIO1 GPIO1_DIR
#endif
	
#ifdef IO2PIN
#define GPIO2_PIN IO2PIN
#define GPIO2_SET IO2SET
#define GPIO2_DIR IO2DIR
#define GPIO2_CLR IO2CLR
#define GPIO2 GPIO2_DIR
#endif

#ifdef IO3PIN
#define GPIO3_PIN IO3PIN
#define GPIO3_SET IO3SET
#define GPIO3_DIR IO3DIR
#define GPIO3_CLR IO3CLR
#define GPIO3 GPIO3_DIR
#endif

// ###################################################################################################################
// PINOS DE INTERRUPÇÕES EXTERNAS

// LPC2294 EINT0 P0_16
#define EINT0_PINSEL	 	PINSEL1			// Porta de seleção dos pinos do LPC
#define EINT0_PINMASK      	(0x00000003)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT0_PINS       	(0x00000001)	// Bits de selecção do registrador PINSEL para configurar modo EINT
	
// LPC2294 EINT1 P0_3
#define EINT1_PINSEL	 	PINSEL0			// Porta de seleção dos pinos do LPC
#define EINT1_PINMASK      	(0x000000C0)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT1_PINS       	(0x000000C0)	// Bits de selecção do registrador PINSEL para configurar modo EINT
	
// LPC2294 EINT2 P0_15
#define EINT2_PINSEL	 	PINSEL0			// Porta de seleção dos pinos do LPC
#define EINT2_PINMASK      	(0xC0000000)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT2_PINS       	(0x80000000)	// Bits de selecção do registrador PINSEL para configurar modo EINT
	
// LPC2294 EINT3 P0_20
#define EINT3_PINSEL	 	PINSEL0			// Porta de seleção dos pinos do LPC
#define EINT3_PINMASK      	(0x00000300)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT3_PINS       	(0x00000300)	// Bits de selecção do registrador PINSEL para configurar modo EINT

// ###################################################################################################################
// UARTS

// UART0
// 		LPC2294 TXD1=P0_0 e RXD0=P0_1
#define U0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define U0_PINMASK      (0x0000000F)	// Mascara para acessar os pinos para UART no PINSEL   
#define U0_PINS       	(0x00000005)  	// Bits de selecção do registrador PINSEL para configurar modo UART

// UART1
// 		LPC2294 TXD1=P0_8 e RXD1=P0_9
#define U1_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define U1_PINMASK      (0x000F0000)	// Mascara para acessar os pinos para UART no PINSEL   
#define U1_PINS       	(0x00050000)	// Bits de selecção do registrador PINSEL para configurar modo UART


// ###################################################################################################################
// I2C

// LPC2294 SCL0=P0_2 e SDA0=P0_3
#define I2C0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define I2C0_PINMASK    (0x000000F0)	// Mascara para acessar os pinos para I2C0 no PINSEL   
#define I2C0_PINS     	(0x00000050)	// Bits de selecção do registrador PINSEL para configurar modo I2C



// ###################################################################################################################
// SPI

// SCK0=P0_4 MISO0=P0_5 MOSI0=P0_6
#define SPI0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define SPI0_PINMASK    (0x0000ff00)	// Mascara para acessar os pinos para SPI0 no PINSEL   
#define SPI0_PINS     	(0x00001500)	// Bits de selecção do registrador PINSEL para configurar modo SPI

// SCK1=P0_17 MISO1=P0_18 MOSI1=P0_19
#define SPI1_PINSEL		PINSEL1			// Porta de seleção dos pinos do LPC
#define SPI1_PINMASK    (0x000003fc)	// Mascara para acessar os pinos para SPP1 no PINSEL   
#define SPI1_PINS     	(0x000000a8)	// Bits de selecção do registrador PINSEL para configurar modo SPP1


// ###################################################################################################################
// ADC

// ---------------------------------------------------------------------------------------
// ADC0 AD0_0=P0_27 	AD0_1=P0_28 	AD0_2=P0_29 	AD0_3=P0_30
#define AD0_PINSEL		PINSEL1			// Porta de seleção dos pinos do LPC
#define AD0_PINMASK    (0x3FC00000)	// Mascara para acessar os pinos para ADC no PINSEL   
#define AD0_PINS       (0x15400000)  	// Bits de selecção do registrador PINSEL para configurar modo ADC

// ###################################################################################################################
// TIMER0

// LPC2294 CAP0_0 = P0_2
#define T0_CAP0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define T0_CAP0_PINMASK     (0x00000030)	// Mascara para acessar o pino CAP no PINSEL
#define T0_CAP0_PIN       	(0x00000020)  	// Bits de selecção do registrador PINSEL (p0.2) para configurar o modo CAP0

// LPC2294 CAP0_1 = P0_4
#define T0_CAP1_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define T0_CAP1_PINMASK     (0x00000300)	// Mascara para acessar o pino CAP no PINSEL
#define T0_CAP1_PIN       	(0x00000200)  	// Bits de selecção do registrador PINSEL para configurar o modo CAP0

// LPC2294 MATCH0 = P0_22
#define T0_MATCH0_PINSEL	PINSEL1		// Porta de seleção dos pinos do LPC
#define T0_MATCH0_PINMASK   (0x00003000)	// Mascara para acessar o pino CAP no PINSEL
#define T0_MATCH0_PIN      	(0x00003000)  	// Bits de selecção do registrador PINSEL (p0.22) para configurar o modo MAT0.0

// LPC2294 MATCH1 = P0_27
#define T0_MATCH1_PINSEL	PINSEL1			// Porta de seleção dos pinos do LPC
#define T0_MATCH1_PINMASK   (0x00C00000)	// Mascara para acessar o pino CAP no PINSEL
#define T0_MATCH1_PIN      	(0x00C00000)  	// Bits de selecção do registrador PINSEL (p0.27) para configurar o modo

// ###################################################################################################################
// TIMER1

// LPC2294 CAP1_0 = P0_10
#define T1_CAP0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define T1_CAP0_PINMASK     (0x00300000)	// Mascara para acessar o pino CAP no PINSEL
#define T1_CAP0_PIN       	(0x00200000)  	// Bits de selecção do registrador PINSEL para configurar o modo CAP0

// LPC2294 CAP1_1 = P0_11
#define T1_CAP1_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define T1_CAP1_PINMASK     (0x00C00000)	// Mascara para acessar o pino CAP no PINSEL
#define T1_CAP1_PIN       	(0x00800000)  	// Bits de selecção do registrador PINSEL para configurar o modo CAP0

// LPC2294 MATCH0 = P0_12
#define T1_MATCH0_PINSEL	PINSEL0		// Porta de seleção dos pinos do LPC
#define T1_MATCH0_PINMASK   (0x03000000)	// Mascara para acessar o pino CAP no PINSEL
#define T1_MATCH0_PIN      	(0x02000000)  	// Bits de selecção do registrador PINSEL (p0.22) para configurar o modo MAT0.0

// LPC2294 MATCH1 = P0_13
#define T1_MATCH1_PINSEL	PINSEL0			// Porta de seleção dos pinos do LPC
#define T1_MATCH1_PINMASK   (0x0C000000)	// Mascara para acessar o pino CAP no PINSEL
#define T1_MATCH1_PIN      	(0x08000000)  	// Bits de selecção do registrador PINSEL (p0.27) para configurar o modo

// ###################################################################################################################
// CONFIGURAÇÕES DOS LEDS

// ###################################################################################################################
// CONFIGURAÇÕES DOS LEDS

#define LED1_DIR			GPIO0_DIR
#define LED1_OFF			GPIO0_CLR
#define LED1_ON				GPIO0_SET
#define LED1				BIT_16

#define LED2_DIR			GPIO0_DIR
#define LED2_OFF			GPIO0_CLR
#define LED2_ON				GPIO0_SET
#define LED2				BIT_17

#define LED3_DIR			GPIO0_DIR
#define LED3_OFF			GPIO0_CLR
#define LED3_ON				GPIO0_SET
#define LED3				BIT_18

#define led1_Init()			LED1_DIR |= LED1
#define led1_On()			LED1_ON = LED1
#define led1_Off()			LED1_OFF = LED1

#define led2_Init()			LED2_DIR |= LED2
#define led2_On()			LED2_ON = LED2
#define led2_Off()			LED2_OFF = LED2

#define led3_Init()			LED3_DIR |= LED3
#define led3_On()			LED3_ON = LED3
#define led3_Off()			LED3_OFF = LED3

#define MISC_LED_DIR		LED1_DIR
#define MISC_LED_OFF		LED1_CLR
#define MISC_LED_ON			LED1_SET
#define MISC_LED			LED1

#define USB_LED_DEVCNT1_DIR	LED2_DIR
#define USB_LED_DEVCNT1_OFF	LED2_CLR
#define USB_LED_DEVCNT1_ON	LED2_ON
#define USB_LED_DEVCNT1		LED2

#define SYSTIME_LED_DIR		LED3_DIR
#define SYSTIME_LED_OFF		LED3_OFF
#define SYSTIME_LED_ON		LED3_ON
#define SYSTIME_LED			LED3

#define ERROR_LED_DIR		LED1_DIR
#define ERROR_LED_OFF		LED1_OFF
#define ERROR_LED_ON		LED1_ON
#define ERROR_LED			LED1

#endif
