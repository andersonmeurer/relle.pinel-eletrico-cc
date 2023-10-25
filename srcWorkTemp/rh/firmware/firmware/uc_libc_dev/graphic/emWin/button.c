#include "visual.h"

void button_Create(pButton obj, pchar text, pTouchFunc onTouchDown, pTouchFunc onTouchPress, pTouchFunc onTouchUp) {
	obj->base.enabled = 1;
	obj->base.visible = 1;
	obj->base.repaint = 1;
	obj->base.touched = 0;
	obj->base.onTouchDown = onTouchDown;
	obj->base.onTouchPress = onTouchPress;
	obj->base.onTouchUp = onTouchUp;
	button_Text(obj, text);

	obj->borderWidth = defaultBOARD_WIDTH;
	obj->borderColor = defaultCOLOR_BORDER;
	obj->backColor = defaultCOLOR_BACK;
	obj->font.type = defaultFONT;
	obj->font.color = defaultCOLOR_FONT;

	visual_RegisterComponent(visualTYPE_tButton, obj, (void*) button_Paint); // registrar esse objeto para controle de repaint e eventos
}

void button_SetPosition(pButton obj, int x, int y, int w, int h) {
	obj->base.rect = asDim(x,y,w,h);
	obj->base.repaint = 1;
}

void button_SetFontColor(pButton obj, tColor color) {
	obj->font.color = color;
	obj->base.repaint = 1;
}

void button_SetFont(pButton obj, const GUI_FONT* font) {
	obj->font.type = *font;
	obj->base.repaint = 1;
}

// troca o texto do botão
void button_Text(pButton obj, pchar text) {
	sformat(obj->font.text, "%s", text);
	obj->base.repaint = 1;
}

// habilita ou não o botão
void button_Enabled(pButton obj, int en) {
	obj->base.enabled = en;
	obj->base.repaint = 1;
}

// torna ou não o objeto visivel
void button_SetVisible(pButton obj, int v) {
	obj->base.visible = v;
	obj->base.repaint = 1;
}

int button_Visible(pButton obj) {
	return obj->base.visible;
}

// força uma repitagem dos botões
void button_Repaint(pButton obj) {
	obj->base.repaint = 1;
}

// método para pintar o botão
void button_Paint(pButton obj) {
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

	GUI_SetPenSize(obj->borderWidth);

	if (obj->base.enabled) GUI_SetBkColor(obj->backColor); else GUI_SetBkColor(buttonCOLOR_BACK_DISABLED);
	GUI_Clear(); // limpa com a cor de fundo

	// Desenhar o botão
	if (obj->base.enabled) GUI_SetColor(obj->borderColor); else GUI_SetColor(buttonCOLOR_BACK_DISABLED);
	if (obj->base.touched)
			GUI_AA_FillRoundedRectEx(&obj->base.rect, buttonROUNDED_RAY); //Draws a filled antialiased rectangle with rounded corners
	else 	GUI_AA_DrawRoundedRectEx(&obj->base.rect, buttonROUNDED_RAY); // Draws the outline of an antialiased rectangle with rounded corners.

	// imprimir texto se tiver
	if (obj->font.text) {
		if (obj->base.enabled) GUI_SetColor(obj->font.color); else GUI_SetColor(buttonCOLOR_FONT_DISABLED);
		GUI_SetFont(&obj->font.type);
		int x = rClip.x0 + ((rClip.x1 - rClip.x0) + 1) / 2; // Encontrar o centro X do botão
		int y = (rClip.y0 + ((rClip.y1 - rClip.y0 -GUI_GetFontDistY()) + 1) / 2); // Encontrar o centro Y do botão, considerando a largura da fonte
		if (obj->base.touched)
				GUI_SetTextMode(GUI_TM_XOR);
		else 	GUI_SetTextMode(GUI_TM_TRANS);
		GUI_DispStringHCenterAt(obj->font.text, x, y);
	}

	// Draw and delete memory device
  	GUI_MEMDEV_Select(hMemOld); // Selects a Memory Device as target for drawing operations.
  	GUI_MEMDEV_Write(hMem); // Writes the content of a Memory Device into the currently selected device considering the alpha channel.
  	GUI_MEMDEV_Delete(hMem); // Deletes a Memory Device.
  	GUI_SetClipRect(pClipOld); // Sets the clipping rectangle used for limiting the output.
}
