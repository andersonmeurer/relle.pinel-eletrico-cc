//
//  $Id: iap.c 188 2008-10-19 14:36:39Z jcw $
//  $Revision: 188 $
//  $Author: jcw $
//  $Date: 2008-10-19 10:36:39 -0400 (Sun, 19 Oct 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/iap/iap.c $
//
#include "iap.h"
#include "vic.h"
#include <string.h>

#if (IAP_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

static void iap_Call (void);
static u32 iapCommands [5];
static u32 iapResults [3];
#define iap_entry(a,b) ((void(*)())(IAP_LOCATION))(a,b)

// VARIÁVEIS DECLARADAS NO LINKSCRIPT
//extern u32 _ivector;
//extern u32 _evector;
extern u32 _itext;
extern u32 _etext;
//static u32 init_of_vector = (u32)&_ivector;
//static u32 end_of_vector = (u32)&_evector;
static u32 init_of_text = (u32)&_itext;
static u32 end_of_text = (u32)&_etext;


#if defined(LPC2468)
flashSectorToAddress_t flashSectorToAddress [] =  {
// Página 809 do datasheet 681
  { 0x00000000, 4096 },   // 0
  { 0x00001000, 4096 },   // 1
  { 0x00002000, 4096 },   // 2
  { 0x00003000, 4096 },   // 3
  { 0x00004000, 4096 },   // 4
  { 0x00005000, 4096 },   // 5
  { 0x00006000, 4096 },   // 6
  { 0x00007000, 4096 },   // 7
  { 0x00008000, 32768 },  // 8
  { 0x00010000, 32768 },  // 9
  { 0x00018000, 32768 },  // 10
  { 0x00020000, 32768 },  // 11
  { 0x00028000, 32768 },  // 12
  { 0x00030000, 32768 },  // 13
  { 0x00038000, 32768 },  // 14
  { 0x00040000, 32768 },  // 15
  { 0x00048000, 32768 },  // 16
  { 0x00050000, 32768 },  // 17
  { 0x00058000, 32768 },  // 18
  { 0x00060000, 32768 },  // 19
  { 0x00068000, 32768 },  // 20
  { 0x00070000, 32768 },  // 21
  { 0x00078000, 4096 },   // 22
  { 0x00079000, 4096 },   // 23
  { 0x0007a000, 4096 },   // 24
  { 0x0007b000, 4096 },   // 25
  { 0x0007c000, 4096 },   // 26
  { 0x0007d000, 4096 },   // 27
};
#elif defined(LPC1788)
// Página 809 do datasheet
// Flash de 512KB
flashSectorToAddress_t flashSectorToAddress [] =  {
  { 0x00000000, 4096 },   // 0
  { 0x00001000, 4096 },   // 1
  { 0x00002000, 4096 },   // 2
  { 0x00003000, 4096 },   // 3
  { 0x00004000, 4096 },   // 4
  { 0x00005000, 4096 },   // 5
  { 0x00006000, 4096 },   // 6
  { 0x00007000, 4096 },   // 7
  { 0x00008000, 4096 },   // 8
  { 0x00009000, 4096 },   // 9
  { 0x0000a000, 4096 },   // 10
  { 0x0000b000, 4096 },   // 11
  { 0x0000c000, 4096 },   // 12
  { 0x0000d000, 4096 },   // 13
  { 0x0000e000, 4096 },   // 14
  { 0x0000f000, 4096 },   // 15
  { 0x00010000, 32768 },  // 16
  { 0x00018000, 32768 },  // 17
  { 0x00020000, 32768 },  // 18
  { 0x00028000, 32768 },  // 19
  { 0x00030000, 32768 },  // 20
  { 0x00038000, 32768 },  // 21
  { 0x00040000, 32768 },  // 22
  { 0x00048000, 32768 },  // 23
  { 0x00050000, 32768 },  // 24
  { 0x00058000, 32768 },  // 25
  { 0x00060000, 32768 },  // 26
  { 0x00068000, 32768 },  // 27
  { 0x00070000, 32768 },  // 28
  { 0x00078000, 32768 },  // 29
};
#elif defined(LPC1752)
// Página 620 do datasheet
// Flash de 64KB
flashSectorToAddress_t flashSectorToAddress [] =  {
  { 0x00000000, 4096 },   // 0
  { 0x00001000, 4096 },   // 1
  { 0x00002000, 4096 },   // 2
  { 0x00003000, 4096 },   // 3
  { 0x00004000, 4096 },   // 4
  { 0x00005000, 4096 },   // 5
  { 0x00006000, 4096 },   // 6
  { 0x00007000, 4096 },   // 7
  { 0x00008000, 4096 },   // 8
  { 0x00009000, 4096 },   // 9
  { 0x0000a000, 4096 },   // 10
  { 0x0000b000, 4096 },   // 11
  { 0x0000c000, 4096 },   // 12
  { 0x0000d000, 4096 },   // 13
  { 0x0000e000, 4096 },   // 14
  { 0x0000f000, 4096 },   // 15
};
#elif defined(LPC1751)
// Página 620 do datasheet
// Flash de 32KB
flashSectorToAddress_t flashSectorToAddress [] =  {
  { 0x00000000, 4096 },   // 0
  { 0x00001000, 4096 },   // 1
  { 0x00002000, 4096 },   // 2
  { 0x00003000, 4096 },   // 3
  { 0x00004000, 4096 },   // 4
  { 0x00005000, 4096 },   // 5
  { 0x00006000, 4096 },   // 6
  { 0x00007000, 4096 },   // 7
};
#else
   #error Este dispositivo não foi implementado IAP
#endif


#if defined(arm7tdmi)
static u32 _cpsr;
static void iap_irqDisable(void);
static void iap_irqEnable(void);

static void iap_irqDisable(void) {
	_cpsr = ints_Disable();
}

static void iap_irqEnable(void) {
	ints_Restore(_cpsr);
}
#endif

#if defined(cortexm3)
#define iap_irqDisable 	irq_Disable
#define iap_irqEnable 	irq_Enable
#endif


//###################################################################################################################
// COMANDOS USUAIS NAS APLICAÇÕES
//###################################################################################################################
//###################################################################################################################


// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Comando para apagar os setores da FLASH
// Parametros:	startingSector: Setor incial para o apagamento
//				endingSector:	Setor final para o apagamento
// Retorna:		pdPASS: Caso os setores foram apagados com sucesso
//				errIAP_PARAM_INVALID: Caso o endereço final é menor que o endereço inicial
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_INVALID: Caso um dos setores (inicil ou final) for inválido
//				errIAP_SECTOR_NOT_PREPARED: Caso se os setores não foram preparados para o apagamento
// -------------------------------------------------------------------------------------------------------------------
int iap_ClearSectors (u32 startingSector, u32 endingSector) {
	int result;

	#if defined(arm7tdmi)
	u32 _memmap;
	#endif

	#if (IAP_USE_DEBUG == pdON)
	plog("IAP: clear sector [%u %u]"CMD_TERMINATOR, startingSector, endingSector);
	#endif

	// SE A GRAVAÇÃO FOR NO VETOR DE Exceções, CHAVEAR O VETOR PARA FLASH CASO ESTIVER EM OUTRO LUGAR
	irq_Disable();

	#if defined(arm7tdmi)
	if (startingSector == 0)  {
		_memmap = MEMMAP;
		MEMMAP = MEMMAP_FLASH;
		#if (IAP_USE_DEBUG == pdON)
		plog("FLASH 0"CMD_TERMINATOR);
		#endif
	}
	#endif

	// PREPAR SETORES PARA O APAGAMENTO
	if ((result = iap_PrepareSectors(startingSector, endingSector) ) !=pdPASS)
		goto end_iap_ClearSectors;


	result = iap_EraseSectors(startingSector, endingSector);

	end_iap_ClearSectors:

	#if defined(arm7tdmi)
	if (startingSector == 0)  {
		MEMMAP = _memmap;
		#if (IAP_USE_DEBUG == pdON)
		plog("FLASH 0"CMD_TERMINATOR);
		#endif
	}
	#endif

	iap_irqEnable();

	#if (IAP_USE_DEBUG == pdON)
	if (result == pdPASS)
		plog("IAP: clear ok [%u %u]"CMD_TERMINATOR, startingSector, endingSector);
	else  plog("IAP: clear fail[%d] [%u %u]"CMD_TERMINATOR, result, startingSector, endingSector);
	#endif

	return result;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Escreve um buffer de dados na memória FLASH do ARM

// ATENÇÃO: 	É PRECISO QUE OS SETORES ESTEJAM APAGADOS com iap_ClearSectors()

// Parametros:	address: Endereço da FLASH para ser gravado. DEVE SER DENTRO DOS LIMITES DE 256 BYTES
//				buffer: Ponteiro do buffer de dados a ser gravado na FLASH
// 				bufferLen: O tamanho do buffer deve ser de 256, 512, 1024 OU 4096 bytes
// Retorna:		pdPASS: Caso a gravação do conetúdo da RAM foi feito com sucesso na FLASH
//				errIAP_SIZE_INVALID: O tamanho do buffer de gravação é inválido. Deve ser 256, 512, 1024 ou 4096 bytes
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço da FLASH
//				errIAP_SECTOR_NOT_PREPARED: O setor a ser gravado não foi preparado para gravação
// 				errIAP_SRC_ADDR:
// 				errIAP_DST_ADDR:
// 				errIAP_SRC_ADDR_NOT_MAPPED:
// 				errIAP_DST_ADDR_NOT_MAPPED:
//				errIAP_COUNT: Caso o contador de bytes não for multiplo de 4
// 				errIAP_COMPARE: Os dados gravados no FLASH não foi conferiru com o buffer de dados
// -------------------------------------------------------------------------------------------------------------------
int iap_WriteSectors (u32 address, u8 *buffer, u16 bufferLen) {
	int result;
	u32 sector;
	#if defined(arm7tdmi)
	u32 _memmap;
	#endif

	//if (!iap_IsSafeSector (startingSector) || !iap_IsSafeSector (endingSector))
		//return -1;

	if ( (bufferLen!=256) && (bufferLen!=512) && (bufferLen!=1024) && (bufferLen!=4096) )
		return errIAP_SIZE_INVALID;

	// Buscar o setor de acordo com endereço a ser gravado
	// 		Cada gravação pelo iap_WriteSectors será sempre no mesmo setor da FLASH
	if ((result = iap_AddressToSector(address, &sector)) != pdPASS)
		return result;

	#if (IAP_USE_DEBUG == pdON)
	plog("IAP: write flash addr 0x%x sector %u"CMD_TERMINATOR, address, sector);
	#endif

	iap_irqDisable();

	#if defined(arm7tdmi)
	// SE A GRAVAÇÃO FOR NO VETOR DE Exceções, CHAVEAR O VETOR PARA FLASH CASO ESTIVER EM OUTRO LUGAR
	if (sector == 0)  {
		_memmap = MEMMAP;
		MEMMAP = MEMMAP_FLASH;
		#if (IAP_USE_DEBUG == pdON)
		plog("IAP: change MEMMAP [%u]"CMD_TERMINATOR, MEMMAP);
		#endif
	}
	#endif

	// PREPARAR O SETOR PARA GRAVAÇÃO
	if ((result = iap_PrepareSectors (sector, sector)) != pdPASS)
		goto end_iap_WriteSectors;


	// COPIAR O CONTEÚDO DA RAM (buffer) PARA O FLASH
	if ((result = iap_CopyRAMtoFLASH(address, buffer, bufferLen)) != pdPASS)
		goto end_iap_WriteSectors;

	#if (IAP_USE_DEBUG == pdON)
	plog("IAP: write flash addr 0x%x [%u] len %u"CMD_TERMINATOR, address, sector, bufferLen);
	#endif

	// Comparar o conteúdo da ram (buffer) com a flash.
	//	Para verificar se o conteúdo foi gravado corretamente
	result = iap_Compare(address, buffer, bufferLen);

	end_iap_WriteSectors:

	#if defined(arm7tdmi)
	if (sector == 0)
		MEMMAP = _memmap;
	#endif

	iap_irqEnable();

	#if (IAP_USE_DEBUG == pdON)
	plog("IAP: write ok"CMD_TERMINATOR);
	#endif

	return result;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Preenche os setores com um determinado valor

// ATENÇÃO: 	É PRECISO QUE OS SETORES ESTEJAM APAGADOS com iap_ClearSectors

// Parametros:	startingSector: Setor inicial para preenchimento
//				endingSector: Setor final para preenchimento
//				byte:  Valor para ser preenchido nos setores
// Retorna:		pdPASS: Caso o preenchimento foi feito com sucesso
//				errIAP_PARAM_INVALID: Caso o endereço final é menor que o endereço inicial
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_INVALID: Caso um dos setores (inicil ou final) for inválido
//				errIAP_SECTOR_NOT_PREPARED: Caso se os setores não foram preparados para o apagamento
// 				errIAP_SRC_ADDR:
// 				errIAP_DST_ADDR:
// 				errIAP_SRC_ADDR_NOT_MAPPED:
// 				errIAP_DST_ADDR_NOT_MAPPED:
// 				errIAP_COMPARE: Caso as duas regiões não contém os mesmos dados
//				errIAP_COUNT: Caso o contador de bytes não for multiplo de 4
// -------------------------------------------------------------------------------------------------------------------
int iap_FillSectors (u32 startingSector, u32 endingSector, u8 byte) {
	int result;
	u32 sector;
	u8 buffer [256];
	#if defined(arm7tdmi)
	u32 _memmap;
	#endif

	//if (!iap_IsSafeSector (startingSector) || !iap_IsSafeSector (endingSector))
		//return -1;

	if (endingSector < startingSector)
		return errIAP_PARAM_INVALID;

	memset (buffer, byte, sizeof(buffer));
	result = pdPASS;
	iap_irqDisable();

	for (sector = startingSector; sector <= endingSector; sector++) {
		u16 i;
		u32 address;
		u32 sectorSize;

		#if defined(arm7tdmi)
		// SE A GRAVAÇÃO FOR NO VETOR DE Exceções, CHAVEAR O VETOR PARA FLASH CASO ESTIVER EM OUTRO LUGAR
		if (sector == 0)  {
			_memmap = MEMMAP;
			MEMMAP = MEMMAP_FLASH;
			#if (IAP_USE_DEBUG == pdON)
			plog("IAP: write flash [%u]"CMD_TERMINATOR, MEMMAP);
			#endif
		}
		#endif

		if ((result = iap_SectorToAddress (sector, &address, &sectorSize)) != pdPASS)
			goto end_iap_FillSectors;

		// FAÇO PARA OS TODOS OS BYTES DO SETOR
		for (i = 0; i < sectorSize; i += sizeof(buffer)) {

			// PREPARAR O SETOR PARA GRAVAÇÃO
			if ((result = iap_PrepareSectors (sector, sector)) != pdPASS)
				break;

			// COPIAR O CONTEÚDO DA RAM (buffer) PARA O FLASH
			if ((result = iap_CopyRAMtoFLASH (address + i, buffer, sizeof(buffer))) != pdPASS)
				break;

			// COMPARAR O CONTEÚDO DA RAM (buffer) COM A FLAHS.
			//	Para verificar se o conteúdo foi gravado corretamente
			if ((result = iap_Compare (address + i, buffer, sizeof (buffer))) != pdPASS)
				break;
		}

		end_iap_FillSectors:

		#if defined(arm7tdmi)
		if (sector == 0)  {
			MEMMAP = _memmap;
			#if (IAP_USE_DEBUG == pdON)
			plog("IAP: write ok [%u]"CMD_TERMINATOR, MEMMAP);
			#endif
		}
		#endif

		if (result != pdPASS)
			break;
	}

	iap_irqEnable();
	return result;
}

//###################################################################################################################
// COMANDOS IAP
//###################################################################################################################
//###################################################################################################################
// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Prepara os setores para apagamento ou gravação
// Parametros:	startingSector: Setor inicial para preparação
//				endingSector: Setor final para preparação
// Retorna:		pdPASS: Caso a preparação foi feita com sucesso
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_INVALID: Caso um dos setores (inicil ou final) for inválido
// -------------------------------------------------------------------------------------------------------------------
int iap_PrepareSectors (u32 startingSector, u32 endingSector) {
//	  if (!iapIsSafeSector (startingSector) || !iapIsSafeSector (endingSector))
	//    return -1;

	iapCommands [0] = IAP_CMD_PREPARE;
	iapCommands [1] = startingSector;
	iapCommands [2] = endingSector;
	iap_Call ();

	#if (IAP_USE_DEBUG == pdON)
	plog("IAP: prepare sector [%u %u]"CMD_TERMINATOR, startingSector, endingSector);
	#endif

	if (iapResults [0] == IAP_RESULT_BUSY)
		return  errIAP_BUSY;
	else if (iapResults [0] == IAP_RESULT_INVALID_SECTOR)
		return  errIAP_SECTOR_INVALID;
	else
		return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Grava um buffer de dados na memória FLASH do ARM

// ATENÇÃO: 	É PRECISO QUE OS SETORES ESTEJAM APAGADOS E PREPARADOS

// Parametros:	address: Endereço da FLASH para ser agravado. DEVE SER DENTRO DOS LIMITES DE 256 BYTES
//				buffer: Ponteiro do buffer de dados a ser gravado na FLASH
// 				bufferLen: O tamanho do buffer. DEVE SER DE 256, 512, 1024 OU 4096 BYTES
// Retorna:		pdPASS: Caso a gravação do conetúdo da RAM foi feito com sucesso na FLASH
//				errIAP_SECTOR_NOT_PREPARED;
// 				errIAP_BUSY;
// 				errIAP_SRC_ADDR;
// 				errIAP_DST_ADDR;
// 				errIAP_SRC_ADDR_NOT_MAPPED;
// 				errIAP_DST_ADDR_NOT_MAPPED;
// 				errIAP_COUNT;
// -------------------------------------------------------------------------------------------------------------------
//  IAP_CMD_COPYRAMTOFLASH can span multiple sectors (2, at any rate, since bufferLen
//  must be 256, 512, 1024 or 4096, and the smallest sectors are 4096 bytes).  Although
//  more than 2 sectors can be prepared for writing, it's useless to do so, since
//  after each IAP_CMD_COPYRAMTOFLASH, the sectors are re-locked.
int iap_CopyRAMtoFLASH (u32 address, u8 *buffer, u16 bufferLen) {
	iapCommands [0] = IAP_CMD_COPYRAMTOFLASH;
	iapCommands [1] = address;
	iapCommands [2] = (u32)buffer;
	iapCommands [3] = (u32)bufferLen;
	iapCommands [4] = CCLK/1000;
	iap_Call ();

	if (iapResults [0] == IAP_RESULT_BUSY)
		return errIAP_BUSY;
	else if (iapResults [0] == IAP_RESULT_SECTOR_NOT_PREPARED)
		return errIAP_SECTOR_NOT_PREPARED;
	else if (iapResults [0] == IAP_RESULT_SRC_ADDR_ERROR)
		return errIAP_SRC_ADDR;
	else if (iapResults [0] == IAP_RESULT_DST_ADDR_ERROR)
		return errIAP_DST_ADDR;
	else if (iapResults [0] == IAP_RESULT_SRC_ADDR_NOT_MAPPED)
		return errIAP_SRC_ADDR_NOT_MAPPED;
	else if (iapResults [0] == IAP_RESULT_DST_ADDR_NOT_MAPPED)
		return errIAP_DST_ADDR_NOT_MAPPED;
	else if (iapResults [0] == IAP_RESULT_COUNT_ERROR)
		return errIAP_COUNT;
	else
		return pdPASS;
}


// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Comando para apagar os setores da FLASH
// Parametros:	startingSector: Setor incial para o apagamento
//				endingSector:	Setor final para o apagamento
// Retorna:		pdPASS: Caso os setores foram apagados com sucesso
//				errIAP_PARAM_INVALID: Caso o endereço final é menor que o endereço inicial
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_INVALID: Caso um dos setores (inicil ou final) for inválido
//				errIAP_SECTOR_NOT_PREPARED: Caso se os setores não foram preparados para o apagamento
// -------------------------------------------------------------------------------------------------------------------
int iap_EraseSectors (u32 startingSector, u32 endingSector) {

	//if (!iap_IsSafeSector (startingSector) || !iap_IsSafeSector (endingSector))
		//return -1;

	if (endingSector < startingSector)
		return errIAP_PARAM_INVALID;

	// APAGAR SETORES
	iapCommands [0] = IAP_CMD_ERASE;
	iapCommands [1] = startingSector;
	iapCommands [2] = endingSector;
	iapCommands [3] = CCLK/1000;
	iap_Call ();

	#if (IAP_USE_DEBUG == pdON)
	plog("IAP: erase sector [%u %u]"CMD_TERMINATOR, startingSector, endingSector);
	#endif

	if (iapResults [0] == IAP_RESULT_BUSY)
		return  errIAP_BUSY;
	else if (iapResults [0] == IAP_RESULT_SECTOR_NOT_PREPARED)
		return  errIAP_SECTOR_NOT_PREPARED;
	else if (iapResults [0] == IAP_RESULT_INVALID_SECTOR)
		return  errIAP_SECTOR_INVALID;
	else
		return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Checa se os setores estão em branco
// Parametros:	startingSectorr: Setor para inicio para análise
// 				endingSector: Setor final para análise
// Retorna:		pdPASS: Caso o setores estão em branco
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_NOT_BLANK: Caso o setores contém dados
//				errIAP_SECTOR_INVALID: Caso foi acessado um setor inválido
// -------------------------------------------------------------------------------------------------------------------
int iap_IsSectorsBlank (u32 startingSector, u32 endingSector) {
	iapCommands [0] = IAP_CMD_BLANKCHECK;
	iapCommands [1] = startingSector;
	iapCommands [2] = endingSector;
	iap_Call ();

	if (iapResults [0] == IAP_RESULT_BUSY)
		return errIAP_BUSY;
	else if (iapResults [0] == IAP_RESULT_SECTOR_NOT_BLANK)
		return errIAP_SECTOR_NOT_BLANK;
	else if (iapResults [0] == IAP_RESULT_INVALID_SECTOR)
		return errIAP_SECTOR_INVALID;
	else
		return pdPASS;
}


// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Lê o número de identificação do ARM
// Parametros:	Nenhum
// Retorna: 	Retorna com ID da ARM
// -------------------------------------------------------------------------------------------------------------------
u32 iap_ReadPartID (void) {
	iapCommands [0] = IAP_CMD_READPARTID;
	iap_Call ();
	return iapResults [1];
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê a versão do bootloader do ARM
// Parametros:	Nenhum
// Retorna:		Retorna com a versão do bootloader
// -------------------------------------------------------------------------------------------------------------------
u32 iap_ReadBootCodeVersion (void) {
	iapCommands [0] = IAP_CMD_READBOOTCODEVER;
	iap_Call ();
	return iapResults [1];
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Faz uma comparação entre a região FLASH com um buffer de dados
// Parametros:	address: Endereço da FLASH para ser comparado
//				buffer: Ponteiro do buffer de dados a ser comparado com a FLASH
// 				bufferLen: O tamanho do buffer. DEVE SER DE 256, 512, 1024 OU 4096 BYTES
// Retorna:		pdPASS: Caso a comparação foi feita com sucesso e as duas regiões contém os mesmos dados
// 				errIAP_COMPARE: Caso as duas regiões não contém os mesmos dados
//				errIAP_COUNT: Caso o contador de bytes não for multiplo de 4
// 				errIAP_SRC_ADDR:
// 				errIAP_DST_ADDR;
// 				errIAP_SRC_ADDR_NOT_MAPPED;
// 				errIAP_DST_ADDR_NOT_MAPPED;
// -------------------------------------------------------------------------------------------------------------------
int iap_Compare (u32 address, u8 *buffer, u16 bufferLen) {
	iapCommands [0] = IAP_CMD_COMPARE;
	iapCommands [1] = address;
	iapCommands [2] = (u32) buffer;
	iapCommands [3] = bufferLen;
	iap_Call ();

	if (iapResults [0] == IAP_RESULT_COMPARE_ERROR)
		return errIAP_COMPARE;
	else if (iapResults [0] == IAP_RESULT_COUNT_ERROR)
		return errIAP_COUNT;
	else if (iapResults [0] == IAP_RESULT_SRC_ADDR_ERROR)
		return errIAP_SRC_ADDR;
	else if (iapResults [0] == IAP_RESULT_DST_ADDR_ERROR)
		return errIAP_DST_ADDR;
	else if (iapResults [0] == IAP_RESULT_SRC_ADDR_NOT_MAPPED)
		return errIAP_SRC_ADDR_NOT_MAPPED;
	else if (iapResults [0] == IAP_RESULT_DST_ADDR_NOT_MAPPED)
		return errIAP_DST_ADDR_NOT_MAPPED;
	else
		return pdPASS;
}



//###################################################################################################################
// COMANDOS AUXILIARES
//###################################################################################################################
//###################################################################################################################
/*
//  Returns a safe sector number or -1 if none available
int iap_FindSafeSector (void) {
	unsigned int i;

	for (i = 0; i < arrsizeof (flashSectorToAddress); i++)
		if (iap_IsSafeSector (i))
			return i;

	iapErrno = IAP_RESULT_X_NOSAFEREGIONAVAIL;
	return -1;
}

//  1 == address in safe region, 0 if not
int iap_IsSafeAddress (u32 address) {
	int eotSector;
	int addressSector;

	if ((eotSector = iap_AddressToSector (end_of_text)) == -1)
		return 0;
	if ((addressSector = iap_AddressToSector (address)) == -1)
		return 0;

	if (addressSector <= eotSector) {
		iapErrno = IAP_RESULT_X_NOTSAFEREGION;
		return 0;
	}

	return 1;
}
*/

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Checa se um determinado setor pertece ao firmware
// Parametros:	Sector: Setor a ser consultado
// Retorna:		pdPASS: Caso o setor não pertence ao firmware
//				errIAP_NO_SAFE_REGION: Caso o setor pertence ao firmware
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço
// -------------------------------------------------------------------------------------------------------------------
int iap_IsSafeSector (u32 sector) {
	int result;
	u32 cmpSectorInit;
	u32 cmpSectorEnd;

	// CAPTURO ENDEREÇOS INICIAL E FINAL DO FIRMWARE NA FLASH
	if ( (result = iap_AddressToSector (init_of_text, &cmpSectorInit)) != pdPASS)
		return result;

	if ( (result = iap_AddressToSector (end_of_text, &cmpSectorEnd)) != pdPASS)
		return result;

	if ( (sector >= cmpSectorInit) && (sector <= cmpSectorEnd) )
		return errIAP_NO_SAFE_REGION;

	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Checa se o setor existe no ARM
// Parametros:	Numero do setor a ser consultado
// Retorna:		pdPASS: Caso o setor exista
//				errIAP_SECTOR_INVALID: Caso o setor não exista
// -------------------------------------------------------------------------------------------------------------------
int iap_IsValidSector (u32 sector) {
	if ((sector >= (u32) arrsizeof (flashSectorToAddress))) {
		return errIAP_SECTOR_INVALID;
	}

	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Traz a que setor pertence o endereço da FLASH
// Parametros:	Address: Endereço da FLASH para consulta
//				sector: Ponteiro do setor para retorno
// Retorna:		pdPASS: Caso foi encontrado o setor
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço
// -------------------------------------------------------------------------------------------------------------------
int iap_AddressToSector (u32 address, u32 *sector) {
	u32 i;

  	for (i = 0; i < (u32) arrsizeof (flashSectorToAddress); i++)
    	if (address < (flashSectorToAddress [i].address + flashSectorToAddress [i].sizeInBytes)) {
    		*sector = i;
			#if (IAP_USE_DEBUG == pdON)
    		plog("IAP: addr2sector [0x%x %u]"CMD_TERMINATOR, address, *sector);
			#endif

    		return pdPASS;
    	}

  	return errIAP_SECTOR_INVALID;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Traz o endereço e o tamanho do setor de um determinado setor
// Parametros:	sectorNumber: Setor a ser consultado
//				address: Ponetiro para retorno do endereço do setor
//				sectorSize: Ponteiro para o retorno do tamanho do setor
// Retorna:		pdPASS: Caso foi encontrado o setor
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o endereço do respectivo setor
// -------------------------------------------------------------------------------------------------------------------
int iap_SectorToAddress (u32 sectorNumber, u32 *address, u32 *sectorSize) {
  	if (sectorNumber >= (u32) arrsizeof (flashSectorToAddress))
    	return errIAP_SECTOR_INVALID;

  	if (address)
    	*address = flashSectorToAddress [sectorNumber].address;
  	if (sectorSize)
    	*sectorSize = flashSectorToAddress [sectorNumber].sizeInBytes;

	#if (IAP_USE_DEBUG == pdON)
  	plog("IAP: sector2addr [%u 0x%x %u]"CMD_TERMINATOR, sectorNumber, *address, *sectorSize);
	#endif

  	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Função de chamada das funções IAP na boot block do ARM
// Parametros:	Nenhum
// Retorna: 	Nada
// -------------------------------------------------------------------------------------------------------------------
static void iap_Call (void) {
	iap_irqDisable();
	iap_entry(iapCommands, iapResults);
	iap_irqEnable();
}
