/*
	Driver usado nas memórias eeprom modelos:
		25lc640
*/

#include "spi_ee25xx.h"

#if (SPI_EEPROM_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Configura o pino CHIP SELECT da eeprom
// parametros	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void ee25xx_Init(void) {	
	ee25xx_SetupPorts();									// Ajustar portas
	ee25xx_Disable();										// Desabilita cartão
}

u8 ee25xx_readStatus(void) {
	u8 sts;
	ee25xx_Enable();	
	ee25xx_TransferByte(EE25XX_CMD_RDSR);
	sts = ee25xx_TransferByte(0);
	ee25xx_Disable();
	return sts;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê um byte da eeprom
// parametros	Endereço para leitura
// Retorna: 	Byte lido
//------------------------------------------------------------------------------------------------------------------
u8 ee25xx_ReadByte(u32 addr) {
	u8 dt;
	
	ee25xx_ReadBuffer(addr, &dt, 1);
	
	return dt;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê um buffer da eeprom
// parametros	parametro1: Endereço inicial para leitura
//				parametro2: O ponteiro do buffer para retorno da leitura
//				parametro3: O tamanho do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void ee25xx_ReadBuffer(u32 addr, u8 *data, u32 nbytes) {
	// ENVIA O COMANDO DE LEITURA
	ee25xx_Enable();
	ee25xx_TransferByte(EE25XX_CMD_READ);
	
	// ENVIA ENDEREÇO INICIAL
	ee25xx_TransferByte(addr>>8);
	ee25xx_TransferByte(addr);
		
	// CAPTURA OS DADOS
	while(nbytes--) {
		 *data++ = ee25xx_TransferByte(0);
	}
	
	ee25xx_Disable();
}


/*

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escreve um byte na eeprom
// parametros	parametro1: Endereço inicial para escrita
//				parametro2: O byte a ser escrito
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void ee25xx_WriteByte(u32 addr, u8 data) {
	// HABILITA A ESCRITA NO EEPROM. QUANDO ENVIAR O COMANDO DE LEITURA A ESCRITA É DESABILITADA AUTOMÁTICAMENTE
	ee25xx_Enable();
	ee25xx_TransferByte(EE25XX_CMD_WREN);
	ee25xx_Disable();

	// begin write
	ee25xx_Enable();		
	ee25xx_TransferByte(EE25XX_CMD_WRITE);

	// ENVIA O ENDEREÇO
	ee25xx_TransferByte(addr>>8);
	ee25xx_TransferByte(addr);

	// ENVIA O DADO
	ee25xx_TransferByte(data);
	ee25xx_Disable();
	
	// ESPERA QUE A ESCRITA SEJA FINALIZADA
	while(ee25xx_readStatus() & EE25XX_STATUS_WIP);
}


// NÃO FAZ CONTROLE DO TAMANHO DA PÁGINA
void ee25xx_WriteBuffer(u32 addr, u8 *data, u32 nbytes) {
	u8 xx;
	
	for (xx = 0; xx<nbytes;xx++) {
		ee25xx_WriteByte(addr++, *data++);
	}
}
*/



void ee25xx_WriteByte(u32 addr, u8 data) {
	 ee25xx_WriteBuffer(addr, &data, 1);
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escreve um buffer na eeprom
// parametros	parametro1: Endereço inicial para escrita
//				parametro2: O ponteiro do buffer dos dados a serem escritos
//				parametro3: O tamanho do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
// O procedimento de gravação na eeprom é diferente da leitura.
// Na leitura podemos ler em burst preocupando somente no tamanho da memória,
//		pois, apontamos o endereço inicial e acessamos os demais endereços automaticamente.
// Porém, no processo de gravação na eeprom é feito por paginação, ou seja, quando vamos gravar um dado na eeprom,
// 		independente de sua quantidade desde que seja no limite da página da memória, o CI lê a página para um buffer
//		e nós atualizamos esse buffer com a gravação. No fim da gravação, quando o pino CS vai em alta, o CI grava os
//		dados do buffer para a página.

// Para que a função de escrita tem o mesmo feito da leitura, teremos que ter o seguinte cuidado:
// Em uma gravação em busrt, modo buffer, é preciso ter um cuidado com o incrementador de endereço interno da memória,
// 		pois, seu incremento é automático a cada gravação, também o endereçamento é circular apontando sempre na mesma página ativa
//			pelo fato da gravação ser por paginação.
// Então, é preciso verificar se o endereço desejado de gravação ainda está dentro da página corrente de gravação.
// Caso não esteja é preciso finalizar a gravação, mesmo que não fora enviados todos os dados do buffer,
//		reiniciar uma nova gravação, com endereço atualizado, para continuar a gravar os demais dados do buffer.
// 		Esse procedimento ativa a próxima página para gravação. 
// Repetir esse processo até todos os dados do buffer sejam enviados para gravação.

void ee25xx_WriteBuffer(u32 addr, u8 *data, u32 nbytes) {
	u32 i;
	u32 qtdsave;
		
	#if (SPI_EEPROM_USE_DEBUG == pdON)
	u16 pagecount = 0;
	plog("EE: INIT WR BUF addr 0x%x size %d"CMD_TERMINATOR, addr, nbytes);
	#endif
	
	while (nbytes) {
		#if (SPI_EEPROM_USE_DEBUG == pdON)
		plog("EE: write buf pagecount %d addr 0x%x size %u"CMD_TERMINATOR, pagecount, addr, nbytes);
		pagecount++;
		#endif

		// HABILITA ESCRITA NA EEPROM. ISTO SERÁ DESABILITADO AUTOMATICAMENTE QUANDO HOUVER UMA LEITURA
		ee25xx_Enable();
		ee25xx_TransferByte(EE25XX_CMD_WREN);
		ee25xx_Disable();

		// EMITE COMANDO DE ESCRITA
		ee25xx_Enable();
		ee25xx_TransferByte(EE25XX_CMD_WRITE);

		// ENVIA ENDEREÇO INICIAL
		ee25xx_TransferByte(addr>>8);
		ee25xx_TransferByte(addr);

		// DETERMINA A QUANTIDADE DE BYTES A SER GRAVADOS NA PÁGINA ATUAL DE ESCRITA
		qtdsave = EE25XX_PAGESIZE - addr % EE25XX_PAGESIZE; // Calcula a quantidade de bytes necessários para  preencher a página
		if (qtdsave > nbytes) qtdsave = nbytes;				// Se a quantidade de bytes for maior a quantidade de dados restantes para gravação, especifica a quantidade restante

		#if (SPI_EEPROM_USE_DEBUG == pdON)
		plog("EE: qtdsave %u"CMD_TERMINATOR, qtdsave);
		#endif

		// ENVIA OS DADOS PARA GRAVAÇÃO
		for(i=0; i<qtdsave; i++)
			ee25xx_TransferByte(*data++);
		
		// CALCULA O ENDEREÇO INICIAL DA PRÓXIMA PAGINA DE GRAVAÇÃO E QUANTIDADE DE DADOS RESTANTES PARA GRAVAÇÃO
		nbytes-=qtdsave;
		addr+=qtdsave;
		
		// FINALIZA E ESPERA ATÉ QUE O PROCESSO DE ESCRITA TERMINE
		ee25xx_Disable();
		while(ee25xx_readStatus() & EE25XX_STATUS_WIP);
	}
	
	#if (SPI_EEPROM_USE_DEBUG == pdON)
	plog("EE: END"CMD_TERMINATOR);
	#endif
}

#if (EE25XX_USE_DUMP == pdON)
#include "utils.h"

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Exibe valores da eeprom no formato HEX ; ASCII em linhas com 16 bytes de dados
// parametros	parametro1: Endereço inicial de leitura
//				parametro2: Quantidade de dados a ser lido na eeprom
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void ee25xx_dump(u32 addr, u32 qtd) {
	u16 qtd_rd;
	u8 buffer[256];
	u8 *ptr;

	while (qtd) {
  		if (qtd > sizeof(buffer)) 
  				qtd_rd = sizeof(buffer);
  		else	qtd_rd = qtd;
  			
  		ee25xx_ReadBuffer(addr, buffer, qtd_rd);
  		qtd-=qtd_rd;
  		ptr=buffer;
  		  		
  		if (qtd_rd >= 16)
  			// Enquanto o qtd_rd for maior que 15 faz, depois incrementea addr do buffer e da eeprom por 16, e decrementa qtd_rd por 16
  			for( ; qtd_rd>=16; ptr+=16, addr+=16, qtd_rd-=16 ) 
  				put_dump16(ptr, addr, 16, 16);
  			
		if (qtd_rd)
			put_dump16(ptr, addr, qtd_rd, 16);
  	}
}
#endif
