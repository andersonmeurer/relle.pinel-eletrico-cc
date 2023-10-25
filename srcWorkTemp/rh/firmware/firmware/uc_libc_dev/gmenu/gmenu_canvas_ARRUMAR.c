/*

ATEN��O
COLOCAR O QUE � DE CANVAS NO ARQUIVO DE MENU E USAR O gmenu.c

_config_lib_gmenu.h OLD

#ifndef __CONFIG_LIB_GMENU_H
#define __CONFIG_LIB_GMENU_H

#define GMENU_N_PRINT_CAPTION	2 			// Quantidade de labels de exibi��o dos itens de menu
#define GMENU_STACK_SIZE 		2 			// Profundidade (stack) gerenciador deve suportar (n�veis de submenu)
#define GMENU_MAX_ELEMENTS		6			// M�ximo de itens usados nos menus e submenus

#define GMENU_USE_CANVAS pdOFF				// pdON o gmenu vai usar o tPanels do canvas
	// defini��es das cores dos panels caso usar canvas
	#define COLOR_MENUTITLE_BORDER 		clWhite
	#define COLOR_MENUTITLE_BK			clNavy
	#define COLOR_MENUTITLE_TEXT		clWhite

	#define COLOR_MENU_BORDER 			clBlue
	#define COLOR_MENU_BORDER_SELECTED 	clWhite
	#define COLOR_MENU_BK				clBlack
	#define COLOR_MENU_BK_SELECTED		clAqua
	#define COLOR_MENU_TEXT 			clWhite
	#define COLOR_MENU_TEXT_SELECTED 	clBlack

#endif


*/


#include "gmenu_canvas.h"
#include "stdio_uc.h"

int gmenu_GetCaption(pchar s, int index);
int gmenu_isItemMenu(pMenu _menu, int index);
int gmenu_isMenu(pMenu _menu, int index);
int gmenu_isMainMenu(void);

#if (GMENU_USE_CANVAS == pdOFF)
// fornecer externamete o mecanismo de refresh
extern void gmenu_Paint (pPrintCaption obj);
#endif


typedef void (*pfunv)(void);

// VARI�VEIS
// -------------------------------------------------------------------------------------------
tMenu menu;

// PROT�TIPOS
// -------------------------------------------------------------------------------------------
static void gmenu_PushStack(void);
static void gmenu_PopStack(void);
static void gmenu_CaptionPrintAdjust(pPrintCaption printCaption, int index);
static void gmenu_MountMenu(pMenu _menu);

// ###################################################################################
// GMENU - GERENCIADOR DE MENUS
// ###################################################################################
// Inicializa o gerenciador de menus e dertemina as condi��es iniciais e o primeiro
// 		menu a controlar
void gmenu_Init(pMenu _menu) {
    //menu.HelpMsg = 0;
    menu.index = 1;		// Seleciona o item de menu padr�o. Compensar o itens especiais
    	// menu.index = GMENU_N_SPECIAL_ITENS; // Seleciona o item de menu padr�o. Compensar o itens especiais
    	// #define GMENU_N_SPECIAL_ITENS	1		// Define quanto itens de menu s�o especiais
    menu.vindex = 0;
    menu.new = _menu; // aponta que � o primeiro menu
    menu.stack.count = 0; // sinaliza que n�o h� nenhum menu salvo no stack de menus

	#if (GMENU_USE_CANVAS == pdON)
    menu.pnlTouched = -1;
    #endif

    //menu.repaint = 0;
    //menu.ptshow = 0;
    //menu.pbvisible = 0;
    //menu.scale = 0;
}

// adiciona ao gerenciador de menus quais panels de menu ele deve gerenciar
void gmenu_AddPrintCaption(int index, pPrintCaption printCaption) {
	menu.printCaption[index] = printCaption;
 	#if (GMENU_USE_CANVAS == pdOFF)
 	menu.printCaption[index]->repaint = 0;
 	menu.printCaption[index]->line = index+1;	// Sinaliza qual linha do LCD pertence esse printCaption, isto � �til para o repaint
 	#endif
}

// Invoca o menu para processamento
void gmenu_Process(void) {
	gmenu_Perform(menu.actual, NULL, 0, MSG_PROC, 0); // Invoca o menu para processamento
}

// A fun��o perform invoca o menu atual para processamento de mensagem
//		Todos os itens de menu devem estar preparado para responder as essas mensagens ou ignor�-las

// 	parametro menu: 	Ponteiro do menu a ser a ser processado, aloe cont�m o indece do item de menu que vai processar a mensagem
// 	parametro s:		String de dados para uso geral, serve tanto para eviar dados para o menu como para retornar. Se n�o usar coloque NULL
// 	parametro Index: 	�ndice do item de menu que deseja acessar para coleta de informa��es, ou para entrar em um submenu. Se n�o usar coloque 0
// 	parametro msg: 		Mensagem que vai ser passado para o menu atual que tipo de a��o ele deve tomar. Se n�o usar coloque MSG_NOP
// 	parametro par: 		parametro da mensagem a ser passado para o menu atual a processar. Se n�o usar coloque 0
int gmenu_Perform(pMenu _menu, char* s, int index, int msg, int msgPar) {
	string cap;
    pMenuMSG m = &menu.perform;

    if (_menu==NULL) return 1;

    // Incializa as vari�veis de controle do menu atual, para novo trabalho
    m->index = index;
    m->isMenu = 0;				// Sinaliza que � este item de menu n�o � um sub menu
    m->isaction = 0;
    m->iswarning = 0;
    //m->tgcolor = clNone;
    m->s = (s==NULL)?cap:s; //se s est� vazio vamos colocar ums atring tempor�ria para usar no _menu(m)
    m->s[0] = '\r'; // porque \r e n�o \0 ????????
    m->msg = msg;
    m->msgPar = msgPar;

    int r = _menu(m); 				// Executa o menu de fato com seus itens

	if (msg==MSG_ISMENU) return m->isMenu;
    if (msg==MSG_CAPSTS) return m->s[0]!='\r';

    return r;
}

// Salva (adiciona) o menu atual na pilha
static void gmenu_PushStack(void) {
    menu.stack.menu[menu.stack.count] = menu.actual;
    menu.stack.index[menu.stack.count] = menu.index;
    menu.stack.count++;
    menu.index = 1;
   	// menu.index = GMENU_N_SPECIAL_ITENS; // Seleciona o item de menu padr�o. Compensar o itens especiais
   	// #define GMENU_N_SPECIAL_ITENS	1		// Define quanto itens de menu s�o especiais
}

// Recupera (retira) o menu da pilha para ser recriado
static void gmenu_PopStack(void) {
    if (menu.stack.count <= 0) return;
    menu.stack.count--;
    menu.new = menu.stack.menu[menu.stack.count];
    menu.index = menu.stack.index[menu.stack.count];
}

// faz a troca do menu, seja sair ou entrar em outro menu, e cria esse novo menu
void gmenu_ChangeMenu(int PushPop) {
	// Durante o boot � recebido um numero negativo para indicar que � o primeiro menu a ser montado
	//	Neste caso somente vamos criar o menu

	// Se estamos entrando em um submenu salva o menu atual no stack para poder retornar depois
	if (PushPop != GMENU_CREATE) { // checa se vamos entrar ou sair de um menu (submenu), e vamos fazer a troca
        //plognp("mudou de menu. ");
        //entrar ou sair do menu
        if (PushPop) gmenu_PushStack(); else gmenu_PopStack();
        //eh um menu valido?
        if (menu.new==NULL) return;
        //evento de saida do menu
        gmenu_Perform(menu.actual, NULL, 0, MSG_ONEXIT, 0); // Invoca o menu para ??????????????
    }

	// Cria o novo menu
    gmenu_Perform(menu.new, NULL, 0, MSG_CREATE, 0); // Invoca o menu para montagenm??? Se sim a mensagem deve ser MSG_MOUNT

    // Checar quantos itens de menu h� nesse menu
    menu.count = 0;
    while (gmenu_isItemMenu(menu.new, menu.count)) {  		// Checa quanto itens tem no menu
	   	if (menu.count++ < GMENU_MAX_ELEMENTS) continue;		// Registra quanto itens tem esse menu
		// 	showmsg("muitos itens no menu"); 			// emitir aviso de erro fatal de programa��o no painel
    	break;
    }

    // evento de entrada no menu
    gmenu_Perform(menu.new, NULL, 0, MSG_ONSHOW, 0); // Invoca o menu para ??????????

    //troca os menus
    menu.actual = menu.new; 	// Sinaliza o menu atual
    menu.new = NULL;			// Sinaliza que n�o h� mais troca de menu

    // sinaliza para pintar todos os printCaption do menu
    int i;
    for (i=0; i<GMENU_N_PRINT_CAPTION; i++)
    	menu.printCaption[i]->repaint = 1;
}

// retorna com o caption do item de menu pelo parametro s
// Se o indece do item de menu � v�lido retorna true, sen�o retorna false
//int gmenu_GetCaption(pchar s, int index, int pos) {
int gmenu_GetCaption(pchar s, int index) {
    //fora dos limites? item invalido?
    if (index >= menu.count) {
    	sformat(s," ");
    	return pdFALSE;
    }

    // INVOCAR O MENU PARA DEVOLVER O SEU CAPTION
    // 	O caption � armazenado no menu.caption
    gmenu_Perform(menu.actual, menu.caption, index, MSG_CAPTION, 0);

	// FORMATAR CAPTION DE ACORDO DO TIPO DE ITEM de MENU
	// ITEM TITULO
    if (index == 0) { 						// Checa se o item de menu � o titulo
   		#if (GMENU_USE_CANVAS == pdON)
   		sformat(s, "%s (%d/%d) - [Voltar]", menu.caption, menu.index, menu.count-1);
   		if (gmenu_isMainMenu()) { 			// Checa se o menu atual � o principal
    		sformat(s,"%s", menu.caption);	// Se for o menu principal n�o vamos formatar
    	} else								// Sen�o � um titulo de submenu, neste caso vamos mostrar comando de voltar e id do menu
		#else
   		#if (GMENU_EN_POS == pdON)
   		sformat(s, "%s (%d/%d)", menu.caption, menu.index, menu.count-1);
   		#else
   		sformat(s, "%s", menu.caption);
   		#endif
   		#endif
       	return pdTRUE;
    }

    // DEMAIS ITENS DE MENU
    //char tag;
    //if (menu.perform.isMenu) tag = '>'; else tag = ' '; // se o item � um menu coloque o simbolo '>'
	// sformat(s,"%2d%c%s", index, tag, menu.caption);
	#if (GMENU_EN_POS == pdON)
	if (menu.perform.isMenu)					// Checa se o item de menu � um submenu
		#if (GMENU_USE_CANVAS == pdON)
		sformat(s, "%s >>", menu.caption);
		#else
		sformat(s, "%d>%s", index, menu.caption);
		#endif
	else
	#endif
		sformat(s, "%s", menu.caption);


    return pdTRUE;
}

// ###################################################################################
// GMENU - AUXILIAR
// ###################################################################################
//proc_menu serve apenas para mostrar o caption do menu e trocar de menu caso
//seja clicado nele
static void gmenu_MountMenu(pMenu _menu) {
    //pode trocar de menu?
    if (gmenu_Perform(_menu, NULL, 0, MSG_MOUNT, 0) == 0) // Invoca o menu para ?????????? Ser� que � menu montado?
    	menu.new = _menu;
}

// Processa um item que � um submenu, caso precise entrar em um novo menu pela msh CLICK ser� feito
// 		Isto para simplicar a constru��o das estruturas de menus para n�o ficar repitindo essas fun��es
void gmenu_ProcMenu(pMenuMSG m, pMenu _menu) {
	m->isMenu = 1; // Sianaliza que esse elemento � um menu

	// click para trocar de menu
    if (m->msg == MSG_CLICK) {
    	gmenu_MountMenu(_menu);
    	return;
    }

	//caption do menu?
    if (m->msg == MSG_CAPTION) { // retorna com o caption caso o perform foi invocado por isso
    	m->index = 0;
    	_menu(m);				// Reentra neste menu para pegar seu caption (titulo)
    }
}

// Checa quanto itens de menu tem no menu
// Faz uma varredura do indece de menu at� chegar ao fim dele
// Retorna TRUE se o indece do menu cont�m um item de menu v�lido, sen�o o item n�o existe
int gmenu_isItemMenu(pMenu _menu, int index) {
    return (gmenu_Perform(_menu, NULL, index, MSG_NOP, 0) == 0);
}

// Retorna TRUE se o indece do menu � um menu, sen�o � um parametro a ser exibido ou ajustado
int gmenu_isMenu(pMenu _menu, int index) {
    return gmenu_Perform(_menu, NULL, index, MSG_ISMENU, 0);
}

// Retorna TRUE se o menu atual � o pricipal, sen�o retorna FALSE
//		sinalizando que estamos em um submenu
// Se n�o h� nada na pilha de menus � porque estamos no menu principal
int gmenu_isMainMenu(void) {
	return (menu.stack.count == 0);
}


// ###################################################################################
// GMENU - REPAINT
// ###################################################################################
void gmenu_Repaint(void) {
   	//if (!menu.repaint) return;
	//menu.repaint = 0; // sinaliza para n�o pintar mais o menu

	//offset do item visual para controle de pagina quando tem mais itens do que cabe na tela
	#if (GMENU_USE_CANVAS == pdON)
	menu.vindex = (menu.index/GMENU_N_PRINT_CAPTION) * (GMENU_N_PRINT_CAPTION - 1);
	// testar ainda no TFT ????????????
	#else
	menu.vindex = menu.index-1;
	#endif

	// IMPRIME O ITEM DE MENU TITULO
	gmenu_CaptionPrintAdjust(menu.printCaption[0], 0); 	// Ajusta a condi��o de exibi��o do item de menu
	gmenu_CaptionRepaint(menu.printCaption[0]);			// Pinta item de menu
	// se usar mais de um item de menu especial considerar GMENU_N_SPECIAL_ITENS; // Seleciona o item de menu padr�o. Compensar o itens especiais

	// IMPRIME OS ITENS DE MENU
	int i;
	for (i=1; i<GMENU_N_PRINT_CAPTION;i++) {
		if (menu.printCaption[i] == NULL) continue;
		gmenu_CaptionPrintAdjust(menu.printCaption[i], i+menu.vindex); 	// Ajusta a condi��o de exibi��o do item de menu
		gmenu_CaptionRepaint(menu.printCaption[i]);		// Pinta item de menu
	}
}

// prepara o estado do menu a ser pintado
static void gmenu_CaptionPrintAdjust(pPrintCaption printCaption, int index) {
	#if (GMENU_USE_CANVAS == pdON)
	tColor bkpanel = COLOR_MENU_BK;
	tColor bdpanel = COLOR_MENU_BORDER;
	tColor fgfont = COLOR_MENU_TEXT;

	//tColor bdc = clLtGray; //border color (borda)
	//tColor tgc = clLtGray; //tag color (marca)
	//int repaint = 0; //repintar tudo

	//pega o caption e verifica se o item existe
	string s;
	if (gmenu_GetCaption(s, index) == pdPASS) { // retorna o caption do item de menu atual, a func = true se encontrou
		if (index == menu.pnlTouched) {
			bkpanel = COLOR_MENU_BK_SELECTED;
			bdpanel = COLOR_MENU_BORDER_SELECTED;
			fgfont  = COLOR_MENU_TEXT_SELECTED;
		} else {
			if (index == 0) { //titulo?
				bkpanel = COLOR_MENUTITLE_BK; //por padrao eh verde
				bdpanel = COLOR_MENUTITLE_BORDER;
				//if (cabl.check) bkc = clDkYellow; //problema com as cabe�as?
				//if (fpga.check) bkc = clDkYellow; //problema com o altera?
				//if (!fpga.enabled) bkc = clMaroon; //sem altera?
			// manter com cor selecionado somente os itens de menu que n�o seja as fun��o especiais
			} else if ( (index > 2) && (index == menu.index) && (!gmenu_isMenu(menu.actual, index))) {
				bkpanel = COLOR_MENU_BK_SELECTED;
				bdpanel = COLOR_MENU_BORDER_SELECTED;
				fgfont  = COLOR_MENU_TEXT_SELECTED;
			}
		}
		//algum item especial com a��o?
		//if ( (menu.perform.isaction)||(index==0) ) fgfont = COLOR_MENUTITLE_TEXT;
		//mostrar tag?
		//if (menu.perform.tgcolor!=clNone) tgc = menu.perform.tgcolor;
		//item pediu para trocar cor do texto?
		//if ((now()/250)&1) //simula um pisca, pois hora pode hora nao pode trocar
			//if (menu.perform.iswarning) fgc = COLOR_BACKGROUND;
		// [borda]
		//bdc = clBlack;
	}

	//alguma delas vai mudar? manda repintar tudo...
	if (printCaption->bkcolor != bkpanel) {
		printCaption->bkcolor = bkpanel;
		printCaption->repaint = 1;
	}

	if (printCaption->bdcolor != bdpanel) {
		printCaption->bdcolor = bdpanel;
		printCaption->repaint = 1;
	}

	if ( (printCaption->label.color != fgfont) || (printCaption->label.bkcolor != bkpanel) ) {
		printCaption->label.color = fgfont;
		printCaption->label.bkcolor = bkpanel;
		printCaption->repaint = 1;
	}
	//if (printCaption->bdcolor != bdc) { printCaption->bdcolor = bdc; repaint = 1; }
	//if (printCaption->tgcolor != tgc) { printCaption->tgcolor = tgc; repaint = 1; }

	if (printCaption->repaint) {
		//if (strcmp(s, printCaption->label.caption) > 0) {
		panel_Caption(printCaption, s);
		//plognp("[%d]%s: fg[0x%x] bk[0x%x] pnl[0x%x]"CMD_TERMINATOR, index, s, printCaption->label.color, printCaption->label.bkcolor, printCaption->bkcolor);
		//} else plognp("[%d] %s rp norpcap"CMD_TERMINATOR, index, s);
	}// else plognp("[%d] %s norp norpcap"CMD_TERMINATOR, index, s);

	//repaint total?
	//if (menu.repaint) repaint = 1;

	//precisa repintar?
	//if (repaint) printCaption->repaint = 1;
	#else
	string s;
	gmenu_GetCaption(s, index); 		// retorna o caption do item de menu atual

	if (strcmp(s, printCaption->caption)) {	// Checa se o texto diferente do menu o que foi impresso
		strcpy(printCaption->caption, s);		// Copia o texto do menu para ser empreso
		printCaption->repaint = 1;				// Sinaliza para pintar
	}
	#endif
}

