/*
 * display7seg.h
 *
 *  Created on: 31/07/2013
 *      Author: alex
 */

#ifndef DISPLAY7SEG_H_
#define DISPLAY7SEG_H_

#include "_config_cpu_.h"
#include "_config_cpu_display.h"

typedef struct {
	u8 module[DISP7SEG_N_MODULE];	// Estados de ligado ou desligado dos leds de cada módulo de 7 seg. 0 = desligado, 1 = ligado
										// bits 7  6  5  4  3  2  1  0
										// led  1  2  3  4  5  6  7  8
										// seg  A  B  C  D  E  F  G  DP
	u8 moduleRefresh; 				// sinaliza qual o módulo a ser atualizado no momento. Valores de 1 a DISP7SEG_N_MODULE
} tDisp7Panel, *pDisp7Panel;

void disp7seg_Init(void);
void disp7seg_SetValue(u8 module, u8 value);
void disp7seg_SetDP(u8 module, u8 action);
void disp7seg_SetSeg(u8 module, u8 seg, u8 action);
void disp7seg_Tick(void);

#endif /* DISPLAY7SEG_H_ */
