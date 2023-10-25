/*
PASSO PARA CONFIGURAR A INICIALIZA��O DO SISTEMA
================================================

Durante a compila��o
	Ajustar o linker script para mapear as mem�rias do processador;
	Ajustar o MAKEFILE para compila��o;
		Especificar fontes;
		Ajustar tipo CPU;
		Determinar o tamanho das STACK de cada modo de opera��o do processador. Colocar tamanho 0 para as stack n�o usadas;
		Determinar o uso ou n�o de mem�ria externa;
		Ajustar formato de sa�da, debugs, avisos e outros par�metros;
	Ajustar configura��es das bibliotecas nos arquivos �_config_xxxx.h�
		Ajustar Freq��ncia e PLL da CPU;
		Ajustar o tipo de interrup��o ser� NESTED ou normal;
		Ajustar se v�o ser usadas vari�veis din�micas ou est�ticas nas bibliotecas;
		Ajustar outras configura��es de bibliotecas.
Durante a programa��o
	Configurar o PLL e a velocidade dos barramentos;
	Inicializar os vetores de interrup��es e ligar as interrup��es caso us�-las;
	Desabilitar os perif�ricos n�o usados atrav�s do registrador PCONP, caso deseja fazer economia de energia;
	Inicializar as portas e demais dispositivos do LPC de acordo com a aplica��o.


// CUIDADOS COM AS INTERRUP��ES
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

#ifndef __CONFIG_CPU_H
#define __CONFIG_CPU_H

#include "_config_lib_.h"

// ###################################################################################################################
// CONFIGURA��ES DE PRIORIDADES DE INTERRUP��ES DOS PERIF�RICOS E APLICA��ES
//		Verificar quais m�dulos que est�o sendo usado e colocar a prioridade na ordem correta
//		O valor 0 tem a mais alta prioridade e o valor mais alto tem baixa prioridade
//		Valores de 0 a 31 para o cortex-m3 e lpc2468 e podem assumir valores iguais
//		Valores de 0 a 15 para os demais lpc arm7tdmi MAS O VALORES TEM QUE SER DIFERENTES
#define UART3_PRIO_LEVEL		0
#define UART1_PRIO_LEVEL		1
#define UART0_PRIO_LEVEL		2
#define SYSTIME_PRIO_LEVEL		0x1f

// #define SYSTIME_PRIO_LEVEL	X
// #define FREERTOS_PRIO_LEVEL 	X // para ARM7
// #define RTC_PRIO_LEVEL		X
// #define TIMER0_PRIO_LEVEL	X
// #define TIMER1_PRIO_LEVEL	X
// #define PWM_PRIO_LEVEL		X
// #define UART0_PRIO_LEVEL		X
// #define UART1_PRIO_LEVEL		X
// #define UART2_PRIO_LEVEL		X
// #define UART3_PRIO_LEVEL		X
// #define UIP_TIMER_PRIO_LEVEL	X
// #define EINT0_PRIO_LEVEL		X
// #define EINT1_PRIO_LEVEL		X
// #define EINT2_PRIO_LEVEL		X
// #define EINT3_PRIO_LEVEL		X
// #define USB_PRIO_LEVEL		X
// #define QEI_PRIO_LEVEL		X

// ###########################################################################################################################################
// ARM7TDMI
// ###########################################################################################################################################
#if defined (arm7tdmi)
// ###################################################################################################################
// AJUSTA O TIPO DE TRATAMENTO PARA INTERRUP��ES
// OBS: N�O TEM EFEITO NO FREE RTOS
#define VIC_USE			VIC_USE_NORMAL
	// VIC_USE_NORMAL = Atende uma interrup��o por vez, ou seja, este tipo de tratamento somente atende as outras interrup��es 
	//					em pedendencia s� quando finalizar o tratamento da interrup��o atual. Salva somente os reg necess�rios
	// VIC_USE_NESTED = (AINDA N�O IMPLEMENTADA) Este tipo de tratamento permite que outras interrup��es de maior prioridade sejam atendidadas antes de finalizar
	// 					o tratamento da interrup��o atual. Salva somente os reg necess�rios


// ###########################################################################################################################################
//	#### VELOCIDADE DA MEM�RIA
//	#### 	0 = Ajusta ciclos de forma automatica
//	#### 	valores de 1 a 7 s�o n�meros de ciclos
#define	ARM_MAMTIM_CYCLES  0

// ###########################################################################################################################################
// #### VELOCIDADE DA MEM�RIA
//	#### 	MAMCR_OFF	= MAM Desligada
//	#### 	MAMCR_PART 	= MAM funcinando parcialmente
//	#### 	MAMCR_FULL 	= MAM funcinando completamente
#define	ARM_MAM MAMCR_FULL

#endif // arm7tdmi


// ###########################################################################################################################################
// ARM7TDMI / CORTEX-M3
// ###########################################################################################################################################

// VELOCIDADE DE ACESSO A MEM�RIA DE PROGRAMA FLASH
//	FLASHTIM_20MHZ = Flash accesses use 1 CPU clock. Use for up to 20 MHz CPU clock with power boost off
//	FLASHTIM_40MHZ = Flash accesses use 2 CPU clocks. Use for up to 40 MHz CPU clock with power boost off
//	FLASHTIM_60MHZ = Flash accesses use 3 CPU clocks. Use for up to 60 MHz CPU clock with power boost off
//	FLASHTIM_80MHZ = Flash accesses use 4 CPU clocks. Use for up to 80 MHz CPU clock with power boost off
//						Use this setting for operation from 100 to 120 MHz operation with power boost on
//	FLASHTIM_100MHZ = Flash accesses use 5 CPU clocks. Use for up to 100 MHz CPU clock with power boost off
//	FLASHTIM_SAFE = Flash accesses use 6 CPU clocks. �Safe� setting for any allowed conditions
//	FLASHTIM_AUTO = Configura a velocidade da FLASH automaticamente
#define FLASHTIM FLASHTIM_AUTO

// DETERMINAR A FREQU�NCIA DE TRABALHO DO BARRAMENTO DAS MEM�RIAS
//	 	MCLK_FULL_CCLK - Barramento de mem�ria vai acessar na mesma velocidade do PLL
// 		MCLK_HALF_CCLK - Barramento de mem�ria vai acessar com a metade da velocidade do PLL
#define MCLK MCLK_FULL_CCLK

#endif
