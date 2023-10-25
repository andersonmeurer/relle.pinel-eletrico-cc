#ifndef __GMENU_CANVAS_H
#define __GMENU_CANVAS_H

#include "_config_cpu_.h"
#include "_config_lib_gmenu.h"
#if (GMENU_USE_CANVAS == pdON)
	#include "canvas_obj.h"
	typedef tPanel tPrintCaption;
	typedef pPanel pPrintCaption;
#else
	typedef struct {
		string caption;			// Texto do label
		int line;
		unsigned repaint:1;
	} tPrintCaption, *pPrintCaption;
#endif

#define gmenu_CaptionRepaint gmenu_Paint

// TIPOS DE MENSSAGENS
//	PRIMEIRA TABULAÇÃO SÃO A MENSAGENS QUE INVOCAREMOS COM perform NA APLICAÇÃO
//	SEGUNDA TABULAÇÃO SÃO OS EVENTOS CHAMADAS PELA OCORRENCIA DA CHAMADA DA PRIMEIRA TABULAÇÃO
// EVENTOS DE CRIAÇÃO DO MENU
#define MSG_CLICK        	13  // Invoca o menu com mensagem clique no item
	#define MSG_MOUNT      	14  // Evento chamado antes de montar o menu com seus itens
	#define MSG_CREATE     	20 	// Evento chamado antes da criação dos subitens do menu
	#define MSG_ONSHOW     	11  // Evento chamado após a criação do menu (entrada)

// EVENTOS PARA USAR O MENUS
#define MSG_ONKEYDOWN      	15	// Evento de tecla pressionada
#define MSG_ONKEYPRESS		17	// Evento de tecla sendo pressionada
#define MSG_ONKEYUP        	16	// Evento de tecla sendo solta
#define MSG_PROC			22 	// Evento quando usado a função gmenu_Process
#define MSG_ALTPAR       	19	// Evento para alteração de parâmetros
#define MSG_HELP         	21	// Evento para tratar mecanismo de ajuda para os usuários
#define MSG_CAPSTS			24 	// Evento para tratar mecanismo para trabalhar com status do sistema
#define MSG_CAPTION      	10  // Evento para retorna o título do menu ou nome do item

// EVENTOS DE SAIDA
#define MSG_CLOSE        	18  // Evento para desmontar o menu atual e voltar ao menu anterior caso existir
	#define MSG_ONEXIT     	12  // Evento chamado antes da desmontagem do menu (saida)

// CONTROLE INTERNO
#define MSG_NOP          	0	// Nenhuma mensagem
#define MSG_ISMENU			23 	// Mensagem para que o item de menu é menu ou item normal

// menu
// ----------------------------------------------------------------------------------
// menssagem passada para os itens dos menus
typedef struct {
	unsigned isMenu: 1; 		// Sinaliza se o item é ou não um sub menu
	unsigned isaction: 1; 		// possui algum tipo de ação?
	unsigned iswarning: 1; 		// mostrar como alerta?
	u32 index;					// Sinalizador que item de menu é enviada a mensagem
	pchar s; 					// Buffer para data in/out
	u32 msg; 					// Mensagem passado para o item de menu
	u32 msgPar; 				// Parametro da mensagem
	//tColor tgcolor;
} tMenuMSG, *pMenuMSG;

// ponteiro função de menu??????
//rotina de controle de um menu.
//result: 0=achou item, 1=nao achou item
typedef int (*pMenu)(pMenuMSG m);

typedef struct {
	//unsigned fastinc: 1; 	// troca de parametro rapido
	string caption;			// Caption do item de menu atual
	int count;				// Conta a quantidade de itens de menu há no menu
	int index;				// Seleção do item de menu para processamento. Nunca recebe valor 0 porque pertence ao TITULO
	int vindex;
	#if (GMENU_USE_CANVAS == pdON)
	int pnlTouched; 		// Sinaliza qual item de menu foi tocado pelo usuário no touch
	#endif
	//char repaint;
	//char clearance; 	// espaçamento entre os menus

	pPrintCaption printCaption [GMENU_N_PRINT_CAPTION];	// Panels que o gerenciador de menus deve controlar na tela
	pMenu actual; 			// Aponta para o menu atual
	pMenu new;				// Sinaliza qual é o próximo menu a ser entrado
	tMenuMSG perform; 		// Estrutura do item de menu

	// Estrutura para salvar os menus quando entramos em submenus
	// para podermos retorna ao menu anterior
	struct {
		pMenu menu[GMENU_STACK_SIZE]; // Ponteiro dos menus
		int index[GMENU_STACK_SIZE];
		int count;
	} stack;
} tMenu;

extern tMenu menu;

void gmenu_Init(pMenu _menu);
void gmenu_AddPrintCaption(int index, pPrintCaption printCaption);
void gmenu_Process(void);
int gmenu_Perform(pMenu _menu, char* s, int index, int msg, int msgPar);
void gmenu_ChangeMenu(int PushPop);
	#define GMENU_CREATE 		-1
	#define GMENU_EXIT   		0
	#define GMENU_ENTER   		1
void gmenu_Repaint(void);
void gmenu_ProcMenu(pMenuMSG m, pMenu _menu);

#endif
