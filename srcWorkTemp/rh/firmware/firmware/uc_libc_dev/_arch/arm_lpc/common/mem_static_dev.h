#ifndef __MEM_STATIC_DEV_H
#define __MEM_STATIC_DEV_H

// ############################################################################################
// DEFINI��ES DAS CONFIGURA��ES DE CADA DIPOSITIVO CONECATADO AO BARRAMENTO SEJA MEM�RIA OU N�O
// ############################################################################################

// REGISTRADOR EMC_STA_CFG
	//	       |----------------------------------------|--|---|--|------------------------------------|-|-----|-|-|---|---|-|-|---|
	// BITS 	31 30 29 28		27 26 25 24		23 22 21 20		19 18 17 16		15 14 13 12		11 10 9 8		7 6 5 4		3 2 1 0 														

	// Bits [31:21]: 		Reservados
	// Bit  [20]: 	P		Write protect. 0 = Write not protect. 1 = write protect
	// Bit  [19]: 	E		Buffer Enabled. 0 = Buffer disabled. 1 = buffer enabled
	// Bits [18:9]: 		Reservados
	// Bit  [8]: 	EW		Extended wait. 0 = disabled. 1 = enabled 
	//						EMC_STA_WAITWR e MC_STA_WAITRD s�o ignorados
	// Bit  [7]: 	PB		Byte lane state. Isto possibilita  diferentes tipos de mem�rias a ser conectadas. 
							//		Para mem�ria est�tica de 8 bits, os sinais BLSN[3:0] do EMC � geralmente conectado ao WE da mem�ria.
							//		Neste caso para leitura todos os pinos BLSN devem ficar em alta e o PB deve ser 0.
							// 		Para mem�ria est�tica de 16 bits geralmente tem os sinais BLSN[3:0] conectados aos UBn e LBn na mem�ria/
							//		Neste caso a escrita de um byte em particular deve acionar os sinais UBn ou LBn correspondete.
							//		Para leitura todos os sinais UBn e LBn devem ficar em n�vel baixo. Neste caso o PB deve ser 1
	// Bit  [6]: 	PC		Chip Select polarity on power on reset. 0 = active LOW chip select 1 = active HIGH chip select 
	// Bits [5:4]: 			Reservados
	// Bit  [3]: 	PM		Page mode. 0 Desabilitado. 1 = Habilitado page mode assincrono, ver tamanho da p�gina
	// Bit  [2]: 			Reservado
	// Bits [1:0]: 	MW		Memory Width: 00 = 8 bits. 01 = 16 bits. 10 = 32 bits. 11 = reservado



// REGISTRADOR EMC_STA_WAITWEN
	// 	Atraso para acionamento WE em rela��o ao CS. 
	//	n+1 ciclos CCLK.  
	//	Valores de 0 a 0xF

// REGISTRADOR EMC_STA_WAITOEN 
	// 	Atraso para acionamento OE em rela��o ao CS. 
	//	n ciclos CCLK. 
	//	Valores de 0 a 0xF

// REGISTRADOR EMC_STA_WAITRD	
	// 	Atraso entre a opera��o de leitura para o pr�xima opera��o de acesso
	//	n+1 ciclos CCLK. 
	//	Valores de 1 a 0x1F

// REGISTRADOR EMC_STA_WAITPAGE
	// 	
	//	n+1 ciclos CCLK. 
	//	Valores de 1 a 0x1F
	
// REGISTRADOR EMC_STA_WAITWR
	// 	Atraso entre a opera��o de escrita para o pr�xima opera��o de acesso
	//	n+1 ciclos CCLK. 
	//	Valores de 1 a 0x1F
	
// REGISTRADOR EMC_STA_WAITTURN
	// 
	//	n+1 ciclos CCLK. 
	//	Valores de 1 a 0xF

// CONFIGURA��O DO BANCO 0 (CS0) Endere�o entre 0x8000_0000 a 0x80ff_ffff 


// CONFIGURA��ES PARA COMPACT_FLASH_IDE
#define COMPACT_FLASH_IDE_CFG			0x00000181	// Sem prote��o de buffer, utiliza wait extended, bus 16 bits
#define COMPACT_FLASH_IDE_WAITWEN		0x00000002
#define COMPACT_FLASH_IDE_WAITOEN		0x00000002
#define COMPACT_FLASH_IDE_WAITRD		0x0000001f
#define COMPACT_FLASH_IDE_WAITPAGE		0x0000001f
#define COMPACT_FLASH_IDE_WAITWR		0x0000001f
#define COMPACT_FLASH_IDE_WAITTURN		0x0000000f

// CONFIGURA��ES PARA A DISPOSITIVO DE 8 BITS EM GERAL (dispositivos gen�ricos de 8 bits)
#define DISPO_GENERIC_8BITS_CFG			0x00000180	// Sem prote��o de buffer, utiliza wait extended, buffer desabilitado, CS ativo em baixa e bus 8 bits com acesso simples
#define DISPO_GENERIC_8BITS_WAITWEN		0x00000002
#define DISPO_GENERIC_8BITS_WAITOEN		0x00000002
#define DISPO_GENERIC_8BITS_WAITRD		0x0000001f
#define DISPO_GENERIC_8BITS_WAITPAGE	0x0000001f
#define DISPO_GENERIC_8BITS_WAITWR		0x0000001f
#define DISPO_GENERIC_8BITS_WAITTURN	0x0000000f

// CONFIGURA��ES PARA A DISPOSITIVO DE 16 BITS EM GERAL (dispositivos gen�ricos de 16 bits)
#define DISPO_GENERIC_16BITS_CFG		0x00000181	// Sem prote��o de buffer, utiliza wait extended, buffer desabilitado, CS ativo em baixa e bus 16 bits com acesso simples
#define DISPO_GENERIC_16BITS_WAITWEN	0x00000002
#define DISPO_GENERIC_16BITS_WAITOEN	0x00000002
#define DISPO_GENERIC_16BITS_WAITRD		0x0000001f
#define DISPO_GENERIC_16BITS_WAITPAGE	0x0000001f
#define DISPO_GENERIC_16BITS_WAITWR		0x0000001f
#define DISPO_GENERIC_16BITS_WAITTURN	0x0000000f

// FAST SRAM de 16 BITS
#define FAST_SRAM_16BITS_CFG 			0x00000081	// Sem prote��o de buffer, n�o utiliza wait extended, buffer desabilitado, CS ativo em baixa e bus 16 bits com acesso simples
#define FAST_SRAM_16BITS_WAITWEN	 	0x00000000
#define FAST_SRAM_16BITS_WAITOEN	 	0x00000000
#define FAST_SRAM_16BITS_WAITRD	 		0x00000001
#define FAST_SRAM_16BITS_WAITPAGE 		0x00000000
#define FAST_SRAM_16BITS_WAITWR	 		0x00000000
#define FAST_SRAM_16BITS_WAITTURN 		0x00000000

// FAST SRAM de 32 BITS
#define FAST_SRAM_32BITS_CFG 	 		0x00000082 // Sem prote��o de buffer, n�o utiliza wait extended, buffer desabilitado, CS ativo em baixa e bus 32 bits com acesso simples
#define FAST_SRAM_32BITS_WAITWEN	 	0x00000000
#define FAST_SRAM_32BITS_WAITOEN	 	0x00000000
#define FAST_SRAM_32BITS_WAITRD	 		0x00000001
#define FAST_SRAM_32BITS_WAITPAGE 		0x00000000
#define FAST_SRAM_32BITS_WAITWR	 		0x00000000
#define FAST_SRAM_32BITS_WAITTURN 		0x00000000

// CHIPSET de 16 BITS. CHIPSET trabalhando a 54Mhz
#define CHIPSET_16BITS_CFG 	 			0x00000081 // Sem prote��o de buffer, n�o utiliza wait extended, buffer desabilitado, CS ativo em baixa e bus 16 bits com acesso simples
#define CHIPSET_16BITS_WAITWEN	 		0x00000002
#define CHIPSET_16BITS_WAITOEN	 		0x00000002
#define CHIPSET_16BITS_WAITRD	 		0x00000004
#define CHIPSET_16BITS_WAITPAGE 		0x00000000
#define CHIPSET_16BITS_WAITWR	 		0x00000003
#define CHIPSET_16BITS_WAITTURN 		0x00000000

#endif
