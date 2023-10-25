#ifndef __ARCH_H
#define __ARCH_H

// ---------------------------------------------------------------------------
// CRP - Code Read Protection
// ---------------------------------------------------------------------------
//	CRP permite atribuir diferentes níveis de segurança de acesso a memória flash,
//		protegendo o código lá gravado, fazendo isto restrigindo os comandos ISP e SWD

//  Quando necessário essa proteção é invocada via programação e atribuindo um valor ID na região 0x2FC da flash
//  Para isso é preciso adicionar a seguinte instrução em qualquer parte do programa
//		__attribute__ ((section(".crp"))) const u32 CRP_WORD = CRPx;
//	E o linkscript deve estar preparado para atribuir esse valor. Ex:
//			.text : {
//				CREATE_OBJECT_SYMBOLS
//				KEEP(*(.vectors)) 	/* indicador de tabela de vetores de execeções do ARM */
//				. = 0x000002fc;		/* Região para identificar o nível de segurança do código na memória flash */
//				KEEP(*(.crp)) 		/* Valor identificador do nível de segurança. */

//  Usando CRP a interface de depuração é desabilitada.
//	Removemos o CRP aplicando o comando IAP erase all. Com exceção do CRP nível 3 que bloqueia totalmente a flash

#define CRP_NONE 0x00000000	// Não uso do CRP
#define CRP1 0x12345678 	// CRP nível 1. Es
	// JTAG desabilitado
	// Escrita em RAM é desabilitado abaixo do endereço 0x10000200
	// Comando de leitura da flash é desabilitado
	// Comando de cópia da RAM para a flash setor 0 é desabilitado
	// Comando GO é desabilitado
	// Comando erase all habilitado
	// Comando erase individual não apaga o setor 0
	// Comando compare é desabilitado
#define CRP2 0x87654321 	// CRP nível 2
	// semelhante ao CPR1, porém
	// Comando write RAM é desabilitado
	// Comando copy RAM to Flash é desabilitado
	// Comando erase individual é desabilitado

// CRP nível 3 é muito perigoso porque bloqueia totalmente a flahs sem possibilidade de apagar ela toda ou gravar novo código
//	Somente usar este quando o produto for relamente o final

// TIPOS DE TRATAMENTO PARA INTERRUPÇÕES
#define VIC_USE_NORMAL	0
#define VIC_USE_NESTED	1

// #### CONSTANTES PARA SELEÇÃO DE DISPOSITIVOS CONECTADO AO BARRAMENTO DE DADOS EXTERNO
#define SRAM_BANK_USE_NONE 					 0		// nada conectado
#define SRAM_BANK_USE_CY7C1061AV33_32BITS 	 1		// 2 memórias SRAM 10ns de 16 bits configurada para 32 bits
#define SRAM_BANK_USE_RTL8019AS				 2		// Placa de rede conectada a 8 bits
#define SRAM_BANK_USE_COMPACT_FLASH_IDE 	 3		//
#define SRAM_BANK_USE_DISPO_GENERIC_8BITS	 4		// Acesso a dispositivo lento de 8 bits em geral
#define SRAM_BANK_USE_DISPO_GENERIC_16BITS	 5		// Acesso a dispositivo lento de 16 bits em geral
#define SRAM_BANK_USE_FAST_SRAM_16BITS		 6		// Acesso a memória SRAM rápida de 16 bits
#define SRAM_BANK_USE_FAST_SRAM_32BITS		 7		// Acesso a memória SRAM rápida de 32 bits
#define SRAM_BANK_USE_CHIPSET				 8		// Acesso ao CHIPSET de 16 bits. CHIPSET trabalhando a 54Mhz

// #### CONSTANTES PARA SELEÇÃO DE MEMÓRIAS DINÂMICAS
#define	SDRAM_BANK_USE_NONE 		0			// Nenhuma SDRAM conectada
#define	MT48LC4M32B2_7_PC133 	 	1			// Sdram de 32bits de 16MB (megabyte) - 128Mb (megabits)
#define	MT48LC16M162P_7E_PC133		2			// Sdram de 16bits 256mbits (32mbytes) velocidade minima de 133Mhz PC133

// #### CONSTANTES PARA NAND FLASH
#define NAND_CONNECTION_PIO			0
#define NAND_CONNECTION_IOMAP		1

#if defined(LPC2101) || defined(LPC2102) || defined(LPC2103) ||  defined(LPC2106)
  	#include "arch_lpc210x.h"
#elif defined(LPC2138)
	#include "arch_lpc213x.h"
#elif defined(LPC2114) || defined(LPC2124)
	#include "arch_lpc212x.h"
#elif defined(LPC2210) || defined(LPC2214) || defined(LPC2294)
	#include "arch_lpc22xx.h"
#elif defined(KIT_ARMVIDEO_7MHZ) || defined(KIT_ARMVIDEO_12MHZ)
	#include "arch_kit_arm_video.h"
#elif defined(KIT_LPC1788_PR111) || defined(KIT_LPC1788_PR212)
	#include "arch_kit_lpc1788_pr111.h"
#elif defined(MULTIMETER_v10) || defined(MULTIMETER_v11)
	#include "arch_multimeter.h"
#elif defined (WORKTEMP)
	#include "arch_worktemp.h"
#elif defined (HARDWARE_RESOURCES_hr01)
	#include "arch_hardware_resources.h"
#elif defined(MOD_LPC1751_MD100) || defined(MOD_LPC1752_MD100) || defined(MOD_LPC1751_MD100_RC) || defined(MOD_LPC1752_MD100_RC)
	#include "arch_md100.h"
#elif defined(KIT_LPC1751_PR102) || defined(KIT_LPC1752_PR102)
	#include "arch_kit_pr102.h"
#elif defined(KIT_LPC1315_MOD1)
	#include "arch_kit_lpc1315_mod1.h"
#endif

// WATCHDOG
// valor para carregar o watchdog
#define wdtc_RCsec(sec) (4000000/4)*sec
#define wdtc_PCLKsec(sec) (PCLK/4)*sec
#define wdtc_RTCsec(sec) (32576/4)*sec

// SYSTIME
// Valores de divisão para gerar os ticks. O systime por padrão gera um tick a cada 1ms
#define SYSTIME_TICK_1ms	1 	// Cada tick é gerado a cada 1ms (1000us)
#define SYSTIME_TICK_500us	2	// Cada tick é gerado a cada 0.5ms (500us)
#define SYSTIME_TICK_250us	4	// Cada tick é gerado a cada 0.25ms (250us)

#endif
