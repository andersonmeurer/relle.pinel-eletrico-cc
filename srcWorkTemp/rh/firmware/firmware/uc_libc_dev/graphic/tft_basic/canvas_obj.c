#include "canvas_obj.h"
#include "stdio_uc.h"

// ################################################################################
// OBJECT BEVEL
// ################################################################################
void bevel_Init(pBevel obj, tRect rect, u8 borderWidth, tColor color) {
	obj->dim = rect;
	obj->color = color;
	obj->borderWidth = borderWidth;
	obj->repaint = 1;

	#if (CANVASOBJ_USE_DEBUG == pdON)
	plog("CANVAS: bevel init"CMD_TERMINATOR);
	#endif
}

void bevel_Repaint (pBevel obj) {
	if (!obj->repaint) return; 	// retorna caso não é preciso pintar mais esse objeto
	obj->repaint = 0; 			// sinaliza que não precisa mais pintar esse objeto

	canvas.pen.color = obj->color;
	// pinta a borda e sua espessura
	u8 i;
	for (i=0; i<obj->borderWidth; i++)
		canvas_Rect(
			obj->x+i,
			obj->y+i,
			obj->x + obj->w -i,
			obj->y + obj->h -i
		);

	#if (CANVASOBJ_USE_DEBUG == pdON)
	plog("CANVAS: bevel repaint"CMD_TERMINATOR);
	#endif
}

// ################################################################################
// OBJECT PANEL
// ################################################################################
void panel_Init(pPanel obj, tRect rect, u8 borderWidth, tColor bdcolor, tColor bdcolorSpc, tColor bkcolor, tColor lblcolor, pchar caption) {
	obj->dim = rect;
	obj->bdcolor = bdcolor;
	obj->bdcolorSpc = bdcolorSpc;
	obj->bkcolor = bkcolor;
	obj->repaint = 1;
	obj->borderWidth = borderWidth;
	obj->last_x_InitPaint = 0;
	obj->last_x_EndPaint = 0;

	label_Init(&obj->label, asPoint(obj->x, obj->y), lblcolor, bkcolor, caption);
	obj->label.overlap = 0;	// sinaliza para o label não apagar o texto anterior quando imprimir o novo texto.
							// Pois será de responsabilidade do repaint do panel a fazer isto

	#if (CANVASOBJ_USE_DEBUG == pdON)
	plog("CANVAS: Panel init %s size %d"CMD_TERMINATOR, obj->label.caption, strlen(obj->label.caption));
	#endif
}

void panel_Caption(pPanel obj, pchar caption) {
	// A função foi testada pelo procedimento abaixo no loop principal do programa:
		/*
			static tTime timeRepaint = 0;
			string s;
			static x=1;

			if (now() > timeRepaint) { // Checa se é tempo de atualia a tela
				timeRepaint = now() + 1000;
				sformat(s, "%d", x);
				panel_Caption(&form.pnlSTS, s);
				if (x==100000) x=1; else x*=10;
				lprintf("%s"CMD_TERMINATOR,s);

				form_Repaint();
			}
		*/

	// se o tamanho do label mudou, teremos que repintar tudo. Isto porque o texto é
	//	justificado no centro e precisamos repintar tudo texto, pois a posição iniciar da string muda
	//	de acordo com o tamaho do texto
	int i;
	if (obj->label.size != obj->label.lastsize)
		for(i=0; i<__STRINGSIZE__; i++) obj->label.pcaption[i]=0;

	label_Caption(&obj->label, caption);
}

// se a posiçao XY estiver sonbre o panel retornar pdTRUE
int panel_MouseOver(pPanel obj, int x, int y) {
	if (x < obj->x) return pdFALSE;
	if (y < obj->y) return pdFALSE;
	if (x > obj->x+obj->w) return pdFALSE;
	if (y > obj->y+obj->h) return pdFALSE;
	return pdTRUE;
}

void panel_Repaint (pPanel obj) {
	// checa se é para pintar o painel
	if (obj->repaint) {
		obj->repaint = 0; // sinaliza para não pintar mais o panel
		// pinta a borda e sua espessura
		canvas.pen.color = obj->bdcolor;
		int i;
		for (i=0; i<obj->borderWidth; i++)
			canvas_Rect(
				obj->x+i,
				obj->y+i,
				obj->x + obj->w -i,
				obj->y + obj->h -i
			);

		// pinta o espaçamento entre a borda e o corpo do componente
		canvas.pen.color = obj->bdcolorSpc;
		canvas_Rect(
			obj->x+i,
			obj->y+i,
			obj->x + obj->w -i,
			obj->y + obj->h -i
		);

		// pinta o fundo e compensa a largura da borda
		canvas.brush.color = obj->bkcolor;
		canvas_FillRect(
			obj->x +1 +i,
			obj->y +1 +i,
			obj->x + obj->w - 1 - i,
			obj->y + obj->h - 1 - i
		);

		// SE O PANEL FOI REPINTAOD É PRECISO REPINTAR O LABEL
		// limpa o sinalizador de impressão de caption para repintá-lo
		for(i=0; i<__STRINGSIZE__; i++) obj->label.pcaption[i]=0;
		obj->label.repaint = 1; // Sinaliza para pintar o label


		#if (CANVASOBJ_USE_DEBUG == pdON)
		plog("CANVAS: panel repaint %s "CMD_TERMINATOR, obj->label.caption);
		#endif
	}

	if (obj->label.repaint) {
		// colocar o label no centro do painel
		tPoint pos;
		pos.x = obj->x + obj->w/2 - (canvas.font.w*strlen(obj->label.caption))/2;
		pos.y = obj->y + obj->h/2 - canvas.font.h/2;
		obj->label.pos = pos;

		// Vamos pintar o inicio e fim do panel do local do label onde contém texto cuja não será preemchido pelo texto atual
		// 	Isto porque quando imprimimos o texto no modo justificado no centro e se o último texto
		// 		que foi impresso for maior ao texto que vai ser impresso agora, precisamos apagar o texto
		//		anterior do seu inicio ao inicio do texto atual, e o mesmo se aplica ao final do texto
		//	Exemplo:
		//		Último texto que foi impresso: TTTTTTTTTTTTTTTTTTTT
		//		Texto a ser impresso agora:          TTTTTTTT
		//		Teremos que apagar        :    XXXXXX        XXXXXX
		if (obj->last_x_InitPaint)
		if (pos.x - obj->last_x_InitPaint > 0) {
			// pintar inicio do label
			canvas.brush.color = obj->bkcolor;
			canvas_FillRect(obj->last_x_InitPaint, pos.y, pos.x-1, pos.y+canvas.font.h-1);
		}

		int xx = pos.x + canvas.font.w*strlen(obj->label.caption);
		if (obj->last_x_EndPaint - xx > 0) {
			// pintar final do label
			canvas.brush.color = obj->bkcolor;
			canvas_FillRect(pos.x+1, pos.y, obj->last_x_EndPaint, pos.y+canvas.font.h-1);
		}

		obj->last_x_InitPaint = obj->label.pos.x;
		obj->last_x_EndPaint = obj->label.pos.x + (canvas.font.w*strlen(obj->label.caption));

		// Pinta o label
		label_Repaint(&obj->label);
	}
}

// ################################################################################
// OBJECT LABEL
// ################################################################################
void label_Init(pLabel obj, tPoint pos, tColor color, tColor bkColor, pchar caption) {
	obj->pos = pos;
	obj->color = color;
	obj->bkcolor = bkColor;
	obj->size = 0;
	obj->lastsize = 0;
	obj->overlap = 1; // Sinaliza para que o texto anterior seja apagado após pintar o novo texto
	label_Caption(obj, caption);
}

void label_Caption(pLabel obj, pchar caption) {
	obj->lastsize = obj->size;
	if (caption == NULL) {
		obj->size = 0;
	} else {
		sformat(obj->caption, "%s", caption);
		obj->size = strlen(obj->caption);
	}

	obj->repaint = 1;

	#if (CANVASOBJ_USE_DEBUG == pdON)
	plog("CANVAS: label caption %s [%d][%d]"CMD_TERMINATOR, obj->caption, obj->size, obj->lastsize);
	#endif
}

// se a posiçao XY estiver sobre o label retornar pdTRUE
int label_MouseOver(pLabel obj, int x, int y) {
	if (x<obj->pos.x) return pdFALSE;
	if (y<obj->pos.y) return pdFALSE;
	if (x>obj->pos.x+canvas.font.w*obj->size) return pdFALSE;
	if (y>obj->pos.y+canvas.font.h) return pdFALSE;
	return pdTRUE;
}

void label_Repaint (pLabel obj) {
	int x, y;
	u8 i;
	char c=32, ok=1;

	if (!obj->repaint) return; 	// retorna caso não é preciso pintar mais esse objeto
	obj->repaint = 0; 			// sinaliza que não precisa mais pintar esse objeto

	// ajusta as cores de frente e fubdi do canvas
	canvas.pen.color = obj->color;
	canvas.brush.color = obj->bkcolor;

	//posição inicial
	x = obj->pos.x;
	y = obj->pos.y;

	//pinta as letras
	for (i=0; i<obj->size; i++) {
		if (i>=sizeof(obj->caption)) break;
		if (ok) c = obj->caption[i];
		if (c==0) { ok = 0; c = 32; }
		//verifica se a letra precisa ser realmente pintada
		if (obj->pcaption[i] != c) {
			//if (c==0x1f) {
				//canvas_PutC(x,y,' ');
				//canvas_FillRectColor(x,y+5,x+canvas.font.w,y+canvas.font.h-5, obj->tgcolor); // TODO que cor é essa ???
			//} else
			//canvas_FillRect(x, y, x+canvas.font.w, y+canvas.font.h);
			canvas_PutC(x, y, c);
			obj->pcaption[i] = c;
		}
		x += canvas.font.w;
	}

	// Checa sem tem que apagar o texto anterior que não foi coberto pelo texto atual
	if ((obj->overlap) && (obj->lastsize-i) > 0) {
		canvas_FillRect(x, y, x+(canvas.font.w*(obj->lastsize-i))-1, y+canvas.font.h-1);
	}

	#if (CANVASOBJ_USE_DEBUG == pdON)
	plog("CANVAS: label repaint %s [%d][%d][%d]"CMD_TERMINATOR, obj->caption, obj->size, obj->lastsize, obj->lastsize-i);
	//plognp("CANVAS: label repaint '%s'[%d] cores [0x%x][0x%x]"CMD_TERMINATOR, obj->caption, obj->size, obj->color, obj->bkcolor);
	#endif
}
