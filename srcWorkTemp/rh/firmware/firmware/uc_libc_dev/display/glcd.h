#ifndef __GLCD_H
#define __GLCD_H

#include "glcd_drv.h"
#include "_config_lib_graphics.h"

#define glcdVERSION 3 // software version of this library

// -----------------------------------------------------------------------------------------------------------
// PARAMETROS PARA O PADRÃO KS0108 >> MGD12864A
#define glcdDISPLAY_WIDTH 		128
#define glcdDISPLAY_HEIGHT 		64

// panel controller chips
#define glcdCHIP_WIDTH     		64  // pixels per chip
#define glcdCHIP_HEIGHT    		64  // pixels per chip

#define glcdON				0x3F
#define glcdOFF				0x3E
#define glcdSET_ADD			0x40
#define glcdDISP_START		0xC0
#define glcdSET_PAGE		0xB8

#define glcdDELAY_RST 	50 // 50uS
// -----------------------------------------------------------------------------------------------------------

#define glcdBLACK			0xFF
#define glcdWHITE			0x00

#define glcdNON_INVERTED 	pdFALSE
#define glcdINVERTED     	pdTRUE


#if glcdCHIP_HEIGHT < glcdDISPLAY_HEIGHT
#define glcd_DevXYval2Chip(x,y) ((x/glcdCHIP_WIDTH) + ((y/glcdCHIP_HEIGHT) * (glcdDISPLAY_HEIGHT/glcdCHIP_HEIGHT)))
#else
#define glcd_DevXYval2Chip(x,y)		((x/glcdCHIP_WIDTH))
#endif
#define glcd_DevXval2ChipCol(x)		((x) % glcdCHIP_WIDTH)

#define _GLCD_absDiff(x,y) ((x>y) ?  (x-y) : (y-x))
#define _GLCD_swap(a,b) \
	do {				\
		u8 t;			\
		t=a;			\
		a=b;			\
		b=t;			\
	} while(0)


// calculate number of chips & round up if width is not evenly divisable
#define glcd_CHIP_COUNT (((glcdDISPLAY_WIDTH + glcdCHIP_WIDTH - 1)  / glcdCHIP_WIDTH) * ((glcdDISPLAY_HEIGHT + glcdCHIP_HEIGHT -1) / glcdCHIP_HEIGHT))

// Note that while for now all these typedefs are the same they
// may not be in the future since each is used to point to a different type of data

typedef const u8* tImageBMP; // a glcd format bitmap (includes width & height)
typedef const u8* tImageXBM; // a "xbm" format bitmap (includes width & height)

// the first two bytes of bitmap data are the width and height
#define bitmapWidth(bitmap)  (*bitmap)
#define bitmapHeight(bitmap)  (*(bitmap+1))

typedef struct {
	int x,y;
	struct {
		#if (glcdXCOL_SUPPORT == pdON)
		u8 col;
		#endif
		u8 page;
	} chip[glcd_CHIP_COUNT];
} tCoord;

typedef struct {
	tCoord Coord;
	int Inverted;
} tGlcd, *pGlcd;

void glcd_Init(u8 invert);
void glcd_SetPixels(u8 x, u8 y, u8 x2, u8 y2, u8 color);
void glcd_GotoXY(u8 x, u8 y);
u8 glcd_ReadData(void);
void glcd_WriteData(u8 data);
void glcd_SetDot(u8 x, u8 y, u8 color);

#endif

