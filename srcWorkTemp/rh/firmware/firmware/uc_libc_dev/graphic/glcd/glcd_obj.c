#include "glcd_obj.h"
#include "stdio_uc.h"

tRect asRect(int x, int y, int w, int h) {
	tRect t;
	t.x = x;
	t.y = y;
	t.w = w;
	t.h = h;
	return t;
}

tPoint asPoint(int x, int y) {
	tPoint t;
	t.x = x;
	t.y = y;
	return t;
}

// ################################################################################
// OBJECT PANEL
// ################################################################################
void panel_Init(pPanel obj, tFont font, tRect rect, u8 color, u8 bkColor, pchar caption) {
	obj->font = font;
	obj->color = color;
	obj->bkcolor = bkColor;
	obj->dim = rect;
	obj->xClear = 0;
	obj->widthClear = 0;

	panel_Caption(obj, caption);
}

void panel_Caption(pPanel obj, pchar caption) {
	sformat(obj->caption, "%s", caption);
	obj->repaint = 1;
}

void panel_Repaint (pPanel obj) {
	if (!obj->repaint) return; 	// retorna caso não é preciso pintar mais esse objeto
	obj->repaint = 0; 			// sinaliza que não precisa mais pintar esse objeto
	glcd_TextSelectFont(obj->font, obj->color);

	// colocar o label no centro do painel
	tPoint pos;
	pos.x = obj->x + obj->w/2 - glcd_TextStringWidth(obj->caption)/2;
	pos.y = obj->y + obj->h/2 - glcd_TextCharHeight()/2;

	// Vamos pintar o inicio e fim do panel do local do label onde contém texto cuja não será preemchido pelo texto atual
	// 	Isto porque quando imprimimos o texto no modo justificado no centro e se o último texto
	// 		que foi impresso for maior ao texto que vai ser impresso agora, precisamos apagar o texto
	//		anterior do seu inicio ao inicio do texto atual, e o mesmo se aplica ao final do texto
	//	Exemplo:
	//		Último texto que foi impresso: TTTTTTTTTTTTTTTTTTTT
	//		Texto a ser impresso agora:          TTTTTTTT
	//		Teremos que apagar        :    XXXXXX        XXXXXX
	if ( (obj->xClear) && (pos.x - obj->xClear > 0) )
		glcd_FillRect(obj->xClear, pos.y, pos.x-obj->xClear, glcd_TextCharHeight()-1, obj->bkcolor);
	obj->xClear = pos.x;

	// apagar final do label
	int xx = pos.x + glcd_TextStringWidth(obj->caption);
	if (obj->widthClear - xx > 0) {
		glcd_FillRect(xx, pos.y, obj->widthClear-xx, glcd_TextCharHeight()-1, obj->bkcolor);
	}
	obj->widthClear = pos.x + glcd_TextStringWidth(obj->caption);

	// Pinta o label
	glcd_TextCursorToXY(pos.x, pos.y);
	rprintf(STDIO_DEV_LCD, "%s", obj->caption);
}

// ################################################################################
// OBJECT LABEL
// ################################################################################
void label_Init(pLabel obj, tFont font, tPoint pos, u8 color, u8 bkColor, pchar caption) {
	obj->font = font;
	obj->pos = pos;
	obj->color = color;
	obj->bkcolor = bkColor;
	obj->widthClear = 0;
	label_Caption(obj, caption);
}

void label_Caption(pLabel obj, pchar caption) {
	sformat(obj->caption, "%s", caption);
	obj->repaint = 1;
}

void label_Repaint (pLabel obj) {
	if (!obj->repaint) return; 	// retorna caso não é preciso pintar mais esse objeto
	obj->repaint = 0; 			// sinaliza que não precisa mais pintar esse objeto

	glcd_TextSelectFont(obj->font, obj->color);
	glcd_TextCursorToXY(obj->pos.x, obj->pos.y);

	// apagar final do label
	int xx = obj->pos.x + glcd_TextStringWidth(obj->caption);
	if (obj->widthClear - xx > 0) {
		glcd_FillRect(xx, obj->pos.y, obj->widthClear-xx, glcd_TextCharHeight()-1, obj->bkcolor);
	}
	obj->widthClear = obj->pos.x + glcd_TextStringWidth(obj->caption);

	rprintf(STDIO_DEV_LCD, "%s", obj->caption);
}
