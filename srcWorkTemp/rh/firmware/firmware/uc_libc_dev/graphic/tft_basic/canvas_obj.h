#ifndef __CANVAS_OBJ_H
#define __CANVAS_OBJ_H

#include "_config_cpu_.h"
#include "canvas.h"

// tBevel
//---------------------------------------------------------------------
typedef struct {
	// POSIÇÃO E DIMENSÕES.
	union {
		struct {
			int x,y,w,h; 	// podemos acessar diretamente tBevel.x = X;
		};
		tRect dim;			// podemos acessar com strutura tBevel.dim.x = X, ou asRect tBevel.dim = asRect(X, Y, W, H);
	};

	tColor color;
	u8 borderWidth;			// Espessura da borda
	unsigned repaint:1;

} tBevel, *pBevel;

// tLabel
//---------------------------------------------------------------------
typedef struct {
	tPoint pos;
	string caption;			// Texto do label
	string pcaption; 		// Texto impresso do label. Usado para não repintar o texto novamente caso seja = caption
	u8 size, lastsize;
	tColor color;
	tColor bkcolor;
	unsigned repaint:1;
	unsigned overlap:1;
} tLabel, *pLabel;

// tPanel
//---------------------------------------------------------------------
typedef struct {
	tLabel label;

	// Sinaliza o inicio da última posição inicial e final da coluna onde o label foi pintado
	// Isto porque quando imprimimos o texto no modo justificado no centro e se o último texto
	// que foi impresso for maior ao texto que vai ser impresso agora, precisamos apagar o texto
	//	anterior do seu inicio ao inicio do texto atual, e o mesmo se aplica ao final do texto
	int last_x_InitPaint, last_x_EndPaint;

	union {
		struct {
			int x,y,w,h; 	// podemos acessar diretamente tBevel.x = X;
		};
		tRect dim;			// podemos acessar com strutura tBevel.dim.x = X, ou asRect tBevel.dim = asRect(X, Y, W, H);
	};

	tColor bkcolor; 		// Cor do fundo
	tColor bdcolor; 		// Cor da borda
	tColor bdcolorSpc;		// Cor dp espaçador entre a borda e a pintura interna do painel
	u8 borderWidth;			// Espessura da borda
	unsigned repaint:1;
}tPanel, *pPanel;

void bevel_Init(pBevel obj, tRect rect, u8 borderWidth, tColor color);
void bevel_Repaint (pBevel obj);

void label_Init(pLabel obj, tPoint pos, tColor color, tColor bkColor, pchar caption);
void label_Caption(pLabel obj, pchar caption);
int label_MouseOver(pLabel obj, int x, int y);
void label_Repaint (pLabel obj);

void panel_Init(pPanel obj, tRect rect, u8 borderWidth, tColor bdcolor, tColor bdcolorSpc, tColor bkcolor, tColor lblcolor, pchar caption);
void panel_Caption(pPanel obj, pchar caption);
int panel_MouseOver(pPanel obj, int x, int y);
void panel_Repaint (pPanel obj);


#endif

