#ifndef __JOYPS_H
#define __JOYPS_H

#include "joyps_drv.h"

void joyps_Init(void);
int joy_ReadControls(int* ctrl );

#define joypsKEY_L1			0x4
#define joypsKEY_L2			0x1
#define joypsKEY_R1			0x8
#define joypsKEY_R2			0x2

#define joypsKEY_SELECT		0x100
#define joypsKEY_START		0x800

#define joypsKEY_TRIANGLE 	0x10
#define joypsKEY_CIRCLE		0x20
#define joypsKEY_X			0x40
#define joypsKEY_SQUARE 	0x80

#define joypsKEY_UP			0x1000
#define joypsKEY_DOWN		0x4000
#define joypsKEY_LEFT		0x8000
#define joypsKEY_RIGHT		0x2000


#endif
