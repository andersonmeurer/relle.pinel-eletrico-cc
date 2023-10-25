#ifndef __GMENU_H
#define __GMENU_H

#include "_config_cpu_.h"
#include "_config_lib_gmenu.h"
typedef struct {
	string caption;			// Texto do item de menu a ser impresso
	int line;				// ID da linha para orientar o gerenciador de impress�o
	unsigned repaint:1;		// Sinalizador se este item de menu foi mudado e necessita ser repintado
} tPrintCaption, *pPrintCaption;

// MENSAGENS GERADAS PELO GMENU
#define MSG_NOP          	0 // O gerenciador emite essa mensagem para checar se existe ou n�o um determinado item de menu
#define MSG_MOUNT      		1 // Mensagem emitida para fun��o de controle durante a montagem do novo menu seja pela fun��o gmenu_Init ou pela fun��o gmenu_MountMenu:
#define MSG_ONSHOW     		2 // Mensagem emitida para fun��o de controle ap�s a montagem do novo menu seja pela fun��o gmenu_Init ou pela fun��o gmenu_MountMenu:
#define MSG_PROC			3 // Esta mensagem invocada atrav�s da fun��o gmenu_Process que geralmente � usada em um loop infinito da aplica��o
#define MSG_CLOSE     		4 // Caso foi feito uma montagem de menu atrav�s da fun��o gmenu_MountMenu o gMenu invoca a essa mensagem para fun��o de controle
#define MSG_UNMOUT     		5 // Caso foi feito uma montagem de menu atrav�s da fun��o gmenu_MountMenu o gMenu invoca a essa mensagem para fun��o de controle
#define MSG_ISMENU			6 // O Gerenicar invoca essa mensagem perguntando ao item de menu se ele � um sub menu ou n�o
#define MSG_CAPTION      	7 // O gerenciador invoca essa mensagem solicitando a aplica��o a retornar com o t�tulo do item de menu

// MENSAGENS GERADAS PELA APLICA��ES
#define MSG_CLICK        	8 // Mensagem usada sinalizando que o item de menu foi selecionado
#define MSG_ONKEYDOWN      	9 // Mensagem emitida no momento que uma tecla fora pressionada
#define MSG_ONKEYPRESS		10 // Mensagem emitida durante que uma tecla se mantem pressionada
#define MSG_ONKEYUP        	11 // Mensagem emitida quando uma tecla foi solta
#define MSG_ALTPAR       	12 // Mensagem para altera��o de par�metros
#define MSG_HELP         	13 // Mensagem para exibir ajuda para os usu�rios
#define MSG_CAPSTS			14 // Mensagem para exibir status do sistema

// invocador de mensagem para o item de menu
typedef struct {
	unsigned isMenu: 1; 		// Sinaliza se o item atual se trata de um menu ou n�o
	u32 index;					// Sinalizador que item de menu � enviada a mensagem
	pchar s; 					// Buffer para data in/out
	u32 msg; 					// Mensagem passado para o item de menu
	u32 msgPar; 				// Parametro da mensagem
} tMenuMSG, *pMenuMSG;

// ponteiro fun��o de menu
// rotina de controle de um menu.
//  result: 0=achou o item, 1=nao achou o item
typedef int (*pFuncMenu)(pMenuMSG m);

// estrutura do menu montado
typedef struct {
	string caption;				// Caption do item de menu mountado
	pPrintCaption printCaption [GMENU_N_PRINT_CAPTION];	// Numero de slots para impress�o dos itens de menu
	void* menu;					// Apontador para o menu. N�o � obrigat�rio o que importa mesmo � a fun��o de controle
	pFuncMenu funcCtrl; 		// Apontador da fun��o de controle do menu montado
	int indexItem;				// Apontadir do item atual na fun��o de controle do menu montado
	int totalItems;				// Conta a quantidade de itens de menu h� no menu montado, incluindo o item 0
	tMenuMSG perform; 			// Estrutura do invocador de mensagem para item de menu

	// Pilha para salvar e restaura os sub menu
	struct {
		void* menu[GMENU_STACK_SIZE];			// Menus
		pFuncMenu funcCtrl[GMENU_STACK_SIZE]; 	// Fun��es de controles dos menus
		int indexItem[GMENU_STACK_SIZE];		// Posi��es dos menus
		int count;								// gerenciador da pilha
	} stack;
} tMenu;

extern tMenu gMenu;

void gmenu_Init(void* _menu, pFuncMenu funcMenu);
void gmenu_AddPrintCaption(pPrintCaption printCaption);
void gmenu_Process(int execMsgProc);
void gmenu_MountMenu(void* _menu, pFuncMenu funcMenu);
void gmenu_UnMountMenu(void);
int gmenu_isMainMenu(void);
int gmenu_isMenu(pFuncMenu funcMenu, int index);
int gmenu_isItemMenu(pFuncMenu funcMenu, int index);
void gmenu_ProcSubMenu(pMenuMSG m, pFuncMenu funcMenu);
int gmenu_Perform(int msg, int index, char* s, int msgPar);

#endif
