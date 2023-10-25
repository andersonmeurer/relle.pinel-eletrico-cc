#ifndef __TSC2046_H
#define __TSC2046_H

#include "tsc2046_drv.h"

void tsc2046_Init(void);
void tsc2046_SwapXY(int sw);
void tsc2046_LcdSize(int h_size, int v_size);
void tsc2046_SetArea(int top, int left, int bottom, int right);
int tsc2046_GetTouchCoord(s16* x, s16* y);
int tsc2046_GetTouchCoordST(s16* x, s16* y);
void tsc2046_Tick(void);

#endif
