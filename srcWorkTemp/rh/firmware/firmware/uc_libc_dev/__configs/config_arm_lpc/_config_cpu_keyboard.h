#ifndef __CONFIG_CPU_KEYBOARD_H
#define __CONFIG_CPU_KEYBOARD_H

#include "_config_cpu_.h"

// ###################################################################################################################
// KEYBOARD 4 IN
// 	Controlador de teclado de matrix de 4 entradas de dados e max 5 colunas (4x1, 4x2, ... , 4x5)

// definir porta de entrada de 4 bits. A porta deve ser a mesma para todos os bits,
//		e os mesmo deveser na sequencia (p1-p4, ou p2-p5, ...),
//	o lin1 LSB e Lin4 MSB
#define KEY4IN_PORTDATA GPIO0
	#define	KEY4IN_LIN1 BIT_6
	#define	KEY4IN_LIN2 BIT_7
	#define	KEY4IN_LIN3 BIT_8
	#define	KEY4IN_LIN4 BIT_9

// Definir a quantidade de colunas de controle, também defenir suas portas e seus bits
// Requer no minimo 1 coluna e no máximo 5 colunas
//	As colunas que não vão ser usadas colocar em comentário
#define KEY4IN_PORTCOL1 GPIO1
	#define	KEY4IN_COL1 BIT_10
#define KEY4IN_PORTCOL2 GPIO1
	#define	KEY4IN_COL2 BIT_14
#define KEY4IN_PORTCOL3 GPIO1
	#define	KEY4IN_COL3 BIT_15
#define KEY4IN_PORTCOL4 GPIO4
	#define	KEY4IN_COL4 BIT_29
#define KEY4IN_PORTCOL5 GPIO4
	#define	KEY4IN_COL5 BIT_28

#endif
