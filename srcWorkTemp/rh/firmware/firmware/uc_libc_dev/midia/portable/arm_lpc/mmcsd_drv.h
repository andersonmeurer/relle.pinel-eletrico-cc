#ifndef __MMCSD_DRV_H
#define __MMCSD_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_spi.h"
   
#if (MMCSD_USE_SPI == 0)
	#include "ssp0.h"
	#define mmcsd_fastClock() 	ssp0_SetClockFreq(25000000UL)
	#define mmcsd_slowClock() 	ssp0_SetClockFreq(400000UL)
	#define mmcsd_TransferByte(c)	ssp0_TransferByte(c)
	#define mmcsd_ReceiveBlock	ssp0_ReceiveBlock
	#define mmcsd_SendBlock 	ssp0_SendBlock
	#define mmcsd_WaitReady		ssp0_WaitReady
#elif (MMCSD_USE_SPI == 1)
	#include "ssp1.h"
	#define mmcsd_fastClock() 	ssp1_SetClockFreq(25000000UL)
	#define mmcsd_slowClock() 	ssp1_SetClockFreq(400000UL)
	#define mmcsd_TransferByte(c)	ssp1_TransferByte(c)
	#define mmcsd_ReceiveBlock	ssp1_ReceiveBlock
	#define mmcsd_SendBlock 	ssp1_SendBlock
	#define mmcsd_WaitReady		ssp1_WaitReady
#else
	#error NÃO FOI DEFINIDO A PORTA SPI PARA O MMCSD, AJUESTE EM _config_cpu_spi.h
#endif


#if (MMC_USE_HARD_CONTROL == pdON)
#define mmcsd_SetupPorts() 	{ MMCSD_CS_DIR |= MMCSD_CS; MMCSD_WR_PROTECT_DIR &= ~MMCSD_WR_PROTECT; MMCSD_PRESENT_DIR &= ~MMCSD_PRESENT_PIN; }
#else
#define mmcsd_SetupPorts() 	MMCSD_CS_DIR |= MMCSD_CS;
#endif

#define mmcsd_Select()  	MMCSD_CS_CLR =  MMCSD_CS 		// CS = 0 ativar MMC
#define mmcsd_Deselect()  	MMCSD_CS_SET =  MMCSD_CS 		// CS = 1 desativar MMC

#endif
