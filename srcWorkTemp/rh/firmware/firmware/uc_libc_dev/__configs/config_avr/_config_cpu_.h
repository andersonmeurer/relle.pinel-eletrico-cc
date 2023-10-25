#ifndef __CONFIG_CPU_H
#define __CONFIG_CPU_H
/*

// STACK POINT (SP)
	Por padr�o o stack pointer � apontando para no �ltimo endere�o 
		da mem�ria interna do micro controlador. Este endere�o final 
		de mem�ria � indicado pela defini��o RAMEND no �arquivo .h� 
		de cada micro controlador Ex iom128.h

	O deslocamento do SP � poss�vel se feito no makefile pela 
		instru��o -minit-stack=nnnn

// CUIDADOS COM AS INTERRUP��ES
	- As interrup��es globais s�o desligadas automaticamente 
		quando a fun��o da interrup��o � atendida, e volta a 
		ser ligada automaticamente no fim da execu��o da fun��o 
		j� com o flag de da interrup��o limpo;
	- Quando acessar vari�veis que sejam compartilhadas com uma 
		interrup��o desligue a mesma seguido de um comando NOP, 
		no fim do processo volte a ligar a interrup��o;
	- Quando usar processos cr�ticos desligue as interrup��es, 
		e no fim do processo volte a lig�-las.
	- Cuidado para n�o acontecer overrun, que � a incapacidade do 
		processador terminar de processar uma interrup��o e a mesma 
		j� � requisitada novamente pelo novo evento. Exemplo: Em uma 
		transfer�ncia serial para o uC com uma taxa rapid�ssima, o 
		processador n�o tem tempo de tirar o byte recebido do registrador 
		da UART para um FIFO antes que chegue um novo byte pela UART. 
		Neste caso houve um overrun com perda dos novos bytes;
	- Cuidado para n�o acontecer overflow em uma FIFO, ou seja, 
		a frequ�ncia da grava��o na FIFO dentro de uma interrup��o seja 
		maior que a leitura de dados em um processo fora da interrup��o vindo 
		a estourar a FIFO. Exemplo: Em uma transfer�ncia serial para o uC com 
		uma taxa muito r�pida, e que a interrup��o de recep��o de dados, onde 
		os dados recebidos sejam adicionados na FIFO, � invocada muito mais vezes 
		do que a quantidade de vezes que um processo externo a interrup��o possa 
		retirar esse dados. Em algum momento a FIFO n�o vai ter mais espa�o 
		ocorrendo um overflow e o novos dados recebidos ser�o perdidos.
*/

#include <avr/io.h>
#include "_config_lib_.h"

#endif
