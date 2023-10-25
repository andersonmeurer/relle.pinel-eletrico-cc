#include "flash_prog.h"
#include <string.h>
#include "iap.h"
#include "intel_hex.h"
#include "utils.h"
#include "vic.h"

#if (FLASH_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

extern flashSectorToAddress_t flashSectorToAddress;

// VARS DECLARADAS NO LINKSCRIPT
extern u32 _idatastore;
extern u32 _itext;
static u32 init_of_text = (u32)&_itext;
static u32 init_of_datastore = (u32)&_idatastore;

typedef struct {
	u16 cacheIdx;				// Guarda a localização onde deve gravar os próximo dados na cache
	u8 cacheEmpty;				// Sinaliza quando a cache está vazia ou quando contém dados para ser gravados na FLASH
	u8 vctTblBootldEmpty;		// Sinaliza que o buffer temporário do vetor do bootloader está vazio
	u32 addrFlashToSaveCache;	// Guarda o endereço da FLASH onde os dados da cache deve ser salvos
	u32 addrCopy;				// Endereço da FLASH para copiar os dados que foram interpretados da linha de comando Intel HEX
	u32 addrBase;				// Endereço base da FLASH para copiar os dados que foram interpretados da linha de comando Intel HEX
								//		Este é o endereço alto de 16 bits que será complementado pelo endereço baixo de 16 bits
								//			recebido atraves da linha de comando 0 Intel HEX
	u32 appCodeKey;				// Código de validação da aplicação a ser gravado no vetor de int

	u32 bootldSectorInit;		// Setor inicial na FLASH onde está o bootloader

	// DATASTORE - REGIÃO DA FLASH PARA ARMAZENAMENTO DE DADOS DO BOOTLOADER
	u32 DataStoreSectorSize;	// Tamanho do setor da região de dados na FLASH
	u32 DataStoreSector;		// O setor inicial da região de dados na FLASH
	u32 DataStoreAddr;			// O endereço inicial da região de dados na FLASH
} flash_t;

// CACHE DE DADOS DE GRAVAÇÃO NA FLASH
// Na FLASH só podemos gravar dados em 256, 512, 1024 ou 4096 bytes por vez.
//	logo foi criada uma cache para quando esta estiver cheia gravaremos a cache na FLASH.
//	Para ser compativel para todos os ARMs usaremos a gravação de 256 bytes.
static u8 cache[CACHE_SIZE]; // Cahce declarado fora da estrutura acima por causa do alinhamento que o compilador faz para 32 bits

// Guarda o vetor de ints da aplicação vindo do arquivo Intel HEX temporariamente durante o processo
static u8 vctTblApp[VECTORTABLE_SIZE];
// Guarda o vetor de ints do bootlodader do dataStore temporariamente, pois durante o processo o dataStore será apagado
static u8 vctTblBootld[VECTORTABLE_SIZE];

static flash_t flash;
static int saveToCache(u8 *buf, u16 addrOffSet, u8 nBytes);
static int commitCache(void);

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa o processo de gravação de dados na FLASH do ARM
//				Esta função deve ser chamada ao iniciar o bootloader
//					e quando iniciar algum procedimento de gravação de aplicativo na FLASH
// Parametros:	Nenhum
// Retorna:		pdPASS:
//				errIAP_INVALID_SECTOR: Caso não foi encontrado o setor do respectivo endereço da FLASH
// -------------------------------------------------------------------------------------------------------------------
int flashProg_Init (void) {
	memset(cache, 0xFF, CACHE_SIZE);
	memset(vctTblApp, 0xFF, VECTORTABLE_SIZE);
	memset(vctTblBootld, 0xFF, VECTORTABLE_SIZE);
	flash.cacheEmpty = pdTRUE; // Sinaliza que a cache de gravação da flash está vazia
	flash.vctTblBootldEmpty = pdTRUE; // Sinaliza que o buffer para gravar o vetores do bootloader está vazio
	flash.addrBase = 0;
	flash.addrFlashToSaveCache = 0;
	flash.appCodeKey = 0;

	// CAPTURO O ENDEREÇO INICIAL E O TAMANHO DO SETOR NA FLASH PARA GRAVAR OS DADOS DO BOOTLOADER
	// O SETOR DE ARMAZENAMENTO DE DADOS DEVE SER UM SETOR ANTES DO BOOTLOADER

	// Converter para setores o endereço inicial do programa do bootloader
	if (iap_AddressToSector (init_of_datastore, &flash.DataStoreSector) != pdPASS)
		return errIAP_SECTOR_INVALID;

	// O setor de armazenamento de dados deve ser um setor anteriror ao do programa
	// flash.DataStoreSector-=1; CASO USAR init_of_text

	// Converter para endereço o inicio do setor de armazenamento de dados
	if (iap_SectorToAddress(flash.DataStoreSector, &flash.DataStoreAddr, &flash.DataStoreSectorSize) != pdPASS)
		return errIAP_SECTOR_INVALID;


	if (iap_AddressToSector (init_of_text, &flash.bootldSectorInit) != pdPASS)
		return errIAP_SECTOR_INVALID;

	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Copia os vetores de interrupções do bootloader da posição 0 da flash para DATASTORE
// Parametros:	Nenhum
// Retorna:		pdPASS: se a gravação foi feita com sucesso
//				errIAP_SIZE_INVALID: O tamanho do buffer de gravação é inválido. Deve ser 256, 512, 1024 ou 4096 bytes
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço da FLASH
//				errIAP_PARAM_INVALID: Caso o endereço final é menor que o endereço inicial
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_NOT_PREPARED: O setor a ser gravado não foi preparado para gravação
// 				errIAP_SRC_ADDR:
// 				errIAP_DST_ADDR:
// 				errIAP_SRC_ADDR_NOT_MAPPED:
// 				errIAP_DST_ADDR_NOT_MAPPED:
// 				errIAP_COMPARE: Os dados gravados no FLASH não foi conferiru com o buffer de dados
//				errIAP_COUNT: Caso o contador de bytes não for multiplo de 4
// -------------------------------------------------------------------------------------------------------------------
int flashProg_SaveVectorBoot(void) {
	int result;

	// Limpar o setor DataStore na FLASH
	if ((result = iap_ClearSectors (flash.DataStoreSector, flash.DataStoreSector)) != pdPASS)
		return result;

	memset(cache, 0xFF, CACHE_SIZE); // limpa a cache

	// copia o conteúdo da FLASH da posição 0 para cache
	u8 *p = 0;
	memcpy(cache+VECTORTABLE_SIZE, p, VECTORTABLE_SIZE);	// +64 para defender os vetores do programa do usuário

	// Salvar o conteúdo da cache para DATASTORE
	flash.addrFlashToSaveCache = flash.DataStoreAddr;
	flash.cacheEmpty = pdFALSE;
	return commitCache();
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com o endereço inicial do DATASTORE na FLASH
// -------------------------------------------------------------------------------------------------------------------
u32 *flashProg_DataStoreAddr(void) {
	return (u32*)flash.DataStoreAddr; // Retorna com o 4 primeiros bytes da memória flash
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Apaga o setor
// Parametro:	Nenhum
// Retorna:		pdPASS: Caso os setores foram apagados com sucesso
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
int flashProg_Format(void) {
	u32 *pSouce = (u32 *)flash.DataStoreAddr+(VECTORTABLE_SIZE/4); // (dwords = 64bytes para lpc2468 ou 256 para lpc1788) para defender os vetores do programa do usuário

	memset(cache, 0xFF, CACHE_SIZE);

	#if (FLASH_USE_DEBUG == pdON)
	plog("F: FORMAT [%d] "CMD_TERMINATOR, flash.vctTblBootldEmpty);
	plog("F: erase cache [0x%x] size %u"CMD_TERMINATOR, cache, sizeof(cache));
	#endif

	// salvar os vetores de ints do bootloader
	if (flash.vctTblBootldEmpty) {// Checa se o buffer dos vetores do bootloader está vazio
		memcpy(vctTblBootld, pSouce, VECTORTABLE_SIZE);
		#if (FLASH_USE_DEBUG == pdON)
		plog("F: copy from datastore bootld[0x%x] to vctTblBootld [0x%x] "CMD_TERMINATOR, pSouce, vctTblBootld);
		#endif
	}

	#if (FLASH_USE_DEBUG == pdON)
	plog("F: clear datastore %u"CMD_TERMINATOR, flash.DataStoreSector);
	plog("F: clear sectors %u to %u"CMD_TERMINATOR, 0, flash.bootldSectorInit-1);
	plog("F: copy from vctTblBootld [0x%x] to cache [0x%x] "CMD_TERMINATOR, vctTblBootld, cache);
	plog("F:    [0x%x%x%x%x]"CMD_TERMINATOR, vctTblBootld[0], vctTblBootld[1], vctTblBootld[2], vctTblBootld[3]);
	plog("F: save cache to addr 0 in the flash"CMD_TERMINATOR);
	#endif

	// apagar o dataStore e região da aplicação na FLASH
	iap_ClearSectors (flash.DataStoreSector, flash.DataStoreSector);
	// apagar toda a FLASH da região do código do usuário
	iap_ClearSectors (0, flash.bootldSectorInit-1);
	// Salvar na cache os vetores de ints do bootloader armazenado temporariamente
	memcpy(cache, vctTblBootld, VECTORTABLE_SIZE);

	memset(vctTblBootld, 0xFF, VECTORTABLE_SIZE);
	flash.vctTblBootldEmpty = pdTRUE; // Sinaliza que o buffer para gravar o vetores do bootloader está vazio

	// Gravar os vetores de ints do bootloader no inicio da FLASH
	return iap_WriteSectors (0, cache, CACHE_SIZE);
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Processa os comandos Intel HEX passados pelo parametro buffer e gerencia a gravação dos dados bna FLASH
// Parametros:  buff: Ponteiro para o buffer de dados contendo a linha de comando no formato Intel HEX
// Retorna:		pdPASS: Caso a linha de comandos foi processada com sucesso
//				errINTEL_HEX_DELIMITER: Não foi encontrado o delimitador de comandos.
//				errINTEL_HEX_CHECKSUM: Erro de checsum da linha de comando Intel HEX
// 				errINTEL_HEX_NUMBER_INVALID: O caractere presente na linha de comando não é um digito
// 				errINTEL_HEX_COMMAND_INVALID;
//				errIAP_PARAM_INVALID: Caso o endereço final é menor que o endereço inicial
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço da FLASH
//				errIAP_SECTOR_NOT_PREPARED: O setor a ser gravado não foi preparado para gravação
// 				errIAP_SECTOR_NOT_BLANK: Esta região já pertence ao bootloader
//				errIAP_SIZE_INVALID: O tamanho do buffer de gravação é inválido. Deve ser 256, 512, 1024 ou 4096 bytes
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço da FLASH
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_NOT_PREPARED: O setor a ser gravado não foi preparado para gravação
// 				errIAP_SRC_ADDR:
// 				errIAP_DST_ADDR:
// 				errIAP_SRC_ADDR_NOT_MAPPED:
// 				errIAP_DST_ADDR_NOT_MAPPED:
// 				errIAP_COMPARE: Os dados gravados no FLASH não foi conferiru com o buffer de dados
//				errIAP_COUNT: Caso o contador de bytes não for multiplo de 4
// -------------------------------------------------------------------------------------------------------------------
int flashProg_Record(u8 *buff) {
	int result;
	int cmdType;
	u8 len;
    u16 addrOffSet;
    u8 bufferData[16];

    cmdType = IntelHexInterpret (
	    buff, &len, &addrOffSet, bufferData
	);

	// CANCELA O PROCESSO SE HOUVE ERRO NA INTERPRETAÇÃO
	//		errINTEL_HEX_DELIMITER: Não foi encontrado o delimitador de comandos.
	//		errINTEL_HEX_CHECKSUM: Erro de checsum da linha de comando Intel HEX
	//		errINTEL_HEX_NUMBER_INVALID: O caractere presente na linha de comando não é um digito
	if (cmdType < 0) {
		#if (FLASH_USE_DEBUG == pdON)
		plog("F: CMD erro [%d]"CMD_TERMINATOR, cmdType);
		#endif

		return cmdType;
	}

	result = pdPASS;

	switch (cmdType) {
    // ---------------------------------------------------------------------
    // COMANDO DO TIPO 0 – GRAVAÇÃO DE DADOS
	case 0: {
		#if (FLASH_USE_DEBUG == pdON)
			plog("F: CMD SAV [0x%x - 0x%x]"CMD_TERMINATOR, flash.addrBase, flash.addrCopy);

		//		plog("F: CMD SAVE [0x%x][%u][",addrOffSet , len);
		//		u8 x;
		//		for (x = 0;x<len;x++)
		//			plognp("0x%x ", bufferData[x]);
		//		plognp("]"CMD_TERMINATOR);
		#endif

		result = saveToCache(bufferData, addrOffSet, len);
		#if (FLASH_USE_DEBUG == pdON)
		if (result != pdPASS)
			plog("F: CMD SAVE erro [%d]"CMD_TERMINATOR, result);
		#endif

		break;
	}

	// ---------------------------------------------------------------------
    // COMANDO DO TIPO 2 - EXTENSÃO DE SEGMENTAÇÃO ENDEREÇO (HEX86)
	case 2: {
		flash.addrBase = ((bufferData[0] << 8) | (bufferData[1] << 4));
		#if (FLASH_USE_DEBUG == pdON)
		plog("F: CMD ADDR BASE (HEX86) 0x%x [0x%x] [0x%x] [0x%x]"CMD_TERMINATOR, flash.addrBase, flash.addrCopy, bufferData[0], bufferData[1]);
		#endif
		break;
	}

	// ---------------------------------------------------------------------
    // COMANDO DO TIPO 3
	case 3: {
		// CANCELA O PROCESSO SE O ARQUIVO INTEL HEX CONTÉM COMANDOS QUE NÃO PODEMOS PROCESSAR
		#if (FLASH_USE_DEBUG == pdON)
		plog("F: CMD não suportado"CMD_TERMINATOR);
		#endif

		result = errINTEL_HEX_COMMAND_INVALID;
	}

	// ---------------------------------------------------------------------
    // COMANDO DO TIPO 4 - EXTENSÃO DE ENDEREÇO LINEAR (HEX386)
	case 4: {
        flash.addrBase = ((bufferData[0] << 24) | (bufferData[1] << 16));
		#if (FLASH_USE_DEBUG == pdON)
        plog("F: CMD ADDR BASE (HEX386) 0x%x [0x%x] [0x%x] [0x%x]"CMD_TERMINATOR, flash.addrBase, flash.addrCopy, bufferData[0], bufferData[1]);
		#endif
        break;
	}

	// ---------------------------------------------------------------------
	// COMANDO DO TIPO 5 - usando pelo compilador da IAR, vamos ignorar
	case 5: {
		#if (FLASH_USE_DEBUG == pdON)
		plog("F: CMD 5 ignorado"CMD_TERMINATOR);
		#endif
		break;
	}

	// ---------------------------------------------------------------------
	// COMANDO DO TIPO 1 – FIM DE ARQUIVO
	default: {
		// salvar os dados remanescente da cache para o seu devido lugar na FLASH
		if ((result = commitCache()!=pdPASS)) return result;

		// ----------------------------------------------------------------------------------------
		// Calcular a chave de validação da aplicação e salvar os seus vetores de ints em DATASTORE
		// ----------------------------------------------------------------------------------------
		// Calculando a chave de validação da aplicação
		// 	Isto é feito somando todos os vetores de interrupção e depois fazer o complemento de 2

		// somando todos os vetores de ints
		flash.appCodeKey = read_LE32u(vctTblApp);
		flash.appCodeKey += read_LE32u(vctTblApp+0x4);
		flash.appCodeKey += read_LE32u(vctTblApp+0x8);
		flash.appCodeKey += read_LE32u(vctTblApp+0xC);
		flash.appCodeKey += read_LE32u(vctTblApp+0x10);
		flash.appCodeKey += read_LE32u(vctTblApp+0x18);
		#if defined (arm7tdmi)
		flash.appCodeKey += read_LE32u(vctTblApp+0x1C);
		#endif
		#if defined (cortexm3)
		flash.appCodeKey += read_LE32u(vctTblApp+0x14);
		#endif

		// fazendo complemento de 2
		flash.appCodeKey = (~flash.appCodeKey + 1);

		// Copiar a chave de validação da aplicação no vetor de int reservado para isto
		#if defined (arm7tdmi)
		memcpy(vctTblApp+0x14, &flash.appCodeKey, 4);
		#endif
		#if defined (cortexm3)
		memcpy(vctTblApp+0x1C, &flash.appCodeKey, 4);
		#endif

		// Salvar na cache os vetores de ints da aplicação armazenado temporariamente
		memset(cache, 0xFF, CACHE_SIZE);
		memcpy(cache, vctTblApp, VECTORTABLE_SIZE);

		// Salvar na cache os vetores de ints do bootloader armazenado temporariamente
		memcpy(cache+VECTORTABLE_SIZE, vctTblBootld, VECTORTABLE_SIZE); // +VECTORTABLE_SIZE para defender os vetores do programa do usuário

		memset(vctTblBootld, 0xFF, VECTORTABLE_SIZE);
		flash.vctTblBootldEmpty = pdTRUE; // Sinaliza que o buffer para gravar o vetores do bootloader está vazio

		// Preparar para salvar os dados na FLASHe emitir o commit para salvamento
		flash.addrFlashToSaveCache = flash.DataStoreAddr;
		flash.cacheEmpty = pdFALSE; // Sinaliza que a cache contém dados
		result = commitCache(); // Força a gravação da cache na FLASH

		#if (FLASH_USE_DEBUG == pdON)
		plog("F: CMD FIM PROG [0x%x]"CMD_TERMINATOR, flash.DataStoreAddr);
		#endif

		break;
	}
	}

	return result;
}


// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Caso houver dados na cache a mesma será gravada na FLASH mesmo que não esteja cheia.
//					Este procedimento é útil quando fim do arquivo Item HEX é preocessado e a cache não está completa,
//					assim emitimos um commit para salvar os dados remanejentes
// Parametros:  Nenhum
// Retorna:		pdPASS: Caso a gravação do conetúdo da RAM foi feito com sucesso na FLASH
//				errIAP_SIZE_INVALID: O tamanho do buffer de gravação é inválido. Deve ser 256, 512, 1024 ou 4096 bytes
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço da FLASH
//				errIAP_PARAM_INVALID: Caso o endereço final é menor que o endereço inicial
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_NOT_PREPARED: O setor a ser gravado não foi preparado para gravação
// 				errIAP_SRC_ADDR:
// 				errIAP_DST_ADDR:
// 				errIAP_SRC_ADDR_NOT_MAPPED:
// 				errIAP_DST_ADDR_NOT_MAPPED:
// 				errIAP_COMPARE: Os dados gravados no FLASH não foi conferiru com o buffer de dados
//				errIAP_COUNT: Caso o contador de bytes não for multiplo de 4
// -------------------------------------------------------------------------------------------------------------------
static int commitCache(void) {
	int result;
	#if (FLASH_USE_DEBUG == pdON)
	if (flash.cacheEmpty)
		plog("F: CMT - cache vazia"CMD_TERMINATOR);
	else {
		plog("F: CMT addr 0x%x"CMD_TERMINATOR, flash.addrFlashToSaveCache);

		//		plog("FLASH: COMMIT addr 0x%x"CMD_TERMINATOR "    data [", flash.addrFlashToSaveCache);
		//		u16 x;
		//		for(x=0;x<CACHE_SIZE;x++)
		//			plognp("0x%x ", cache[x]);
		//		plognp(CMD_TERMINATOR);
	}
	#endif

	if (flash.cacheEmpty) return pdPASS;

	// Se for endereço 0 para gravação na FLASH, salvar os vetors de interrupções da aplicação
	//	e do bootloader em DATASTORE
	if (flash.addrFlashToSaveCache == 0) {
		// Salva os vetores de ints da aplicação
		memcpy(vctTblApp, cache, VECTORTABLE_SIZE);

		// Salvar os vetores de ints do bootloader do DATASTORE temporariamente, pois este setor será apagado
		u32 *pSource = ((u32 *)flash.DataStoreAddr) + (VECTORTABLE_SIZE/4); // (dwords = 64bytes para lpc2468 ou 256 para lpc1788) para defender os vetores do programa do usuário
		memcpy(vctTblBootld, pSource, VECTORTABLE_SIZE);
		flash.vctTblBootldEmpty = pdFALSE; // Sinaliza que o buffer para gravar o vetores do bootloader contém dados

		// Copiar para cache o endereço do bootloader no vetor de reset
		u8 *p = (u8*)0;
		#if defined (arm7tdmi)
		memcpy(cache, p, 4);
		#endif
		#if defined (cortexm3)
		memcpy(cache, p, 8); // Copiar além do vetor de reset, copiar também o valor do stack
		#endif

		// calcular a nova chave de validação da aplicação pois o endereço do vetor de reset foi alterado
		//		na região de dados na flash
		// Isto é feito somando todos os vetores de interrupção e depois fazer o complemento de 2
		// 	somando todos os vetores de ints
		flash.appCodeKey = read_LE32u(cache);
		flash.appCodeKey += read_LE32u(cache+0x4);
		flash.appCodeKey += read_LE32u(cache+0x8);
		flash.appCodeKey += read_LE32u(cache+0xC);
		flash.appCodeKey += read_LE32u(cache+0x10);
		flash.appCodeKey += read_LE32u(cache+0x18);
		#if defined (arm7tdmi)
		flash.appCodeKey += read_LE32u(vctTblApp+0x1C);
		#endif
		#if defined (cortexm3)
		flash.appCodeKey += read_LE32u(vctTblApp+0x14);
		#endif

		// fazendo complemento de 2
		flash.appCodeKey = (~flash.appCodeKey + 1);

		// copia a chave de validação  da app na posição correta
		#if defined (arm7tdmi)
		memcpy(cache+0x14, &flash.appCodeKey, 4);
		#endif
		#if defined (cortexm3)
		memcpy(cache+0x1C, &flash.appCodeKey, 4);
		#endif

		#if (FLASH_USE_DEBUG == pdON)
		plog("F: Erase FLASH [0x%x] [0x%x%x%x%x]"CMD_TERMINATOR, flash.appCodeKey, vctTblBootld[0], vctTblBootld[1], vctTblBootld[2], vctTblBootld[3]);
		#endif

		// Apaga primeiro o endereço em DATASTORE onde estão os vetores de ints.
		// 	Isto caso houver algum erro durante o processo de gravação, não haverá aplicação válida na FLASH
		//		requerendo outra atualização
		if ((result = iap_ClearSectors (flash.DataStoreSector, flash.DataStoreSector)) != pdPASS) return result;
		// apagar toda a FLASH da região do código do usuário
		if ((result = iap_ClearSectors (0, flash.bootldSectorInit-1)) != pdPASS ) return result;

		// ----------------------------------------------------------------------------
		// O PROCEDIMENTO ABAIXO É PARA QUANDO USAR O DATASTORE ANTES DO PROGRAMA FLASH
		// ----------------------------------------------------------------------------
		// APAGAR A FLASH DO TOPO DA MEMÓRIA PARA O ENDEREÇO 0
		// if ((result = iap_ClearSectors (flash.DataStoreSector, flash.DataStoreSector)) != pdPASS) return result;
		// if ((result = iap_ClearSectors (0, flash.DataStoreSector-1)) != pdPASS ) return result;
	}

	result = iap_WriteSectors (flash.addrFlashToSaveCache, cache, CACHE_SIZE);

	// LIMPA A CACHE PARA A PRÓXIMA GRAVAÇÃO NA FLASH
	memset(cache, 0xFF, CACHE_SIZE);
	flash.cacheEmpty = pdTRUE;

	#if (FLASH_USE_DEBUG == pdON)
	if (result != pdPASS)
		plog("F: CMT erro [%d]"CMD_TERMINATOR, result);
	#endif

	return result;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Grava os dados interpretados do arquivo Intel HEX na chache de gravação da FLASH.
//				E quando a cache estiver cheia a mesma será gravada na FLASH
// Parametros:  buf: Ponteiro do buffer com os dados a ser copiados na cache
//				addrOffSet:	Endereço baixo de 16 bits onde deve ser gravado na FLASH.
//								Este endereço é somado com o endereço alto totalizando um endereço de 32 bits
//				nBytes: 	Quantidade de bytes contidos no buffer, geralmente não maior que 16
// Retorna:		pdPASS:	Caso a copia do dados fora deita com sucesso
//				errIAP_PARAM_INVALID: Caso o endereço final é menor que o endereço inicial
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço da FLASH
//				errIAP_SECTOR_NOT_PREPARED: O setor a ser gravado não foi preparado para gravação
// 				errIAP_SECTOR_NOT_BLANK: Esta região já pertence ao bootloader
//				errIAP_SIZE_INVALID: O tamanho do buffer de gravação é inválido. Deve ser 256, 512, 1024 ou 4096 bytes
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço da FLASH
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_NOT_PREPARED: O setor a ser gravado não foi preparado para gravação
// 				errIAP_SRC_ADDR:
// 				errIAP_DST_ADDR:
// 				errIAP_SRC_ADDR_NOT_MAPPED:
// 				errIAP_DST_ADDR_NOT_MAPPED:
// 				errIAP_COMPARE: Os dados gravados no FLASH não foi conferiru com o buffer de dados
//				errIAP_COUNT: Caso o contador de bytes não for multiplo de 4
// -------------------------------------------------------------------------------------------------------------------
static int saveToCache(u8 *buf, u16 addrOffSet, u8 nBytes) {
	int result;
	u32 aux;
	u16 sectorQtdToComplete;
	u16 sectorQtdToSave;
	u16 bufIdx;

	// CAPTURAR O ENDEREÇO DA MEMÓRIA PARA GRAVAR O BUFFER DE DADOS
	flash.addrCopy = flash.addrBase | (addrOffSet & 0xFFFF);

	// GRAVA TODOS OS BYTES NA FLASH
    //  O BYTES ESTÃO NA SEQUENCIA DE ENDEREÇO
	bufIdx = 0;
    while (nBytes) {
    	// captura o endereço do inicio do setor para gravação da cache na FLASH quando esta estiver cheia
    	aux = ( (u32)flash.addrCopy / CACHE_SIZE) * CACHE_SIZE;
        // checa se o endereço para gravar o código da aplicação pertence ao código do bootloader
        //if (aux >= flash.DataStoreAddr) return errIAP_SECTOR_NOT_BLANK;// se usar datastore antes do bootloader na flash
    	if (aux >= init_of_text) return errIAP_SECTOR_NOT_BLANK;// se usar datastore antes do bootloader na flash

		//		#if (FLASH_USE_DEBUG == pdON)
		//		// plog("F: save cache [%u] [0x%x] [%u][%u]"CMD_TERMINATOR, nBytes, flash.addrCopy, aux, flash.addrFlashToSaveCache);
		//		 plog("F: save cache [0x%x] [%u]"CMD_TERMINATOR, flash.addrCopy, flash.addrFlashToSaveCache);
		//		#endif

        // se trocou de setor e há dados na cache, grava cache na FLASH
        if (aux != flash.addrFlashToSaveCache)
        	if ( (result = commitCache())!= pdPASS) return result;

        flash.addrFlashToSaveCache = aux;
    	// determinar a quantidade de bytes que faltam para completar o setor atual de gravação da FLASH
    	sectorQtdToComplete = CACHE_SIZE - (flash.addrCopy % CACHE_SIZE);

        // checa se há espaco restante no setor
        if (nBytes >= sectorQtdToComplete)
                sectorQtdToSave = sectorQtdToComplete;
        else   	sectorQtdToSave = nBytes;

        // sinaliza que possição da cache deve ser artualizado com os dados
        // a ser gravados. O tamanho da cache deve ser o mesmo tamanho do
        // buffer de gravação na FLASH
        flash.cacheIdx = CACHE_SIZE - sectorQtdToComplete;

        memcpy(cache + flash.cacheIdx, buf+bufIdx, sectorQtdToSave);
        flash.cacheEmpty = pdFALSE;

        //		#if (FLASH_USE_DEBUG == pdON)
        //        plog("   [%u] [%u] [%u] [%u]"CMD_TERMINATOR,
        //        		bufIdx,
        //        		flash.cacheIdx,
        //        		sectorQtdToSave,
        //        		sectorQtdToComplete
        //        );
        //		#endif

        // grava a cache na flash caso a cache estiver cheia
        if (nBytes >= sectorQtdToComplete)
        	if ( (result = commitCache())!= pdPASS) return result;

        // grava a quantidade de bytes na flash que preenche o setor
        nBytes-= sectorQtdToSave;
        flash.addrCopy += sectorQtdToSave;
		bufIdx += sectorQtdToSave;
    }

	return pdPASS;
}
