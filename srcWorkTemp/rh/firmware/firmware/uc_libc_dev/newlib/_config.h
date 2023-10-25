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

// PARA FAM�LIA LPC2400
#if defined(LPC2468) 
	// OBS: O CLOCK DO N�CLEO DO ARM FAMILIA DO LPC24000 E SEUS DISPOSITIVOS PODE SER DERIVADOR DE TR�S FONTES DE OSILADORES: RC INTERNO, RTC E OSCILADOR PRINCIPAL
	// NO MOMENTO ESTAMOS USANDO SOMENTE O OSCILADOR PRINCIPAL PARA USO DO CLOCK DA CPU, ENT�O AS BIBLIOTECAS N�O EST�O PREPARDOS PARA OUTROS TIPOS DE FONTES DE OSCILADORES
	
	// DEFINI��ES PARA AJUSTES
	#define PLL_N	1									// Pr�-divisor na entrada da etapa multiplicadora (entre 1 a 32). Come�ar pelo menor n�mero para produzir uma multiplica��o menor
	#define CCLK 		72000000							// Freq��ncia desejada para a CPU (entre 10MHz a 72MHz)
	#define F_CPU 		12000000							// Freq��ncia de entrada (entre 1MHz a 24MHz). 2*F_CPU deve ser multiplo de CCLK
	#define FCCO		288000000							// Come�ar pelo FCCOmin >> raz�o = FCCO_MIN/CCLK. Se o valor for inteiro o FCCO ser� o FCCO_MIN, sen�o FCCO = (raz�o + 1)*CCLK
	#define OSC_RANGE 	SCS_OSCRANGE_1MHZ_20MHZ				// Define a faixa de freq��ncia do oscilador principal
																// SCS_OSCRANGE_1MHZ_20MHZ  - Define que o a freq��ncia de entrada do clock para o oscilador principal est� na faixa de 1MHZ a 20MHZ
																// SCS_OSCRANGE_15MHZ_24MHZ - Define que o a freq��ncia de entrada do clock para o oscilador principal est� na faixa de 15MHZ a 24MHZ
	#define PBSD       	PBSD_1_1_CCLK						// Fator para o ajuste da freq��ncia dos dispositivos internos do ARM
															//	Cada dispositivo pode selecionar de forma independete, mas aqui estamos tratando todos por iguais
																// PBSD_1_4_CCLK Pclk roda a 1/4 da velocidade do CCLK
																// PBSD_1_2_CCLK Pclk roda a 1/2 da velocidade do CCLK
																// PBSD_1_1_CCLK Pclk roda a mesma velocidade do CCLK

	
	// F�RMULAS PARA AJUSTES DO CLOCK
	#define	PLL_M ((FCCO*PLL_N)/(2*F_CPU))		// Multiplicador M do PLL (6 a 512)
	#define PCLK (CCLK / PBSD) 								// Freq��ncia do barramemto dos dispositivos internos do ARM
	
// PARA FAM�LIA LPC2000
#else				
	// FREQUENCIA DESEJADA DO PROCESSADOR (CCLK = F_CPU * PLL_M)
	
	// DEFINI��ES PARA AJUSTES
	#define F_CPU 		14745600							// Freq��ncia de entrada (10MHz a 25MHz).
	#define PLL_M 		4	        						// Multiplicador M do PLL (1 a 32, na pr�tica entre 1 a 6)
	#define PLL_P 		2	        						// Divisor P do PLL para ajustar corretamete o Fcco (1, 2, 4 ou 8)
	#define PBSD       	PBSD_1_1_CCLK						// Fator para o ajuste da freq��ncia dos dispositivos internos do ARM.
																// PBSD_1_4_CCLK Pclk roda a 1/4 da velocidade do CCLK
																// PBSD_1_2_CCLK Pclk roda a 1/2 da velocidade do CCLK
																// PBSD_1_1_CCLK Pclk roda a mesma velocidade do CCLK
														

	// F�RMULAS PARA AJUSTES DO CLOCK
	#define CCLK (F_CPU * PLL_M) 						// Freq��ncia da CPU
	#define PCLK (CCLK / PBSD) 								// Freq��ncia do barramemto dos dispositivos internos do ARM
	#define FCCO (F_CPU * PLL_M * 2 * PLL_P)	// Freq��ncia do oscilador controlado por corrente
#endif



// ###################################################################################################################
// AJUSTA O TIPO DE TRATAMENTO PARA INTERRUP��ES
// OBS: N�O TEM EFEITO NO FREE RTOS
#define VIC_USE_NORMAL	0
#define VIC_USE_NESTED	1
#define VIC_USE			VIC_USE_NORMAL
	// VIC_USE_NORMAL = Atende uma interrup��o por vez, ou seja, este tipo de tratamento somente atende as outras interrup��es 
	//					em pedendencia s� quando finalizar o tratamento da interrup��o atual. Salva somente os reg necess�rios
	// VIC_USE_NESTED = (N�O FUNCIONA NO SIMULADOR PROTEUS) Este tipo de tratamento permite que outras interrup��es de maior prioridade sejam atendidadas antes de finalizar
	// 					o tratamento da interrup��o atual. Salva somente os reg necess�rios

//###################################################################################################################
// CONFIGURA��O DE USO DE RAM PARA AS LIBS
#define USE_MALLOC pdOFF	// pdON = As posi��o de RAMs s�o geradas pelo malloc. Util caso deseja usar a posi��o da heap na mem�ria externa

// ###################################################################################################################
// USO DO USB NO LPC2400
#define	USE_USB	pdOFF		// Especifica se � para usar o n�o USB
							// pdOFF = N�o usar USB
							// pdON = Usar USB. Configurar corretamente o FCCO para que a frequencia do bloco USB seja 48MHz com 50% duty cycle

// ###################################################################################################################
// CONFIGURA��ES PARA PORTA DE SAIDA DEBUG
#define USE_DEBUG_OUT_UART	pdON	// pdON = Utiliza a saida UART para debug
#define USE_DEBUG_UART		0 	// Selecione qual a UART deve ser usada para saida debug. 0 = Utiliza uart0, 1 = Utiliza uart1 e assim sucessivamente



// ###################################################################################################################
// DETERMINA QUE ATITUDE DEVE SER TOMADAS EM LOOPS DE ESPERA NA FUN��ES DAS BIBLIOTECAS
#if defined(RTOS_UCLINUX)
	#define CRITICAL_WAIT	swap()		// For�a a troca de contexto
#else
	#define CRITICAL_WAIT	continue	// Faz nada, espero no loop
#endif



// ###################################################################################################################
// DEFINI��ES DE MACROS
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
