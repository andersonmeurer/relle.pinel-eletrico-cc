#include "visual.h"

void barProgress_Create(pBarProgress obj) {
	obj->base.repaint = 1;
	obj->base.visible = 1;
	obj->printPercent = 1;
	obj->printBar = 1;
	obj->printText = 0;
	obj->base.onTouchDown = NULL;
	obj->base.onTouchPress = NULL;
	obj->base.onTouchUp = NULL;

	obj->borderColor = barProgressCOLOR_BORDER;
	obj->backColor = defaultCOLOR_BACK;
	obj->progressColor = barProgressCOLOR_HIGHLIGHT;
	obj->borderWidth = defaultBOARD_WIDTH;
	obj->font.type = defaultFONT;
	obj->font.color = defaultCOLOR_FONT;
	obj->min = 0;
	obj->max = 100;
	obj->value = 0;
	barProgress_Text(obj, NULL);

	visual_RegisterComponent(visualTYPE_tBarProgress, obj, (void*) barProgress_Paint); // registrar esse objeto para controle de repaint e eventos
}

void barProgress_SetPosition(pBarProgress obj, int x, int y, int w, int h) {
	obj->base.rect = asDim(x,y,w,h);
	obj->base.repaint = 1;
}

void barProgress_SetFontColor(pBarProgress obj, tColor color) {
	obj->font.color = color;
	obj->base.repaint = 1;
}

void barProgress_SetFont(pBarProgress obj, const GUI_FONT* font) {
	obj->font.type = *font;
	obj->base.repaint = 1;
}

void barProgress_SetMinMax(pBarProgress obj, int min, int max) {
	obj->min = min;
	obj->max = max;
	obj->base.repaint = 1;
}

int barProgress_SetValue(pBarProgress obj, int value) {
	if (value < obj->min) value = obj->min;
	else if (value > obj->max) value = obj->max;
	obj->value = value;
	obj->printPercent = 1;
	obj->base.repaint = 1;
	return value;
}

void barProgress_EnPrintBar(pBarProgress obj, int st) {
	obj->printBar = st;
	obj->base.repaint = 1;
}

void barProgress_EnPrintPorcent(pBarProgress obj, int st) {
	obj->printPercent = st;
	obj->base.repaint = 1;
}

void barProgress_EnPrintText(pBarProgress obj, int st) {
	obj->printText = st;
	obj->base.repaint = 1;
}

// adciona o texto para ser impreso na barra
void barProgress_Text(pBarProgress obj, pchar text) {
	sformat(obj->font.text, "%s", text);
	obj->printText = (text)?1:0;
	obj->base.repaint = 1;
}

// torna ou não o objeto visivel
void barProgress_SetVisible(pBarProgress obj, int v) {
	obj->base.visible = v;
	obj->base.repaint = 1;
}

int barProgress_Visible(pBarProgress obj) {
	return obj->base.visible;
}


// força uma repitagem da barra
void barProgress_Repaint(pBarProgress obj) {
	obj->base.repaint = 1;
}

// método para pintar
void barProgress_Paint(pBarProgress obj) {
  	GUI_MEMDEV_Handle   hMem;
  	GUI_MEMDEV_Handle   hMemOld;
  	const GUI_RECT    * pClipOld;
  	GUI_RECT rClip;

	if (!obj->base.repaint) return;
	obj->base.repaint = 0;

   	// Somente pintamos o fundo se o objeto for invisivel
   	if (!obj->base.visible) return;

	rClip = obj->base.rect;
 	pClipOld = GUI_SetClipRect(&rClip); // Sets the clipping rectangle used for limiting the output.
 		// The clipping area is limited to the configured (virtual) display size per default.
		// Under some circumstances it can be useful to use a smaller clipping rectangle, which
		// can be set using this function. The rectangle referred toshould remain unchanged
		// until the function is calledagain with a NULL pointer.

  	// Create & select memory device
  	hMem    = GUI_MEMDEV_CreateEx(rClip.x0, rClip.y0, rClip.x1, rClip.y1, GUI_MEMDEV_NOTRANS); //Creates a Memory Device.
  	hMemOld = GUI_MEMDEV_Select(hMem); // Selects a Memory Device as target for drawing operations.

	// Somente pintamos o fundo se o objeto for invisivel
   	//if (obj->base.visible) {
		GUI_SetPenSize(obj->borderWidth);
		GUI_SetBkColor(obj->backColor);
	//} else GUI_SetBkColor(defaultCOLOR_BACKGROUND);
	GUI_Clear(); // limpa com a cor de fundo

    //if (obj->base.visible) {
		// Desenhar o barProgress
		GUI_SetColor(obj->borderColor);
		GUI_AA_DrawRoundedRectEx(&obj->base.rect, barProgressROUNDED_RAY); // Draws the outline of an antialiased rectangle with rounded corners.

		int porcent = ((float)obj->value/((float)(obj->max-obj->min)))*100;

		// imprimir a barra
		if ( obj->printBar ) {
			GUI_RECT lBar = obj->base.rect;
			lBar.x0+=(obj->borderWidth/2);
			lBar.y0+=(obj->borderWidth/2);
			lBar.y1-=(obj->borderWidth/2);

			lBar.x1 = lBar.x0 + (( ((float)(porcent)) * ((float)(lBar.x1-lBar.x0)) )/100);
			lBar.x1-=(obj->borderWidth/2);

			if (lBar.x1 - lBar.x0 > 0) {
				GUI_SetColor(obj->progressColor);
				//GUI_AA_DrawRoundedRectEx(&lBar, barProgressROUNDED_RAY); // Draws the outline of an antialiased rectangle with rounded corners.
				GUI_AA_FillRoundedRectEx(&lBar, barProgressROUNDED_RAY); //Draws a filled antialiased rectangle with rounded corners
			}
		}

		// imprimir texto se tiver
		if ( (obj->printPercent) || (obj->printText) ) {
			GUI_SetColor(obj->font.color);
			GUI_SetFont(&obj->font.type);
			GUI_SetTextMode(GUI_TM_TRANS);
			int x = rClip.x0 + ((rClip.x1 - rClip.x0) + 1) / 2; // Encontrar o centro X do botão
			int y = (rClip.y0 + ((rClip.y1 - rClip.y0 -GUI_GetFontDistY()) + 1) / 2); // Encontrar o centro Y do botão, considerando a largura da fonte
			string s;
			if ( (obj->printPercent) && (obj->printText) ) sformat(s, "%s-%d%%", obj->font.text, porcent);
			else if (obj->printPercent) sformat(s, "%d%%", porcent);
			else  sformat(s, "%s", obj->font.text);

			GUI_DispStringHCenterAt(s, x, y);
		}
	//}

	// Draw and delete memory device
  	GUI_MEMDEV_Select(hMemOld); // Selects a Memory Device as target for drawing operations.
  	GUI_MEMDEV_Write(hMem); // Writes the content of a Memory Device into the currently selected device considering the alpha channel.
  	GUI_MEMDEV_Delete(hMem); // Deletes a Memory Device.
  	GUI_SetClipRect(pClipOld); // Sets the clipping rectangle used for limiting the output.
}
