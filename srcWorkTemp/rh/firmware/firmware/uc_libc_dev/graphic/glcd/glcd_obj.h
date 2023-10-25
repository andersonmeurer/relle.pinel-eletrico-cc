#ifndef __GLCD_OBJ_H
#define __GLCD_OBJ_H

#include "_config_cpu_.h"
#include "glcd.h"
#include "glcd_primitives.h"
#include "glcd_text.h"

// tPanel
//---------------------------------------------------------------------
typedef struct {
	unsigned repaint:1;
	string caption;			// Texto do label
	union {
		struct {
			int x,y,w,h; 	// podemos acessar diretamente tBevel.x = X;
		};
		tRect dim;			// podemos acessar com strutura tBevel.dim.x = X, ou asRect tBevel.dim = asRect(X, Y, W, H);
	};
	u8 color;
	u8 bkcolor;
	tFont font;

	// Sinaliza o inicio da última posição inicial e final da coluna onde o label foi pintado
	// Isto porque quando imprimimos o texto no modo justificado no centro e se o último texto
	// que foi impresso for maior ao texto que vai ser impresso agora, precisamos apagar o texto
	//	anterior do seu inicio ao inicio do texto atual, e o mesmo se aplica ao final do texto
	int xClear, widthClear;
} tPanel, *pPanel;

// tLabel
//---------------------------------------------------------------------
typedef struct {
	unsigned repaint:1;
	tPoint pos;
	string caption;			// Texto do label
	u8 color;
	u8 bkcolor;
	tFont font;
	int widthClear;
} tLabel, *pLabel;

tRect asRect(int x, int y, int w, int h);
tPoint asPoint(int x, int y);

void panel_Init(pPanel obj, tFont font, tRect rect, u8 color, u8 bkColor, pchar caption);
void panel_Caption(pPanel obj, pchar caption);
void panel_Repaint (pPanel obj);

void label_Init(pLabel obj, tFont font, tPoint pos, u8 color, u8 bkColor, pchar caption);
void label_Caption(pLabel obj, pchar caption);
void label_Repaint (pLabel obj);

#endif
