#ifndef __TOUCH_H
#define __TOUCH_H

#include "_config_cpu_.h"
#include "_config_lib_tft.h"

void touch_Init(void);
void touch_Tick(void);
void touch_GetADC(int* x, int* y);
int touch_GetPos(int* x, int* y);
void touch_WaitXY(char convert, int* x, int* y);
void touch_Convert(int * x, int * y);
void touch_Config(void);

typedef struct {
	//char lcd[6][32];
	//char rxbuf[32]; //dados recebidos do painel lcd
	//volatile char tecla;
	//volatile int botoes,pot,vel;
	volatile int touchx, touchy; //valores analógicos lidos do painel
	int coefa[2]; //fator de 1024
	int coefb[2];
	//int tmbeep; //[ms]
	//unsigned usestdout : 1; //imprimir tudo no stdout
	//unsigned repaint : 1; //repintar todos os labels graficos
	//tRECTX rec; //area da menssagem
}tTouch;

// ###############################################################################
// VARS GLOBAL - para todos as libs usar essas vars contidas dentro de canvas.c
// ###############################################################################
extern tTouch touch;
#endif
