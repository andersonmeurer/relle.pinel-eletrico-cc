#ifndef __ARCH_ARCH_KITLPC1315_MOD1_H
#define __ARCH_ARCH_KITLPC1315_MOD1_H


// ESTA ARQUITETURA UTILIZA O PROCESSADOR LPC1315

#include "lpc1315.h"

/*
// ###################################################################################################################
// AJUSTE DE CLOCKS
// Das três opções de fonte de clock (OSC interno, RTC e OSC externo).
// 	Vamos trabalhar somente com o ocilador externo conectado ao PLL
//	As bibliotecas não estão prepardos para outros tipos de fontes de clock
	
// Passo 1: Escolher a frequencia do cristal (entre 1MHz a 25MHz).
// 		Se utilizar USB escolher valores de 12, 16 ou 24MHz para podemor gerar um clock exato de 48MHz para a USB
#define F_CPU 		12000000
// Passo 2: Determinar a frequência desejada para a CPU (entre 10MHz a 120MHz).
//		Levar em consideração da velocidade das memórias
// 		Deve ser divisor inteiro de F_CPU
#define CCLK 		120000000
// Passo 3: Determinar a frequência desejada para os periféricos (entre 10MHz a 120MHz).
// 		Deve ser divisor inteiro de F_CPU
// 		Quanto menor o clock menor será o consumo e aquecimento
#define PCLK		60000000

// VAMOS CALCULAR MANUALMENTE OS VALORES DOS DIVISORES PARA OBTER AS FREQUENCIAS ACIMA
// Passo 4: Calcular o valor da saída do PLL0 (PLL_OUT) de acordo com CCLKDIV. Utilize a fórmula:
//		CCLKDIV: Valores entre 1 a 31. Divisor do PLL(FCCO) para o CCLK. Começar pelo valor baixo.
//		PLL_OUT = CCLK * CCLKDIV
//			PLL_OUT deve ficar entre entre 9.75MHz a 160MHz.
//				Se não ficar nesse limite encontre outro CCLKDIV ou CCLK
#define PLL_OUT 120000000
#define CCLKDIV 1
// Passo 5: Definir o PLL_M. Calcule pela formula:
//		PLL_M = PLL_OUT/F_CPU
//			PLL_M deve ficar entre 1 a 31
//				Se não ficar dentro desse limite, volte ao passo 5 e encontre outro PLL_OUT ou F_CPU
#define PLL_M	10
// Passo 6: Definir o PLL_P através do FCCO. Calcule pela formula:
//		PLL_P deve ser 1, 2, 4 ou 6. Começe pelo valor baixo
//		FCCO = PLL_OUT * 2 * PLL_P
//			FCCO deve ficar entre 156MHz a 320MHz.
//				Se não ficar dentro desse limite, mude PLL_P ou volte ao passo 5 e encontre outro PLL_OUT ou F_CPU
#define PLL_P 	1
// Passo 7: Definir valor para PCLKDIV, que é a divisão do clock do PLL(FCCO) para o PCLK. Utilize a fórmula:
//		PCLKDIV = (PLL_OUT/PCLK)
//		PCLKDIV deve ficar entre 1 a 31
//			Se o valor for fracionário arredonde o valor para cima e recalcule PCLK pelas fórmulas:
//				PCLKDIV = PCLKSEL_PCLKDIV_arrendondado
//				PLCK = (PLLOUT / PCLKDIV)
#define PCLKDIV 2

// VAMOS CALCULAR MANUALMENTE OS VALORES DOS DIVISORES PARA OBTER A FREQUENCIA DO PLL1 para o USB
// Passo 8: Calcular o valor da saída do PLL1 (PLL1_OUT) de acordo com USBCLKDIV. Utilize a fórmula:
//		USBCLKDIV: Valores entre 1 a 31. Divisor do PLL1 (FCCO) para o USBCLK. Começar pelo valor baixo.
//		PLL1_OUT = USBCLK * USBCLKDIV
//		USBCLK é sempre 48Mhz, logo:
//		PLL1_OUT = 48 * USBCLKDIV
//			PLL1_OUT deve ficar entre entre 9.75MHz a 160MHz.
//				Se não ficar nesse limite encontre outro USBCLKDIV
#define PLL1_OUT 96000000
#define USBCLKDIV 2
// Passo 9: Definir o PLL1_M. Calcule pela formula:
//		PLL1_M = PLL1_OUT/F_CPU
//			PLL1_M deve ficar entre 1 a 31
//				Se não ficar dentro desse limite, volte ao passo 8 e encontre outro PLL1_OUT ou F_CPU
#define PLL1_M	8
// Passo 10: Definir o PLL1_P através do FCCO. Calcule pela formula:
//		PLL_P deve ser 1, 2, 4 ou 6. Começe pelo valor baixo
//		FCCO = PLL_OUT * 2 * PLL_P
//			FCCO deve ficar entre 156MHz a 320MHz.
//				Se não ficar dentro desse limite, mude PLL_P ou volte ao passo 5 e encontre outro PLL_OUT ou F_CPU
#define PLL1_P 	1

#define PLLCFG_MSEL   ((PLL_M-1) << 0) 	// Multiplicador M do PLL
#if (PLL_P == 1)
#define PLLCFG_PSEL   0		// Divisor P da saída do PLL
#elif (PLL_P == 2)
#define PLLCFG_PSEL   (1ul<<5)		// Divisor P da saída do PLL
#elif (PLL_P == 4)
#define PLLCFG_PSEL   (2ul<<5)		// Divisor P da saída do PLL
#elif (PLL_P == 8)
#define PLLCFG_PSEL   (3ul<<5)		// Divisor P da saída do PLL
#endif

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

//// ###################################################################################################################
//// PINOS DE INTERRUPÇÕES EXTERNAS
//
//// EINT0 P2_10
//#define EINT0_PINSEL	 	PINSEL4			// Porta de seleção dos pinos do LPC
//#define EINT0_PINMASK      	(0x00300000)	// Mascara para acessar os pinos para EINT no PINSEL
//#define EINT0_PINS       	(0x00100000)	// Bits de selecção do registrador PINSEL para configurar modo EINT
//
//// EINT1 P2_11
//#define EINT1_PINSEL	 	PINSEL4			// Porta de seleção dos pinos do LPC
//#define EINT1_PINMASK      	(0x00C00000)	// Mascara para acessar os pinos para EINT no PINSEL
//#define EINT1_PINS       	(0x00400000)	// Bits de selecção do registrador PINSEL para configurar modo EINT
//
//// EINT2 P2_12
//#define EINT2_PINSEL	 	PINSEL4			// Porta de seleção dos pinos do LPC
//#define EINT2_PINMASK      	(0x03000000)	// Mascara para acessar os pinos para EINT no PINSEL
//#define EINT2_PINS       	(0x01000000)	// Bits de selecção do registrador PINSEL para configurar modo EINT
//
//// EINT3 P2_13
//#define EINT3_PINSEL	 	PINSEL4			// Porta de seleção dos pinos do LPC
//#define EINT3_PINMASK      	(0x0C000000)	// Mascara para acessar os pinos para EINT no PINSEL
//#define EINT3_PINS       	(0x04000000)	// Bits de selecção do registrador PINSEL para configurar modo EINT
//
// ###################################################################################################################
// UARTs
// BAUNDRATES COM TAXA 0% DE ERRO PARA O 73728000
//  Valores padrões até 115200, 921600, 1152000, 1536000, e 2304000
// IOCON Consultar datasheet página 113
#define UART_BAUDRATE_MAX 1536000


// UART0
// ------------------------------------------------------------------------------
// UART0 - P0_2 como TXD0 e P0_3 como RXD0 em pullup
#define uart0_ConfigPort() 				{ IOCON_P0_02 = 1; IOCON_P0_03 = 0x11;}
// UART0 HANDSHAKING MANUAL - P0_8 como CTS0 e P0_7 como RTS0
//#define uart0_ConfigHandshakingPort() 	{ IOCON_P0_08 = 0x0; IOCON_P0_07 = 0x0;}
	// PINO DE ENTRADA DE CONTROLE DE FLUXO
	#define U0_CTS_DIR		GPIO0_DIR
	#define U0_CTS_PIN		GPIO0_PIN
	#define U0_RST			BIT_8
	// PINO DE SAÍDA DE CONTROLE DE FLUXO
	#define U0_RTS_DIR		GPIO0_DIR
	#define U0_RTS_SET		GPIO0_SET
	#define U0_RTS_CLR		GPIO0_CLR
	#define U0_CTS			BIT_7


// UART1
// ------------------------------------------------------------------------------
// UART1 - P0_15 como TXD1 e P0_16 como RXD1 em pullup
#define uart1_ConfigPort() 				{ IOCON_P0_15 = 1; IOCON_P0_16 = 0x11;}
// UART1 HANDSHAKING MANUAL - P0_17 como CTS1 e P0_6 como RTS1
//#define uart1_ConfigHandshakingPort() 	{ IOCON_P0_17 = 0; IOCON_P0_06 = 0x0;}
	// PINO DE ENTRADA DE CONTROLE DE FLUXO
	#define U1_CTS_DIR		GPIO0_DIR
	#define U1_CTS_PIN		GPIO0_PIN
	#define U1_RST			BIT_17
	// PINO DE SAÍDA DE CONTROLE DE FLUXO
	#define U1_RTS_DIR		GPIO1_DIR
	#define U1_RTS_SET		GPIO1_SET
	#define U1_RTS_CLR		GPIO1_CLR
	#define U1_CTS			BIT_6

// UART2
// ------------------------------------------------------------------------------
// UART2 - P0_10 como TXD2 e P0_11 como RXD2 em pullup
#define uart2_ConfigPort() 				{ IOCON_P0_10 = 1; IOCON_P0_11 = 0x11;}
// UART2 HANDSHAKING MANUAL- sem suporte
//#define uart2_ConfigHandshakingPort() 	{ #error SEM SUPORTE PARA HANDSHAKING para UART2}

// UART3
// ------------------------------------------------------------------------------
// UART3 - P0_0 como TXD3 e P0_1 como RXD3 em pullup
#define uart3_ConfigPort() 				{ IOCON_P0_00 = 2; IOCON_P0_01 = 0x12;}
// UART3 HANDSHAKING MANUAL - P1_30 como CTS3 e P0_4 como RTS3
//#define uart3_ConfigHandshakingPort() 	{ IOCON_P1_30 = 0; IOCON_P0_04 = 0;}
	// PINO DE ENTRADA DE CONTROLE DE FLUXO
	#define U3_CTS_DIR		GPIO0_DIR
	#define U3_CTS_PIN		GPIO0_PIN
	#define U3_RST			BIT_4
	// PINO DE SAÍDA DE CONTROLE DE FLUXO
	#define U3_RTS_DIR		GPIO1_DIR
	#define U3_RTS_SET		GPIO1_SET
	#define U3_RTS_CLR		GPIO1_CLR
	#define U3_CTS			BIT_30


//// ###################################################################################################################
//// I2C
//
//// SCL=P0_28 e SDA=P0_27
//#define I2C0_PINSEL		PINSEL1			// Porta de seleção dos pinos do LPC
//#define I2C0_PINMASK    (0x03C00000)		// Mascara para acessar os pinos para I2C0 no PINSEL
//#define I2C0_PINS     	(0x01400000)	// Bits de selecção do registrador PINSEL para configurar modo I2C
//
//

// ###################################################################################################################
// SPI CHIP SELECTS
#define MMCSD_CS_DIR  		GPIO4_DIR
#define MMCSD_CS_CLR		GPIO4_CLR
#define MMCSD_CS_SET		GPIO4_SET
#define MMCSD_CS  			(1<<26)

#define ENC28J60_CS_DIR  	GPIO4_DIR
#define ENC28J60_CS_CLR		GPIO4_CLR
#define ENC28J60_CS_SET		GPIO4_SET
#define ENC28J60_CS			(1<<21)


// ###################################################################################################################
// SPP0: CLK = P2_22  	MISO = P2_26 	MOSI = P2_27
#define SSP0_PINSEL			PINSEL_P2	// Porta de seleção dos pinos do LPC
#define SSP0_CLK_NUMPIN		22			// Número do Pino do CLK
#define SSP0_MISO_NUMPIN	26			// Número do Pino do MISO
#define SSP0_MOSI_NUMPIN	27			// Número do Pino do MOSI
#define SSP0_NUNFUNC 		2			// Número da função dos pinos para SSP0

// ###################################################################################################################
// SPP1: CLK = P4_20 	MISO = P4_22 	MOSI = P4_23
#define SSP1_PINSEL			PINSEL_P4	// Porta de seleção dos pinos do LPC
#define SSP1_CLK_NUMPIN		20			// Número do Pino do CLK
#define SSP1_MISO_NUMPIN	22			// Número do Pino do MISO
#define SSP1_MOSI_NUMPIN	23			// Número do Pino do MOSI
#define SSP1_NUNFUNC 		3			// Número da função dos pinos para SSP1


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
// LPC1788 MATCH0 = P3_25 TODO - valores temporários
#define MATCH0_ConfigPort0()		{ IOCON_P3_25 = 3; }
// LPC1788 MATCH1 = P3_26 TODO - valores temporários
#define MATCH0_ConfigPort1()		{ IOCON_P3_26 = 3; }

// ###################################################################################################################
// TIMER1
// LPC1788 MATCH0 = P3_29 TODO - valores temporários
#define MATCH1_ConfigPort0() 		{ IOCON_P3_29 = 3; }
// LPC1788 MATCH1 = P3_30 TODO - valores temporários
#define MATCH1_ConfigPort1()		{ IOCON_P3_30 = 3; }

// ###################################################################################################################
// USB
#define USB1_PINSEL			PINSEL_P0		// Porta de seleção dos pinos do LPC para USB2
#define USB1_DP_NUMPIN		29				// Número do Pino do D+
#define USB1_DM_NUMPIN		30				// Número do Pino do D-
#define USB1_NUNFUNC 		1				// Número da função dos pinos para USB

#define USB2_PINSEL			PINSEL_P0		// Porta de seleção dos pinos do LPC para USB2
#define USB2_DP_NUMPIN		31				// Número do Pino do D+
#define USB2_NUNFUNC 		1				// Número da função dos pinos para USB

#define USB_PORT_FUNC		3				// USB1 = HOST e USB2 device
#define HcRhPortStatus 	HcRhPortStatus1 // Definindo que será a porta 1 que vamos trabalhar no USBHostLite

// INTERVALO DO FRAME DE DADOS
#define USB_FI                     0x2EDF           // 12000 bits per frame (-1)                              
#define USB_DEFAULT_FMINTERVAL     ((((6 * (USB_FI - 210)) / 7) << 16) | USB_FI)

*/

// ###################################################################################################################
// CONFIGURAÇÕES DOS LEDS

#define LED2_DIR			GPIO0_DIR
#define LED2_OFF			GPIO0_CLR
#define LED2_ON				GPIO0_SET
#define LED2				BIT_23

#define LED3_DIR			GPIO1_DIR
#define LED3_OFF			GPIO1_CLR
#define LED3_ON				GPIO1_SET
#define LED3				BIT_15

#define LED4_DIR			GPIO1_DIR
#define LED4_OFF			GPIO1_CLR
#define LED4_ON				GPIO1_SET
#define LED4				BIT_16

#define LED5_DIR			GPIO1_DIR
#define LED5_OFF			GPIO1_CLR
#define LED5_ON				GPIO1_SET
#define LED5				BIT_25

#define MISC_LED_DIR		LED1_DIR
#define MISC_LED_OFF		LED1_CLR
#define MISC_LED_ON			LED1_SET
#define MISC_LED			LED1

#define USB_LED_DEVCNT1_DIR	LED2_DIR
#define USB_LED_DEVCNT1_OFF	LED2_CLR
#define USB_LED_DEVCNT1_ON	LED2_ON
#define USB_LED_DEVCNT1		LED2

#define SYS_TIME_LED_DIR	LED3_DIR
#define SYS_TIME_LED_OFF	LED3_OFF
#define SYS_TIME_LED_ON		LED3_ON
#define SYS_TIME_LED		LED3
/*
// ###################################################################################################################
// CONFIGURAÇÕES TFT

#define TFT_R0_PINSEL		PINSEL_P2	// Endereço base da porta de seleção do pino
#define TFT_R0_NUMPIN		12	// p2_12
#define TFT_R0_NUNFUNC		5
#define TFT_R1_PINSEL		PINSEL_P2	// Endereço base da porta de seleção do pino
#define TFT_R1_NUMPIN		6 	// p2_6
#define TFT_R1_NUNFUNC		7
#define TFT_R2_PINSEL		PINSEL_P2	// Endereço base da porta de seleção do pino
#define TFT_R2_NUMPIN		7 	// p2_7
#define TFT_R2_NUNFUNC		7
#define TFT_R3_PINSEL		PINSEL_P2	// Endereço base da porta de seleção do pino
#define TFT_R3_NUMPIN		8 	// p2_8
#define TFT_R3_NUNFUNC		7
#define TFT_R4_PINSEL		PINSEL_P2	// Endereço base da porta de seleção do pino
#define TFT_R4_NUMPIN		9 	// p2_9
#define TFT_R4_NUNFUNC		7

#define TFT_G0_PINSEL		PINSEL_P1	// Endereço base da porta de seleção do pino
#define TFT_G0_NUMPIN		20 	// p1_20
#define TFT_G0_NUNFUNC		7
#define TFT_G1_PINSEL		PINSEL_P1	// Endereço base da porta de seleção do pino
#define TFT_G1_NUMPIN		21 	// p1_21
#define TFT_G1_NUNFUNC		7
#define TFT_G2_PINSEL		PINSEL_P1	// Endereço base da porta de seleção do pino
#define TFT_G2_NUMPIN		22 	// p1_22
#define TFT_G2_NUNFUNC		7
#define TFT_G3_PINSEL		PINSEL_P1	// Endereço base da porta de seleção do pino
#define TFT_G3_NUMPIN		23 	// p1_23
#define TFT_G3_NUNFUNC		7
#define TFT_G4_PINSEL		PINSEL_P1	// Endereço base da porta de seleção do pino
#define TFT_G4_NUMPIN		24 	// p1_24
#define TFT_G4_NUNFUNC		7
#define TFT_G5_PINSEL		PINSEL_P1	// Endereço base da porta de seleção do pino
#define TFT_G5_NUMPIN		25 	// p1_25
#define TFT_G5_NUNFUNC		7

#define TFT_B0_PINSEL		PINSEL_P2	// Endereço base da porta de seleção do pino
#define TFT_B0_NUMPIN		13 	// p2_13
#define TFT_B0_NUNFUNC		7
#define TFT_B1_PINSEL		PINSEL_P1	// Endereço base da porta de seleção do pino
#define TFT_B1_NUMPIN		26	// p1_26
#define TFT_B1_NUNFUNC		7
#define TFT_B2_PINSEL		PINSEL_P1	// Endereço base da porta de seleção do pino
#define TFT_B2_NUMPIN		27	// p1_27
#define TFT_B2_NUNFUNC		7
#define TFT_B3_PINSEL		PINSEL_P1	// Endereço base da porta de seleção do pino
#define TFT_B3_NUMPIN		28	// p1_28
#define TFT_B3_NUNFUNC		7
#define TFT_B4_PINSEL		PINSEL_P1	// Endereço base da porta de seleção do pino
#define TFT_B4_NUMPIN		29	// p1_29
#define TFT_B4_NUNFUNC		7

#define TFT_DCLK_PINSEL		PINSEL_P2	// Endereço base da porta de seleção do pino
#define TFT_DCLK_NUMPIN		2	// p2_2
#define TFT_DCLK_NUNFUNC	7
#define TFT_ENAB_PINSEL		PINSEL_P2	// Endereço base da porta de seleção do pino
#define TFT_ENAB_NUMPIN		4	// p2_29
#define TFT_ENAB_NUNFUNC	7

#define TFT_CONFIG 			0x2c
	// LcdEn = 0: Disable LCD controller
	// lcdbpp = 6: 16 bpp, 5:6:5 mode.
	// LcdTFT = 1: TFT panel
	// LcdDual = 0: single panel
	// BGR = 0: normal output
	// BEBO = 0: little endian byte order
	// BEPO = 0: little endian pix order
	// LcdPwr = 0: disable power

#define TFT_CLKDIV 7 // 1:8, CPU=120M
	// div7 para 120MHz

#define VIDEO_MEM_SIZE 			0x100000
#define VIDEO_MEM_BANCK0 		SDRAM_BANK0_BASE_ADDR	//1mega de video para cada pagina
#define VIDEO_MEM_BANCK1		SDRAM_BANK0_BASE_ADDR + VIDEO_MEM_SIZE	 // 0x100000
#define VIDEO_MEM_BANCK2		SDRAM_BANK0_BASE_ADDR + VIDEO_MEM_SIZE*2   // 0x200000
#define VIDEO_MEM_BANCK3		SDRAM_BANK0_BASE_ADDR + VIDEO_MEM_SIZE*3   // 0x300000

#define TFT_BACKLIGHT_DIR		GPIO2_DIR
#define TFT_BACKLIGHT_CLR		GPIO2_CLR
#define TFT_BACKLIGHT_SET		GPIO2_SET
#define TFT_BACKLIGHT_NUMPIN	(1<<0)

#define TOUCH_X0_DIR			GPIO0_DIR
#define TOUCH_X0_CLR			GPIO0_CLR
#define TOUCH_X0_SET			GPIO0_SET
#define TOUCH_X0_NUMPIN			BIT_25
#define TOUCH_Y0_DIR			GPIO0_DIR
#define TOUCH_Y0_CLR			GPIO0_CLR
#define TOUCH_Y0_SET			GPIO0_SET
#define TOUCH_Y0_NUMPIN			BIT_26
#define TOUCH_X1_DIR			GPIO0_DIR
#define TOUCH_X1_CLR			GPIO0_CLR
#define TOUCH_X1_SET			GPIO0_SET
#define TOUCH_X1_NUMPIN			BIT_27
#define TOUCH_Y1_DIR			GPIO0_DIR
#define TOUCH_Y1_CLR			GPIO0_CLR
#define TOUCH_Y1_SET			GPIO0_SET
#define TOUCH_Y1_NUMPIN			BIT_28

#define TOUCH_SAMPLES 			4 // define a quatidade de amostras por canal para validar uma leitura
#define TOUCH_USE_READ			TOUCH_USE_AD
	#define TOUCH_ADC_CLK		10000000 		// Velocidade de 10MHz para o clock do AD para leitura do touch
	#define TOUCH_X0_AD0		AD0_CHANNEL2	// Canal AD2 para o X0
	#define TOUCH_Y0_AD1		AD0_CHANNEL3	// Canal AD2 para o Y0

	#define TOUCH_X0_IOCON		IOCON_P0_25
	#define TOUCH_Y0_IOCON		IOCON_P0_26
	#define TOUCH_X1_IOCON		IOCON_P0_27
	#define TOUCH_Y1_IOCON		IOCON_P0_28
		#define TOUCH_PIN_MODEAD		9 		// pino para adc modo analógico com resistor pulldown
		#define TOUCH_PIN_MODEIO		0x80	// pino para gpio modo digital sem resistor pull


// ###################################################################################################################
// ADC e DAC (TOUCH)

#define ADC0_ConfigPort()		{ IOCON_P0_24 = 1; } 	// pino P0_24 como ADC[1], sem resistores de PULL, entrada analógica filtrada
														// os demais AD deixar em cargo do controlador do touchscreen

// ###################################################################################################################
// DAC

// DACOUT = P0_26
#define DAC_ConfigPort()		{ IOCON_P0_26 = 0x10002; } 	// pino P0_26 como DAC_OUT, sem resistores de PULL, saida analógica filtrada

// ###########################################################################################################################################
// ########### CONFIGURAÇÕES DO BEEP
#define BEEP_DIR				GPIO0_DIR
#define BEEP_CLR				GPIO0_CLR
#define BEEP_SET				GPIO0_SET
#define BEEP_NUMPIN				(1<<5)

// ###########################################################################################################################################
// ########### CONFIGURAÇÕES DO BARRAMENTO EXTERNO
// TODO ainda não testado este modelo de configuração de memórias
//#### CONFIG  MEMÓRIAS STÁTICAS OU DISPOSITIVOS MAPEANDOS POR RAM
#define SRAM_MEMORY_EXTENDED_WAIT 0								// Valor de 0 a x3ff. Para dispositivo lentos que usam Tempo de espera extra nos acessos ao barramento
#define SRAM_BANK0_USE SRAM_BANK_USE_NONE						//
#define SRAM_BANK1_USE SRAM_BANK_USE_NONE						//
#define SRAM_BANK2_USE SRAM_BANK_USE_DISPO_GENERIC_8BITS		// Banco selecionado pelo #CS2 entre os endereços 0x98000000 - 0x9bffffff
#define SRAM_BANK3_USE SRAM_BANK_USE_DISPO_GENERIC_16BITS		// Banco selecionado pelo #CS3 entre os endereços 0x9c000000 - 0x9fffffff

//	#### CONFIG MEMÓRIAS SDRAMS. SOMENTE ESTAMOS TRABALHANDO COM O BANCO 0,
//		PORÉM SE FAZER PARA OUTROS BANCOS VAMOS USAR O MESMO VALOR PARA TODOS
// 	os tipos estão em arch.h
#define	SDRAM_BANK0_USE MT48LC16M162P_7E_PC133					// Banco selecionado pelo #DYCS0 entre os endereços 0xA000 0000 a 0xAFFF FFFF

// Ajuste fino no atraso do clock do EMC a SDRAM.
// pagina 185 datasheet
//	O ajuste do atraso pode ser feito  entre 250ps a 7.75nS.
// Neste hardare estamos usando somente a SDRAM no CLK0, por isso vamos ajustar somemte CMDDLY, FBCLKDLY, CLKOUT0DLY
//   CLK --> CMDDLY[4:0]  --> EMC --> CLKOUT0DLY[20:16] --> SDRAM
//								  <-- FBCLKDLY[12:8]
// Fórmula REG+1 * 250ps

// OBS: Geralmente ajustamos FBCLKDLY
#if defined (KIT_LPC1788_PR111)
// Nesta SDRAM vamos adotar atrasos de 5nS
#define SDRAM_CLK_DELAY		0x141414
#else
// Nesta SDRAM vamos adotar atrasos de 4ns
#define SDRAM_CLK_DELAY		0x101010
// #define SDRAM_CLK_DELAY		0x101410
// #define SDRAM_CLK_DELAY		0x081414
#endif

// ###########################################################################################################################################
// ########### NAND FLASH
#define NAND_CONNECTION	NAND_CONNECTION_PIO		// Tipo de conexão que esse hardware tem com a memória NAND FLASH
#define nand_Delay()	{ nop(); nop(); nop(); nop(); nop(); nop(); } //6 nop * 120MHz = 50ns

#define NAND_BUS_DIR			GPIO1_DIR
#define NAND_BUS_SET			GPIO1_SET
#define NAND_BUS_CLR			GPIO1_CLR
#define NAND_BUS_PIN			GPIO1_PIN
#define NAND_BUSDATA_PIN		GPIO1_PIN0 // somente acessa GPIO[7:0]
#define NAND_CE					BIT_8
#define NAND_RE					BIT_9
#define NAND_CLE				BIT_10
#define NAND_ALE				BIT_13
#define NAND_WE					BIT_14
#define NAND_WP					BIT_15
#define NAND_RB					BIT_16

// ###########################################################################################################################################
// ########### DEVIO
#define DEVIO_QTD_DEVICES 10

// ###################################################################################################################
// INTERRUPÇÕES EXTERNAS (valores temporários)

#define ENTI0_ConfigPort()		{ IOCON_P0_29 = 2; } 	// pino P0_29 como EINT0
#define ENTI1_ConfigPort()		{ IOCON_P0_30 = 2; } 	// pino P0_30 como EINT1
*/

#endif
