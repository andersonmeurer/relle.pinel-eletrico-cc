#include "spi0.h"
#if (SPI0_USE_DEBUG	== pdON)
#include "stdio_uc.h"
#endif

/* Para cortexm3 utilize spp */


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o barramento SPI
// parametros	paremetro 1: mode: modo da CPU
//					SPI0_SLAVE
//					SPI0_MASTER
//				paremetro 2: config
//					SPI0_CPHA | SPI0_CPOL | SPI0_LSBF | SPI0_SPIE | SPI0_BITS_x ( x de 8 a 16)
//						SPI0_CPHA Este bit em 0 configura a captura o dado na primeira borda do clock, bit 1 captura na segunda borda
//  					SPI0_CPOL 
//							Este bit em 1 o clock permanecer em nível alto até que o byte seja transmitido, 
//								quando a borda do clock cai, o estado no barramento é mudado e quando o a borda do clock sobe o barramento é lido
//								Este bit em 0 faz o inverso acima
//			  			SPI0_LSBF Este bit em 0 o primeiro bit a ser transmitido será o MSB
// 						SPI0_SPIE Este bit em 0 desabilita a interrupção, bit 1 habilita
//						SPI0_BITS_x Determina os bits de transferencia onde x sinaliza de 8 a 16 bits. Somenente o lpc2468, lpc2138 e lpc2148 tem suporte
//					SPI0_STANDARD Tem valor 0. Ajusta o modo padrão para a SPI, ou seja,
//						transmissão com 8 bits de transferencia sem interrupção, SPI0_CPOL = SPI0_CPOL = 0, e MSB transmitido primeiro
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void spi0_Init(u8 mode, u16 config) {

	PCONP |= PCSPI;														// Ligar o SPI no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões

	// CONFIGURANDO A FUNÇÃO DOS PINOS DO CHIP
	SPI0_PINSEL = (SPI0_PINSEL & ~SPI0_PINMASK) | SPI0_PINS; 			// Seleciona o pino externo para a função SPI

	#if defined(LPC2468)
	SPI0_PINSEL1 = (SPI0_PINSEL1 & ~SPI0_PINMASK1) | SPI0_PINS1; 		// Seleciona o pino externo para a função SPI
	#endif

	S0SPCR = config;
	spi0_SetMode( mode );
  	//  Clock Freq. Identification Mode < 400kHz
  	spi0_SetClockFreq (400000);

	S0SPSR;																// Lendo registrador de status para limpa-lo

	#if (SPI0_USE_DEBUG	== pdON)
  	plog("SPI0: init %u"CMD_TERMINATOR, PCLK);
  	plog("SPI0: frequency initial %u"CMD_TERMINATOR, frequency);
  	#endif

}

#if defined(SPI0_BITS_MASK)
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o barramento SPI no modo de transferencia de 8 bits
// parametros	bits:  SPI0_BITS_x ( x de 8 a 16)
//				Determina os bits de transferencia onde x sinaliza de 8 a 16 bits. Somenente o lpc2468, lpc2138 e lpc2148 tem suporte
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void spi0_SetBitsPerTransfer(u16 bits) {
	S0SPCR &=~SPI0_BITS_MASK;
	S0SPCR |= bits;
}
#endif


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta a frequencia do barramento
// parametros	frequency: velocidade do barramento e HZ
// Retorna: 	A frequencia ajustada no barramento
//------------------------------------------------------------------------------------------------------------------
u32 spi0_SetClockFreq(u32 frequency) {
	u32 d = PCLK / frequency;

  	//if (d < 8)    	d = 8;
  	if (d < 6)    	d = 6;
  	//if (d < 1)    	d = 2;
  	else if (d > 254) d = 254;

  	++d; 
  	d &= ~1;
  	S0SPCCR = d;
  	
  	#if (SPI0_USE_DEBUG	== pdON)
  	plog("SPI0: PLCK      %u"CMD_TERMINATOR, PCLK);
  	plog("SPI0: frequency %u"CMD_TERMINATOR, PCLK / d);
  	plog("SPI0: div       %u"CMD_TERMINATOR, d);
  	#endif

  	return (PCLK / d);
}

/*
#define SPI0_SPEED_MAX 0xFF
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta a frequencia do barramento
// parametros	Frequencia em MHZ ou SPI0_SPEED_MAX
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void spi0_SetBitrate(const u8 bitrate) {
	u8 rate = 8;
	
	if (bitrate != SPI0_SPEED_MAX) {		
		rate  = (u8)(PCLK / (bitrate*1000000));
		rate &= 0xFE; 								// O valor deve ser sempre par, então o bit0 deve ser sempre 0
	}
	
	S0SPCCR = (rate < 8)? 8 : rate;
}
*/

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o modo de trabalho do barramento como mestre ou escravo
// parametros	mode: Modo da CPU
//					SPI0_SLAVE
//					SPI0_MASTER
// Retorna: 	Retorna pdPASS se a seleção foi feito com sucesso senão retorna código do erro
//------------------------------------------------------------------------------------------------------------------
int spi0_SetMode ( u8 mode ) {
    n8 status;

    status = pdPASS;

    switch( mode ) {
    	case SPI0_MASTER:
        	S0SPCR |= SPI0_MSTR; 			// Mestre
        	break;

    	case SPI0_SLAVE:
        	S0SPCR &= ~SPI0_MSTR; 			// escravo
        	break;

    	default:
        	status = errSPI_ERROR_SET_MODE;
        	break;
    }
    
    return status;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Troca de bytes entre os dispositivos
// Parametros:	Byte a ser enviado
// Retorna:		Byte Recebido
//------------------------------------------------------------------------------------------------------------------
u8 spi0_TransferByte(u8 data) {
	S0SPDR = data;					// write SPI data
	while(!(S0SPSR & SPI0_SPIF));	// wait until SPI transfer completes
	return S0SPDR;					// return received data
}
