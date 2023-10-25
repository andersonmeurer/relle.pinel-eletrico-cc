// data de ataulização: 2014/07/09

#include "ssp1.h"
#if defined(cortexm3)
#include "arm.h"
#endif

#if (SSP1_USE_DEBUG	== pdON)
#include"stdio_uc.h"
#endif

// TODO: COLOCAR TIMEROUTS NOS WHILES INFINITOS
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o barramento SSP
// parametros	mode: Modo da CPU
//					SSP1_MASTER
//					SSP1_SLAVE
//				config:
//					SSP1_CPHA_[L/H] | SSP1_CPOL_[L/H] | SSP1_BITS_[16:4] | FORMAT_FRAME
//						FORMAT_FRAME: SSP1_FRF_SPI, SSP1_FRF_SSI ou SSP1_FRF_MW
//							SSP1_FRF_SPI: Formato frame = SPI
//							SSP1_FRF_SSI: Formato frame = TI
//							SSP1_FRF_MW:  Formato frame = Microwire
//						SSP1_BITS_[16:4]: Determina os bits de transferencia onde x sinaliza de 8 a 16 bits.
//						SSP1_CPHA_L: Dados capturados na primeira borda do clock
//						SSP1_CPHA_H: Dados capturados na segunda borda do clock
//						SSP1_CPOL_L: Clock em baixa entre os frames
//						SSP1_CPOL_H: Clock em alta entre os frames
//					SSP1_STANDARD Tem valor 0x7. Ajusta o modo padrão para a SSP, ou seja, transmissão com 8 bits de transferencia sem interrupção
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void ssp1_Init (u8 mode, u16 config) {
  	u32 i;
  	volatile u32 dummy __attribute__ ((unused)); // Usar atributo para não emitir aviso pelo compilador
  	
  	PCONP |= PCSSP1;
	#if defined (arm7tdmi)
  	SSP1_PINSEL = (SSP1_PINSEL & ~SSP1_PINMASK) | SSP1_PINS; 			// Seleciona o pino externo para a função SPI
	#endif

  	#if defined (cortexm3)
  	#if defined (LPC1788)
  	pinsel_cfg_t pinCFG;
  	pinCFG.addrPort = &SSP1_PINSEL; 	// Endereço da porta SSP1
	pinCFG.numPin = SSP1_CLK_NUMPIN;	// Número do pino do ARM para o CLK
	pinCFG.nunFunc = SSP1_NUNFUNC;		// Ajustado a função do pino para SSP
	pinCFG.openDrain = 0;				// Dreno aberto desligado
	pinCFG.pinMode = 0;					// Sem resistor de pull
	pinsel_configPin(&pinCFG);			// Aplica configurações
	pinCFG.numPin = SSP1_MISO_NUMPIN;	// Número do pino do ARM para o MISO
	pinsel_configPin(&pinCFG);			// Aplica configurações
	pinCFG.numPin = SSP1_MOSI_NUMPIN;	// Número do pino do ARM para o MOSI
	pinsel_configPin(&pinCFG);			// Aplica configurações
	#else
		ssp1_ConfigPort();
	#endif
  	#endif

	SSP1CR0  = config; // | SSP1_FRF_SPI;
  	spp1_SetMode( mode );
  	SSP1IMSC = 0x0;
  	
  	//  Clock Freq. Identification Mode < 400kHz
  	ssp1_SetClockFreq (400000);

  	//  Enable SPI
  	SSP1CR1 |= SSP1_SSE_ON; // Habilita SSP

	#if (SSP1_USE_DEBUG	== pdON)
  	plog("SSP1: INIT"CMD_TERMINATOR);
  	plog("SSP1: SSP0CR0 %u"CMD_TERMINATOR, SSP1CR0);
  	plog("SSP1: SSP0CR1 %u"CMD_TERMINATOR, SSP1CR1);
	#endif

  	for (i = 0; i < 8; i++)
    	dummy = SSP1DR;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o número de bits na transferencia de dados
// parametros	bits:  SSP1_DSS_x ( x de 4 a 16)
//				Determina os bits de transferencia onde x sinaliza de 8 a 16 bits. Somenente o lpc2468, lpc2138 e lpc2148 tem suporte
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void spi1_SetBitsPerTransfer(u16 bits) {
	SSP1CR0 &=~ SSP1_DSS_MASK;
	SSP1CR0 |= bits;

	#if (SSP1_USE_DEBUG	== pdON)
	plog("SSP1: SET BITS TRANS"CMD_TERMINATOR);
	plog("SSP1: SSP0CR0 %u"CMD_TERMINATOR, SSP1CR0);
	#endif
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta a frequencia do barramento
// parametros	frequency: velocidade do barramento e HZ
//					cortexm3 max 10Mhz, para os demais 400KHz
// Retorna: 	A frequencia ajustada no barramento
//------------------------------------------------------------------------------------------------------------------
u32 ssp1_SetClockFreq(u32 frequency) {
	u32 div = PCLK / frequency;

  	if (div < 8)    	div = 8;
  	//if (div < 1)    	div = 2;
  	else if (div > 254) div = 254;

  	++div; 
  	div &= ~1;
  	SSP1CPSR = div;
  	
  	#if (SSP1_USE_DEBUG	== pdON)
  	plog("SSP1: PLCK      %u"CMD_TERMINATOR, PCLK);
  	plog("SSP1: frequency %u"CMD_TERMINATOR, frequency);
  	plog("SSP1: div       %u"CMD_TERMINATOR, div);
  	plog("SSP1: SSP1CPSR  %u"CMD_TERMINATOR, SSP1CPSR);
  	#endif

  	return (PCLK / div);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o modo de trabalho do barramento como mestre ou escravo
// parametros	mode: Modo da CPU
//					SSP1_MASTER
//					SSP1_SLAVE
// Retorna: 	Retorna pdPASS se a seleção foi feito com sucesso senão retorna código do erro
//------------------------------------------------------------------------------------------------------------------
int spp1_SetMode ( u8 mode ) {
    n8 status;

    status = pdPASS;

    switch( mode ) {
    	case SSP1_MASTER:
        	SSP1CR1 |= SSP1_MASTER; 				// Mestre
        	break;

    	case SSP1_SLAVE:
    		SSP1CR1 &= ~SSP1_MASTER; 			// escravo
        	break;

    	default:
        	status = errSPP1_ERROR_SET_MODE;
        	break;
    }

    return status;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Espera pelo cartão ficar pronto
// Parametros:	Nenhum
// Retorna:		Retorna sempre 0xFF
//------------------------------------------------------------------------------------------------------------------
u8 ssp1_WaitReady(void) {
  	u8 res;

  	do {      
    	SSP1DR = 0xff;
    	while (!(SSP1SR & SSP1_RNE));
    	res = SSP1DR;
  	} while (res != 0xff);

  	return res;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Troca de bytes entre os dispositivos
// Parametros:	Byte a ser enviado
// Retorna:		Byte Recebido
//------------------------------------------------------------------------------------------------------------------
u8 ssp1_TransferByte (u8 c) {
	while (!(SSP1SR & SSP1_TNF)); // Espere se a FIFO estiver cheia
  	SSP1DR = c;
  	while (SSP1SR & SSP1_BSY);

  	return SSP1DR;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Envia um buffer de dados para o dispositivo ignorando o que recebe
// Parametros: 	*pData: Ponteiro do buffer a ser transmitido
//				size: tamanho do buffer
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void ssp1_SendBlock(const u8 *pData, u32 size) {
  	volatile u32 dummy __attribute__ ((unused)); // Usar atributo para não emitir aviso pelo compilador;

  	while (size--)  {
  		while (!(SSP1SR & SSP1_TNF)); // Espere se a FIFO estiver cheia
    	SSP1DR = *pData++;
    	while (!(SSP1SR & SSP1_RNE));
    	dummy = SSP1DR;
  	}
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Recebe um buffer de dados do dispositivo, onde este ignora os dados enviados
// Parametros: 	*pData: Ponteiro do buffer a ser preenchido com os dados recebidos
//				size: tamanho do buffer
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void ssp1_ReceiveBlock(u8 *pData, u32 size) {
  	u32 delta = 0;

  	while (size || delta) {
    	while ((SSP1SR & SSP1_TNF) && (delta < SSP1_FIFO_DEPTH) && size) {
      		SSP1DR = 0xff;
      		--size; 
      		++delta;
    	}

    	while (SSP1SR & SSP1_RNE) {
      		*pData++ = SSP1DR;
      		--delta;
    	}
  	}
}
