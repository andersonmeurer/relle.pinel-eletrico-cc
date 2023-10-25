#include "canvas_obj.h"
#include "stdio_uc.h"

//tFILEp iconfile;

// ################################################################################################
// OBJECT LABEL
// ################################################################################################
void label_Init(pLabel label) {
	label->fgcolor = clWhite;
	label->bkcolor = clBlack;
	label->pos.x = 0;
	label->pos.y = 0;
	label->size = 32;
	label->repaint = 1;

	#if CANVAS_USE_DEBUG == pdON
	plog("CAP init label"CMD_TERMINATOR);
	#endif
}

void label_Caption(pLabel label, pchar caption) {
	sformat(label->caption, "%s", caption);
	label->size = strlen(label->caption);
	label->repaint = 1;

	#if CANVAS_USE_DEBUG == pdON
	plog("CAP label %s "CMD_TERMINATOR, label->caption);
	#endif
}

void label_Pos(pLabel label, int x, int y) {
	label->pos.x = x;
	label->pos.y = y;
	label->repaint = 1;

	#if CANVAS_USE_DEBUG == pdON
	plog("POS label %s XY[%d][%d]"CMD_TERMINATOR, label->caption, label->pos.x, label->pos.y);
	#endif
}

void label_Color(pLabel label, tColor fgcolor, tColor bkcolor, tColor bdcolor) {
	label->fgcolor = fgcolor;
	label->bkcolor = bkcolor;
	label->bdcolor = bdcolor;
	label->repaint = 1;
}

void label_Repaint(pLabel label) {
	u32 i, x, y;
	char c=32, ok=1;

	//ajusta o canvas
	canvas.pen.color = label->fgcolor;
	canvas.brush.color = label->bkcolor;

	//posição inicial
	x = label->pos.x;
	y = label->pos.y;

	// pinta as letras
	for (i=0; i<label->size; i++) {
		if (i >= sizeof(label->caption)) break;
		if (ok) c = label->caption[i];
		if (c==0) {
			ok = 0;
			c = 32;
		}

		//verifica se a letra precisa ser realmente pintada
		if ((label->pcap[i]!=c)||(label->repaint)) {
			if (c==0x1f) {
				canvas_PutC(x,y,' ');
				canvas_FillRectColor(x,y+5,x+canvas.font.w,y+canvas.font.h-5,label->tgcolor);
			} else canvas_PutC(x,y,c);
			label->pcap[i] = c;
		}
		x += canvas.font.w;
	}

	//tem que pintar tudo? garante que pinte o fundo
	if ((label->repaint)&&((label->size-i)>0)) {
		canvas_FillRect(x,y,x+(canvas.font.w*(label->size-i))-1,y+canvas.font.h-1);
	}

	// pinta borda
	if (label->repaint) {
		x = label->pos.x;
		y = label->pos.y;
		int w = canvas.font.w*(label->size);
		int h = canvas.font.h;
		canvas.pen.width = 1;
		canvas.pen.color = label->bdcolor;
		canvas_Rect(x-1, y-1, x + w, y + h);
	}

	// nao precisa mais pintar tudo
	label->repaint = 0;
}

// se a posiçao XY estiver sonbre o label retornar pdTRUE
int label_MouseOver(pLabel label, int x, int y) {
	if (x<label->pos.x) return pdFALSE;
	if (y<label->pos.y) return pdFALSE;
	if (x>label->pos.x+canvas.font.w*label->size) return pdFALSE;
	if (y>label->pos.y+canvas.font.h) return pdFALSE;
	return pdTRUE;
}

/*
// ################################################################################################
// OBJECT BUTTON
// ################################################################################################
void button_Init(pButton button) {
	label_Init(&button->label);
	button->pos.x = 0;
	button->pos.y = 0;
	button->width = 120;
	button->height = 50;
	//button->icon = -1;
}

void button_Repaint(pButton button) {
	int l = 150;
	int a = 48;

	canvas.brush.color = clWhite;
	canvas_FillRect(button->pos.x-l/2, button->pos.y-a/2, button->pos.x+l/2, button->pos.y+a/2);
//if (icon>=0) canvasPRINTICON(icon,x-l/2,y-48/2);
	canvas.pen.color = clLtGray;
	canvas_Rect(button->pos.x-l/2,button->pos.y-a/2,button->pos.x+l/2,button->pos.y+a/2);
	canvas.pen.color = clBlack;
	canvas_TextOut(button->pos.x, button->pos.y, button->label.caption);
}

void canvasPRINTICON(int id, int x, int y) {
	int i;
	char buf[48*3];

	//apenas limpar onde seria colocado um icone
	if (id==-1) {
		canvasFILLRECT(x,y,x+48,y+48);
		return;
	}

	id = id%12;

	if (iconfile==NULL) {
		if (!fileexists("icones.bmb")) return;
		if (fileopen( &iconfile, "icones.bmb",'r')) return;
	}

	if (fileseek(iconfile,id*48*48*3)) return;

	for (i=0;i<48;i++) {
		if (fileread(iconfile,buf,48*3)) return;
		canvasPASTE(buf,x,y+i,x+47,y+i);
	}
}
*/

tRect asRect(int x, int y, int w, int h) {
	tRect t;
	t.x = x;
	t.y = y;
	t.w = w;
	t.h = h;
	return t;
}
