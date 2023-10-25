#include "_config_lib_.h"
#include "spi_m25pxx.h"
#include "delay.h"

#if (SPI_FLASH_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif


typedef struct spiFlashDev_s {
	SPI_FLASH_DEV device;
	u8 sig3, sig2, sig1, sig0;
} spiFlashDev_t;

static const spiFlashDev_t spiFlashDev [] = {
	// dev		----- sig[3:0] ------
	{AT25DF041, 0, 	1, 	0x44, 	0x1f},
	{ST25P40, 	0,	0,	0,		0x12},
	{ST25P32, 	0,	0,	0,		0x15},
};


#define M25PXX_PAGESIZE			256		// 256 bytes/page

// DEVICE COMMANDS
#define M25PXX_CMD_WREN			0x06	// write enable
#define M25PXX_CMD_WRDI			0x04	// write disable
#define M25PXX_CMD_READ			0x03	// read
#define M25PXX_CMD_RDID			0x9F	// read ID register
#define M25PXX_CMD_FASTREAD		0x0B	// high-speed read
#define M25PXX_CMD_PP			0x02	// page program
#define	M25PXX_BE_4K			0x20	// Erase 4KiB block
#define	M25PXX_BE_32K			0x52	// Erase 32KiB block
#define M25PXX_CMD_SE			0xD8	// sector erase
#define M25PXX_CMD_CHIPERASE	0xC7	// chip erase	
#define M25PXX_CMD_WRSR			0x01	// write status register
#define M25PXX_CMD_RES			0xAB 	//Release Deep and Read Electronic Signature 
#define M25PXX_CMD_RDSR			0x05	// read status register
	#define M25PXX_STATUS_WIP		0x01	// write in progress
	#define M25PXX_STATUS_WEN		0x02	// write enable
	#define M25PXX_STATUS_BP0		0x04	// block protect 0
	#define M25PXX_STATUS_BP1		0x08	// block protect 1
	#define M25PXX_STATUS_BP2		0x10	// block protect 2
	#define M25PXX_STATUS_WPEN		0x80	// write protect enabled

//comandos adicionais para AT25DF041A (Atmel)
#define M25PXX_CMD_SIGNATURE		0x9F	//read eletronic signature

static u8 m25pxx_ReadStatus(void);
static int m25pxx_WriteStatus (u8 status);
static int m25pxx_Wait(u32 ms);
static int m25pxx_WriteBlock(u32 addr, u8 *data, u32 nbytes);


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa a porta do uC para acessar a memória SPI FLAH e procura pela mesma no barramento
//					de acordo com a lista de memporias registradas.
//					Uma vez encontrada a memória a mesma é configurada e inicializada.
// parametros	Nenhum
// Retorna: 	SPI_FLASH_NONE caso não encontrar nenhuma SPI FLASH, senão retorna com o tipo da memória
//------------------------------------------------------------------------------------------------------------------
SPI_FLASH_DEV m25pxx_Init(void) {
	m25pxx_SetupPorts();									// Ajusta portas
	m25pxx_Disable();										// Desabilita memória
	
	#if (SPI_FLASH_USE_DEBUG == pdON)
	plog("FE: INIT FLASH "CMD_TERMINATOR);
	#endif

	u8 xx;
	for(xx=0; xx<arrsizeof(spiFlashDev); xx++)
		if (m25pxx_SeftTest(xx) == pdPASS) {
			//apenas libera toda a memória para permitir a gravação
			m25pxx_WriteStatus(0);
			#if (SPI_FLASH_USE_DEBUG == pdON)
			plog("FE: FLASH SELF TEST PASS sig=0x%x%x%x%x"CMD_TERMINATOR, spiFlashDev[xx].sig3, spiFlashDev[xx].sig2, spiFlashDev[xx].sig1, spiFlashDev[xx].sig0);
			#endif

			return spiFlashDev[xx].device;
		}

	return SPI_FLASH_NONE;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Faz um teste na flash lendo a sua assinatura
//				As memórias da atmel os IDs e as assinaturas são os mesmos
//				As memórias da ST os valores do IDs e da assinaturas são diferentes
// parametros	Nenhum
// Retorna: 	pdPASS Teste feito com sucesso
//				errSPI_FLASH_SEFTEST Houve erro de teste
//------------------------------------------------------------------------------------------------------------------
int m25pxx_SeftTest(u8 idx) {
	u8 sig[4];
	m25pxx_Signature(idx, sig, sizeof(sig));

	if (spiFlashDev[idx].device == AT25DF041) {
		if ( (spiFlashDev[idx].sig3 == sig[3]) && (spiFlashDev[idx].sig2 == sig[2]) && (spiFlashDev[idx].sig1 == sig[1]) && (spiFlashDev[idx].sig0 == sig[0]))
			return pdPASS;
	} else if (spiFlashDev[idx].device == ST25P32) {
		if ( spiFlashDev[idx].sig0 == sig[0])
			return pdPASS;
	} else if (spiFlashDev[idx].device == ST25P40) {
		if ( spiFlashDev[idx].sig0 == sig[0])
			return pdPASS;
	}

	#if (SPI_FLASH_USE_DEBUG == pdON)
	plog("FE: FLASH SELF TEST FAIL sig=0x%x%x%x%x"CMD_TERMINATOR, sig[3], sig[2], sig[1], sig[0]);
	#endif

	return errSPI_FLASH_SEFTEST;
}	

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Faz uma leitura na assinatura da flash
// parametros	Parametro 1: Ponteiro do buffer para o retorno da assinatura
//				Parametro 2: Tamaho do buffer do retorno da assinatura
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void m25pxx_Signature(u8 idx, u8 *sig, u8 size_sig) {
	
	if (spiFlashDev[idx].device == AT25DF041) {
		// MEMÓRIA DA ATMEL
		m25pxx_Enable();
		m25pxx_TransferByte(M25PXX_CMD_SIGNATURE);
		while (size_sig--) *sig++ = m25pxx_TransferByte(0);
		m25pxx_Disable();
	} else {
		// MEMÓRIA da ST
		m25pxx_Enable();
		m25pxx_TransferByte(M25PXX_CMD_RES);

		// Envio bytes dummy
		m25pxx_TransferByte(0);
		m25pxx_TransferByte(0);
		m25pxx_TransferByte(0);

		while (size_sig--) *sig++ = m25pxx_TransferByte(0);
		m25pxx_Disable();
	}
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê po ID da flash  OBS: Suporta somente em alguns modelos de flash
//				As memórias da atmel o ID e Assinatura são os mesmos
//				As memórias da ST os valores do ID e da assinatura são diferentes
// parametros	Nenhum
// Retorna: 	Retorna o ID da flash
//------------------------------------------------------------------------------------------------------------------
u32 m25pxx_GetID(void) {
	u8 id1, id2, id3, id4;

	m25pxx_Enable();
	m25pxx_TransferByte(M25PXX_CMD_RDID);
	id1 = m25pxx_TransferByte(0);
	id2 = m25pxx_TransferByte(0);
	id3 = m25pxx_TransferByte(0);
	id4 = m25pxx_TransferByte(0);
	
	#if (SPI_FLASH_USE_DEBUG == pdON)
	plog("id1: 0x%x"CMD_TERMINATOR, id1);
	plog("id2: 0x%x"CMD_TERMINATOR, id2);
	plog("id3: 0x%x"CMD_TERMINATOR, id3);
	plog("id4: 0x%x"CMD_TERMINATOR, id4);
	#endif
	
	m25pxx_Disable();

	return ((u32)id4)<<24 | ((u32)id3)<<16 | ((u32)id2)<<8 | id1;
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Apaga a memória inteira
// parametros	Nenhum
// Retorna: 	pdPASS se a apagamento foi um sucesso
//				errSPI_FLASH_ERASE Se houve erro no apagamento da flash
//------------------------------------------------------------------------------------------------------------------
int m25pxx_Erase(void) {
	#if (SPI_FLASH_USE_DEBUG == pdON)
	plog("FE: INIT ERASE"CMD_TERMINATOR);
	#endif

	// enable write
	m25pxx_Enable();
	m25pxx_TransferByte(M25PXX_CMD_WREN);
	m25pxx_Disable();

	// do chip erase
	m25pxx_Enable();
	m25pxx_TransferByte(M25PXX_CMD_CHIPERASE);
	m25pxx_Disable();
	
	#if (SPI_FLASH_USE_DEBUG == pdON)
	plog("FE: WAIT TO BE READY"CMD_TERMINATOR);
	#endif
	
	if (m25pxx_Wait(80000) == errSPI_FLASH_WRITE)
			return errSPI_FLASH_ERASE;
	else	return pdPASS;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Apaga a um determinado setor memória inteira
// parametros	Setor a ser apagado
// Retorna: 	pdPASS se a apagamento foi um sucesso
//				errSPI_FLASH_ERASE Se houve erro no apagamento da flash
//------------------------------------------------------------------------------------------------------------------
int m25pxx_SectorErase(u32 sector) {
	#if (SPI_FLASH_USE_DEBUG == pdON)
	plog("FE: INIT ERASE SECTOR"CMD_TERMINATOR);
	#endif

	m25pxx_Enable();
	m25pxx_TransferByte(M25PXX_CMD_WREN);
	m25pxx_Disable();

	m25pxx_Enable();	
	m25pxx_TransferByte(M25PXX_CMD_SE);
	m25pxx_TransferByte(sector>>16);
	m25pxx_TransferByte(sector>>8);
	m25pxx_TransferByte(sector);
	
	m25pxx_Disable();
	
	#if (SPI_FLASH_USE_DEBUG == pdON)
	plog("FE: WAIT DO READY"CMD_TERMINATOR);
	#endif

	if (m25pxx_Wait(3000) == errSPI_FLASH_WRITE)
			return errSPI_FLASH_ERASE;
	else	return pdPASS;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê um byte da flash
// parametros	Endereço para leitura
// Retorna: 	Byte lido
//------------------------------------------------------------------------------------------------------------------
u8 m25pxx_ReadByte(u32 addr) {
	u8 dt;
	
	m25pxx_ReadBuffer(addr, &dt, 1);
	
	return dt;
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê um buffer da flash
// parametros	parametro1: Endereço inicial para leitura
//				parametro2: O ponteiro do buffer para retorno da leitura
//				parametro3: O tamanho do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void m25pxx_ReadBuffer(u32 addr, u8 *data, u32 nbytes) {
	// begin read
	m25pxx_Enable();
	// issue read command
	m25pxx_TransferByte(M25PXX_CMD_READ);
	
	// send address
	m25pxx_TransferByte(addr>>16);
	m25pxx_TransferByte(addr>>8);
	m25pxx_TransferByte(addr);
		
	// read data
	while(nbytes--) {
		 *data++ = m25pxx_TransferByte(0);
	}
	
	// end read
	m25pxx_Disable();
}



//				============================================
//  				===================================
//    					===========================
//    						===================
//    							===========
//    								===

// ATENCÃO: O setor da FLASH onde o endereço alvo vai ser gravado deve 
//	ser apagado antes da chamadas das funções abaixo.
// Deixamos o cargo da aplicação a fazer o apagamento dos setores.
// Senão, a cada byte que desejamos gravar na flash, seria invocado para 
//	apagar o setor, e isto deixaria o sistema muito lento.

//    								===
//    							===========
//    						===================
//    					===========================
//  				===================================
//				============================================


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escreve um byte na flash
// ATENCÃO: 	O setor da FLASH deve ser apagado antes de ser gravado
// parametros	parametro1: Endereço inicial para escrita
//				parametro2: O byte a ser escrito
// Retorna: 	pdPASS se a gravação foi um sucesso
//				errSPI_FLASH_WRITE Se a flash ficou travada na gravação
//------------------------------------------------------------------------------------------------------------------
int m25pxx_WriteByte(u32 addr, u8 data) {
	return m25pxx_WriteBuffer(addr, &data, 1);	
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escreve um buffer na flash
// ATENCÃO: 	O setor da FLASH deve ser apagado antes de ser gravado
// parametros	parametro1: Endereço inicial para escrita
//				parametro2: O ponteiro do buffer dos dados a serem escritos
//				parametro3: O tamanho do buffer
// Retorna: 	pdPASS se a gravação foi um sucesso
//				errSPI_FLASH_WRITE Se a flash ficou travada na gravação
//------------------------------------------------------------------------------------------------------------------
// O procedimento de gravação na flash é diferente da leitura.
// Na leitura podemos ler em burst se preocupando somente no tamanho da memória, 
//		pois apontamos o endereço inicial e acessamos os demais endereços automaticamente.
// Porém o processo de gravação na flash é feito por paginação. Ou seja, quando vamos gravar um dado na flash,
// 		independente da quantidade desde que seja no limite da página da memória, o CI lê a página para um buffer 
//		e nós atualizamos esse buffer com a gravação. No fim da gravação, quando o CS vai em alta, o CI grava os dados do buffer para a página.

// Para que a função de escrita tem o mesmo feito da leitura, teremos que ter o seguinte cuidado:
// Em uma gravação em busrt, modo buffer, é preciso ter um cuidado com o incrementador de endereço interno da memória,
// 		pois seu incremento automático a cada gravação é circular apontando sempre na mesma página ativa, 
//			isto pelo fato da gravação ser por paginação.
// Então é preciso verificar se o endereço desejado de gravação ainda está dentro da página corrente de gravação.
// Caso não esteja é preciso finalizar a gravação, mesmo que não fora enviados todos os dados do buffer,
//		reiniciar uma nova gravação, com endereço atualizado, para continuar a gravar os demais dados do buffer.
// 		Esse procedimento ativa a próxima página para gravação. 
// Repetir esse processo até todos os dados do buffer sejam enviados para gravação.

int m25pxx_WriteBuffer(u32 addr, u8 *data, u32 nbytes) {
	u32 qtdsave;
		
	#if (SPI_FLASH_USE_DEBUG == pdON)
	u16 pagecount = 0;
	plog("FE: INIT WR BUF addr 0x%x size %d"CMD_TERMINATOR, addr, nbytes);
	#endif
	
	while (nbytes) {
		// DETERMINA A QUANTIDADE DE BYTES A SER GRAVADOS NA PÁGINA ATUAL DE ESCRITA
		qtdsave = M25PXX_PAGESIZE - addr % M25PXX_PAGESIZE; // Calcula a quantidade de bytes necessários para  preencher a página
		if (qtdsave > nbytes) qtdsave = nbytes;				// Se a quantidade de bytes for maior a quantidade de dados restantes para gravação, especifica a quantidade restante

		#if (SPI_FLASH_USE_DEBUG == pdON)
		plog("FE: write buf pagecount %d addr 0x%x size %u"CMD_TERMINATOR, pagecount, addr, nbytes);
		pagecount++;
		plog("FE: qtdsave %u"CMD_TERMINATOR, qtdsave);
		#endif
		
		if (m25pxx_WriteBlock(addr, data, qtdsave) == errSPI_FLASH_WRITE) return errSPI_FLASH_WRITE;
				
		// CALCULA O ENDEREÇO DA PRÓXIMA PAGINA DE GRAVAÇÃO E QUANTIDADE DE DADOS RESTANTES PARA GRAVAÇÃO
		data += qtdsave;
		nbytes-=qtdsave;
		addr+=qtdsave;		
	}
	
	return pdPASS;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escreve um buffer na flash
// ATENCÃO: 	O setor da FLASH deve ser apagado antes de ser gravado
// parametros	parametro1: Endereço inicial para escrita
//				parametro2: O ponteiro do buffer dos dados a serem escritos
//				parametro3: O tamanho do buffer
// Retorna: 	pdPASS se a gravação foi um sucesso
//				errSPI_FLASH_WRITE Se a flash ficou travada na gravação
//------------------------------------------------------------------------------------------------------------------
static int m25pxx_WriteBlock(u32 addr, u8 *data, u32 nbytes) {
	// HABILITA ESCRITA NA EEPROM. ISTO SERÁ DESABILITADO AUTOMATICAMENTE QUANDO HOUVER UMA LEITURA
	m25pxx_Enable();
	m25pxx_TransferByte(M25PXX_CMD_WREN);
	m25pxx_Disable();

	// EMITE COMANDO DE ESCRITA E ENDEREÇO INICIAL
	m25pxx_Enable();
	m25pxx_TransferByte(M25PXX_CMD_PP);
	m25pxx_TransferByte(addr>>16);
	m25pxx_TransferByte(addr>>8);
	m25pxx_TransferByte(addr);
	
	// ENVIA DADOS PARA GRAVAÇÃO
	while (nbytes--) m25pxx_TransferByte(*data++);
	m25pxx_Disable();
	
	return m25pxx_Wait(5);
}


#if (M25PXX_USE_DUMP == pdON)
#include "utils.h"

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Exibe valores da flash no formato HEX ; ASCII em linhas com 16 bytes de dados
// parametros	parametro1: Endereço inicial de leitura
//				parametro2: Quantidade de dados a ser lido na flash
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void m25pxx_dump(u32 addr, u32 qtd) {
	u16 qtd_rd;
	u8 buffer[256];
	u8 *ptr;

	while (qtd) {
  		if (qtd > sizeof(buffer)) 
  				qtd_rd = sizeof(buffer);
  		else	qtd_rd = qtd;
  			
  		m25pxx_ReadBuffer(addr, buffer, qtd_rd);
  		qtd-=qtd_rd;
  		ptr=buffer;
  		  		
  		if (qtd_rd >= 16)
  			// Enquanto o qtd_rd for maior que 15 faz, depois incrementea addr do buffer e da flash por 16, e decrementa qtd_rd por 16
  			for( ; qtd_rd>=16; ptr+=16, addr+=16, qtd_rd-=16 ) 
  				put_dump16(ptr, addr, 16, 16);
  			
		if (qtd_rd)
			put_dump16(ptr, addr, qtd_rd, 16);
  	}
}
#endif


static u8 m25pxx_ReadStatus(void) {
	u8 sts;
	m25pxx_Enable();
	m25pxx_TransferByte(M25PXX_CMD_RDSR);
	sts = m25pxx_TransferByte(0);
	m25pxx_Disable();
	return sts;
}

//status register possui alguns bits não voláteis, e por isso
//a escrita desse registrador pode levar até 15ms
static int m25pxx_WriteStatus (u8 status) {
	m25pxx_Enable();
	m25pxx_TransferByte(M25PXX_CMD_WREN);
	m25pxx_Disable();

	m25pxx_Enable();
	m25pxx_TransferByte(M25PXX_CMD_WRSR);
	m25pxx_TransferByte(status);
	m25pxx_Disable();

	//teste feito com M25P40 em 16.12.11 levou ~5ms
	return m25pxx_Wait(15);
}

//aguarda o sinal de busy
static int m25pxx_Wait(u32 ms) {
    long i = (long)ms*100L;
    while (i--) {
        if ((m25pxx_ReadStatus()&M25PXX_STATUS_WIP)==0) return pdPASS;
        delay_us(10);
    }

    //erro
    #if (SPI_FLASH_USE_DEBUG == pdON)
	plog("FE: FLASH BUSY LONG TIME"CMD_TERMINATOR);
	#endif

    return errSPI_FLASH_WRITE;
}
