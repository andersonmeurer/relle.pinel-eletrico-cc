#ifndef __MEM_DYNAMCI_H
#define __MEM_DYNAMCI_H

#include "_config_cpu_.h"

// ################################################################################################################################
// SDRAM_CONFIG
//	       |----------------------------------------|------|--|---------------|--|--|------------------------|---|-------|-----| 
// BITS 	31 30 29 28		27 26 25 24		23 22 21 20		19 18 17 16		15 14 13 12		11 10 9 8		7 6 5 4		3 2 1 0 														
//		
// Bits [31:21]: Reservados			0
// Bit  [20] 	Write protect		0 = not protect 1 = write protect
// Bit  [19]	Buffer enabled		0 = buffer not enabled 1 = buffer enabled for access to chip select
// Bits [18:15]	Reservados			0
// Bit  [14]	Address Mapping		Consultar datasheet lpc2468 página 84 e ajustar ao tipo de memória
// Bit  [13]	Reservado			0
// Bits [12:7]	Address Mapping		Consultar datasheet lpc2468 página 84 e ajustar ao tipo de memória
// Bits [6:5]	Reservados			0
// Bits [4:3]	Memory device		00=SDRAM    01=low power SDRAM   10=Micro SyncFlash		11=reservado
// Bits [2:0]	Reservados			0

// SDRAM_LATENCY
//	       |-------------------------------------------------------------------------------------|---|-----|---------------|---| 
// BITS 	31 30 29 28		27 26 25 24		23 22 21 20		19 18 17 16		15 14 13 12		11 10 9 8		7 6 5 4		3 2 1 0 														
//		
// Bits [31:10] 	Reservados			0
// Bits [9:8] 	CAS latency			00=reservado	01=one CCLK cycle 	10=two CCLK cycles	01=three CCLK cycles
// Bits [7:2] 	Reservados			0
// Bits [1:0] 	RAS latency			00=reservado	01=one CCLK cycle 	10=two CCLK cycles	01=three CCLK cycles
		
// SDRAM_REFRESH_PERIOD_US
// 		ajusta o tempo entre SDRAM refresh cycles. Ex: 64ms para 4096 linhas = 15.625uS

#if (SDRAM_BANK0_USE == MT48LC4M32B2_7_PC133)
	#define	SDRAM_SIZE_IN_BYTES 	16777216		// Tamanho de 16Mbytes (128megabits)

	#define	SDRAM_CONFIG  			0x00005500 		// Configuração memória para 128 MB (4Mx32), 4 banks, row length = 12, column length = 8
	#define	SDRAM_LATENCY 			0x00000202		// Latencia do RAS e CAS cl=2
	#define	SDRAM_MODE_CMD			0x00008800		// Comando MODE
	#define	SDRAM_AUTO_REFRESH		2			    // Necesita de dois ciclos de refresh durante a configuração
				
	#define	SDRAM_TRP				1 				// (n + 1) -> teremos 2 clock cycles no TRC
	#define	SDRAM_TRAS			 	3 				// (n + 1) -> teremos 4 clock cycles no TRAS
	#define	SDRAM_TSREX			  	5	 			// (n + 1) -> teremos 5 clock cycles Self-refresh period
	#define	SDRAM_TAPR			  	1 				// (n + 1) -> teremos 2 clock cycles Data out to active
	#define	SDRAM_TDAL 				5				// (n + 1) -> 6 clock cycles Data in to active
	#define	SDRAM_TWR 				1				// (n + 1) -> 2 clock cycles Write recovery
	#define	SDRAM_TRC 				5				// (n + 1) -> 6 clock cycles Active to Active cmd
	#define	SDRAM_TRFC 				5				// (n + 1) -> 6 clock cycles Auto-refresh
	#define	SDRAM_TXSR 				5				// (n + 1) -> 6 clock cycles Exit self-refresh
	#define	SDRAM_TRRD 				1				// (n + 1) -> 2 clock cycles Active bank A->B
	#define	SDRAM_TMRD 				1				// (n + 1) -> 2 clock cycles Load Mode to Active cmd
				
	#define	SDRAM_REFRESH_PERIOD_US	15.625 		// ajusta o tempo entre SDRAM refresh cycles. 64ms para 4096 linhas = 15.625uS
#elif (SDRAM_BANK0_USE == MT48LC16M162P_7E_PC133)
	// TIPO DE SDRAM O ARM VAI ACESSAR
	// 	Consultar tabela 133 página 178 do datasheet e registrador EMCDynamicConfig0 da página 177
	// 	Memória do tipo (16Mx16), 4 banks, row length = 13, column length = 9
	//	Mesmo tendo slot para duas SDRAM no kit estamos usando somente uma (U8)
	#define	SDRAM_CONFIG  			0x680			// 1 memória de 16 bits // Row, bank, col
		#define	SDRAM_SIZE_IN_BYTES 	33554432		// Sdram 32mBytes (256mbits)
 	//#define	SDRAM_CONFIG  			0x1680			// 1 memória de 16 bits //  bank, Row, col
	//#if defined (__ARCH_ARCH_KITLPC1788PR111_H)
	//#define	SDRAM_CONFIG  		0x00004680			// 2 memórias de 16 bits criando um barramento de 32 bits
	//#endif
	#define	SDRAM_LATENCY 			0x202			// Latencia do RAS e CAS cl=2
	#define	SDRAM_MODE_CMD			0x23000
	#define	SDRAM_AUTO_REFRESH		1

	#define	SDRAM_TRP				3				// (n + 1) -> clock cycles no TRC
	#define	SDRAM_TRAS			 	6				// (n + 1) -> clock cycles no TRAS
	#define	SDRAM_TSREX			  	9 	 			// (n + 1) -> clock cycles Self-refresh period  		- 10 (valor calculado)
	#define	SDRAM_TAPR			  	1 				// (n + 1) -> clock cycles Data out to active
	#define	SDRAM_TDAL 				5				// (n + 1) -> clock cycles Data in to active
	#define	SDRAM_TWR 				14				// (n + 1) -> clock cycles Write recovery 				- 3 (valor calculado)
	#define	SDRAM_TRC 				9				// (n + 1) -> clock cycles Active to Active cmd
	#define	SDRAM_TRFC 				9				// (n + 1) -> clock cycles Auto-refresh 				- 8 (valor calculado)
	#define	SDRAM_TXSR 				9				// (n + 1) -> clock cycles Exit self-refresh
	#define	SDRAM_TRRD 				2				// (n + 1) -> clock cycles Active bank A->B
	#define	SDRAM_TMRD 				2				// (n + 1) -> clock cycles Load Mode to Active cmd 		- 3 (valor calculado)

	#define	SDRAM_REFRESH_PERIOD_US	15.625 			// ajusta o tempo entre SDRAM refresh cycles. 64ms para 4096 linhas = 15.625uS
#else
	#error NÃO FOI DEFINIDO O TIPO DE ACESSO A MEMÓRIA DYNAMICA EXTERNA 
#endif

#endif
