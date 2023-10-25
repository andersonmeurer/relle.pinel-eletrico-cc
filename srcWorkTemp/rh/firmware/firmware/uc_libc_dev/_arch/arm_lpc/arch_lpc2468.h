#ifndef __ARCH_LPC2468_H
#define __ARCH_LPC2468_H

#include "lpc24xx.h"

// ###################################################################################################################
// AJUSTE DE CLOCKS
// Das três opções de fonte de clock (OSC interno, RTC e OSC externo).
// 	Vamos trabalhar somente com o ocilador externo conectado ao PLL
//	As bibliotecas não estão prepardos para outros tipos de fontes de clock
	
	
// DEFINIÇÕES PARA AJUSTES DE CLOCK DA CPU
// ---------------------------------------
	
//	DEFINIÇÕES	UART 0% ERRO ATÉ 3M6(testar)	OVERCLOCK UART 0% ERRO ATÉ 900K(testar)		MÁXIMA VELOCIDADE DA CPU
//	F_CPU 		7372800 ou 14745600				7372800										12000000
//	CCLK 		58982400						73728000									72000000
//	FCCO		294912000 						294912000									288000000
//	PLL_N 1								1											1
//	PLL_M									20
#define F_CPU 		7372800								// Freqüência de entrada (entre 1MHz a 24MHz).
#define CCLK 		73728000							// Freqüência desejada para a CPU (entre 10MHz a 72MHz). Deve ser multiplo de 2*F_CPU
#define FCCO		294912000							// Começar pelo FCCO MIM (275000000). Pode ser que mesmo os valores corretos o oscilador não funcione corretamente, o mais sensivel são os baudrate (UART e outros)
	
#define PLL_N	1									// Pré-divisor na entrada da etapa multiplicadora (entre 1 a 32). Começar pelo menor número para produzir uma multiplicação menor
#define	PLL_M 	20									//  Multiplicador M do PLL (6 a 512).
															// ESTE VALOR DEVE SER REDONDO. Calcule manualmemte pela formula: PLL_M = ((FCCO*PLL_N)/(2*F_CPU))
															// Se o valor der fracionario, arredonde o valor para cima e re-calcule FCCO pela fórmula: 
															// FCCO = (PLL_M arrendondado  *  2*F_CPU )/ PLL_N

#define OSC_RANGE SCS_OSCRANGE_1MHZ_20MHZ				// Define a faixa de freqüência do oscilador principal
															// SCS_OSCRANGE_1MHZ_20MHZ  - Define que o a freqüência de entrada do clock para o oscilador principal está na faixa de 1MHZ a 20MHZ
															// SCS_OSCRANGE_15MHZ_24MHZ - Define que o a freqüência de entrada do clock para o oscilador principal está na faixa de 15MHZ a 24MHZ
	

// DEFINIÇÕES PARA AJUSTES DE CLOCK DOS PERIFÉRICOS
// ---------------------------------------
#define PBSD PBSD_1_1_CCLK								// Fator para o ajuste da freqüência dos dispositivos (periféricos) internos do ARM
															//	Cada dispositivo pode selecionar de forma independete, mas aqui estamos tratando todos por iguais
																// PBSD_1_4_CCLK = 4 = Pclk roda a 1/4 da velocidade do CCLK
																// PBSD_1_2_CCLK = 2 = Pclk roda a 1/2 da velocidade do CCLK
																// PBSD_1_1_CCLK = 1 = Pclk roda a mesma velocidade do CCLK
#define PCLK (CCLK / PBSD) 								// Freqüência do barramemto dos dispositivos (periféricos) internos do ARM



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


// ###################################################################################################################
// SSP

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
