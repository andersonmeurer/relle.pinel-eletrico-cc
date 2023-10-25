#ifndef __CONFIG_CPU_DISPLAY_H
#define __CONFIG_CPU_DISPLAY_H

#include "_config_cpu_.h"

// ###################################################################################################################
// CONFIGURAÇÕES DE ACESSO

// ESSE PINO DE CONTROLE É PARA MANTER O PADRÃO PARA OS DISPLAUS ANTIGOS ONDE O IDENTIFICADO DE LEITURA E ESCRITA NÃO FUNCIONA NO WR/RD DO uC
#define LCD_PORT_CTRL_CONFIG		DDRB
#define LCD_PORT_CTRL_IN			PINB
#define LCD_PORT_CTRL_OUT			PORTB
#define LCD_PIN_RW					PB1			// Pino para acessar o LCD para gravação ou leitura. GRAVANDO NO LCD WR = 0 / LENDO NO LCD WR = 1

// CONFIGURAÇÕES PARA MODO RAM
#define LCD_CONNECTION_IOMAP  		1			// Tem que habilitar a SRAM externa no MCUR
	#define LCD_ADDR_CMD			0x8100 		// Acessa comandos
	#define LCD_ADDR_DATA			0x8200 		// Acessa dados

// CONFIGURAÇÕES PARA MODO GPIO
#define LCD_CONNECTION_GPIO			0
	#define LCD_PIN_RS				PB0			// Pino para que o LCD indentifique que é um acesso de dados ou de comando ACESSANDO DATA     RS = 1 / ACESSANDO COMANDO  RS = 0
	#define LCD_PIN_E				PB2			// Pino para habilitar o LCD HABILITANDO LCD E = 1
	#define LCD_PORT_DATA_CONFIG	DDRA
	#define LCD_PORT_DATA_OUT		PORTA
	#define LCD_PORT_DATA_IN		PINA

// AJUSTA O TIPO DE ACESSO A RTL
#define LCD_CONNECTION LCD_CONNECTION_GPIO		
//						LCD_CONNECTION_GPIO  Acesso a NIC via porta GPIO
//						LCD_CONNECTION_IOMAP Acesso a NIC como RAM

// ###################################################################################################################
// CONFIGURAÇÕES DO LCD
#define LCD_DELAY_HOME 				2			// 2ms
#define LCD_DELAY					1			// 1ms

//###################################################################################################################
// CONFIGURAÇÔES DISPLAY 7 SEGMENTOS
#define	DISP_7SEG_NUMBERS_OF_MODULES	7 			// Quantidade de display no painel. Máximo 8 módulos
#define DISP_7SEG_INVERTED				pdOFF		// pdON se o lógica de acionamento dos led e módulos é a nível baixo
#define DISP_7SEG_USE_SPI				pdOFF		// (NÃO IMPLEMENTADO)pdON utilizar a porta SPI para atualizar os leds a,b,c,d,e,f,g e p, senão utiliza GPIO

// PINOS PARA OS LEDS A, B, C, D, E, F, G e P para GPIO
#define DISP_7SEG_A_PORT	DDRC
#define DISP_7SEG_A_OUT		PORTC
#define DISP_7SEG_A			PC0

#define DISP_7SEG_B_PORT	DDRC
#define DISP_7SEG_B_OUT		PORTC
#define DISP_7SEG_B			PC1

#define DISP_7SEG_C_PORT	DDRC
#define DISP_7SEG_C_OUT		PORTC
#define DISP_7SEG_C			PC2

#define DISP_7SEG_D_PORT	DDRC
#define DISP_7SEG_D_OUT		PORTC
#define DISP_7SEG_D			PC3

#define DISP_7SEG_E_PORT	DDRC
#define DISP_7SEG_E_OUT		PORTC
#define DISP_7SEG_E			PC4

#define DISP_7SEG_F_PORT	DDRC
#define DISP_7SEG_F_OUT		PORTC
#define DISP_7SEG_F			PC5

#define DISP_7SEG_G_PORT	DDRC
#define DISP_7SEG_G_OUT		PORTC
#define DISP_7SEG_G			PC6

#define DISP_7SEG_P_PORT	DDRC
#define DISP_7SEG_P_OUT		PORTC
#define DISP_7SEG_P			PC7

// PINOS PARA OS MÓDUOS DE 1 a 8
#define DISP_7SEG_C1_PORT	DDRB
#define DISP_7SEG_C1_OUT	PORTB
#define DISP_7SEG_C1		PB0

#define DISP_7SEG_C2_PORT	DDRB
#define DISP_7SEG_C2_OUT	PORTB
#define DISP_7SEG_C2		PB1

#define DISP_7SEG_C3_PORT	DDRB
#define DISP_7SEG_C3_OUT	PORTB
#define DISP_7SEG_C3		PB2

#define DISP_7SEG_C4_PORT	DDRB
#define DISP_7SEG_C4_OUT	PORTB
#define DISP_7SEG_C4		PB3

#define DISP_7SEG_C5_PORT	DDRB
#define DISP_7SEG_C5_OUT	PORTB
#define DISP_7SEG_C5		PB4

#define DISP_7SEG_C6_PORT	DDRD
#define DISP_7SEG_C6_OUT	PORTD
#define DISP_7SEG_C6		PD2

#define DISP_7SEG_C7_PORT	DDRD
#define DISP_7SEG_C7_OUT	PORTD
#define DISP_7SEG_C7		PD3

#define DISP_7SEG_C8_PORT	DDRD
#define DISP_7SEG_C8_OUT	PORTD
#define DISP_7SEG_C8		PD4

#endif
