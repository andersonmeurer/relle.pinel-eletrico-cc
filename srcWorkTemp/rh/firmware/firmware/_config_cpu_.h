/*
PASSO PARA CONFIGURAR A INICIALIZAÇÃO DO SISTEMA
================================================

Durante a compilação
	Ajustar o linker script para mapear as memórias do processador;
	Ajustar o MAKEFILE para compilação;
		Especificar fontes;
		Ajustar tipo CPU;
		Determinar o tamanho das STACK de cada modo de operação do processador. Colocar tamanho 0 para as stack não usadas;
		Determinar o uso ou não de memória externa;
		Ajustar formato de saída, debugs, avisos e outros parâmetros;
	Ajustar configurações das bibliotecas nos arquivos “_config_xxxx.h”
		Ajustar Freqüência e PLL da CPU;
		Ajustar o tipo de interrupção será NESTED ou normal;
		Ajustar se vão ser usadas variáveis dinâmicas ou estáticas nas bibliotecas;
		Ajustar outras configurações de bibliotecas.
Durante a programação
	Configurar o PLL e a velocidade dos barramentos;
	Inicializar os vetores de interrupções e ligar as interrupções caso usá-las;
	Desabilitar os periféricos não usados através do registrador PCONP, caso deseja fazer economia de energia;
	Inicializar as portas e demais dispositivos do LPC de acordo com a aplicação.


// CUIDADOS COM AS INTERRUPÇÕES
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

#ifndef __CONFIG_CPU_H
#define __CONFIG_CPU_H

#include "_config_lib_.h"

// ###################################################################################################################
// CONFIGURAÇÕES DE PRIORIDADES DE INTERRUPÇÕES DOS PERIFÉRICOS E APLICAÇÕES
//		Verificar quais módulos que estão sendo usado e colocar a prioridade na ordem correta
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
// AJUSTA O TIPO DE TRATAMENTO PARA INTERRUPÇÕES
// OBS: NÃO TEM EFEITO NO FREE RTOS
#define VIC_USE			VIC_USE_NORMAL
	// VIC_USE_NORMAL = Atende uma interrupção por vez, ou seja, este tipo de tratamento somente atende as outras interrupções 
	//					em pedendencia só quando finalizar o tratamento da interrupção atual. Salva somente os reg necessários
	// VIC_USE_NESTED = (AINDA NÃO IMPLEMENTADA) Este tipo de tratamento permite que outras interrupções de maior prioridade sejam atendidadas antes de finalizar
	// 					o tratamento da interrupção atual. Salva somente os reg necessários


// ###########################################################################################################################################
//	#### VELOCIDADE DA MEMÓRIA
//	#### 	0 = Ajusta ciclos de forma automatica
//	#### 	valores de 1 a 7 são números de ciclos
#define	ARM_MAMTIM_CYCLES  0

// ###########################################################################################################################################
// #### VELOCIDADE DA MEMÓRIA
//	#### 	MAMCR_OFF	= MAM Desligada
//	#### 	MAMCR_PART 	= MAM funcinando parcialmente
//	#### 	MAMCR_FULL 	= MAM funcinando completamente
#define	ARM_MAM MAMCR_FULL

#endif // arm7tdmi


// ###########################################################################################################################################
// ARM7TDMI / CORTEX-M3
// ###########################################################################################################################################

// VELOCIDADE DE ACESSO A MEMÓRIA DE PROGRAMA FLASH
//	FLASHTIM_20MHZ = Flash accesses use 1 CPU clock. Use for up to 20 MHz CPU clock with power boost off
//	FLASHTIM_40MHZ = Flash accesses use 2 CPU clocks. Use for up to 40 MHz CPU clock with power boost off
//	FLASHTIM_60MHZ = Flash accesses use 3 CPU clocks. Use for up to 60 MHz CPU clock with power boost off
//	FLASHTIM_80MHZ = Flash accesses use 4 CPU clocks. Use for up to 80 MHz CPU clock with power boost off
//						Use this setting for operation from 100 to 120 MHz operation with power boost on
//	FLASHTIM_100MHZ = Flash accesses use 5 CPU clocks. Use for up to 100 MHz CPU clock with power boost off
//	FLASHTIM_SAFE = Flash accesses use 6 CPU clocks. “Safe” setting for any allowed conditions
//	FLASHTIM_AUTO = Configura a velocidade da FLASH automaticamente
#define FLASHTIM FLASHTIM_AUTO

// DETERMINAR A FREQUÊNCIA DE TRABALHO DO BARRAMENTO DAS MEMÓRIAS
//	 	MCLK_FULL_CCLK - Barramento de memória vai acessar na mesma velocidade do PLL
// 		MCLK_HALF_CCLK - Barramento de memória vai acessar com a metade da velocidade do PLL
#define MCLK MCLK_FULL_CCLK

#endif
