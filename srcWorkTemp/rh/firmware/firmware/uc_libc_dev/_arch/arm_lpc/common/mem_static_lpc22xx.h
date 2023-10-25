#ifndef __MEM_STATIC_H
#define __MEM_STATIC_H

#include "_config_cpu_.h"

// ############################################################################################
// DEFINI��ES DAS CONFIGURA��ES DE CADA DIPOSITIVO CONECATADO AO BARRAMENTO SEJA MEM�RIA OU N�O
// ############################################################################################

// REGISTRADOR BCFG
//	       |-----|------|  |--|--|-------------------------------------|   |------------------|--|---------------|-|--------------| 
// BITS 	31 30 29 28		27 26 25 24		23 22 21 20		19 18 17 16		15 14 13 12		11 10 9 8		7 6 5 4		3 2 1 0 														
//		

// Bits [31:30]: 		Sempre 0
// Bits [29:28] MW:  	00 = Barramento de 8 bits / 01 = Barramento de 16 bits / 10 = Barramento de 32 bits /  11 = Reservado
// Bit 27 		BM: 	1 = Habilita o banco de mem�ria para BURST ROM 		
// Bit 26 		WP: 	1 = Prote��o de escrita no banco;		
// Bits [25:16]: 		N�o sei e reservado. Colocar 0
// Bits [15:11] WST2: 	Controla o tamanho de acesso para escrita
// Bit 10 		RBLE: 	Habilita o uso do pinos BLS para acessar bytes em dispositivos n�o 8 bits. 
								// 0 = para dispositivo de 8 bits  / 1 = para dispositivos de 16 ou 32 bits 
// Bits [9:5] 	WST1: 	Controla o tamanho de acesso de leitura;
// Bit 4		    : 	Reservado = 0
// Bits [3:0] 	IDCY: 	Controla o n�mero m�nimo de ciclos ocioso entre as opera��es de escrita e leitura

// CONFIGURA��ES PARA DISPOSTIVOS GEN�RICOS DE 16 BITS
#define BANK_CONFIG_16_BITS_CONFIG 		0x1000FFEF	// Bus de 16 bits, acesso sem BUSRT, acesso de leitura e esccrita o mais lento possivel

// CONFIGURA��ES PARA SRAM FAST DE 32 BITS
#define BANK_CONFIG_CY7C1061AV33  		0x20000420	// Bus 32 bits, Sem BUSRT ROM, sem prote��o de escrita, sem intervalo entre escrita e leituras (RAM r�pida), Tamnanho escrita = 0, tamanho leitura = 1
#define BANK_CONFIG_CY7C1061AV33_SIZE	0x400000

// CONFIGURA��ES PARA RTL8019AS
#define BANK_CONFIG_RTL8019 			0x0000FFEF	// Bus 8 bits, sem BUSRT ROM, sem prote��o de escrita, tamanho de acesso de leitura e escrita e intervalo entre elas o mais lento possivel







// ############################################################
// MAPEAMENTO DOS DIPOSITIVOS AOS SEUS RESPECTIVOS CHIP SELECTS
// ############################################################

// CONFIGURA��O DO BANCO 0 (CS0) Endere�o entre 0x8000_0000 a 0x80ff_ffff 
#if (SRAM_BANK0_USE == SRAM_BANK_USE_CY7C1061AV33_32BITS)
	#define	SRAM_BANK0_CONFIG 								BANK_CONFIG_CY7C1061AV33
#elif (SRAM_BANK0_USE == SRAM_BANK_USE_RTL8019AS)
	#define	SRAM_BANK0_CONFIG								BANK_CONFIG_RTL8019
#elif (SRAM_BANK0_USE == SRAM_BANK_USE_DISPO_GENERIC_16BITS)
	#define	SRAM_BANK0_CONFIG								BANK_CONFIG_16_BITS_CONFIG
#elif (SRAM_BANK0_USE != SRAM_BANK_USE_NONE)
	#error N�O FOI DEFINIDO O TIPO DE ACESSO A MEM�RIA EST�TICA EXTERNA PARA O BANCO 0
#endif
	
// CONFIGURA��O DO BANCO 1 (CS1) Endere�o entre 0x8100_0000 a 0x81ff_ffff
#if (SRAM_BANK1_USE == SRAM_BANK_USE_CY7C1061AV33_32BITS)
	#define	SRAM_BANK1_CONFIG 								BANK_CONFIG_CY7C1061AV33
#elif (SRAM_BANK1_USE == SRAM_BANK_USE_RTL8019AS)
	#define	SRAM_BANK1_CONFIG								BANK_CONFIG_RTL8019
#elif (SRAM_BANK1_USE == SRAM_BANK_USE_DISPO_GENERIC_16BITS)
	#define	SRAM_BANK1_CONFIG								BANK_CONFIG_16_BITS_CONFIG
#elif (SRAM_BANK1_USE != SRAM_BANK_USE_NONE)
	#error N�O FOI DEFINIDO O TIPO DE ACESSO A MEM�RIA EST�TICA EXTERNA PARA O BANCO 1
#endif

// CONFIGURA��O DO BANCO 2 (CS2) Endere�o entre 0x8200_0000 a 0x82ff_ffff
#if (SRAM_BANK2_USE == SRAM_BANK_USE_CY7C1061AV33_32BITS)
	#define	SRAM_BANK2_CONFIG 								BANK_CONFIG_CY7C1061AV33
#elif (SRAM_BANK3_USE == SRAM_BANK_USE_RTL8019AS)
	#define	SRAM_BANK2_CONFIG								BANK_CONFIG_RTL8019
#elif (SRAM_BANK2_USE == SRAM_BANK_USE_DISPO_GENERIC_16BITS)
	#define	SRAM_BANK2_CONFIG								BANK_CONFIG_16_BITS_CONFIG
#elif (SRAM_BANK2_USE != SRAM_BANK_USE_NONE)
	#error N�O FOI DEFINIDO O TIPO DE ACESSO A MEM�RIA EST�TICA EXTERNA PARA O BANCO 2
#endif

// CONFIGURA��O DO BANCO 3 (CS3) Endere�o entre 0x8300_0000 a 0x83ff_ffff
#if (SRAM_BANK3_USE == SRAM_BANK_USE_CY7C1061AV33_32BITS)
	#define	SRAM_BANK3_CONFIG 								BANK_CONFIG_CY7C1061AV33
#elif (SRAM_BANK3_USE == SRAM_BANK_USE_RTL8019AS)
	#define	SRAM_BANK3_CONFIG								BANK_CONFIG_RTL8019
#elif (SRAM_BANK3_USE == SRAM_BANK_USE_DISPO_GENERIC_16BITS)
	#define	SRAM_BANK3_CONFIG								BANK_CONFIG_16_BITS_CONFIG
#elif (SRAM_BANK3_USE != SRAM_BANK_USE_NONE)
	#error N�O FOI DEFINIDO O TIPO DE ACESSO A MEM�RIA EST�TICA EXTERNA PARA O BANCO 3
#endif

#endif
