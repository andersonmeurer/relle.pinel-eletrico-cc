#ifndef __KEYBOARD_4IN_H
#define __KEYBOARD_4IN_H

#include "_config_cpu_.h"
#include "_config_cpu_keyboard.h"

typedef struct {
	int nCol;
	regCpu* portData;
	regCpu* portCol1;
	#if defined KEY4IN_PORTCOL2
	regCpu* portCol2;
	#endif
	#if defined KEY4IN_PORTCOL3
	regCpu* portCol3;
	#endif
	#if defined KEY4IN_PORTCOL4
	regCpu* portCol4;
	#endif
	#if defined KEY4IN_PORTCOL5
	regCpu* portCol5;
	#endif
}tKeyIn4, *pKeyIn4;
tKeyIn4 keyIn4;

void key4In_Init(void);
int key4In_Read(void);

#endif
