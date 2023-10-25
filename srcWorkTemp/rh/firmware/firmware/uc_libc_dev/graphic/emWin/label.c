#include "visual.h"

void label_Create(pLabel obj, pchar text) {
	obj->base.repaint = 1;
	obj->base.enabled = 1; // por causa da verificação do touch no visual.c
	obj->base.touched = 0;
	obj->base.visible = 1;

	obj->printPanel = 0;
	obj->base.onTouchDown = NULL;
	obj->base.onTouchPress = NULL;
	obj->base.onTouchUp = NULL;

	obj->borderColor = defaultCOLOR_BACKGROUND;
	obj->backColor = defaultCOLOR_BACKGROUND;
	obj->borderWidth = defaultBOARD_WIDTH;
	obj->font.type = defaultFONT;
	obj->font.color = defaultCOLOR_FONT;
	obj->font.alignment = GUI_TA_HCENTER | GUI_TA_VCENTER;
	label_Text(obj, text);

	visual_RegisterComponent(visualTYPE_tLabel, obj, (void*) label_Paint); // registrar esse objeto para controle de repaint e eventos
}

void label_SetTouch(pLabel obj, pTouchFunc onTouchDown, pTouchFunc onTouchPress, pTouchFunc onTouchUp) {
	obj->base.onTouchDown = onTouchDown;
	obj->base.onTouchPress = onTouchPress;
	obj->base.onTouchUp = onTouchUp;
}

void label_SetPosition(pLabel obj, int x, int y, int w, int h) {
	obj->base.rect = asDim(x,y,w,h);
	obj->base.repaint = 1;
}

void label_SetFontColor(pLabel obj, tColor color) {
	obj->font.color = color;
	obj->base.repaint = 1;
}

void label_SetFont(pLabel obj, const GUI_FONT* font) {
	obj->font.type = *font;
	obj->base.repaint = 1;
}

void label_PrintPanel(pLabel obj, int st, uint borderWidth, tColor colorBorder, tColor colorBack) {
	obj->printPanel = st;
	obj->borderWidth = borderWidth;
	obj->borderColor = colorBorder;
	obj->backColor = colorBack;
	obj->base.repaint = 1;
}

// adciona o texto para ser impresso
// alignment tipo de alinhamento. Usar | entre alinhamento horizontal e vertical Ex: GUI_TA_HCENTER | GUI_TA_VCENTER);
		//Text alignment flags, horizontal
		//GUI_TA_HORIZONTAL
		//GUI_TA_LEFT
		//GUI_TA_RIGHT
		//GUI_TA_CENTER
		//GUI_TA_HCENTER
		//
		//Text alignment flags, vertical
		//GUI_TA_VERTICAL
		//GUI_TA_TOP
		//GUI_TA_BOTTOM
		//GUI_TA_BASELINE
		//GUI_TA_VCENTER
void label_Text(pLabel obj, pchar text) {
	sformat(obj->font.text, "%s", text);
	obj->base.repaint = 1;
}

void label_SetAlignment(pLabel obj, int alignment) {
	obj->font.alignment = alignment;
	obj->base.repaint = 1;
}

// torna ou não o objeto visivel
void label_SetVisible(pLabel obj, int v) {
	obj->base.visible = v;
	obj->base.repaint = 1;
}

int label_Visible(pLabel obj) {
	return obj->base.visible;
}

// força uma repitagem da barra
void label_Repaint(pLabel obj) {
	obj->base.repaint = 1;
}

// método para pintar
void label_Paint(pLabel obj) {
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

   	//if (obj->base.visible) {
   		GUI_SetBkColor(obj->backColor);
		GUI_SetColor(obj->borderColor);
	//} else GUI_SetBkColor(defaultCOLOR_BACKGROUND);
	GUI_Clear(); // limpa com a cor de fundo

   	//if (obj->base.visible) {
		if (obj->printPanel) {
			if (obj->borderWidth) {
				GUI_SetPenSize(obj->borderWidth);
				GUI_AA_DrawRoundedRectEx(&obj->base.rect, labelROUNDED_RAY); // Draws the outline of an antialiased rectangle with rounded corners.
			}
		}

		// imprimir texto se tiver
		if (obj->font.text) {
			GUI_SetColor(obj->font.color);
			GUI_SetFont(&obj->font.type);
			GUI_SetTextMode(GUI_TM_TRANS);
			GUI_DispStringInRect(obj->font.text, &obj->base.rect, obj->font.alignment);
		}
//	}

	// Draw and delete memory device
  	GUI_MEMDEV_Select(hMemOld); // Selects a Memory Device as target for drawing operations.
  	GUI_MEMDEV_Write(hMem); // Writes the content of a Memory Device into the currently selected device considering the alpha channel.
  	GUI_MEMDEV_Delete(hMem); // Deletes a Memory Device.
  	GUI_SetClipRect(pClipOld); // Sets the clipping rectangle used for limiting the output.
}
