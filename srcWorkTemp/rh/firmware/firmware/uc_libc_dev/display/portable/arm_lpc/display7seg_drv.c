#include "display7seg_drv.h"

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa a porta do processador para controlar os módulos de 7 segmentos
// Parametro:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void disp7seg_SetupPorts(void) {

	// Pinos para os leds 1 a 8 como saida
	DISP7SEG_DIR |= (
		DISP7SEG_LED1 |
		DISP7SEG_LED2 |
		DISP7SEG_LED3 |
		DISP7SEG_LED4 |
		DISP7SEG_LED5 |
		DISP7SEG_LED6 |
		DISP7SEG_LED7 |
		DISP7SEG_LED8
	);

	// desligar os leds dos segmentos
	DISP7SEG_SET = (
		DISP7SEG_LED1 |
		DISP7SEG_LED2 |
		DISP7SEG_LED3 |
		DISP7SEG_LED4 |
		DISP7SEG_LED5 |
		DISP7SEG_LED6 |
		DISP7SEG_LED7 |
		DISP7SEG_LED8
	);

	// Pinos para os comuns dos leds como saida
	// desligar os comuns dos leds
	int x;
	u32 bit = DISP7SEG_CINIT;
	for (x=0;x<DISP7SEG_N_MODULE;x++) {
		DISP7SEG_C_DIR |= bit; // Pinos para os comuns dos leds como saida
		DISP7SEG_C_SET = bit;	// desligar os comuns dos leds
		bit = bit<<1;
	}
}

void disp7seg_WriteSeg(int value) {
	#if (DISP7SEG_INV_SEG == pdON)
	if (value & 0x80)	DISP7SEG_CLR = DISP7SEG_LED1; else DISP7SEG_SET = DISP7SEG_LED1;
	if (value & 0x40)	DISP7SEG_CLR = DISP7SEG_LED2; else DISP7SEG_SET = DISP7SEG_LED2;
	if (value & 0x20)	DISP7SEG_CLR = DISP7SEG_LED3; else DISP7SEG_SET = DISP7SEG_LED3;
	if (value & 0x10)	DISP7SEG_CLR = DISP7SEG_LED4; else DISP7SEG_SET = DISP7SEG_LED4;
	if (value & 0x8)	DISP7SEG_CLR = DISP7SEG_LED5; else DISP7SEG_SET = DISP7SEG_LED5;
	if (value & 0x4)	DISP7SEG_CLR = DISP7SEG_LED6; else DISP7SEG_SET = DISP7SEG_LED6;
	if (value & 0x2)	DISP7SEG_CLR = DISP7SEG_LED7; else DISP7SEG_SET = DISP7SEG_LED7;
	if (value & 0x1)	DISP7SEG_CLR = DISP7SEG_LED8; else DISP7SEG_SET = DISP7SEG_LED8;
	#else
	if (value & 0x80)	DISP7SEG_SET = DISP7SEG_LED1; else DISP7SEG_CLR = DISP7SEG_LED1;
	if (value & 0x40)	DISP7SEG_SET = DISP7SEG_LED2; else DISP7SEG_CLR = DISP7SEG_LED2;
	if (value & 0x20)	DISP7SEG_SET = DISP7SEG_LED3; else DISP7SEG_CLR = DISP7SEG_LED3;
	if (value & 0x10)	DISP7SEG_SET = DISP7SEG_LED4; else DISP7SEG_CLR = DISP7SEG_LED4;
	if (value & 0x8)	DISP7SEG_SET = DISP7SEG_LED5; else DISP7SEG_CLR = DISP7SEG_LED5;
	if (value & 0x4)	DISP7SEG_SET = DISP7SEG_LED6; else DISP7SEG_CLR = DISP7SEG_LED6;
	if (value & 0x2)	DISP7SEG_SET = DISP7SEG_LED7; else DISP7SEG_CLR = DISP7SEG_LED7;
	if (value & 0x1)	DISP7SEG_SET = DISP7SEG_LED8; else DISP7SEG_CLR = DISP7SEG_LED8;
	#endif
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Liga somente o módulo desejado desligando os demais
// Parametro:	module: Módulo que deseja acionar, valores de 0 a DISP7SEG_N_MODULE-1, valor -1 deslisga todos
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void disp7seg_SetModule(int module) {
	int x;
	u32 bit = DISP7SEG_CINIT;
	for (x=0;x<DISP7SEG_N_MODULE;x++) {
		#if (DISP7SEG_INV_COMMOM == pdON)
		if (x==module) DISP7SEG_C_CLR = bit; else  DISP7SEG_C_SET = bit;
		#else
		if (x==module) DISP7SEG_C_SET = bit; else  DISP7SEG_C_CLR = bit;
		#endif
		bit = bit<<1;
	}
}

