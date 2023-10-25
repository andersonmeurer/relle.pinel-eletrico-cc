#ifndef __VISUAL_H
#define __VISUAL_H

#include "GUI.h"
#include "stdio_uc.h"
#include "colors.h"

// ##########################################################################################################
// definições padrões dos componentes visuais e seus comportamentos
#define touchPRESS_REPEAT1  500 // Define o tempo (ms) em que o evento OnTouchPress seja emitido pela primeira vez
#define touchPRESS_REPEAT2  100 // Define o tempo (ms) em que o evento OnTouchPress seja emitido sucessivamente quando a tecla for mantida pressionada
#define touchFASTINC_REPEAT 1	// Define o tempo em segundos para que a var touchFastInc seja incrementada
#define touchREPEAT touchFASTINC_REPEAT*1000/touchPRESS_REPEAT2;

#define defaultCOLOR_BACKGROUND 	clBlack
#define defaultCOLOR_FONT 			clWhite
#define defaultCOLOR_BORDER 		clDodgerBlue
#define defaultCOLOR_BACK 			defaultCOLOR_BACKGROUND
#define defaultBOARD_WIDTH 			4
#define defaultFONT 				GUI_Font24B_ASCII

#define barProgressROUNDED_RAY 		2
#define barProgressCOLOR_HIGHLIGHT	clDodgerBlue
#define barProgressCOLOR_BORDER		defaultCOLOR_BORDER

#define buttonROUNDED_RAY 			10
#define buttonCOLOR_FONT_DISABLED 	clWhite
#define buttonCOLOR_BOARD_DISABLED 	clWhite
#define buttonCOLOR_BACK_DISABLED 	clDarkGray

#define labelROUNDED_RAY 			2


// ##########################################################################################################
// definições de tipos de objetos para o gerenciador visual possa indentificar que tipo objeto a tratar
#define visualTYPE_tButton 		0
#define visualTYPE_tBarProgress 1
#define visualTYPE_tLabel 		2

typedef void (*pTouchFunc) (int);
typedef void (*pTouchFuncPaint) (void *);

extern GUI_COLOR colorBackground;

// #################################################################################################
// GERENCIADOR DE OBJETOS
void visual_Init(GUI_COLOR clrBackground);
int visual_RegisterComponent(int type, void* obj, pTouchFuncPaint funcPaint);
void visual_Run(int v);
void visual_Process(void);

// MISC
GUI_RECT asDim(I16 x, I16 y, I16 w, I16 h);

typedef struct {
	GUI_FONT type;
	GUI_COLOR color;
	int alignment;
	string text;
} tFont;

typedef struct {
	unsigned repaint:1;
	unsigned touched:1;			// Sinaliza que o botão foi tocado
	unsigned enabled:1;
	unsigned visible:1;
	GUI_RECT rect;				// Posição e tamanho do botão
	//float xScale, yScale;		// Escala de plotagem
	pTouchFunc onTouchDown;
	pTouchFunc onTouchPress;
	pTouchFunc onTouchUp;
} tBase, *pBase;


// #################################################################################################
// tButton
typedef struct {
	GUI_COLOR backColor; 		// Cor de fundo
	GUI_COLOR borderColor; 		// Cor da borda
	uint borderWidth;			// Espessura da borda
	tFont font;
	tBase base;
} tButton, *pButton;

void button_Create(pButton obj, pchar text, pTouchFunc onTouchDown, pTouchFunc onTouchPress, pTouchFunc onTouchUp);
void button_Text(pButton obj, pchar text);
void button_Enabled(pButton obj, int en);
void button_SetVisible(pButton obj, int v);
int button_Visible(pButton obj);
void button_SetPosition(pButton obj, int x, int y, int w, int h);
void button_SetFontColor(pButton obj, tColor color);
void button_SetFont(pButton obj, const GUI_FONT* font);
void button_Paint(pButton obj);
void button_Repaint(pButton obj);

// #################################################################################################
// tBarProgress
typedef struct {
	unsigned printText:1;
	unsigned printPercent:1;
	unsigned printBar:1;
	GUI_COLOR backColor; 		// Cor de fundo
	GUI_COLOR progressColor; 	// Cor da barra de progresso
	GUI_COLOR borderColor; 		// Cor da borda
	int min, max, value;
	uint borderWidth;			// Espessura da borda
	tFont font;
	tBase base;
} tBarProgress, *pBarProgress;

void barProgress_Create(pBarProgress obj);
void barProgress_SetMinMax(pBarProgress obj, int min, int max);
int barProgress_SetValue(pBarProgress obj, int value);
void barProgress_SetPosition(pBarProgress obj, int x, int y, int w, int h);
void barProgress_SetFontColor(pBarProgress obj, tColor color);
void barProgress_SetFont(pBarProgress obj, const GUI_FONT* font);
void barProgress_Text(pBarProgress obj, pchar text);
void barProgress_SetVisible(pBarProgress obj, int v);
int barProgress_Visible(pBarProgress obj);
void barProgress_EnPrintBar(pBarProgress obj, int st);
void barProgress_EnPrintPorcent(pBarProgress obj, int st);
void barProgress_EnPrintText(pBarProgress obj, int st);
void barProgress_Repaint(pBarProgress obj);
void barProgress_Paint(pBarProgress obj);

// #################################################################################################
// tLabel

typedef struct {
	unsigned printPanel:1;
	GUI_COLOR backColor; 		// Cor de fundo
	GUI_COLOR borderColor; 		// Cor da borda
	uint borderWidth;			// Espessura da borda
	tFont font;
	tBase base;
} tLabel, *pLabel;

void label_Create(pLabel obj, pchar text);
void label_SetTouch(pLabel obj, pTouchFunc onTouchDown, pTouchFunc onTouchPress, pTouchFunc onTouchUp);
void label_SetPosition(pLabel obj, int x, int y, int w, int h);
void label_SetFontColor(pLabel obj, tColor color);
void label_SetFont(pLabel obj, const GUI_FONT* font);
void label_PrintPanel(pLabel obj, int st, uint borderWidth, tColor colorBorder, tColor colorBack);
void label_Text(pLabel obj, pchar text);
void label_SetAlignment(pLabel obj, int alignment);
void label_SetVisible(pLabel obj, int v);
int label_Visible(pLabel obj);
void label_Repaint(pLabel obj);
void label_Paint(pLabel obj);

#endif // __VISUAL_H
