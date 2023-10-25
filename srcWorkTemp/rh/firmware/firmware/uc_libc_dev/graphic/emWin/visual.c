#include "visual.h"
#include <stdlib.h>

struct stComponents {
	int order;
	int type;
	void* obj;
	pTouchFuncPaint funcPaint;
	struct stComponents* nextComponent;
};

static int visualRun = pdFALSE; // flag para gerenciar ou não
GUI_COLOR colorBackground;
typedef struct stComponents tListComponents;
static tListComponents firstComponent;
static tListComponents* actualComponentToAdd; // ponteiro para o componente atual na lista para facilitar adicionamento do próximo componente a lista

GUI_RECT asDim(I16 x, I16 y, I16 w, I16 h) {
 	GUI_RECT a;
 	a.x0 = x; a.y0 = y;
	a.x1 = x+w; a.y1 = y + h;
 	return a;
}

// Inicializa o gerenciador visual
void visual_Init(GUI_COLOR clrBackground) {
	colorBackground = clrBackground;
	GUI_Init();
  	GUI_SetBkColor(colorBackground);
 	GUI_Clear();

	// Sinalizar que não há nenhum componente para controle
	visualRun = pdFALSE; // sinaliza para não gerenciar ainda
	firstComponent.order = 0;
	firstComponent.obj = NULL;
	firstComponent.funcPaint = NULL;
	firstComponent.nextComponent = NULL;
	actualComponentToAdd = NULL;
}

// adicionar um novo componente para controle de eventos
int visual_RegisterComponent(int type, void* obj, pTouchFuncPaint funcPaint) {
	if (firstComponent.obj == NULL) { // se não há nenhum componente adicionado para controle
		firstComponent.order = 1;	// Sinaliza primeiro componente a ser registrado
		firstComponent.type = type;
		firstComponent.funcPaint = funcPaint;
		firstComponent.obj = obj;
		firstComponent.nextComponent = NULL; // Slot vazio para o próximo componente
		actualComponentToAdd = &firstComponent; // Guardar o endereço do slot atual da lista
		return pdPASS;
	}

	tListComponents* new = malloc(sizeof(tListComponents)); // Aloca um novo slot
	if (new == NULL) return pdFALSE; // Retorna false se o gerenciador de memória não alocou memória

	actualComponentToAdd->nextComponent = new; // Guardar no slot atual o endereço do próximo slot
	new->order = actualComponentToAdd->order + 1;
	new->type = type;
	new->obj = obj;
	new->funcPaint = funcPaint;
	new->nextComponent = NULL; // Slot vazio para o próximo componente
	actualComponentToAdd = new; // Guardar o endereço do slot atual da lista
	return pdPASS;
}

// Se a posiçao XY estiver sobre o componente retornar pdTRUE, senão retornar pdFALSE
static int visual_TouchOver(pBase obj, int x, int y) {
	obj->touched = pdFALSE;
	if (!obj->visible) return pdFALSE;
	if (!obj->enabled) return pdFALSE;

	if (x < obj->rect.x0) return pdFALSE;
	if (y < obj->rect.y0) return pdFALSE;
	if (x > obj->rect.x1) return pdFALSE;
	if (y > obj->rect.y1) return pdFALSE;

	obj->touched = pdTRUE;
	return pdTRUE;
}

void visual_Run(int v) {
	visualRun = v;
}

int touchFastInc; // sinaliza para o processos externos para incrementar ou decrementar de forma mais rápida quando a tecla ficar pressionada
extern tTime now (void);

// processo do gerenciador de objetos visuais
// Será controlado o repaint dos objetos e eventos de touch
void visual_Process(void) {
	if (!visualRun) return;

	tListComponents* c = &firstComponent;
	tListComponents* cTouched = NULL;
	GUI_PID_STATE state;

	// fazer para todos os objetos registrados na lista de componentes para controle
	do {
		c->funcPaint(c->obj); // plotagem do objeto no visor

		// checa se o componente atual foi tocado
		if (GUI_PID_GetState(&state) && (cTouched == NULL)) {
			if (c->type == visualTYPE_tButton) {
				pButton bt = c->obj;
				if (visual_TouchOver(&bt->base, state.x, state.y)) cTouched = c;
			} else if (c->type == visualTYPE_tBarProgress) {
				pBarProgress bar = c->obj;
				if (visual_TouchOver(&bar->base, state.x, state.y)) cTouched = c;
			} else if (c->type == visualTYPE_tLabel) {
				pLabel lbl = c->obj;
				if (visual_TouchOver(&lbl->base, state.x, state.y)) cTouched = c;
			}
		}
		c = c->nextComponent; // Ir para o próximo componente da lista
	} while(c != NULL);

	//if (cTouched) lprintf("bt %d touched"CMD_TERMINATOR, cTouched->order);
	static pBase base;
	// capturar a base do componente tocado
	if (cTouched) {
		if (cTouched->type == visualTYPE_tButton) {
			pButton bt = cTouched->obj;
			base = &bt->base;
		} else if (cTouched->type == visualTYPE_tLabel) {
			pLabel lbl = cTouched->obj;
			base = &lbl->base;
		}
	}

	// processar os eventos touchDown, touchPress e touchUp
	static int estado=0;
	static tListComponents* cTouchedNow=NULL;
	static tListComponents* cTouchedOld=NULL;
    static tTime time=0;
    static uint repeat=0;
    static int id = -1;

	cTouchedOld = cTouchedNow;
    cTouchedNow = cTouched;

    switch (estado) {
    case 0: 									// Estado de verificação se tecla foi pressionada
    	repeat = touchREPEAT;						// A cada X segundos incrementos o touchFastInc
		touchFastInc = 0;						// Sinaliza para não incrementar de modo rápido
    	if ((cTouchedOld==NULL)&&(cTouchedNow)) {			// Checa se é a descida da tecla, se sim emitir o evento onTouchDown e onTouchPress
            if (base) {
				id = cTouched->order;
				if (base->onTouchDown) base->onTouchDown(id);		// Emitir o evento onTouchDown e onTouchPress
				if (base->onTouchPress) base->onTouchPress(id);	// Emitir o evento onTouchPress
				if (cTouched->type == visualTYPE_tButton) {
					base->repaint = 1;
					base->touched = 1;
				}
			}

			time = now() + touchPRESS_REPEAT1;	// Determina em que tempo vamos repetir o evento onTouchPress após a primeira vez
    		estado = 1;							// Vá para o estado de verificação de tecla precionada
    	}
    	break;
    case 1: 									// Estado de verificação se a tecla esta sendo pressionada ou tecla foi solta
    	if ((cTouchedOld)&&(cTouchedNow==NULL)) {			// Checa se é a subida da tecla, se sim emitir o evento onKeyUp
    		if (base) {
				if (base->onTouchUp) base->onTouchUp(id);
				if (cTouchedOld->type == visualTYPE_tButton) {
					base->repaint = 1;
					base->touched = 0;
				}
			}
    		estado = 0;							// Volta ao estado para esperar a próxima tecla a ser pressionada
    	} else if (now()>time) {				// Checa se está na hora de repetir o evento onTouchPress
            time = now() + touchPRESS_REPEAT2; // Determina em que tempo vamos repetir o evento onTouchPress sucessivamente quando a tecla for mantida pressionada
            if ( (base) && (base->onTouchPress) ) base->onTouchPress(id);	// Chama o evento onTouchPress

            // checar o momento de incrementar a var FastInc
            if (repeat) repeat--;
            else {
            	repeat = touchREPEAT;
            	touchFastInc++;
            }
        }
    	break;
    }
}
