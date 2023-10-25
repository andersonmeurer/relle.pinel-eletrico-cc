#ifndef __ARCH_LPC213X_H
#define __ARCH_LPC213X_H

#include "lpc213x.h"


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

#define PLLCFG_MSEL   ((PLL_M - 1) 	 << 0) 				// Multiplicador M do PLL
#define PLLCFG_PSEL   ((PLL_P - 1)&0x3 << 5) 			// Divisor P do PLL para ajustar corretamete o Fcco (1, 2, 4 ou 8)

// ###################################################################################################################
// DEFINIÇÕES DE MACROS PARA ACESSO A PORTAS GPIO
//				GPIO0		GPIO1		GPIO2		GPIO3		GPIO04
// LPC2214		SLOW		SLOW		SLOW		SLOW		Não tem
// LPC2294		SLOW		SLOW		SLOW		SLOW		Não tem

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

// EINT0 P0_16
#define EINT0_PINSEL	 	PINSEL1			// Porta de seleção dos pinos do LPC
#define EINT0_PINMASK      	(0x00000003)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT0_PINS       	(0x00000001)	// Bits de selecção do registrador PINSEL para configurar modo EINT
	
// EINT1 P0_3
#define EINT1_PINSEL	 	PINSEL0			// Porta de seleção dos pinos do LPC
#define EINT1_PINMASK      	(0x000000C0)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT1_PINS       	(0x000000C0)	// Bits de selecção do registrador PINSEL para configurar modo EINT
	
// EINT2 P0_15
#define EINT2_PINSEL	 	PINSEL0			// Porta de seleção dos pinos do LPC
#define EINT2_PINMASK      	(0xC0000000)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT2_PINS       	(0x80000000)	// Bits de selecção do registrador PINSEL para configurar modo EINT
	
// EINT3 P0_20
#define EINT3_PINSEL	 	PINSEL0			// Porta de seleção dos pinos do LPC
#define EINT3_PINMASK      	(0x00000300)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT3_PINS       	(0x00000300)	// Bits de selecção do registrador PINSEL para configurar modo EINT


// ###################################################################################################################
// UARTS

// UART0
#define U0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define U0_PINMASK      (0x0000000F)	// Mascara para acessar os pinos para UART no PINSEL   
#define U0_PINS       	(0x00000005)  	// Bits de selecção do registrador PINSEL para configurar modo UART

// UART1
#define U1_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define U1_PINMASK      (0x000F0000)	// Mascara para acessar os pinos para UART no PINSEL   
#define U1_PINS       	(0x00050000)	// Bits de selecção do registrador PINSEL para configurar modo UART


// ###################################################################################################################
// I2C

#define I2C0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define I2C0_PINMASK    (0x000000F0)	// Mascara para acessar os pinos para I2C0 no PINSEL   
#define I2C0_PINS     	(0x00000050)	// Bits de selecção do registrador PINSEL para configurar modo I2C


// ###################################################################################################################
// SPI

// SCK0=P0_4 MISO0=P0_5 MOSI0=P0_6
#define SPI0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define SPI0_PINMASK    (0x0000ff00)	// Mascara para acessar os pinos para SPI0 no PINSEL   
#define SPI0_PINS     	(0x00001500)	// Bits de selecção do registrador PINSEL para configurar modo SPI

// ###################################################################################################################
// SSP

// SSP0
// LPC2138 LPC2148
// SCK1=P0_17 MISO1=P0_18 MOSI1=P0_19
#define SSP0_PINSEL		PINSEL1			// Porta de seleção dos pinos do LPC
#define SSP0_PINMASK    (0x000003fc)	// Mascara para acessar os pinos para SSP no PINSEL   
#define SSP0_PINS     	(0x000000a8)	// Bits de selecção do registrador PINSEL para configurar modo SSP

// SSP1


// ###################################################################################################################
// TIMER0

// LPC2294 CAP0_0 = P0_2
#define T0_CAP0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define T0_CAP0_PINMASK     (0x00000030)	// Mascara para acessar o pino CAP no PINSEL
#define T0_CAP0_PINS       	(0x00000020)  	// Bits de selecção do registrador PINSEL para configurar o modo CAP0

// LPC2294 CAP0_1 = P0_4
#define T0_CAP1_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define T0_CAP1_PINMASK     (0x00000300)	// Mascara para acessar o pino CAP no PINSEL
#define T0_CAP1_PINS       	(0x00000200)  	// Bits de selecção do registrador PINSEL para configurar o modo CAP0


// ###################################################################################################################
// TIMER1

// LPC2294 CAP1_0 = P0_10
#define T1_CAP0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define T1_CAP0_PINMASK     (0x00300000)	// Mascara para acessar o pino CAP no PINSEL
#define T1_CAP0_PINS       	(0x00200000)  	// Bits de selecção do registrador PINSEL para configurar o modo CAP0

// LPC2294 CAP1_1 = P0_11
#define T1_CAP1_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define T1_CAP1_PINMASK     (0x00C00000)	// Mascara para acessar o pino CAP no PINSEL
#define T1_CAP1_PINS       	(0x00800000)  	// Bits de selecção do registrador PINSEL para configurar o modo CAP0

#endif
