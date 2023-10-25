#ifndef __CANVAS_H
#define __CANVAS_H

#include "_config_cpu_.h"
#include "_config_lib_tft.h"
#include "colors.h"

// tRGB
//---------------------------------------------------------------------
typedef union {
	struct { u8 r, g, b, a; };
	tColor c;
}tRGB;

// tRGB565
//---------------------------------------------------------------------
typedef union {
	struct {
		int r: 5;
		int g: 6;
		int b: 5;
	};
	tColor c;
}tRGB565;


//tRectX asRECTx(int xi, int yi, int xf, int yf);
//char rectxMOUSEOVER(tRectX r, int x, int y);

// tPen
//---------------------------------------------------------------------
typedef struct {
	tPoint pos;
	tColor color;
	int width;
}tPen;

// tBrush
//---------------------------------------------------------------------
typedef enum {
	bsSolid = 0, bsClear, bsHorizontal, bsVertical,
    bsFDiagonal, bsBDiagonal, bsCross, bsDiagCross
}tBrushStyle;

typedef struct {
	tColor color;
	tBrushStyle style;
}tBrush;

// tFont
//---------------------------------------------------------------------
typedef struct {
    u8 bpc; //bytes por char
    u8 width,height; //[pixels]
    u8 first,last;
    cpchar pfont;
}tFontBin, *pFontBin;

typedef struct {
	u8 w; 		// width [pixels]
	u8 h; 		// height [pixels]
	u8 size; 	// 0,1 ou 2
	u8 align; 	// alinhamento
	u8 fake; 	// [pixels]
	tColor color;
}tFont;

//font style options
#define fsBold				BIT_0
#define fsItalic			BIT_1
#define fsUnderline			BIT_2
#define fsStrikeOut			BIT_3

#define taLeftJustify		0
#define taRightJustify		1
#define taCenter			2

// tCanvas
//---------------------------------------------------------------------
typedef struct {
	//variáveis
	tPen pen;
	tBrush brush;
	tFont font;
	u16* frame; 	// buffer do video
	int frameid; 	// numero da pagina do video
	int cx,cy,con; 	// cursor
	int w,h; 		// width,height;
	tColor bkcolor;
}tCanvas;

typedef tCanvas* pCanvas;

tRect asRect(int x, int y, int w, int h);
tPoint asPoint(int x, int y);

void canvas_Create(void);
void canvas_SetBackColor(tColor bkColor);
tColor canvas_GetBackColor(void);
u32 canvas_GetFrame(int f);
void canvas_SetFrame(int f);
void canvas_SaveFrame(void);
void canvas_LoadFrame(void);

void canvas_Clear(void);
void canvas_Scrool(int n);
void canvas_FillRect(int xi, int yi, int xf, int yf);
void canvas_FillRectX(tRectC r);
void canvas_FillRectColor(int xi, int yi, int xf, int yf, tColor c);
void canvas_PutPixel(int x, int y);
void canvas_Line(int x0, int y0, int x1, int y1);
void canvas_LineTo(int x, int y);
void canvas_MoveTo(int x, int y);
void canvas_Cross(int x, int y, int size);
void canvas_Rect(int xi, int yi, int xf, int yf);
void canvas_Rect2(tRectC r);
void canvas_RectX(tRectC r);

void canvas_SetFont(int size, int fake);
void canvas_PutC(int x, int y, int c);
void canvas_TextOut(int x, int y, cpchar text);

char canvas_Over(int x, int y);
char canvas_RectXClick(tRectC r, int x, int y);

#if CANVAS_USE_TESTBENCH == pdON
void canvas_Fractal_TestBench(void);
void canvas_PutChars_TestBench(tColor color);
void canvas_Pallet_TestBench(void);
void canvas_PutPixel_TestBench(void);
#endif

// ###############################################################################
// VARS GLOBAL - para todos as libs usar essas vars contidas dentro de canvas.c
// ###############################################################################
extern tCanvas canvas;

#if CANVAS_USE_TESTBENCH == pdON
#define CL_TABLECOUNT	18
extern const tColor clTable[CL_TABLECOUNT];
#endif

#endif  
