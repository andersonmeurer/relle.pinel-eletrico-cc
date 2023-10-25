#ifndef __BEEP_H
#define __BEEP_H

#include "beep_drv.h"

#define beepCONTINUOUS 			1 	// Buzzer ligado direto
#define beepCONTINUOUS_ONOFF 	2	// Buzzer liga e desliga em periodos reguladores e mantem nessa condição até ser desligado
#define beep1SHORTBEEP 			3	// Buzzer é emitido 1 beep de teclado
#define beep3SHORTBEEP 			4	// Buzzer é emitido 3 beeps em intevalos curtos e depois é deligado

#define beepMINTIME_INTERVAL 	100 // Tempo minimo entre os intervalos entre acionamento do novo alarme a partir do desligamento do anteirior

void beep_Init(void);
void beep_Play(uint type);
uint beep_Playing (void);
void beep_Stop(void);
void beep_Tick(void);

#endif
