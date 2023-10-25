#ifndef __ARCH_ARCH_KITARMVIDEO_H
#define __ARCH_ARCH_KITARMVIDEO_H

// ESTA ARQUITETURA UTILIZA O PROCESSADOR LPC2468
#include "lpc24xx.h"

// ###################################################################################################################
// AJUSTE DE CLOCKS
// Das três opções de fonte de clock (OSC interno, RTC e OSC externo).
// 	Vamos trabalhar somente com o ocilador externo conectado ao PLL
//	As bibliotecas não estão prepardos para outros tipos de fontes de clock
	
	
// DEFINIÇÕES PARA AJUSTES DE CLOCK DA CPU
// ---------------------------------------
#if defined(KIT_ARMVIDEO_7MHZ)
#define F_CPU 		7372800		// Freqüência de entrada (entre 1MHz a 24MHz).
#define CCLK 		73728000	// Freqüência desejada para a CPU (entre 10MHz a 72MHz). Deve ser multiplo de 2*F_CPU
#define FCCO		294912000	// Começar pelo FCCO MIM (275000000). Pode ser que mesmo os valores corretos o oscilador não funcione corretamente, o mais sensivel são os baudrate (UART e outros)

// PRÉ-DIVISOR NA ENTRADA DA ETAPA MULTIPLICADORA (ENTRE 1 A 32).
// Começar pelo menor número para produzir uma multiplicação menor
#define PLL_N	1

//  MULTIPLICADOR M DO PLL (6 A 512).
// ESTE VALOR DEVE SER REDONDO. Calcule manualmemte pela formula: PLL_M = ((FCCO*PLL_N)/(2*F_CPU))
// Se o valor der fracionario, arredonde o valor para cima e re-calcule FCCO pela fórmula:
// FCCO = (PLL_M arrendondado  *  2*F_CPU )/ PLL_N
#define	PLL_M 	20

// DEFINE A FAIXA DE FREQÜÊNCIA DO OSCILADOR PRINCIPAL
// SCS_OSCRANGE_1MHZ_20MHZ  - Define que o a freqüência de entrada do clock para o oscilador principal está na faixa de 1MHZ a 20MHZ
// SCS_OSCRANGE_15MHZ_24MHZ - Define que o a freqüência de entrada do clock para o oscilador principal está na faixa de 15MHZ a 24MHZ
#define OSC_RANGE SCS_OSCRANGE_1MHZ_20MHZ

// DEFINIÇÕES PARA AJUSTES DE CLOCK DOS PERIFÉRICOS
// Fator para o ajuste da freqüência dos dispositivos (periféricos) internos do ARM
// Cada dispositivo pode selecionar de forma independete, mas aqui estamos tratando todos por iguais
// 		PBSD_1_4_CCLK = 4 = Pclk roda a 1/4 da velocidade do CCLK
// 		PBSD_1_2_CCLK = 2 = Pclk roda a 1/2 da velocidade do CCLK
// 		PBSD_1_1_CCLK = 1 = Pclk roda a mesma velocidade do CCLK
#define PBSD PBSD_1_1_CCLK

// FREQÜÊNCIA DO BARRAMEMTO DOS DISPOSITIVOS (PERIFÉRICOS) INTERNOS DO ARM
#define PCLK (CCLK / PBSD)

#elif defined(KIT_ARMVIDEO_12MHZ)
#define F_CPU 		12000000	// Freqüência de entrada (entre 1MHz a 24MHz).
#define CCLK 		72000000	// Freqüência desejada para a CPU (entre 10MHz a 72MHz). Deve ser multiplo de 2*F_CPU
#define FCCO		288000000	// Começar pelo FCCO MIM (275000000). Pode ser que mesmo os valores corretos o oscilador não funcione corretamente, o mais sensivel são os baudrate (UART e outros)

// PRÉ-DIVISOR NA ENTRADA DA ETAPA MULTIPLICADORA (ENTRE 1 A 32).
// Começar pelo menor número para produzir uma multiplicação menor
#define PLL_N	3

// MULTIPLICADOR M DO PLL (6 A 512).
// 	ESTE VALOR DEVE SER REDONDO.
//	Calcule manualmemte pela formula: PLL_M = ((FCCO*PLL_N)/(2*F_CPU))
// 	Se o valor der fracionario, arredonde o valor para cima e re-calcule FCCO pela fórmula:
// 		FCCO = (PLL_M arrendondado  *  2*F_CPU )/ PLL_N
#define	PLL_M 	36

// DEFINE A FAIXA DE FREQÜÊNCIA DO OSCILADOR PRINCIPAL
// SCS_OSCRANGE_1MHZ_20MHZ  - Define que o a freqüência de entrada do clock para o oscilador principal está na faixa de 1MHZ a 20MHZ
// SCS_OSCRANGE_15MHZ_24MHZ - Define que o a freqüência de entrada do clock para o oscilador principal está na faixa de 15MHZ a 24MHZ
#define OSC_RANGE SCS_OSCRANGE_1MHZ_20MHZ

// DEFINIÇÕES PARA AJUSTES DE CLOCK DOS PERIFÉRICOS
// Fator para o ajuste da freqüência dos dispositivos (periféricos) internos do ARM
// Cada dispositivo pode selecionar de forma independete, mas aqui estamos tratando todos por iguais
// 		PBSD_1_4_CCLK = 4 = Pclk roda a 1/4 da velocidade do CCLK
// 		PBSD_1_2_CCLK = 2 = Pclk roda a 1/2 da velocidade do CCLK
// 		PBSD_1_1_CCLK = 1 = Pclk roda a mesma velocidade do CCLK
#define PBSD PBSD_1_1_CCLK

// FREQÜÊNCIA DO BARRAMEMTO DOS DISPOSITIVOS (PERIFÉRICOS) INTERNOS DO ARM
#define PCLK (CCLK / PBSD)

#else
#error NÃO FOI DEFINIDO O TIPO DE ARQUITETURA NO MAKEFILE
#endif

#define PLLCFG_MSEL   ((PLL_M - 1) << 0) 					// Multiplicador M do PLL
#define PLLCFG_NSEL   ((PLL_N - 1) << 16) 					// Pré-divisor N na entrada multiplicadora do PLL

// ###################################################################################################################
// DEFINIÇÕES DE MACROS PARA ACESSO A PORTAS GPIO
//				GPIO0		GPIO1		GPIO2		GPIO3		GPIO04
// LPC2468		SLOW/FAST	SLOW/FAST	FAST		FAST		FAST
// ADOTARMOS TUDO COMO ACESSO RÁPIDO

#ifdef FIO0PIN
#define GPIO0_PIN FIO0PIN
#define GPIO0_SET FIO0SET
#define GPIO0_DIR FIO0DIR
#define GPIO0_CLR FIO0CLR
#define GPIO0 GPIO0_DIR
#endif
	
#ifdef FIO1PIN
#define GPIO1_PIN FIO1PIN
#define GPIO1_SET FIO1SET
#define GPIO1_DIR FIO1DIR
#define GPIO1_CLR FIO1CLR
#define GPIO1 GPIO1_DIR
#endif
	
#ifdef FIO2PIN
#define GPIO2_PIN FIO2PIN
#define GPIO2_SET FIO2SET
#define GPIO2_DIR FIO2DIR
#define GPIO2_CLR FIO2CLR
#define GPIO2 GPIO2_DIR
#endif
	
#ifdef FIO3PIN
#define GPIO3_PIN FIO3PIN
#define GPIO3_SET FIO3SET
#define GPIO3_DIR FIO3DIR
#define GPIO3_CLR FIO3CLR
#define GPIO3 GPIO3_DIR
#endif

#ifdef FIO4PIN
#define GPIO4_PIN FIO4PIN
#define GPIO4_SET FIO4SET
#define GPIO4_DIR FIO4DIR
#define GPIO4_CLR FIO4CLR
#define GPIO4 GPIO4_DIR
#endif

// ###################################################################################################################
// PINOS DE INTERRUPÇÕES EXTERNAS

// EINT0 P2_10
#define EINT0_PINSEL	 	PINSEL4			// Porta de seleção dos pinos do LPC
#define EINT0_PINMASK      	(0x00300000)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT0_PINS       	(0x00100000)	// Bits de selecção do registrador PINSEL para configurar modo EINT

// EINT1 P2_11
#define EINT1_PINSEL	 	PINSEL4			// Porta de seleção dos pinos do LPC
#define EINT1_PINMASK      	(0x00C00000)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT1_PINS       	(0x00400000)	// Bits de selecção do registrador PINSEL para configurar modo EINT

// EINT2 P2_12
#define EINT2_PINSEL	 	PINSEL4			// Porta de seleção dos pinos do LPC
#define EINT2_PINMASK      	(0x03000000)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT2_PINS       	(0x01000000)	// Bits de selecção do registrador PINSEL para configurar modo EINT

// EINT3 P2_13
#define EINT3_PINSEL	 	PINSEL4			// Porta de seleção dos pinos do LPC
#define EINT3_PINMASK      	(0x0C000000)	// Mascara para acessar os pinos para EINT no PINSEL   
#define EINT3_PINS       	(0x04000000)	// Bits de selecção do registrador PINSEL para configurar modo EINT
		
// ###################################################################################################################
// UARTS
// BAUNDRATES COM TAXA 0% DE ERRO PARA O 73728000
//  Valores padrões até 115200, 921600, 1152000, 1536000, e 2304000

#define UART_BAUDRATE_MAX 1536000

// UART0
// 		LPC2468 TXD0=P0_2 e RXD0=P0_3
#define U0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define U0_PINMASK      (0x000000F0)	// Mascara para acessar os pinos para UART no PINSEL   
#define U0_PINS       	(0x00000050)	// Bits de selecção do registrador PINSEL para configurar modo UART		

// UART1 
// 		LPC2468 TXD1=P0_15 e RXD1=P0_16
#define U1_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define U1_PINMASK      (0xC0000000)	// Mascara para acessar os pinos para UART no PINSEL   
#define U1_PINS       	(0x40000000)
#define U1_PINSEL1		PINSEL1			// Porta de seleção dos pinos do LPC
#define U1_PINMASK1     (0x00000003)	// Mascara para acessar os pinos para UART no PINSEL   
#define U1_PINS1     	(0x00000001)	// Bits de selecção do registrador PINSEL para configurar modo UART

// UART2
// 		LPC2468 TXD2=P0_10 e RXD2=P0_11
#define U2_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define U2_PINMASK      (0x00F00000)	// Mascara para acessar os pinos para UART no PINSEL   
#define U2_PINS       	(0x00500000)	// Bits de selecção do registrador PINSEL para configurar modo UART

// UART3
// 		LPC2468 TXD3=P0_0 e RXD3=P0_1
#define U3_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define U3_PINMASK      (0x0000000F)	// Mascara para acessar os pinos para UART no PINSEL   
#define U3_PINS       	(0x0000000A)	// Bits de selecção do registrador PINSEL para configurar modo UART

// PINO DE ENTRADA DE CONTROLE DE FLUXO
#define U3_CTS_DIR		GPIO0_DIR
#define U3_CTS_PIN		GPIO0_PIN
#define U3_CTS			(1<<4)
// PINO DE SAÍDA DE CONTROLE DE FLUXO
#define U3_RTS_DIR		GPIO0_DIR
#define U3_RTS_SET		GPIO0_SET
#define U3_RTS_CLR		GPIO0_CLR
#define U3_RTS			(1<<5)

// ###################################################################################################################
// I2C

// SCL=P0_28 e SDA=P0_27
#define I2C0_PINSEL		PINSEL1			// Porta de seleção dos pinos do LPC
#define I2C0_PINMASK    (0x03C00000)	// Mascara para acessar os pinos para I2C0 no PINSEL   
#define I2C0_PINS     	(0x01400000)	// Bits de selecção do registrador PINSEL para configurar modo I2C


// ###################################################################################################################
// SPI

// SCK0=P0_15 MISO0=P0_17 MOSI0=P0_18
#define SPI0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define SPI0_PINMASK    (0xC0000000)	// Mascara para acessar os pinos para SPI0 no PINSEL   
#define SPI0_PINS     	(0xC0000000)	// Bits de selecção do registrador PINSEL para configurar modo SPI
#define SPI0_PINSEL1	PINSEL1			// Porta de seleção dos pinos do LPC
#define SPI0_PINMASK1   (0x0000003f)	// Mascara para acessar os pinos para SPI0 no PINSEL   
#define SPI0_PINS1     	(0x0000003c)	// Bits de selecção do registrador PINSEL para configurar modo SPI

#define SPI1_PINSEL		PINSEL1			// Porta de seleção dos pinos do LPC
#define SPI1_PINMASK    (0x000003fc)	// Mascara para acessar os pinos para SPP1 no PINSEL   
#define SPI1_PINS     	(0x000000a8)	// Bits de selecção do registrador PINSEL para configurar modo SPP1

// SSP0
// SCK0=P0_15 MISO0=P0_17 MOSI0=P0_18
#define SSP0_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define SSP0_PINMASK    (0xC0000000)	// Mascara para acessar os pinos para SSP no PINSEL   
#define SSP0_PINS     	(0x80000000)	// Bits de selecção do registrador PINSEL para configurar modo SSP
#define SSP0_PINSEL1	PINSEL1			// Porta de seleção dos pinos do LPC
#define SSP0_PINMASK1   (0x0000003f)	// Mascara para acessar os pinos para SSP no PINSEL   
#define SSP0_PINS1     	(0x00000028)	// Bits de selecção do registrador PINSEL para configurar modo SSP

// SSP1
// SCK1=P0_7 MISO1=P0_8 MOSI1=P0_9
#define SSP1_PINSEL		PINSEL0			// Porta de seleção dos pinos do LPC
#define SSP1_PINMASK    (0x000ff000)	// Mascara para acessar os pinos para SPP1 no PINSEL   
#define SSP1_PINS     	(0x000a8000)	// Bits de selecção do registrador PINSEL para configurar modo SPI


// CHIP SELECTS DOS DIPOSITIVOS SPIs
#define ENC28J60_CS_DIR  			GPIO0_DIR
#define ENC28J60_CS_CLR				GPIO0_CLR
#define ENC28J60_CS_SET				GPIO0_SET
#define ENC28J60_CS					(1<<20)

#define MMCSD_CS_DIR  				GPIO0_DIR
#define MMCSD_CS_CLR				GPIO0_CLR
#define MMCSD_CS_SET				GPIO0_SET
#define MMCSD_CS  					(1<<19)

#define EE25XX_CS_DIR  				GPIO0_DIR
#define EE25XX_CS_CLR				GPIO0_CLR
#define EE25XX_CS_SET				GPIO0_SET
#define EE25XX_CS  					(1<<21)

#define M25PXX_CS_DIR  				GPIO0_DIR
#define M25PXX_CS_CLR				GPIO0_CLR
#define M25PXX_CS_SET				GPIO0_SET
#define M25PXX_CS  					(1<<21)

#define MMCSD_WR_PROTECT_DIR 		GPIO0_DIR
#define MMCSD_WR_PROTECT_PIN		GPIO0_PIN
#define MMCSD_WR_PROTECT  			(1<<15)
#define MMCSD_PRESENT_DIR 			GPIO0_DIR
#define MMCSD_PRESENT_PIN			GPIO0_PIN
#define MMCSD_PRESENT  				(1<<14)

// ###################################################################################################################
// ADC

// ADC0 AD0_0=P0_23 	AD0_1=P0_24 	AD0_2=P0_25 	AD0_3=P0_26
#define AD0_PINSEL		PINSEL1			// Porta de seleção dos pinos do LPC
#define AD0_PINMASK    (0x003FC000)	// Mascara para acessar os pinos para ADC no PINSEL   
#define AD0_PINS       (0x00154000)	// Bits de selecção do registrador PINSEL para configurar modo ADC


// ###################################################################################################################
// DAC

// AOUT = P0_26
#define DAC_PINSEL		PINSEL1			// Porta de seleção dos pinos do LPC
#define DAC_PINMASK    (0x00300000)		// Mascara para acessar os pinos para DAC no PINSEL   
#define DAC_PIN        (0x00200000)		// Bits de selecção do registrador PINSEL para configurar modo ADC

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
// LPC2468 MATCH0 = P1_28
#define T0_MATCH0_PINSEL	PINSEL3		// Porta de seleção dos pinos do LPC
#define T0_MATCH0_PINMASK   (0x03000000)	// Mascara para acessar o pino CAP no PINSEL
#define T0_MATCH0_PIN      	(0x03000000)  	// Bits de selecção do registrador PINSEL (p0.22) para configurar o modo MAT0.0

// LPC2468 MATCH1 = P1_29
#define T0_MATCH1_PINSEL	PINSEL3			// Porta de seleção dos pinos do LPC
#define T0_MATCH1_PINMASK   (0x0C000000)	// Mascara para acessar o pino CAP no PINSEL
#define T0_MATCH1_PIN      	(0x0C000000)  	// Bits de selecção do registrador PINSEL (p0.27) para configurar o modo

// ###################################################################################################################
// TIMER1
// LPC2468 MATCH0 = P1_22
#define T1_MATCH0_PINSEL	PINSEL3		// Porta de seleção dos pinos do LPC
#define T1_MATCH0_PINMASK   (0x00003000)	// Mascara para acessar o pino CAP no PINSEL
#define T1_MATCH0_PIN      	(0x00003000)  	// Bits de selecção do registrador PINSEL (p0.22) para configurar o modo MAT0.0

// LPC2468 MATCH1 = P1_25
#define T1_MATCH1_PINSEL	PINSEL3			// Porta de seleção dos pinos do LPC
#define T1_MATCH1_PINMASK   (0x000C0000)	// Mascara para acessar o pino CAP no PINSEL
#define T1_MATCH1_PIN      	(0x000C0000)  	// Bits de selecção do registrador PINSEL (p0.27) para configurar o modo

// ###################################################################################################################
// CONFIGURAÇÕES DO RTC
#define	RTC_USE_CLOCK_EXTERNAL  pdON

// ###################################################################################################################
// USB
#define USB1_PINSEL		PINSEL1			// Porta de seleção dos pinos do LPC
#define USB1_PINMASK    (0x3C000000)	// Mascara para acessar os pinos para USBD1 no PINSEL
#define USB1_PINS      	(0x14000000)	// Bits de selecção do registrador PINSEL para configurar modo USB

#define USB2_PINSEL		PINSEL1			// Porta de seleção dos pinos do LPC
#define USB2_PINMASK    (0xC0000000)	// Mascara para acessar os pinos para USBD2 no PINSEL
#define USB2_PINS      	(0x40000000)	// Bits de selecção do registrador PINSEL para configurar modo USB

#define USB_PORT_FUNC	0				// USB1 device e USB2 = HOST

#define HcRhPortStatus HcRhPortStatus2 // Definindo que será a porta 2 que vamos trabalhar no USBHostLite

// INTERVALO DO FRAME DE DADOS
#define USB_FI                     0x2EDF           /* 12000 bits per frame (-1)                               */
#define USB_DEFAULT_FMINTERVAL     ((((6 * (USB_FI - 210)) / 7) << 16) | USB_FI)

// ###################################################################################################################
// CONFIGURAÇÕES DOS LEDS

#define USB_LED_DEVCNT1_DIR	GPIO1_DIR
#define USB_LED_DEVCNT1_OFF	GPIO1_CLR
#define USB_LED_DEVCNT1_ON	GPIO1_SET
#define USB_LED_DEVCNT1		(1<<16)

#define SYS_TIME_LED_DIR	GPIO1_DIR
#define SYS_TIME_LED_OFF	GPIO1_CLR
#define SYS_TIME_LED_ON		GPIO1_SET
#define SYS_TIME_LED		(1<<24)

// ###########################################################################################################################################
// ########### CONFIGURAÇÕES DO BARRAMENTO EXTERNO
// TODO ainda não testado este modelo de configuração de memórias
//#### CONFIG  MEMÓRIAS STÁTICAS OU DISPOSITIVOS MAPEANDOS POR RAM
#define SRAM_MEMORY_EXTENDED_WAIT 0								// Valor de 0 a x3ff. Para dispositivo lentos que usam Tempo de espera extra nos acessos ao barramento
#define SRAM_BANK0_USE SRAM_BANK_USE_NONE						// Banco selecionado pelo #CS0 entre os endereços 0x8000 0000 a 0x80FF FFFF
#define SRAM_BANK1_USE SRAM_BANK_USE_NONE						// Banco selecionado pelo #CS1 entre os endereços 0x8100 0000 a 0x81FF FFFF
#define SRAM_BANK2_USE SRAM_BANK_USE_DISPO_GENERIC_8BITS		// Banco selecionado pelo #CS2 entre os endereços 0x8200 0000 a 0x82FF FFFF
#define SRAM_BANK3_USE SRAM_BANK_USE_NONE						// Banco selecionado pelo #CS3 entre os endereços 0x8300 0000 a 0x83FF FFFF

//	#### CONFIG MEMÓRIAS SDRAMS. SOMENTE ESTAMOS TRABALHANDO COM O BANCO 0,
//		PORÉM SE FAZER PARA OUTROS BANCOS VAMOS USAR O MESMO VALOR PARA TODOS
// 	os tipos estão em arch.h
#define	SDRAM_BANK0_USE MT48LC4M32B2_7_PC133			// Banco selecionado pelo #DYCS0 entre os endereços 0xA000 0000 a 0xAFFF FFFF

#endif
