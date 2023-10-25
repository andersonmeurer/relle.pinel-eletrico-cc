#ifndef __CONFIG_LIB_GMENU_H
#define __CONFIG_LIB_GMENU_H

#define GMENU_PRINT_TITLE	pdOFF	// pdON = Exibe o titulo do menu no primeiro label
									// pdOFF= Libera o primeiro label para exibir o item do menu
#define GMENU_SHOW_POS		pdOFF	// pdON = Exibe quantidade de itens e qual selecionado
									// pdOFF = Desliga posições de itens de menu

#define GMENU_N_PRINT_CAPTION	1 	// Quantidade de labels de exibição dos itens de menu
#define GMENU_STACK_SIZE 		2 	// Profundidade (stack) gerenciador deve suportar (níveis de submenu)
#define GMENU_MAX_ELEMENTS		10	// Máximo de itens permitido no menu

#endif
