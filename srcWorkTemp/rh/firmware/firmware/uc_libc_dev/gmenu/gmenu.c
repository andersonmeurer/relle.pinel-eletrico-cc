#include "gmenu.h"
#include "stdio_uc.h"

tMenu gMenu;
extern void gmenu_Paint(pPrintCaption obj);
typedef void (*pfunv)(void);
static void* newMenu;
static pFuncMenu newFuncMenu;

static int gmenu_PerformPar(pFuncMenu funcMenu, int msg, int index, char* s, int msgPar);
static void gmenu_ChangeMenu(int action);
	#define GMENU_CREATE 		-1
	#define GMENU_UNMOUNT  		0
	#define GMENU_MOUNT   		1
static void gmenu_PushStack(void);
static void gmenu_PopStack(void);
static int gmenu_GetCaption(pchar s, int index);
static void gmenu_CaptionPrintAdjust(pPrintCaption printCaption, int index);
static void gmenu_Repaint(void);

// ###################################################################################
// INICIALIZA��O

// Inicializa o gerenciador de menus
// 	Parametro _menu: Ponteiro do menu atual. N�o � obrigat�rio � s� um auxiliar para o menu, caso n�o usar coloque NULL, o que importa mesmo � a fun��o de controle do menu atual
// 	Parametro funcMenu: Ponteiro da fun��o de controle do menu atual a ser montado
void gmenu_Init(void* _menu, pFuncMenu funcMenu) {
    gMenu.stack.count = 0; 			// Sinaliza que n�o h� nenhum menu salvo no stack
    newMenu = _menu;				// Aponta para menu atual
    newFuncMenu = funcMenu;			// Aponta a fun��o de controle do menu atula que ser� montado
    gmenu_ChangeMenu(GMENU_CREATE); // Cria o menu atual
}

// Adiciona ao gerenciador de menus os locais onde ser�o exibidos os itens de menu
void gmenu_AddPrintCaption(pPrintCaption printCaption) {
	static int idx = 0;
	if (idx == GMENU_N_PRINT_CAPTION) return;
	gMenu.printCaption[idx] = printCaption;
 	gMenu.printCaption[idx]->repaint = 0;
 	gMenu.printCaption[idx]->line = idx+1;	// coloca o ID da linha para que o gerenciador de impress�o se oriente na impress�o
 	idx++;
}

// ###################################################################################
// MANIPULADORES PARA A APLICA��O

// Essa fun��o sempre deve ser usada pelo loop principal do programa para o gerenciamento do menu atual
// Parametro execMsgProc: pdTRUE emite uma mensagem PROC para a fun��o de controle,
//		se n�o processamento de mensagens PROC nas fun��es de controle coloque pdFALSE
void gmenu_Process(int execMsgProc) {
	gmenu_Repaint(); // Imprime itens do menu caso houve mudan�as
	// Enviar uma mensagem de processamento para o menu atual
	if (execMsgProc == pdTRUE) gmenu_PerformPar(gMenu.funcCtrl, MSG_PROC, 0, NULL, 0);
}

// 	Esta fun��o � para simplificar a programa��o das fun��es de controle, esta fun��o processa os itens de menu que s�o sub menus
// 	Parametro 1: gerenciador de menu
// 	Parametro 2: ponteiro da fun��o de controle do item de menu a ser analisado
void gmenu_ProcSubMenu(pMenuMSG m, pFuncMenu funcMenu) {
	m->isMenu = 1; 				// Sianaliza que esse menu em an�lise deve ser tratado como menu

    if (m->msg == MSG_CAPTION) {	// Checa se � um pedido de caption
    	m->index = 0;				// Sinaliza para apontar para o item de a��o do menu
    	funcMenu(m);				// Executa a fun��o de controle do menu em an�lise para pegar o seu caption
    }
}

// Invoca a fun��o de controle do item de menu atual montado para processar uma determinada mensagem
int gmenu_Perform(int msg, int index, char* s, int msgPar) {
	return gmenu_PerformPar(gMenu.funcCtrl, msg, index, s, msgPar);
}

// Invoca a fun��o de controle do item de menu atual montado para processar uma determinada mensagem
//		Mesmo que o item de menu n�o processe essa mensagem, o memso deve responde ao gerenciador que existe tal intem (retorna 0)
// 	N�O HAVER� PROBLEMAS SE N�O H� MENU MONTADO
// 	parametro funcMenu: Ponteiro da fun��o de controle de menu a ser a ser processado
// 	parametro msg: 		Mensagem que vai ser passado para a fun��o de menu atual
//							Se n�o usar coloque MSG_NOP
// 	parametro Index: 	�ndice do item de menu que deseja acessar para coleta de informa��es, ou para entrar em um submenu.
//							Se n�o usar coloque 0
// 	parametro s:		String de dados para uso geral, serve tanto para enviar dados como para retornar.
//							Se n�o usar coloque NULL
// 	parametro msgPar: 	Dados para uso geral, geralmente usado para levar informa��es para a fun��o de controle.
//							Se n�o usar coloque 0
static int gmenu_PerformPar(pFuncMenu funcMenu, int msg, int index, char* s, int msgPar) {
	string cap;
    pMenuMSG m = &gMenu.perform;

    if (funcMenu==NULL) return 1;

    // Incializa as vari�veis de controle do menu atual, para novo trabalho
    m->index = index;
    m->isMenu = 0;				// Sinaliza que � este item de menu n�o � um sub menu
    //m->isAction = 0;
    //m->isWarning = 0;
    m->s = (s==NULL)?cap:s; //se s est� vazio vamos colocar ums atring tempor�ria para usar no funcMenu(m)
    m->s[0] = '\r'; // porque \r e n�o \0 ????????
    m->msg = msg;
    m->msgPar = msgPar;

    int r = funcMenu(m); 				// Chama a fun��o de controle do menu

	if (msg==MSG_ISMENU) return m->isMenu;
    if (msg==MSG_CAPSTS) return m->s[0]!='\r';

    return r;
}

// ###################################################################################
// TROCA DE MENU

// Procedimento de montagem do novo menu
// 	Parametro _menu: Ponteiro do menu atual. N�o � obrigat�rio � s� um auxiliar para o menu, caso n�o usar coloque NULL, o que importa mesmo � a fun��o de controle do menu
// 	Parametro funcMenu: Ponteiro da fun��o de controle do menu a ser montado
void gmenu_MountMenu(void* _menu, pFuncMenu funcMenu) {
	newMenu = _menu;				// Aponta para o menu atual
	newFuncMenu = funcMenu;			// Aponta a nova fun��o de controle do menu atual
	gmenu_ChangeMenu(GMENU_MOUNT);  // Chama a fun��o para montagem do novo menu
}

// Procedimento de desmontagem do menu atual
void gmenu_UnMountMenu(void) {
	gmenu_ChangeMenu(GMENU_UNMOUNT);
}

// Essa fun��o cria o primeiro menu ou faz a troca entre menus, seja entrar ou sair entre sub menus
// Parametros: 	GMENU_CREATE 	Usado somemte na cria��o do menu atual
//				GMENU_MOUNT   	Sinaliza para sair do menu atual e entrar em um sub menu
//				GMENU_UNMOUNT  	Sinaliza para sair do menu atual e retornar ao menu anterior
static void gmenu_ChangeMenu(int action) {
	if (action != GMENU_CREATE) {
       	// Enviar uma mensagem de fechamento para o menu atual. Isto para ele processar qualquer coisa l� no momento de sua sa�da
        gmenu_PerformPar(gMenu.funcCtrl, MSG_CLOSE, 0, NULL, 0);

		// Enviar uma mensagem de saida para o menu atual. Isto para ele processar qualquer coisa l� no momento de sua destrui��o
        gmenu_PerformPar(gMenu.funcCtrl, MSG_UNMOUT, 0, NULL, 0);

       	// se for entrar em um novo menu, vamos salvar o menu atual no STACK, sen�o vamos restaurar o menu anterior do stack
        if (action==GMENU_MOUNT) gmenu_PushStack(); else gmenu_PopStack();
    } else
    	gMenu.indexItem = 1; // Seleciona o item de menu padr�o compensando o iten de a��o do menu (processamento de mensagens)

	// se o menu atual foi restaurado da pilha n�o devemos apontar o novo menu
    if (action!=GMENU_UNMOUNT) {
		gMenu.menu = newMenu;				//Aponta menu
		gMenu.funcCtrl = newFuncMenu;     	// Aponta a nova fun��o de controle do menu atual
    }

    // Verificar quantos itens de menu h� no menu atual, incluindo o item 0
    gMenu.totalItems = 0;
    while (gmenu_isItemMenu(gMenu.funcCtrl, gMenu.totalItems)) {  	// Checa se existe o item de menu apontado por menu.totalItems, se retornar 0 � porque n�o mais itens
	   	if (gMenu.totalItems++ < GMENU_MAX_ELEMENTS) continue;		// Conta quanto itens no menu
		// 	showmsg("muitos itens no menu"); 						// Emitir aviso de erro fatal de programa��o no painel
    	break;
    }

	// Enviar uma mensagem de montagem para o menu atual. Isto para ele processar qualquer coisa l� no momento de sua cria��o
    gmenu_PerformPar(gMenu.funcCtrl, MSG_MOUNT, 0, NULL, 0);
	// Enviar uma mensagem de exibi��o para o menu atual. Isto para ele processar qualquer coisa l� no momento de sua exibi��o
    gmenu_PerformPar(gMenu.funcCtrl, MSG_ONSHOW, 0, NULL, 0);

    // sinaliza para exibir todos os itens de menu no viso incluindo o t�tulo
    int i; for (i=0; i<GMENU_N_PRINT_CAPTION; i++)
    	gMenu.printCaption[i]->repaint = 1;
}

// ###################################################################################
// AUXILIAR

// Retorna TRUE se o menu atual � o principal, sen�o retorna FALSE sinalizando que estamos em um sub menu
int gmenu_isMainMenu(void) {
	return (gMenu.stack.count == 0); //	Se n�o h� nada na pilha de menus � porque estamos no menu principal
}

// Retorna TRUE se o indece do menu � um menu, sen�o � um item de menu padr�o
int gmenu_isMenu(pFuncMenu funcMenu, int index) {
    return gmenu_PerformPar(funcMenu, MSG_ISMENU, index, NULL, 0);
}

// Checa se existe o item de menu
// Parametro 1: ponteiro da fun��o de controle do item de menu a ser analisado
// Parametro 2: indece do menu a ser analisado
// Retorna 1 se existe o item de menu analisado, sen�o retorna 0 sinalizando que n�o existe o item de menu
int gmenu_isItemMenu(pFuncMenu funcMenu, int index) {
    // envia uma mensagem nula para fun��o de controle para ver se a mesma responde se h� ou n�o o determiado item
    return (gmenu_PerformPar(funcMenu, MSG_NOP, index, NULL, 0) == 0);
}


// ###################################################################################
// STACK DOS MENUS

// Salva o menu atual na pilha
static void gmenu_PushStack(void) {
    gMenu.stack.menu[gMenu.stack.count] = gMenu.menu;
    gMenu.stack.funcCtrl[gMenu.stack.count] = gMenu.funcCtrl;
    gMenu.stack.indexItem[gMenu.stack.count] = gMenu.indexItem;
    gMenu.stack.count++;
    gMenu.indexItem = 1; // Seleciona o item de menu padr�o compensando o iten de a��o do menu (processamento de mensagens)
}

// Recupera o menu da pilha para se tornar o menu atual
static void gmenu_PopStack(void) {
    if (gMenu.stack.count <= 0) return;
    gMenu.stack.count--;
    gMenu.menu = gMenu.stack.menu[gMenu.stack.count];
    gMenu.funcCtrl = gMenu.stack.funcCtrl[gMenu.stack.count];
    gMenu.indexItem = gMenu.stack.indexItem[gMenu.stack.count];
}

// ###################################################################################
// IMPRIMIR ITENS DE MENU

// Retorna com o caption do item de menu atrav�s do parametro s
// Se o indece do item de menu � v�lido retorna true, sen�o retorna false
static int gmenu_GetCaption(pchar s, int index) {
    // fora dos limites? item invalido?
    if (index >= gMenu.totalItems) {
    	sformat(s," ");
    	return pdFALSE;
    }

    // Invocar o menu para devolver o seu caption
    gmenu_PerformPar(gMenu.funcCtrl, MSG_CAPTION, index, gMenu.caption, 0);

	// Formatar caption de acordo do tipo de item de menu
	if (index == 0) {			// Checa se � o item de a��o do menu
   		#if (GMENU_SHOW_POS == pdON)
   		sformat(s, "%s (%d/%d)", gMenu.caption, gMenu.indexItem, gMenu.totalItems-1); // formata o t�tulo sinalizando posi��es dos menus e seus respectivos itens
   		#else
   		sformat(s, "%s", gMenu.caption); // retorna somente o t�tulo do caption
   		#endif
       	return pdTRUE;
    }

    // Demais itens de menu
    // char tag;
    // if (menu.perform.isMenu) tag = '>'; else tag = ' '; // se o item � um menu coloque o simbolo '>'
	// sformat(s,"%2d%c%s", index, tag, menu.caption);
	#if (GMENU_SHOW_POS == pdON)
	if (gMenu.perform.isMenu)					// Checa se o item de menu � um submenu
		sformat(s, "%d>%s", index, gMenu.caption); // formata o t�tulo sinalizando posi��es dos menus e seus respectivos itens
	else
	#endif
		sformat(s, "%s", gMenu.caption); // retorna somente o t�tulo do caption

    return pdTRUE;
}

// prepara o estado do menu a ser pintado
static void gmenu_CaptionPrintAdjust(pPrintCaption printCaption, int index) {
	string s;
	gmenu_GetCaption(s, index); 			// retorna o caption do item de menu atual

	if (strcmp(s, printCaption->caption)) {	// Checa se o texto diferente do menu o que foi impresso
		strcpy(printCaption->caption, s);	// Copia o texto do menu para ser empreso
		printCaption->repaint = 1;			// Sinaliza para pintar
	}
}

// preenche as linhas com itens de menu para ser exibidos no visor
static void gmenu_Repaint(void) {
	int vindex; // posi��o inicial do item a ser exibido

	// >>>>>> ATEN��O
	// O index 0 sempre � reservado para o titulo, indepenedente se vamos imprimir ou n�o
	// gMenu.indexItem aponta para o item atual do menu, e nesta fun��o nunca vem com o valor 0

	#if (GMENU_PRINT_TITLE == pdOFF)
	vindex = gMenu.indexItem;

	int i;
	for (i=0; i<GMENU_N_PRINT_CAPTION;i++) {
		if (gMenu.printCaption[i] == NULL) continue;
		gmenu_CaptionPrintAdjust(gMenu.printCaption[i], i+vindex); 	// Ajusta a condi��o de exibi��o do item de menu
		gmenu_Paint(gMenu.printCaption[i]);		// Pinta item de menu
	}
	#endif

	#if (GMENU_PRINT_TITLE == pdON)
	vindex = gMenu.indexItem-1;
	// imprime o titulo do menu
	gmenu_CaptionPrintAdjust(gMenu.printCaption[0], 0); 	// Ajusta a condi��o de exibi��o do item de menu
	gmenu_Paint(gMenu.printCaption[0]);			// Pinta item de menu

	int i;
	for (i=1; i<GMENU_N_PRINT_CAPTION;i++) {
		if (gMenu.printCaption[i] == NULL) continue;
		gmenu_CaptionPrintAdjust(gMenu.printCaption[i], i+vindex); 	// Ajusta a condi��o de exibi��o do item de menu
		gmenu_Paint(gMenu.printCaption[i]);		// Pinta item de menu
	}
	#endif
}
