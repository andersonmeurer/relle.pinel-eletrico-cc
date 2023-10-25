/*
	Driver usado nas mem�rias eeprom modelos:
		25lc640
*/

#include "spi_ee25xx.h"

#if (SPI_EEPROM_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Configura o pino CHIP SELECT da eeprom
// parametros	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void ee25xx_Init(void) {	
	ee25xx_SetupPorts();									// Ajustar portas
	ee25xx_Disable();										// Desabilita cart�o
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
// Descri��o: 	L� um byte da eeprom
// parametros	Endere�o para leitura
// Retorna: 	Byte lido
//------------------------------------------------------------------------------------------------------------------
u8 ee25xx_ReadByte(u32 addr) {
	u8 dt;
	
	ee25xx_ReadBuffer(addr, &dt, 1);
	
	return dt;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	L� um buffer da eeprom
// parametros	parametro1: Endere�o inicial para leitura
//				parametro2: O ponteiro do buffer para retorno da leitura
//				parametro3: O tamanho do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void ee25xx_ReadBuffer(u32 addr, u8 *data, u32 nbytes) {
	// ENVIA O COMANDO DE LEITURA
	ee25xx_Enable();
	ee25xx_TransferByte(EE25XX_CMD_READ);
	
	// ENVIA ENDERE�O INICIAL
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
// Descri��o: 	Escreve um byte na eeprom
// parametros	parametro1: Endere�o inicial para escrita
//				parametro2: O byte a ser escrito
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void ee25xx_WriteByte(u32 addr, u8 data) {
	// HABILITA A ESCRITA NO EEPROM. QUANDO ENVIAR O COMANDO DE LEITURA A ESCRITA � DESABILITADA AUTOM�TICAMENTE
	ee25xx_Enable();
	ee25xx_TransferByte(EE25XX_CMD_WREN);
	ee25xx_Disable();

	// begin write
	ee25xx_Enable();		
	ee25xx_TransferByte(EE25XX_CMD_WRITE);

	// ENVIA O ENDERE�O
	ee25xx_TransferByte(addr>>8);
	ee25xx_TransferByte(addr);

	// ENVIA O DADO
	ee25xx_TransferByte(data);
	ee25xx_Disable();
	
	// ESPERA QUE A ESCRITA SEJA FINALIZADA
	while(ee25xx_readStatus() & EE25XX_STATUS_WIP);
}


// N�O FAZ CONTROLE DO TAMANHO DA P�GINA
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
// Descri��o: 	Escreve um buffer na eeprom
// parametros	parametro1: Endere�o inicial para escrita
//				parametro2: O ponteiro do buffer dos dados a serem escritos
//				parametro3: O tamanho do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
// O procedimento de grava��o na eeprom � diferente da leitura.
// Na leitura podemos ler em burst preocupando somente no tamanho da mem�ria,
//		pois, apontamos o endere�o inicial e acessamos os demais endere�os automaticamente.
// Por�m, no processo de grava��o na eeprom � feito por pagina��o, ou seja, quando vamos gravar um dado na eeprom,
// 		independente de sua quantidade desde que seja no limite da p�gina da mem�ria, o CI l� a p�gina para um buffer
//		e n�s atualizamos esse buffer com a grava��o. No fim da grava��o, quando o pino CS vai em alta, o CI grava os
//		dados do buffer para a p�gina.

// Para que a fun��o de escrita tem o mesmo feito da leitura, teremos que ter o seguinte cuidado:
// Em uma grava��o em busrt, modo buffer, � preciso ter um cuidado com o incrementador de endere�o interno da mem�ria,
// 		pois, seu incremento � autom�tico a cada grava��o, tamb�m o endere�amento � circular apontando sempre na mesma p�gina ativa
//			pelo fato da grava��o ser por pagina��o.
// Ent�o, � preciso verificar se o endere�o desejado de grava��o ainda est� dentro da p�gina corrente de grava��o.
// Caso n�o esteja � preciso finalizar a grava��o, mesmo que n�o fora enviados todos os dados do buffer,
//		reiniciar uma nova grava��o, com endere�o atualizado, para continuar a gravar os demais dados do buffer.
// 		Esse procedimento ativa a pr�xima p�gina para grava��o. 
// Repetir esse processo at� todos os dados do buffer sejam enviados para grava��o.

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

		// HABILITA ESCRITA NA EEPROM. ISTO SER� DESABILITADO AUTOMATICAMENTE QUANDO HOUVER UMA LEITURA
		ee25xx_Enable();
		ee25xx_TransferByte(EE25XX_CMD_WREN);
		ee25xx_Disable();

		// EMITE COMANDO DE ESCRITA
		ee25xx_Enable();
		ee25xx_TransferByte(EE25XX_CMD_WRITE);

		// ENVIA ENDERE�O INICIAL
		ee25xx_TransferByte(addr>>8);
		ee25xx_TransferByte(addr);

		// DETERMINA A QUANTIDADE DE BYTES A SER GRAVADOS NA P�GINA ATUAL DE ESCRITA
		qtdsave = EE25XX_PAGESIZE - addr % EE25XX_PAGESIZE; // Calcula a quantidade de bytes necess�rios para  preencher a p�gina
		if (qtdsave > nbytes) qtdsave = nbytes;				// Se a quantidade de bytes for maior a quantidade de dados restantes para grava��o, especifica a quantidade restante

		#if (SPI_EEPROM_USE_DEBUG == pdON)
		plog("EE: qtdsave %u"CMD_TERMINATOR, qtdsave);
		#endif

		// ENVIA OS DADOS PARA GRAVA��O
		for(i=0; i<qtdsave; i++)
			ee25xx_TransferByte(*data++);
		
		// CALCULA O ENDERE�O INICIAL DA PR�XIMA PAGINA DE GRAVA��O E QUANTIDADE DE DADOS RESTANTES PARA GRAVA��O
		nbytes-=qtdsave;
		addr+=qtdsave;
		
		// FINALIZA E ESPERA AT� QUE O PROCESSO DE ESCRITA TERMINE
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
// Descri��o: 	Exibe valores da eeprom no formato HEX ; ASCII em linhas com 16 bytes de dados
// parametros	parametro1: Endere�o inicial de leitura
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
