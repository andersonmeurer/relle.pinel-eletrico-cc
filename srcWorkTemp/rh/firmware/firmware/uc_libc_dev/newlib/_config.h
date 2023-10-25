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
*/


#ifndef __CONFIG_H
#define __CONFIG_H

#if defined(LPC2106)
  	#include "lpc210x.h"
#elif defined(LPC2138) || defined(LPC2114) || defined(LPC2124)
	#include "lpc21xx.h"
#elif defined(LPC2210) || defined(LPC2214) || defined(LPC2294)
	#include "lpc22xx.h"
#elif defined(LPC2468)
	#include "lpc24xx.h"
#endif

#include "uc_libdefs.h"

// ###################################################################################################################
// AJUSTE DO CLOCK DO SISTEMA

// PARA FAMÍLIA LPC2400
#if defined(LPC2468) 
	// OBS: O CLOCK DO NÚCLEO DO ARM FAMILIA DO LPC24000 E SEUS DISPOSITIVOS PODE SER DERIVADOR DE TRÊS FONTES DE OSILADORES: RC INTERNO, RTC E OSCILADOR PRINCIPAL
	// NO MOMENTO ESTAMOS USANDO SOMENTE O OSCILADOR PRINCIPAL PARA USO DO CLOCK DA CPU, ENTÃO AS BIBLIOTECAS NÃO ESTÃO PREPARDOS PARA OUTROS TIPOS DE FONTES DE OSCILADORES
	
	// DEFINIÇÕES PARA AJUSTES
	#define PLL_N	1									// Pré-divisor na entrada da etapa multiplicadora (entre 1 a 32). Começar pelo menor número para produzir uma multiplicação menor
	#define CCLK 		72000000							// Freqüência desejada para a CPU (entre 10MHz a 72MHz)
	#define F_CPU 		12000000							// Freqüência de entrada (entre 1MHz a 24MHz). 2*F_CPU deve ser multiplo de CCLK
	#define FCCO		288000000							// Começar pelo FCCOmin >> razão = FCCO_MIN/CCLK. Se o valor for inteiro o FCCO será o FCCO_MIN, senão FCCO = (razão + 1)*CCLK
	#define OSC_RANGE 	SCS_OSCRANGE_1MHZ_20MHZ				// Define a faixa de freqüência do oscilador principal
																// SCS_OSCRANGE_1MHZ_20MHZ  - Define que o a freqüência de entrada do clock para o oscilador principal está na faixa de 1MHZ a 20MHZ
																// SCS_OSCRANGE_15MHZ_24MHZ - Define que o a freqüência de entrada do clock para o oscilador principal está na faixa de 15MHZ a 24MHZ
	#define PBSD       	PBSD_1_1_CCLK						// Fator para o ajuste da freqüência dos dispositivos internos do ARM
															//	Cada dispositivo pode selecionar de forma independete, mas aqui estamos tratando todos por iguais
																// PBSD_1_4_CCLK Pclk roda a 1/4 da velocidade do CCLK
																// PBSD_1_2_CCLK Pclk roda a 1/2 da velocidade do CCLK
																// PBSD_1_1_CCLK Pclk roda a mesma velocidade do CCLK

	
	// FÓRMULAS PARA AJUSTES DO CLOCK
	#define	PLL_M ((FCCO*PLL_N)/(2*F_CPU))		// Multiplicador M do PLL (6 a 512)
	#define PCLK (CCLK / PBSD) 								// Freqüência do barramemto dos dispositivos internos do ARM
	
// PARA FAMÍLIA LPC2000
#else				
	// FREQUENCIA DESEJADA DO PROCESSADOR (CCLK = F_CPU * PLL_M)
	
	// DEFINIÇÕES PARA AJUSTES
	#define F_CPU 		14745600							// Freqüência de entrada (10MHz a 25MHz).
	#define PLL_M 		4	        						// Multiplicador M do PLL (1 a 32, na prática entre 1 a 6)
	#define PLL_P 		2	        						// Divisor P do PLL para ajustar corretamete o Fcco (1, 2, 4 ou 8)
	#define PBSD       	PBSD_1_1_CCLK						// Fator para o ajuste da freqüência dos dispositivos internos do ARM.
																// PBSD_1_4_CCLK Pclk roda a 1/4 da velocidade do CCLK
																// PBSD_1_2_CCLK Pclk roda a 1/2 da velocidade do CCLK
																// PBSD_1_1_CCLK Pclk roda a mesma velocidade do CCLK
														

	// FÓRMULAS PARA AJUSTES DO CLOCK
	#define CCLK (F_CPU * PLL_M) 						// Freqüência da CPU
	#define PCLK (CCLK / PBSD) 								// Freqüência do barramemto dos dispositivos internos do ARM
	#define FCCO (F_CPU * PLL_M * 2 * PLL_P)	// Freqüência do oscilador controlado por corrente
#endif



// ###################################################################################################################
// AJUSTA O TIPO DE TRATAMENTO PARA INTERRUPÇÕES
// OBS: NÃO TEM EFEITO NO FREE RTOS
#define VIC_USE_NORMAL	0
#define VIC_USE_NESTED	1
#define VIC_USE			VIC_USE_NORMAL
	// VIC_USE_NORMAL = Atende uma interrupção por vez, ou seja, este tipo de tratamento somente atende as outras interrupções 
	//					em pedendencia só quando finalizar o tratamento da interrupção atual. Salva somente os reg necessários
	// VIC_USE_NESTED = (NÃO FUNCIONA NO SIMULADOR PROTEUS) Este tipo de tratamento permite que outras interrupções de maior prioridade sejam atendidadas antes de finalizar
	// 					o tratamento da interrupção atual. Salva somente os reg necessários

//###################################################################################################################
// CONFIGURAÇÃO DE USO DE RAM PARA AS LIBS
#define USE_MALLOC pdOFF	// pdON = As posição de RAMs são geradas pelo malloc. Util caso deseja usar a posição da heap na memória externa

// ###################################################################################################################
// USO DO USB NO LPC2400
#define	USE_USB	pdOFF		// Especifica se é para usar o não USB
							// pdOFF = Não usar USB
							// pdON = Usar USB. Configurar corretamente o FCCO para que a frequencia do bloco USB seja 48MHz com 50% duty cycle

// ###################################################################################################################
// CONFIGURAÇÕES PARA PORTA DE SAIDA DEBUG
#define USE_DEBUG_OUT_UART	pdON	// pdON = Utiliza a saida UART para debug
#define USE_DEBUG_UART		0 	// Selecione qual a UART deve ser usada para saida debug. 0 = Utiliza uart0, 1 = Utiliza uart1 e assim sucessivamente



// ###################################################################################################################
// DETERMINA QUE ATITUDE DEVE SER TOMADAS EM LOOPS DE ESPERA NA FUNÇÕES DAS BIBLIOTECAS
#if defined(RTOS_UCLINUX)
	#define CRITICAL_WAIT	swap()		// Força a troca de contexto
#else
	#define CRITICAL_WAIT	continue	// Faz nada, espero no loop
#endif



// ###################################################################################################################
// DEFINIÇÕES DE MACROS
#define _BV(n)              (1 << (n))
#define __BIT_MASK(A) (1<<(A))
//#define set_bit( SFR, MASK ) (SFR) |=  (MASK)
//#define clr_bit( SFR, MASK ) (SFR) &= ~(MASK)
#define set_bit(x,y) 		(x |= (y)) 					// Seta  o bit y no byte x
#define clear_bit(x,y) 		(x &= (~y)) 				// Limpa o bit y no byte x
#define check_bit(x,y) 		(x & (y)) 					// Checa o bit y no byte x

// Macros de minimo maximo e absoluto
#define min(a,b)			((a<b)?(a):(b))
#define max(a,b)			(((a)>(b))?(a):(b))
#define abs(x)				(((x)>0)?(x):(-x))

#define nop()  __asm__ __volatile__ ("nop" ::)

#define __builtin_expect(x, expected_value) (x)
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)



#endif
