#ifndef __CONFIG_CPU_DISPLAY_H
#define __CONFIG_CPU_DISPLAY_H

#include "_config_cpu_.h"

// ###################################################################################################################
// CONFIGURAÇÕES MÓDULOS DE DISPLAY DE 7 SEGMENTOS
#define DISP7SEG_N_MODULE 4 // Define a quantidade de módulos de sete segmentos que o sistema vai controlar
							// Valor minimo 1 e valor máximo que o controlador pode gerenciar
#define DISP7SEG_SHOW_INIT	' ' 	// Define, em ASCII, o que sinal exibir nos módulos display 7seg ao iniciá-los
									// ' ' tudo apagado  '-' sinal de menos, '0' ... '9', 'A' ...
#define DISP7SEG_INV_SEG	pdOFF	// pdON inverte o sinal de controle de acionamento dos leds dos segmentos
#define DISP7SEG_INV_COMMOM	pdOFF	// pdON inverte o sinal de controle de acionamento do comum dos módulos

// definir portas de acesso aos segmentos dos módulos
//	deve usar a mesma porta e não necessariamente deve ser na sequência de bits
#define DISP7SEG_DIR	GPIO2_DIR
#define DISP7SEG_SET	GPIO2_SET
#define DISP7SEG_CLR	GPIO2_CLR
#define DISP7SEG_MASK	GPIO2_MASK
	#define DISP7SEG_LED1	BIT_4		// a
	#define DISP7SEG_LED2	BIT_3		// b
	#define DISP7SEG_LED3	BIT_6		// c
	#define DISP7SEG_LED4	BIT_8		// d
	#define DISP7SEG_LED5	BIT_9		// e
	#define DISP7SEG_LED6	BIT_2		// f
	#define DISP7SEG_LED7	BIT_5		// g
	#define DISP7SEG_LED8	BIT_7		// dp

// definir portas de acionamento dos módulos
//	deve usar a mesma porta e na sequência de bits como bit0, bit1 ... ou bit5, bit6, ...
#define DISP7SEG_C_DIR	GPIO1_DIR
#define DISP7SEG_C_SET	GPIO1_SET
#define DISP7SEG_C_CLR	GPIO1_CLR
	#define DISP7SEG_CINIT	BIT_22

// ###################################################################################################################
// CONFIGURAÇÕES DE ACESSO AO LCD TEXTO
#define LCD_CONNECTION 		LCD_CONNECTION_GPIO	// Tipo de acesso
	//	LCD_CONNECTION_GPIO  = Acesso ao LCD via porta GPIO
	//	LCD_CONNECTION_IOMAP = Acesso ao LCD como RAM

#if (LCD_CONNECTION == LCD_CONNECTION_GPIO)
	// definir a porta do barramento de dados
	//	deve usar a mesma porta e não necessariamente deve ser na sequência de bits
	#define LCD_PORTDATA_DIR GPIO2_DIR
	#define LCD_PORTDATA_CLR GPIO2_CLR
	#define	LCD_PORTDATA_SET GPIO2_SET
	#define	LCD_PORTDATA_IN  GPIO2_PIN
		#define LCD_PIN_D0	BIT_0
		#define LCD_PIN_D1	BIT_1
		#define LCD_PIN_D2	BIT_2
		#define LCD_PIN_D3	BIT_3
		#define LCD_PIN_D4	BIT_4
		#define LCD_PIN_D5	BIT_5
		#define LCD_PIN_D6	BIT_6
		#define LCD_PIN_D7	BIT_7

	// definir os pinos de controle, nem todos os pinos são usados para um determinado modelo de display
	//	deve usar a mesma porta e não necessariamente deve ser na sequência de bits
	#define LCD_PORTCTRL_DIR GPIO0_DIR
	#define LCD_PORTCTRL_CLR GPIO0_CLR
	#define	LCD_PORTCTRL_SET GPIO0_SET
		#define LCD_PIN_RS	BIT_22
		#define LCD_PIN_WR	BIT_11
		#define LCD_PIN_E	BIT_10
		#define LCD_PIN_RD	BIT_24
		#define LCD_PIN_CE	BIT_26
		#define LCD_PIN_CD	BIT_27

	#define LCD_DELAY 		40
	#define LCD_DELAY_HOME 	2
#else
	#define LCD_IOMAP_OFFSET 		0x82000000	// Endereço de OFFSET para acessar o LCD
#endif


// ###################################################################################################################
// CONFIGURAÇÕES DE ACESSO AO LCD GRÁFICO
#define glcdCONNECTION 		glcdCONNECTION_GPIO	// Tipo de acesso
	//	glcdCONNECTION_GPIO  = Acesso ao LCD via porta GPIO
	//	glcdCONNECTION_IOMAP = Acesso ao LCD como RAM

#if (glcdCONNECTION == glcdCONNECTION_GPIO)
	// definir a porta do barramento de dados
	//	deve usar a mesma porta e não necessariamente deve ser na sequência de bits
	#define glcdPORTDATA_DIR GPIO2_DIR
	#define glcdPORTDATA_CLR GPIO2_CLR
	#define	glcdPORTDATA_SET GPIO2_SET
	#define	glcdPORTDATA_IN  GPIO2_PIN
		#define glcdPIN_D0	BIT_0
		#define glcdPIN_D1	BIT_1
		#define glcdPIN_D2	BIT_2
		#define glcdPIN_D3	BIT_3
		#define glcdPIN_D4	BIT_4
		#define glcdPIN_D5	BIT_5
		#define glcdPIN_D6	BIT_6
		#define glcdPIN_D7	BIT_7

	// definir os pinos de controle, nem todos os pinos são usados para um determinado modelo de display
	//	deve usar a mesma porta e não necessariamente deve ser na sequência de bits
	#define glcdPORTCTRL_DIR GPIO0_DIR
	#define glcdPORTCTRL_CLR GPIO0_CLR
	#define	glcdPORTCTRL_SET GPIO0_SET
		#define glcdPIN_RS	BIT_22
		#define glcdPIN_WR	BIT_11
		#define glcdPIN_E	BIT_10

	// definir os pinos de chip select que são usados por alguns modelos de displays graficos
	//	deve usar a mesma porta e não necessariamente deve ser na sequência de bits
	#define glcdPORTCS_DIR GPIO2_DIR
	#define glcdPORTCS_CLR GPIO2_CLR
	#define glcdPORTCS_SET GPIO2_SET
		#define glcdPIN_CS0	BIT_8
		#define glcdPIN_CS1	BIT_9
#else
	#define glcdIOMAP_OFFSET 		0x82000000	// Endereço de OFFSET para acessar o LCD
#endif

#endif
