#include "enc28j60_drv.h"

void enc28j60_SetupPorts(void) {
	//CONFIGURAR PORTAS DE COMUNICA��O COM A NIC
	ENC28J60_SPI_DDR |= 1 << ENC28J60_SPI_CS;					// Inicializa a porta de controle SPI CS
	ENC28J60_CSPASSIVE;											// Desabilitar barramento para NIC
	ENC28J60_SPI_DDR  |= 1<<ENC28J60_SPI_MOSI |1<<ENC28J60_SPI_SCK | 1<<ENC28J60_SPI_SS; 	// Pinos MOSI, SCK e SS como saida
	cbi(ENC28J60_SPI_DDR, ENC28J60_SPI_MISO); 					// Pino MISO como entrada
   	cbi(ENC28J60_SPI_PORT, ENC28J60_SPI_MOSI); 					// MOSI low
    cbi(ENC28J60_SPI_PORT, ENC28J60_SPI_SCK); 					// SCK low
    
    // INICIALIZAR INTERFACE SPI
	sbi(SPCR, MSTR);
	
	cbi(SPCR, CPOL);											// Ajusta o clock de captura de dados no meio do clock alto
	cbi(SPCR,DORD);												// Data order MSB first
	cbi(SPCR, SPR0);											// switch to f/4 2X = f/2 bitrate
	cbi(SPCR, SPR1);
	sbi(SPSR, SPI2X);
	sbi(SPCR, SPE);												// Habilitar o barramento SPI	
}




// #################################################################################################################
// #################################################################################################################
// DRIVERS DE ACESSO
// #################################################################################################################
//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Opera��o de leitura do registrador na NIC
// Parametros: 	Tipo de opera��o e o endere�o desejado
// Retorna: 	O valor do registrador lido
//------------------------------------------------------------------------------------------------------------------
u8 enc28j60_ReadOp(u8 op, u8 address) {	
	u8 data;

	ENC28J60_CSACTIVE;								// Ativar NIC no barramento
    // issue read command
    SPDR = op | (address & ADDR_MASK);				
    waitspi();							// Esperar a finaliza��o da transmiss�o no barramento

    // read data
    SPDR = 0x00;
    waitspi(); // Esperar a finaliza��o da transmiss�o no barramento

    // do dummy read if needed (for mac and mii, see datasheet page 29)
    if(address & 0x80) {
    	SPDR = 0x00;
        waitspi(); 						// Esperar a finaliza��o da transmiss�o no barramento
    }
    
	data = SPDR;
    
    ENC28J60_CSPASSIVE;
    
    return data;
}
//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	L� um buffer do DMA da NIC
// Parametros: 	Tamanho do buffer e o ponteiro do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void enc28j60_ReadBuffer(u16 len, u8* data) {
	ENC28J60_CSACTIVE;
    // issue read command
    SPDR = ENC28J60_READ_BUF_MEM;
    waitspi();// Esperar a finaliza��o da transmiss�o no barramento
    
    while(len--) {
    	SPDR = 0x00;
    	waitspi();// Esperar a finaliza��o da transmiss�o no barramento
    	*data++ = SPDR;
    }
    ENC28J60_CSPASSIVE;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Opera��o de escrita no registrador da NIC
// Parametros: 	Tipo de opera��o e o endere�o desejado
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void enc28j60_WriteOp(u8 op, u8 address, u8 data) {
	ENC28J60_CSACTIVE;							// Ativar NIC no barramento
    SPDR = op | (address & ADDR_MASK);			// Emitir o comando de escrita
    waitspi();									// Esperar a finaliza��o da transmiss�o no barramento
    SPDR = data;								// Escrever o dado para transmiss�o
    waitspi();									// Esperar a finaliza��o da transmiss�o no barramento
    ENC28J60_CSPASSIVE;							// Desativar NIC no barramento
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Escreve um buffer no DMA da NIC
// Parametros: 	Tamanho do buffer e o ponteiro do buffer
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void enc28j60_WriteBuffer(u16 len, u8* data) {
	ENC28J60_CSACTIVE; 
    // issue write command
    SPDR = ENC28J60_WRITE_BUF_MEM;
    waitspi();// Esperar a finaliza��o da transmiss�o no barramento
    while(len--) {
        // write data
        SPDR = *data++;
        waitspi();// Esperar a finaliza��o da transmiss�o no barramento
    }
    ENC28J60_CSPASSIVE; 
}
