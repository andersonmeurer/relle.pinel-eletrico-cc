/*
Cada módulo terão os sinais dos leds compartilhados e cada módulo terá um sinal dedicado para o seu sinal comum.
Logo, a conexão entre a CPU com os módulos é feito em duas partes.
	Parte 1: Conexão utilizada para os 7 segmentos, 
		deve usar a mesma porta e não necessariamente deve ser na sequência de bits
	Parte 2: Conexão para o acionamento de cada módulo,
		deve usar a mesma porta e na sequência de bits como bit0, bit1 ... ou bit5, bit6, ...

Estrutura de cada módulo de 7 segmentos
           		   A (led1)
       		  -----------------
			|					|
			|					|
   F (led6) |					| B (led2)
       		|					|
       		|	  G (led7)		|
       		 ------------------
			|					|
			|					|
   E (led5) |					| C (led3)
       		|					|
       		|					|
       		 ------------------
				  D (led4)           . DP (led8)

	u8 module[DISP_N_MODULE];
		bits 7  6  5  4  3  2  1  0
		led  1  2  3  4  5  6  7  8
		seg  A  B  C  D  E  F  G  DP

	ASCII		A B C D  E F G DP	Valor a ser escrito em disp7seg_WriteSeg
	NULL (0)	0 0 0 0  0 0 0 X  	0x0
	- (45)		0 0 0 0  0 0 1 X	0x2
	0 (48)		1 1 1 1  1 1 0 X  	0xfc
	1 (49)		0 1 1 0  0 0 0 X	0x60
	2 (50)		1 1 0 1  1 0 1 X	0xda
	3 (51)		1 1 1 1  0 0 1 X	0xf2
	4 (52)		0 1 1 0  0 1 1 X 	0x66
 	5 (53)		1 0 1 1  0 1 1 X	0xb6
  	6 (54)		1 0 1 1  1 1 1 X	0xbe
	7 (55)		1 1 1 0  0 0 0 X	0xe0
	8 (56)		1 1 1 1  1 1 1 X	0xfe
 	9 (57)		1 1 1 1  0 1 1 X	0xf6
 	A (65)		1 1 1 0  1 1 1 X	0xee
	B (66)		0 0 1 1  1 1 1 X	0x3e
	C (67)		1 0 0 1  1 1 0 X	0x9c
	D (68)		0 1 1 1  1 0 1 X	0x7a
	E (69)		1 0 0 1  1 1 1 X	0x9e
	F (70)		1 0 0 0  1 1 1 X	0x8e
	G (71)		1 0 1 1  1 1 1 X	0xbe
	H (72)		0 1 1 0  1 1 1 X	0x6e
	I (73)		0 1 1 0  0 0 0 X	0x60
	J (74)		0 1 1 1  1 0 0 X	0x78
	L (76)		0 0 0 1  1 1 0 X	0x1c
	N (78)		0 0 1 0  1 0 1 X	0x2a
	O (79)		0 0 1 1  1 0 1 X	0x3a
	P (80)		1 1 0 0  1 1 1 X	0xce
	R (82)		0 0 0 0  1 0 1 X	0xa
	S (83)		1 0 1 1  0 1 1 X	0xb6
	T (84)		0 0 0 1  1 1 1 X	0x1e
	U (85)		0 1 1 1  1 1 0 X	0x7c
	Y (89)		0 1 1 1  0 1 1 X 	0x76
	_ (95)		0 0 0 1  0 0 0 X 	0x10
	¯ (175)		1 0 0 0  0 0 0 X 	0x80
	° (176)		1 1 0 0  0 1 1 X    0xC6
	º (186)		1 1 0 1  0 1 1 X    0xD6
*/


// Para informações de como usar essa libs veja comentários no fim do arquivo

#include "display7seg.h"
#include "display7seg_drv.h"

#if (DISP7SEG_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

static tDisp7Panel disp7Panel;

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa o processador para controlar os módulos de 7 segmentos
// Parametro:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void disp7seg_Init(void) {
	disp7seg_SetupPorts();
	disp7Panel.moduleRefresh = 0;

	// Inicializar o valor de exibição de todos os módulos
	int x;
	// for (x=0;x<DISP7SEG_N_MODULE;x++) disp7Panel.module[x] = DISP7SEG_SHOW_INIT;
	for (x=0;x<DISP7SEG_N_MODULE;x++) disp7seg_SetValue(x, DISP7SEG_SHOW_INIT);
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o valor a ser exibido no módulo especifico
// Parametros:	module: Módulo a ser adicioando o valor a ser exibido, valor de 0 a DISP7SEG_N_MODULE-1
//				value: Valor em ASCII para exibição no módulo, valor NULL (0) apaga todos os leds do módulo
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void disp7seg_SetValue(u8 module, u8 value) {
	if (module >= DISP7SEG_N_MODULE) return;

	// VALORES SEG VER FIM DO ARQUIVO
	// adicionar valor e proteger o ponto
	u8* m = &disp7Panel.module[module];
	*m &= 0x1; // Desliga todos os leds do 7 -- seg NULL (0)

	switch (value) {
	case 45: value = 0x2; break;	//	-
	case 48: value = 0xfc; break;	//	0
	case 49: value = 0x60; break;	//	1
	case 50: value = 0xda; break;	//	2
	case 51: value = 0xf2; break;	//	3
	case 52: value = 0x66; break;	//	4
	case 83: 						//  S
	case 53: value = 0xb6; break;	// 	5
	case 54: value = 0xbe; break;	// 	6
	case 55: value = 0xe0; break;	//	7
	case 56: value = 0xfe; break;	//	8
	case 57: value = 0xf6; break;	// 	9
	case 65: value = 0xee; break;	// 	A
	case 66: value = 0x3e; break;	//	B
	case 67: value = 0x9c; break;	//	C
	case 68: value = 0x7a; break;	//	D
	case 69: value = 0x9e; break;	//	E
	case 70: value = 0x8e; break;	//	F
	case 71: value = 0xbe; break;	//	G
	case 72: value = 0x6e; break;	//	H
	case 73: value = 0x60; break;	//	I
	case 74: value = 0x78; break;	//	J
	case 76: value = 0x1c; break;	//	L
	case 78: value = 0x2a; break;	//	N
	case 79: value = 0x3a; break;	//	O
	case 80: value = 0xce; break;	//	P
	case 82: value = 0xa; break;	//	R
	case 84: value = 0x1e; break;	//	T
	case 85: value = 0x7c; break;	//	U
	case 89: value = 0x76; break;	//	Y
	case 95: value = 0x10; break;	//	_
	case 175: value = 0x80; break;	//	¯
	case 176: value = 0xc6; break;	//	°
	case 186: value = 0xd6; break;	//	º
	default: value = 0;
	}

	*m |= value;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Liga ou desliga o ponto do módulo.
// Parametros:	module: Módulo a ser adicioando o valor a ser exibido, valor de 0 a DISP7SEG_N_MODULE-1
//				action: pdON liga o ponto e pdOFF desliga o ponto
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void disp7seg_SetDP(u8 module, u8 action) {
	if (module >= DISP7SEG_N_MODULE) return;
	// Ligar o deligar o DP e protegendo o valor do 7seg
	if (action == pdON) disp7Panel.module[module] |= 0x01; else disp7Panel.module[module] &= 0xfe;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Liga ou desliga os leds do módulo.
// Parametros:	module: Módulo a ser adicioando o valor a ser exibido, valor de 0 a DISP7SEG_N_MODULE-1
//				seg: sinaliza qual o segmento a ser ligado ou deligado o módulo
//					seg  		1  2  3  4  5  6  7  8
//					led  		1  2  3  4  5  6  7  8
//					segmento  	A  B  C  D  E  F  G  DP
//				action: pdON liga o led e pdOFF desliga o led
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void disp7seg_SetSeg(u8 module, u8 seg, u8 action) {
	if (module >= DISP7SEG_N_MODULE) return;
	seg--;
	if (action == pdON)
			disp7Panel.module[module] |= (1<<(7-seg));
	else	disp7Panel.module[module] &= ~(1<<(7-seg));
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Função para ser chamada periodicamente externamente para fazer o controle
//					de exibição dos módulos de display de 7 segmentos.
//					TEMPO RECOMENDADO
//					Dependendo da quantidade de módulos a gerenciar o tempo máximo entre ticks varia
//						para 3 módulos valores de 0.25ms a 4ms funciona bem
//							tempo superior a 4 ms gera flinks, e tempos muito inferior a 1ms pode deixar os leds com
//							iluminação fraca
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void disp7seg_Tick(void) {
	// com o trecho de código abaixo damos uma
	//	static int count=0;
	//	count++;
	//	if (count%2==0) {
	//		disp7seg_SetModule(-1);
	//		return;
	//	}

	// OBS: desse modo não demos um intervalo de ascender um modulo para outro, desse modos alguns leds
	//	podem ascender de for quase imperceptivel. Colocando o trecho de código acima existe esse intervalo
	//	porém a intensidade dos leds caem bastante

	disp7seg_SetModule(disp7Panel.moduleRefresh);					// Ativa o módulo
	disp7seg_WriteSeg(disp7Panel.module[disp7Panel.moduleRefresh]); // Imprime o conteúdo do módulo


	if (disp7Panel.moduleRefresh < DISP7SEG_N_MODULE-1)
			disp7Panel.moduleRefresh++;
	else	disp7Panel.moduleRefresh = 0;
}
