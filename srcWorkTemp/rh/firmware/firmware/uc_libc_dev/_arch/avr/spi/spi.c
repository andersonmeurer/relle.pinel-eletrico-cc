#include "spi.h"

//#if (SPI_USE_INT == pdON)
	//volatile n8 spiTxComplete;
	//volatile n8 spiRxComplete;
//#endif


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializo a porta SPI
// Parametros:	Modo  SPI_MODE_SLAVE ou SPI_MODE_MASTER
//				config (SPI_CPHA_1 ou SPI_CPHA_2) | (SPI_CLK_LOW_IDLE ou SPI_CLK_HIGH_IDLE) | (SPI_LSB ou SPI_MSB)
//					SPI_CLK_LOW_IDLE Captura dado na borda de subida clock. Clock ocioso fica em nível baixo
//					SPI_CLK_HIGH_IDLE Captura o dado na borda de descida do clock. Clock ocioso fica em nível alto
//					SPI_LSB = LSB primeiro
//					SPI_MSB = MSB primeiro
//					SPI_CPHA_1 = Captura o dado na primeira borda do clock
//					SPI_CPHA_2 = Captura o dado na segunda borda do clock
//				Speed  
//					Todos os AVRs: 
//						SPI_SPEED_F_DIV_4 ou SPI_SPEED_F_DIV_16 ou SPI_SPEED_F_DIV_64 ou SPI_SPEED_F_DIV_128
//					Somente para quem tem suporte como os atmegas: 
//						SPI_SPEED_F_DIV_2 ou SPI_SPEED_F_DIV_8 ou SPI_SPEED_F_DIV_32
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void spi_Init(u8 mode, u8 config, u8 speed) {
	//#if (SPI_USE_INT == pdON)
		//spiTxComplete = pdTRUE;
		//spiRxComplete = pdFALSE;
	//#endif

	if (mode == SPI_MODE_MASTER) {
		sbi(SPCR, MSTR); 							// Ajustar para modo mestre				
		sbi(SPI_PORT_CONFIG, SPI_SCK);				// Ajustar SCK como saida	
		sbi(SPI_PORT_CONFIG, SPI_MOSI);				// Ajustar MOSI como saida
		
		sbi(SPI_PORT_OUT, SPI_SCK);					// Ajustar SCK nível alto 
	} else {
		cbi(SPCR, MSTR); 								// Ajustar para modo slave
		sbi(SPI_PORT_CONFIG, SPI_MISO);				// Ajustar MISO como saida
	}
	
	SPCR = (SPCR & ~(_BV(CPOL) | _BV(CPHA))) | config;	// Configurando
	SPCR = (SPCR & ~(_BV(SPR1) | _BV(SPR0))) | (speed & 0x3 );	// Ajustando a velocidade
	
	// DOBRAR A VELOCIDADE DO BARRAMENTO SPI CASO TENHA SUPORTE
	#if defined (SPI2X)
		if (speed & 0x80) sbi(SPSR, SPI2X);
	#endif
	
	sbi(SPCR, SPE); 									// Habilita barramento SPI
	
	//#if (SPI_USE_INT == pdON)
		//sbi(SPCR, SPIE);								// Habilita a interrupção
	//#endif
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Transmite um byte ao barramento SPI
// Parametros:	Byte para transmissão
// Retorna:		O último byte recebido de outro dispositivo 
//------------------------------------------------------------------------------------------------------------------
u8 spi_TransferByte(u8 data) {
	//#if (SPI_USE_INT == pdON)
		//spiTxComplete = pdFALSE;								// Sinalizo que esta sendo transmetindo o byte
		//SPDR = data;										// Atualizo o registrador para transferir
		//while(!spiTxComplete);								// Espero pela finalização da transferencia do byte
	//#else
		SPDR = data; 										// Emite o byte
		while(!(SPSR & _BV(SPIF))); 						// Espero pela finalização da transferencia do byte
	//#endif
	
	return SPDR;											// Retorno com o dado recebido
}
