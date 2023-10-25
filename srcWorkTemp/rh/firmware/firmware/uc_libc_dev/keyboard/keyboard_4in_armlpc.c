#include "keyboard_4in_armlpc.h"

// ###########################################################################################
// TECLADO DE 4 ENTRADAS E X COLUNAS
// ###########################################################################################
//	Este driver requer que o teclado tenha 4 entradas (LIN) na sequencia (p1-p4, ou p2-p5, ...),
//		e no minimo uma saida de controle (COL) para leitura das entradas

// Se desejarmos ler a primeira coluna devemos colocar nível 1 nela e as demais em nível 0
// Quando não há nenhum botão pressionado os valores de LIN são todos 1
// Se há alguma tecla pressionada a linha correspondente a tecla ficará em nível 0
// Se há mais de uma tecla pressionada será considerada a tecla da última coluna e última linha

// 	LIN0 <--------|x|--|x|--	... 	|x|
//	LIN1 <--------|x|--|x|--	... 	|x|
// 	LIN2 <--------|x|--|x|--	... 	|x|
//	LIN3 <--------|x|--|x|--	...		|x|
//				   ^	^    			 ^
// 				   |    |    			 |
// 	COL1 ----------|    |                |
// 	COL2 ---------------|                |
//                                       |
// 	COL5 --------------------------------|

// ----------------------------------------------------------------------------------
// INICIALIZA O CONTROLADOR DE LEITURA DO TECLADO
// ----------------------------------------------------------------------------------
void key4In_Init(void) {
	keyIn4.portData = &KEY4IN_PORTDATA;

	keyIn4.nCol=1; // Sinaliza que o teclado tem pelo menos uma coluna
	keyIn4.portCol1 = &KEY4IN_PORTCOL1;
	*keyIn4.portCol1 |= KEY4IN_COL1; 				// pino para saida
	*(regCpu*)(keyIn4.portCol1+0x6) = KEY4IN_COL1; 	// ligar pino

	#if defined KEY4IN_PORTCOL2
	keyIn4.nCol++;
	keyIn4.portCol2 = &KEY4IN_PORTCOL2;
	*keyIn4.portCol2 |= KEY4IN_COL2; 				// pino para saida
	*(regCpu*)(keyIn4.portCol2+0x7) = KEY4IN_COL2; 	// desligar pino
	#endif
	#if defined KEY4IN_PORTCOL3
	keyIn4.nCol++;
	keyIn4.portCol3 = &KEY4IN_PORTCOL3;
	*keyIn4.portCol3 |= KEY4IN_COL3; 				// pino para saida
	*(regCpu*)(keyIn4.portCol3+0x7) = KEY4IN_COL3; 	// desligar pino
	#endif
	#if defined KEY4IN_PORTCOL4
	keyIn4.nCol++;
	keyIn4.portCol4 = &KEY4IN_PORTCOL4;
	*keyIn4.portCol4 |= KEY4IN_COL4; 				// pino para saida
	*(regCpu*)(keyIn4.portCol4+0x7) = KEY4IN_COL4; 	// desligar pino
	#endif
	#if defined KEY4IN_PORTCOL5
	keyIn4.nCol++;
	keyIn4.portCol5 = &KEY4IN_PORTCOL5;
	*keyIn4.portCol5 |= KEY4IN_COL5; 				// pino para saida
	*(regCpu*)(keyIn4.portCol5+0x7) = KEY4IN_COL5; 	// desligar pino
	#endif
}

// ------------------------------------------------------------------------------------
// Leitura do teclado, chamar essa função através do tick de sistema entre 1ms a 20ms
// ------------------------------------------------------------------------------------
int key4In_Read(void) {
	static regCpu* r;
	static char n=0, cod=0, col=0;

	// ler barramentos de dados
	r = (regCpu*)(keyIn4.portData+0x5);// de GPIOx_DIR para GIPOx_pin = 0x14 dividiremos por 4 porque endereço é em 32 bits
	if ( (*r&KEY4IN_LIN1) == 0) n = col*4 + 1;
	if ( (*r&KEY4IN_LIN2) == 0)	n = col*4 + 2;
	if ( (*r&KEY4IN_LIN3) == 0)	n = col*4 + 3;
	if ( (*r&KEY4IN_LIN4) == 0)	n = col*4 + 4;

	// Apontar para a próxima coluna e checar se é o fim da leitura
	if (++col >= keyIn4.nCol) {
		col=0;	cod=n;	n=0;
	}

	// colocar todas as colunas em alta
	*(regCpu*)(keyIn4.portCol1+0x7) = KEY4IN_COL1; 	// desligar pino
	#if defined KEY4IN_PORTCOL2
	*(regCpu*)(keyIn4.portCol2+0x7) = KEY4IN_COL2; 	// desligar pino
	#endif
	#if defined KEY4IN_PORTCOL3
	*(regCpu*)(keyIn4.portCol3+0x7) = KEY4IN_COL3; 	// desligar pino
	#endif
	#if defined KEY4IN_PORTCOL4
	*(regCpu*)(keyIn4.portCol4+0x7) = KEY4IN_COL4; 	// desligar pino
	#endif
	#if defined KEY4IN_PORTCOL5
	*(regCpu*)(keyIn4.portCol5+0x7) = KEY4IN_COL5; 	// desligar pino
	#endif

	// ajustar a coluna de leitura
	if (col == 0) *(regCpu*)(keyIn4.portCol1+0x6) = KEY4IN_COL1; 		// ligar pino
	#if defined KEY4IN_PORTCOL2
	else if (col == 1) *(regCpu*)(keyIn4.portCol2+0x6) = KEY4IN_COL2; 	// ligar pino
	#endif
	#if defined KEY4IN_PORTCOL3
	else if (col == 2) *(regCpu*)(keyIn4.portCol3+0x6) = KEY4IN_COL3; 	// ligar pino
	#endif
	#if defined KEY4IN_PORTCOL4
	else if (col == 3) *(regCpu*)(keyIn4.portCol4+0x6) = KEY4IN_COL4; 	// ligar pino
	#endif
	#if defined KEY4IN_PORTCOL5
	else if (col == 4) *(regCpu*)(keyIn4.portCol5+0x6) = KEY4IN_COL5; 	// ligar pino
	#endif

	//return keymap[cod];
	return cod;
}
