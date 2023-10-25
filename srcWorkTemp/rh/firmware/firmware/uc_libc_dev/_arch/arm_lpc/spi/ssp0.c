// data de ataulização: 2014/07/09

#include "ssp0.h"
#if defined(cortexm3)
#include "arm.h"
#endif

#define usesSSP

#if (SSP0_USE_DEBUG	== pdON)
#include"stdio_uc.h"
#endif

// PARA O LPC2468 e CORTEX-M3 ESTA LIB SE REFERE AO SSP0, PARA OS LPC2138 E LPC2148 SE REFERE AO SSP1
// ==================================================================================================
// ==================================================================================================

// TODO: colocar timerouts nos whiles infinitos
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o barramento SSP
//				mode: Modo da CPU
//					SSP0_MASTER
//					SSP0_SLAVE
//				config:
//					SSP0_CPHA_[L/H] | SSP0_CPOL_[L/H] | SSP0_BITS_[16:4] | FORMAT_FRAME
//						FORMAT_FRAME: SSP0_FRF_SPI, SSP0_FRF_SSI ou SSP0_FRF_MW
//							SSP0_FRF_SPI: Formato frame = SPI
//							SSP0_FRF_SSI: Formato frame = TI
//							SSP0_FRF_MW:  Formato frame = Microwire
//						SSP0_BITS_[16:4]: Determina os bits de transferencia onde x sinaliza de 8 a 16 bits.
//						SSP0_CPHA_L: Dados capturados a partir do primeiro clock
//						SSP0_CPHA_H: Dados capturados a partir do segundo clock
//						SSP0_CPOL_L: Clock em baixa entre os frames
//						SSP0_CPOL_H: Clock em alta entre os frames
//					SSP0_STANDARD Tem valor 0x7. Ajusta o modo padrão para a SSP, ou seja, transmissão com 8 bits de transferencia sem interrupção
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void ssp0_Init (u8 mode, u16 config) {
  	u32 i;
  	volatile u32 dummy __attribute__ ((unused)); // Usar atributo para não emitir aviso pelo compilador
  	
	PCONP |= PCSSP0;													// Liga o SSP0
	#if defined (arm7tdmi)
	SSP0_PINSEL = (SSP0_PINSEL & ~SSP0_PINMASK) | SSP0_PINS; 			// Seleciona o pino externo para a função SPI
  	SSP0_PINSEL1 = (SSP0_PINSEL1 & ~SSP0_PINMASK1) | SSP0_PINS1; 	// Seleciona o pino externo para a função SPI
	#endif

	#if defined (cortexm3)
	#if defined (LPC1788)
	pinsel_cfg_t pinCFG;
	pinCFG.addrPort = &SSP0_PINSEL; 	// Endereço da porta SSP0
	pinCFG.numPin = SSP0_CLK_NUMPIN;	// Número do pino do ARM para o CLK
	pinCFG.nunFunc = SSP0_NUNFUNC;		// Ajustado a função do pino para SSP
	pinCFG.openDrain = 0;				// Dreno aberto desligado
	pinCFG.pinMode = 0;					// Sem resistor de pull
	pinsel_configPin(&pinCFG);			// Aplica configurações
	pinCFG.numPin = SSP0_MISO_NUMPIN;	// Número do pino do ARM para o MISO
	pinsel_configPin(&pinCFG);			// Aplica configurações
	pinCFG.numPin = SSP0_MOSI_NUMPIN;	// Número do pino do ARM para o MOSI
	pinsel_configPin(&pinCFG);			// Aplica configurações
	#else
	ssp0_ConfigPort();
	#endif
	#endif

  	SSP0CR0 = config; // | SSP0_FRF_SPI;
  	spp0_SetMode( mode );
  	SSP0IMSC = 0x0;

  	//  Clock Freq. Identification Mode < 400kHz
  	ssp0_SetClockFreq (400000);

  	//  Enable SPI
  	SSP0CR1 |= SSP0_SSE_ON; // Habilita SSP

  	#if (SSP0_USE_DEBUG	== pdON)
	plog("SSP0: INIT"CMD_TERMINATOR);
	plog("SSP0: SSP0CR0 0x%x"CMD_TERMINATOR, SSP0CR0);
	plog("SSP0: SSP0CR1 0x%x"CMD_TERMINATOR, SSP0CR1);
	#endif

  	for (i = 0; i < 8; i++) dummy = SSP0DR;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o número de bits na transferencia de dados
// parametros	bits:  SSP0_DSS_x ( x de 4 a 16)
//				Determina os bits de transferencia onde x sinaliza de 8 a 16 bits. Somenente o lpc2468, lpc2138 e lpc2148 tem suporte
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void spi0_SetBitsPerTransfer(u16 bits) {
	SSP0CR0 &=~ SSP0_DSS_MASK;
	SSP0CR0 |= bits;

	#if (SSP0_USE_DEBUG	== pdON)
	plog("SSP0: SET BITS TRANS"CMD_TERMINATOR);
	plog("SSP0: SSP0CR0 %u"CMD_TERMINATOR, SSP0CR0);
	#endif
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta a frequencia do barramento
// parametros	frequency: velocidade do barramento e HZ
//					cortexm3 max 10Mhz, para os demais 400KHz
// Retorna: 	A frequencia ajustada no barramento
//------------------------------------------------------------------------------------------------------------------
u32 ssp0_SetClockFreq(u32 frequency) {
	u32 div = PCLK / frequency;

  	if (div < 8)    	div = 8;
  	//if (div < 1)    	div = 2;
  	else if (div > 254) div = 254;

  	++div; 
  	div &= ~1;
  	SSP0CPSR = div;
  	
  	#if (SSP0_USE_DEBUG	== pdON)
  	plog("SSP0: PLCK      %u"CMD_TERMINATOR, PCLK);
  	plog("SSP0: frequency %u"CMD_TERMINATOR, frequency);
  	plog("SSP0: div       %u"CMD_TERMINATOR, div);
  	plog("SSP0: SSP1CPSR  %u"CMD_TERMINATOR, SSP0CPSR);
  	#endif

  	return (PCLK / div);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o modo de trabalho do barramento como mestre ou escravo
// parametros	mode: Modo da CPU
//					SSP0_MASTER
//					SSP0_SLAVE
// Retorna: 	Retorna pdPASS se a seleção foi feito com sucesso senão retorna código do erro
//------------------------------------------------------------------------------------------------------------------
int spp0_SetMode ( u8 mode ) {
    n8 status;

    status = pdPASS;

    switch( mode ) {
    	case SSP0_MASTER:
        	SSP0CR1 |= SSP0_MASTER; 				// Mestre
        	break;

    	case SSP0_SLAVE:
    		SSP0CR1 &= ~SSP0_MASTER; 			// escravo
        	break;

    	default:
        	status = errSPP0_ERROR_SET_MODE;
        	break;
    }

    return status;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Espera pelo cartão ficar pronto
// Parametros:	Nenhum
// Retorna:		Retorna sempre 0xFF
//------------------------------------------------------------------------------------------------------------------
u8 ssp0_WaitReady(void) {
  	u8 res;

  	do {      
    	SSP0DR = 0xff;
    	while (!(SSP0SR & SSP0_RNE));
    	res = SSP0DR;
  	} while (res != 0xff);

  	return res;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Troca de bytes entre os dispositivos
// Parametros:	Byte a ser enviado
// Retorna:		Byte Recebido
//------------------------------------------------------------------------------------------------------------------
u8 ssp0_TransferByte (u8 c) {
	while (!(SSP0SR & SSP0_TNF)); // Espere se a FIFO estiver cheia
	SSP0DR = c;
	while (SSP0SR & SSP0_BSY);

	return SSP0DR;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Envia um buffer de dados para o dispositivo ignorando o que recebe
// Parametros: 	*pData: Ponteiro do buffer a ser transmitido
//				size: tamanho do buffer
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void ssp0_SendBlock(const u8 *pData, u32 size) {
  	volatile u32 dummy __attribute__ ((unused)); // Usar atributo para não emitir aviso pelo compilador

  	while (size--)  {
    	while (!(SSP0SR & SSP0_TNF)); // Espere se a FIFO estiver cheia
    	SSP0DR = *pData++;
    	while (!(SSP0SR & SSP0_RNE));
    	dummy = SSP0DR;
  	}
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Recebe um buffer de dados do dispositivo, onde este ignora os dados enviados
// Parametros: 	*pData: Ponteiro do buffer a ser preenchido com os dados recebidos
//				size: tamanho do buffer
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void ssp0_ReceiveBlock(u8 *pData, u32 size) {
  	u32 delta = 0;

  	while (size || delta) {
    	while ((SSP0SR & SSP0_TNF) && (delta < SSP0_FIFO_DEPTH) && size) {
      		SSP0DR = 0xff;
      		--size; 
      		++delta;
    	}

    	while (SSP0SR & SSP0_RNE) {
      		*pData++ = SSP0DR;
      		--delta;
    	}
  	}
}
