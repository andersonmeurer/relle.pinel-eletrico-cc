#ifndef __GMENU_H
#define __GMENU_H

#include "_config_cpu_.h"
#include "_config_lib_gmenu.h"
typedef struct {
	string caption;			// Texto do item de menu a ser impresso
	int line;				// ID da linha para orientar o gerenciador de impressão
	unsigned repaint:1;		// Sinalizador se este item de menu foi mudado e necessita ser repintado
} tPrintCaption, *pPrintCaption;

// MENSAGENS GERADAS PELO GMENU
#define MSG_NOP          	0 // O gerenciador emite essa mensagem para checar se existe ou não um determinado item de menu
#define MSG_MOUNT      		1 // Mensagem emitida para função de controle durante a montagem do novo menu seja pela função gmenu_Init ou pela função gmenu_MountMenu:
#define MSG_ONSHOW     		2 // Mensagem emitida para função de controle após a montagem do novo menu seja pela função gmenu_Init ou pela função gmenu_MountMenu:
#define MSG_PROC			3 // Esta mensagem invocada através da função gmenu_Process que geralmente é usada em um loop infinito da aplicação
#define MSG_CLOSE     		4 // Caso foi feito uma montagem de menu através da função gmenu_MountMenu o gMenu invoca a essa mensagem para função de controle
#define MSG_UNMOUT     		5 // Caso foi feito uma montagem de menu através da função gmenu_MountMenu o gMenu invoca a essa mensagem para função de controle
#define MSG_ISMENU			6 // O Gerenicar invoca essa mensagem perguntando ao item de menu se ele é um sub menu ou não
#define MSG_CAPTION      	7 // O gerenciador invoca essa mensagem solicitando a aplicação a retornar com o título do item de menu

// MENSAGENS GERADAS PELA APLICAÇÕES
#define MSG_CLICK        	8 // Mensagem usada sinalizando que o item de menu foi selecionado
#define MSG_ONKEYDOWN      	9 // Mensagem emitida no momento que uma tecla fora pressionada
#define MSG_ONKEYPRESS		10 // Mensagem emitida durante que uma tecla se mantem pressionada
#define MSG_ONKEYUP        	11 // Mensagem emitida quando uma tecla foi solta
#define MSG_ALTPAR       	12 // Mensagem para alteração de parâmetros
#define MSG_HELP         	13 // Mensagem para exibir ajuda para os usuários
#define MSG_CAPSTS			14 // Mensagem para exibir status do sistema

// invocador de mensagem para o item de menu
typedef struct {
	unsigned isMenu: 1; 		// Sinaliza se o item atual se trata de um menu ou não
	u32 index;					// Sinalizador que item de menu é enviada a mensagem
	pchar s; 					// Buffer para data in/out
	u32 msg; 					// Mensagem passado para o item de menu
	u32 msgPar; 				// Parametro da mensagem
} tMenuMSG, *pMenuMSG;

// ponteiro função de menu
// rotina de controle de um menu.
//  result: 0=achou o item, 1=nao achou o item
typedef int (*pFuncMenu)(pMenuMSG m);

// estrutura do menu montado
typedef struct {
	string caption;				// Caption do item de menu mountado
	pPrintCaption printCaption [GMENU_N_PRINT_CAPTION];	// Numero de slots para impressão dos itens de menu
	void* menu;					// Apontador para o menu. Não é obrigatório o que importa mesmo é a função de controle
	pFuncMenu funcCtrl; 		// Apontador da função de controle do menu montado
	int indexItem;				// Apontadir do item atual na função de controle do menu montado
	int totalItems;				// Conta a quantidade de itens de menu há no menu montado, incluindo o item 0
	tMenuMSG perform; 			// Estrutura do invocador de mensagem para item de menu

	// Pilha para salvar e restaura os sub menu
	struct {
		void* menu[GMENU_STACK_SIZE];			// Menus
		pFuncMenu funcCtrl[GMENU_STACK_SIZE]; 	// Funções de controles dos menus
		int indexItem[GMENU_STACK_SIZE];		// Posições dos menus
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
