#ifndef __ARCH_WORKTEMP_H
#define __ARCH_WORKTEMP_H


// ESTA ARQUITETURA UTILIZA O PROCESSADOR LPC175x

#include "lpc175x.h"

// ###################################################################################################################
// AJUSTE DE CLOCKS
// Das três opções de fonte de clock (OSC interno, RTC e OSC externo).
// 	Vamos trabalhar somente com o ocilador externo conectado ao PLL
//	As bibliotecas não estão prepardos para outros tipos de fontes de clock

#if defined(OSC_RC)
// Passo 1: Frequencia interna do RC interno
#define F_CPU 	4000000

// Passo 2: Determinar a frequência desejada para a CPU (entre 10MHz a 100MHz).
// 		Deve ser divisor inteiro de F_CPU
#define CCLK 	100000000

// Passo 3: Determinar a frequência desejada para os periféricos
// 		Deve ser divisor inteiro de CCLK por 1, 2, 4 ou 8
// 		Quanto menor o clock menor será o consumo e aquecimento
#define PCLK	25000000

// VAMOS CALCULAR MANUALMENTE OS VALORES DOS DIVISORES PARA OBTER AS FREQUENCIAS ACIMA
// Passo 4: Calcular o valor da saída do PLL0 (PLLCLK = FCCO) de acordo com CCLKDIV.
//		FCCO: Escolher um número inteiro que seja multiplo de CCLK
//			FCCO deve ficar entre 275000000	a 550000000
//		CCLKDIV: Valores entre 1 a 255. Divisor do PLL(FCCO) para o CCLK. Começar pelo valor baixo.
//		CCLKDIV = FCCO/CCLK
#define FCCO 300000000
#define CCLKDIV 3

// Passo 5: Definir o divisor PLL_N.
//		PLL_N deve ser inteiro e ficar entre 1 a 256.
//		Começe pelo menor número para gerar multiplicções do pll baixa e gerando menos jitters
#define PLL_N 2

// Passo 6: Definir o multiplicador  PLL_M através do FCCO. Calcule pela formula:
//		PLL_M = (FCCO*PLL_N)/(2*F_CPU)
//			PLL_M deve ser inteiro e ficar entre 6 a 512
//			Se não ficar dentro dos limite volte ao passo 5 e encontre outro FCCO ou PLL_N
#define	PLL_M 75
#endif


#if defined(OSC_XTAL)
// Passo 1: Escolher a frequencia do cristal (entre 1MHz a 25MHz).
// 		Se utilizar USB escolher valores de 12, 16 ou 24MHz para podemor gerar um clock exato de 48MHz para a USB
#define F_CPU 	10000000

// Passo 2: Determinar a frequência desejada para a CPU (entre 10MHz a 100MHz).
// 		Deve ser divisor inteiro de F_CPU
#define CCLK 	100000000

// Passo 3: Determinar a frequência desejada para os periféricos (entre 10MHz a 120MHz).
// 		Deve ser divisor inteiro de CCLK por 1, 2, 4 ou 8
// 		Quanto menor o clock menor será o consumo e aquecimento
#define PCLK	25000000

// VAMOS CALCULAR MANUALMENTE OS VALORES DOS DIVISORES PARA OBTER AS FREQUENCIAS ACIMA
// Passo 4: Calcular o valor da saída do PLL0 (PLLCLK = FCCO) de acordo com CCLKDIV.
//		FCCO: Escolher um número inteiro que seja multiplo de CCLK
//			FCCO deve ficar entre 275000000	a 550000000
//		CCLKDIV: Valores entre 1 a 255. Divisor do PLL(FCCO) para o CCLK. Começar pelo valor baixo.
//		CCLKDIV = FCCO/CCLK
#define FCCO 300000000
#define CCLKDIV 3
// Passo 5: Definir o divisor PLL_N.
//		PLL_N deve ser inteiro e ficar entre 1 a 256.
//		Começe pelo menor número para gerar multiplicções do pll baixa e gerando menos jitters
#define PLL_N 1

// Passo 6: Definir o multiplicador  PLL_M através do FCCO. Calcule pela formula:
//		PLL_M = (FCCO*PLL_N)/(2*F_CPU)
//			PLL_M deve ser inteiro e ficar entre 6 a 512
//			Se não ficar dentro dos limite volte ao passo 5 e encontre outro FCCO ou PLL_N
#define	PLL_M 15


// VAMOS CALCULAR MANUALMENTE OS VALORES DOS DIVISORES PARA OBTER A FREQUENCIA DO PLL1 para o USB
// Passo 7: Calcular o valor da saída do PLL1 (PLL1_OUT) de acordo com USBCLKDIV.
//		USBCLKDIV: Valores entre 1 a 31. Divisor do PLL1 (FCCO) para o USBCLK. Começar pelo valor baixo.
//		PLL1_OUT = USBCLK * USBCLKDIV
//		USBCLK é sempre 48Mhz, logo:
//		PLL1_OUT = 48 * USBCLKDIV
//			PLL1_OUT deve ficar entre entre 9.75MHz a 160MHz.
//				Se não ficar nesse limite encontre outro USBCLKDIV
#define PLL1_OUT 96000000
#define USBCLKDIV 2

// Passo 8: Definir o PLL1_M. Calcule pela formula:
//		PLL1_M = PLL1_OUT/F_CPU//
//			PLL1_M deve ficar entre 1 a 31//
//				Se não ficar dentro desse limite, volte ao passo 8 e encontre outro PLL1_OUT ou F_CPU
#define PLL1_M	8
// Passo 9: Definir o PLL1_P através do FCCO. Calcule pela formula://
//		PLL_P deve ser 1, 2, 4 ou 6. Começe pelo valor baixo//
//		FCCO = PLL_OUT * 2 * PLL_P//
//			FCCO deve ficar entre 156MHz a 320MHz.//
//				Se não ficar dentro desse limite, mude PLL_P ou volte ao passo 5 e encontre outro PLL_OUT ou F_CPU//
#define PLL1_P 	1
#endif

// PLL0
#define PLLCFG_MSEL		(PLL_M-1)
#define PLLCFG_PSEL		((PLL_N-1)<<16)

#if defined(OSC_XTAL)
// PLL1
#define PLL1CFG_MSEL   ((PLL1_M-1) << 0) 	// Multiplicador M do PLL
#if (PLL1_P == 1)
#define PLL1CFG_PSEL   0		// Divisor P da saída do PLL
#elif (PLL1_P == 2)
#define PLL1CFG_PSEL   (1ul<<5)		// Divisor P da saída do PLL
#elif (PLL1_P == 4)
#define PLL1CFG_PSEL   (2ul<<5)		// Divisor P da saída do PLL
#elif (PLL1_P == 8)
#define PLL1CFG_PSEL   (3ul<<5)		// Divisor P da saída do PLL
#endif
#endif

// ###################################################################################################################
// DEFINIÇÕES DE MACROS PARA ACESSO A PORTAS GPIO
//				GPIO0		GPIO1		GPIO2		GPIO3		GPIO04
// LPC175x		FAST		FAST		FAST		FAST		FAST

#ifdef FIO0PIN
#define GPIO0_PIN FIO0PIN
#define GPIO0_SET FIO0SET
#define GPIO0_DIR FIO0DIR
#define GPIO0_CLR FIO0CLR
#define GPIO0_MASK FIO0MASK
#define GPIO0 GPIO0_DIR
#endif

#ifdef FIO1PIN
#define GPIO1_PIN FIO1PIN
#define GPIO1_PIN0 FIO1PIN0
#define GPIO1_PIN1 FIO1PIN1
#define GPIO1_PIN2 FIO1PIN2
#define GPIO1_PIN3 FIO1PIN3
#define GPIO1_PIN4 FIO1PIN4
#define GPIO1_PIN5 FIO1PIN5
#define GPIO1_SET FIO1SET
#define GPIO1_SET0 FIO1SET0
#define GPIO1_SET1 FIO1SET1
#define GPIO1_SET2 FIO1SET2
#define GPIO1_SET3 FIO1SET3
#define GPIO1_SET4 FIO1SET4
#define GPIO1_SET5 FIO1SET5
#define GPIO1_DIR FIO1DIR
#define GPIO1_DIR0 FIO1DIR0
#define GPIO1_DIR1 FIO1DIR1
#define GPIO1_DIR2 FIO1DIR2
#define GPIO1_DIR3 FIO1DIR3
#define GPIO1_DIR4 FIO1DIR4
#define GPIO1_DIR5 FIO1DIR5
#define GPIO1_CLR FIO1CLR
#define GPIO1_CLR0 FIO1CLR0
#define GPIO1_CLR1 FIO1CLR1
#define GPIO1_CLR2 FIO1CLR2
#define GPIO1_CLR3 FIO1CLR3
#define GPIO1_CLR4 FIO1CLR4
#define GPIO1_CLR5 FIO1CLR5
#define GPIO1_MASK FIO1MASK
#define GPIO1 GPIO1_DIR
#endif

#ifdef FIO2PIN
#define GPIO2_PIN FIO2PIN
#define GPIO2_SET FIO2SET
#define GPIO2_DIR FIO2DIR
#define GPIO2_CLR FIO2CLR
#define GPIO2_MASK FIO2MASK
#define GPIO2 GPIO2_DIR
#endif

#ifdef FIO3PIN
#define GPIO3_PIN FIO3PIN
#define GPIO3_PIN0 FIO3PIN0
#define GPIO3_PIN1 FIO3PIN1
#define GPIO3_PIN2 FIO3PIN2
#define GPIO3_PIN3 FIO3PIN3
#define GPIO3_SET FIO3SET
#define GPIO3_SET0 FIO3SET0
#define GPIO3_SET1 FIO3SET1
#define GPIO3_SET2 FIO3SET2
#define GPIO3_SET3 FIO3SET3
#define GPIO3_DIR FIO3DIR
#define GPIO3_DIR0 FIO3DIR0
#define GPIO3_DIR1 FIO3DIR1
#define GPIO3_DIR2 FIO3DIR2
#define GPIO3_DIR3 FIO3DIR3
#define GPIO3_CLR FIO3CLR
#define GPIO3_CLR0 FIO3CLR0
#define GPIO3_CLR1 FIO3CLR1
#define GPIO3_CLR2 FIO3CLR2
#define GPIO3_CLR3 FIO3CLR3
#define GPIO3_MASK FIO3MASK
#define GPIO3 GPIO3_DIR
#endif

#ifdef FIO4PIN
#define GPIO4_PIN FIO4PIN
#define GPIO4_SET FIO4SET
#define GPIO4_DIR FIO4DIR
#define GPIO4_CLR FIO4CLR
#define GPIO4_MASK FIO4MASK
#define GPIO4 GPIO4_DIR
#endif

#ifdef FIO5PIN
#define GPIO5_PIN FIO5PIN
#define GPIO5_SET FIO5SET
#define GPIO5_DIR FIO5DIR
#define GPIO5_CLR FIO5CLR
#define GPIO5_MASK FIO5MASK
#define GPIO5 GPIO5_DIR
#endif

// ###################################################################################################################
// UARTs
// BAUNDRATES COM TAXA 0% DE ERRO PARA O 73728000
//  Valores padrões até 115200, 921600, 1152000, 1536000, e 2304000
// IOCON Consultar datasheet página 113
#define UART_BAUDRATE_MAX 1536000


// UART0
// ------------------------------------------------------------------------------
// UART0 - P0_2 como TXD0 e P0_3 como RXD0
#define uart0_ConfigPort() 				{ PINSEL0 |= 0x50;}
	// PINO DE ENTRADA DE CONTROLE DE FLUXO
	#define U0_CTS_DIR		GPIO1_DIR
	#define U0_CTS_PIN		GPIO1_PIN
	#define U0_CTS			BIT_1

	// PINO DE SAÍDA DE CONTROLE DE FLUXO
	#define U0_RTS_DIR		GPIO1_DIR
	#define U0_RTS_SET		GPIO1_SET
	#define U0_RTS_CLR		GPIO1_CLR
	#define U0_RTS			BIT_0


// UART1
// ------------------------------------------------------------------------------
// UART1 - P0_15 como TXD1 e P0_16 como RXD1
#define uart1_ConfigPort() 				{ PINSEL0 |= 0x40000000; PINSEL1 |= 0x1;}
	// PINO DE ENTRADA DE CONTROLE DE FLUXO
	#define U1_CTS_DIR		GPIO0_DIR
	#define U1_CTS_PIN		GPIO0_PIN
	#define U1_RST			BIT_17
	// PINO DE SAÍDA DE CONTROLE DE FLUXO
	#define U1_RTS_DIR		GPIO0_DIR
	#define U1_RTS_SET		GPIO0_SET
	#define U1_RTS_CLR		GPIO0_CLR
	#define U1_CTS			BIT_22

// UART2
// ------------------------------------------------------------------------------
// UART2 - P0_10 como TXD2 e P0_11 como RXD2
#define uart2_ConfigPort() 				{ PINSEL0 |= 0x500000;}

// UART3
// ------------------------------------------------------------------------------
// UART3 - P0_0 como TXD3 e P0_1 como RXD3
#define uart3_ConfigPort() 				{ PINSEL0 |= 0xA;}


// ###################################################################################################################
// I2C

// I2C2_SCL = P0_11 e I2C2_SDA = P0_10
#define i2c2_ConfigPort()	{PINSEL0 |= 0xA00000;}

// ###################################################################################################################
// SSP

// SSP0: CLK=P0_15, SSEL=P0_16 (usar como GPIO), MISO=P0_17 e MOSI=P0_18
//#define ssp0_ConfigPort() 	{ PINSEL0 |= 0x80000000; PINSEL1 |= 0x28;}

// SSP1: SSEL=P0_6 (usar como GPIO), CLK=P0_7, MISO=P0_8 e MOSI=P0_9
#define ssp1_ConfigPort() 			{ PINSEL0 |= 0xA8000; }

#define EE25XX_CS_DIR  				GPIO0_DIR
#define EE25XX_CS_CLR				GPIO0_CLR
#define EE25XX_CS_SET				GPIO0_SET
#define EE25XX_CS  					(1<<6)

// ###################################################################################################################
// TIMERS
// NÃO USAR ESSES TIMERS COM SISTEMA OPERACIOANAIS
#define SYSTIME_USE_TIMER 		0 	// Especifica qual timer o clock do systime vai usar.
									// 	0 se usar o timer0 ou 1 se usar usar o timer1
#define UIP_CLOCK_USE_TIMER 	1 	// Especifica qual timer o clock do uip vai usar
									// 	0 se usar o timer0 ou 1 se usar usar o timer1
#define SERVUS_USE_TIMER		2	// Especifica qual timer que o servus.c vai usar
#define US04_USE_TIMER			2	// Especifica qual timer que o hc-sr04.c vai usar


// ###################################################################################################################
// TIMER0
// LPC175x MATCH0 = P1_28 TODO - valores temporários
#define MATCH0_ConfigPort0()		{ PINSEL3 |= 0x3000000; }
// LPC175x MATCH1 = P1_29 TODO - valores temporários
#define MATCH0_ConfigPort1()		{ PINSEL3 |= 0xC000000; }

// ###################################################################################################################
// TIMER1
// LPC175x MATCH0 = P1_22 TODO - valores temporários
#define MATCH1_ConfigPort0() 		{ PINSEL3 |= 0x3000; }
// LPC175x MATCH1 = P1_25 TODO - valores temporários
#define MATCH1_ConfigPort1()		{ PINSEL3 |= 0xC0000; }
// lpc175x cap 0 timer1 = P1.18   TODO - valores temporários
#define t1cap0_ConfigPort()	 { PINSEL3 |= 0x30; }
// lpc175x cap 0 timer1 = P1.19   TODO - valores temporários
#define t1cap1_ConfigPort()	 { PINSEL3 |= 0xC0; }

// ###################################################################################################################
// CONFIGURAÇÕES DOS LEDS
#define SYSTIME_LED_DIR		GPIO0_DIR
#define SYSTIME_LED_OFF		GPIO0_CLR
#define SYSTIME_LED_ON		GPIO0_SET
#define SYSTIME_LED			BIT_11

#define ERROR_LED_DIR		GPIO0_DIR
#define ERROR_LED_OFF		GPIO0_CLR
#define ERROR_LED_ON		GPIO0_SET
#define ERROR_LED			BIT_18

// ###################################################################################################################
// ADC e DAC
#define ADC0_ConfigPort()		{ PINSEL1 |= 0x140000; PINSEL3 |= 0xF0000000;}
	// PINSEL1 = Ajustar P0_25 para AD0[2] e P0_26 para AD0[3]
	// PINSEL3 = Ajustar P1_31 para AD0[5] e P1_30 para AD0[4]
#define ADC0_DisablePulls()		{ PINMODE1 |= 0x280000; PINMODE3 |= 0xA0000000;}
	// PINMODE1 = P0_25 e P0_26 sem pulls
	// PINMODE3 = P1_31 e P1_30 sem pulls

// ###################################################################################################################
// PWM 1 p2_0 = pwm1_1
#define pwm1_ConfigPort() 		{ PINSEL4 |= 1; }
#define pwm1_EnableOutputs();   { PWM1PCR |= PWM1PCR_PWM_ENABLE1; }	// habilita saida pwm1_1
#define PWM1MRx 				PWM1MR1
#define PWM1LERx_ENABLE 		PWM1LER1_ENABLE

// ###########################################################################################################################################
// DEVIO
#define DEVIO_QTD_DEVICES 10

// ###################################################################################################################
// INTERRUPÇÕES EXTERNAS (valores temporários)
#define ENTI0_ConfigPort()		{ PINSEL4 |= 0x100000; } 	// pino P2_10 como EINT0


// ###########################################################################################################################################
// CONFIGURAÇÕES DO BUZZER
#define BUZZER_DIR			GPIO1_DIR
#define BUZZER_OFF			GPIO1_CLR
#define BUZZER_ON			GPIO1_SET
#define BUZZER				BIT_14

// ###########################################################################################################################################
// RS485
#define rs485_ENTX_DIR	GPIO0_DIR
#define rs485_ENTX_ON	GPIO0_SET
#define rs485_ENTX_OFF 	GPIO0_CLR
#define rs485_ENTX		BIT_22

#endif
