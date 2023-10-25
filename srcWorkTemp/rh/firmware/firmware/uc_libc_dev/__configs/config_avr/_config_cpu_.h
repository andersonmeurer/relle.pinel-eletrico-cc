#ifndef __CONFIG_CPU_H
#define __CONFIG_CPU_H
/*

// STACK POINT (SP)
	Por padrão o stack pointer é apontando para no último endereço 
		da memória interna do micro controlador. Este endereço final 
		de memória é indicado pela definição RAMEND no “arquivo .h” 
		de cada micro controlador Ex iom128.h

	O deslocamento do SP é possível se feito no makefile pela 
		instrução -minit-stack=nnnn

// CUIDADOS COM AS INTERRUPÇÕES
	- As interrupções globais são desligadas automaticamente 
		quando a função da interrupção é atendida, e volta a 
		ser ligada automaticamente no fim da execução da função 
		já com o flag de da interrupção limpo;
	- Quando acessar variáveis que sejam compartilhadas com uma 
		interrupção desligue a mesma seguido de um comando NOP, 
		no fim do processo volte a ligar a interrupção;
	- Quando usar processos críticos desligue as interrupções, 
		e no fim do processo volte a ligá-las.
	- Cuidado para não acontecer overrun, que é a incapacidade do 
		processador terminar de processar uma interrupção e a mesma 
		já é requisitada novamente pelo novo evento. Exemplo: Em uma 
		transferência serial para o uC com uma taxa rapidíssima, o 
		processador não tem tempo de tirar o byte recebido do registrador 
		da UART para um FIFO antes que chegue um novo byte pela UART. 
		Neste caso houve um overrun com perda dos novos bytes;
	- Cuidado para não acontecer overflow em uma FIFO, ou seja, 
		a frequência da gravação na FIFO dentro de uma interrupção seja 
		maior que a leitura de dados em um processo fora da interrupção vindo 
		a estourar a FIFO. Exemplo: Em uma transferência serial para o uC com 
		uma taxa muito rápida, e que a interrupção de recepção de dados, onde 
		os dados recebidos sejam adicionados na FIFO, é invocada muito mais vezes 
		do que a quantidade de vezes que um processo externo a interrupção possa 
		retirar esse dados. Em algum momento a FIFO não vai ter mais espaço 
		ocorrendo um overflow e o novos dados recebidos serão perdidos.
*/

#include <avr/io.h>
#include "_config_lib_.h"

#endif
