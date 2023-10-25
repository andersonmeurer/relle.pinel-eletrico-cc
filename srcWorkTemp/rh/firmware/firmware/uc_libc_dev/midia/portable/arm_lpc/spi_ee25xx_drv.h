#ifndef __SPI_EE25XX_DRV_H
#define __SPI_EE25XX_DRV_H

#include "_config_cpu_spi.h"

#if (EE25XX_USE_SPI == 1)
	#if defined(cortexm3)
	#include "ssp1.h"
	#define ee25xx_TransferByte(c)	ssp1_TransferByte(c)
	#else
	#include "spi1.h"
	#define ee25xx_TransferByte(c)	spi1_TransferByte(c)
	#endif

#elif (EE25XX_USE_SPI == 0)
	#if defined(cortexm3)
	#include "ssp0.h"
	#define ee25xx_TransferByte(c)	ssp0_TransferByte(c)
	#else
	#include "spi0.h"
	#define ee25xx_TransferByte(c)	spi0_TransferByte(c)
	#endif
#else
	#error NÃO FOI DEFINIDO A PORTA SPI PARA A EEPROM, AJUESTE EM _config_cpu_spi.h
#endif

// FUNCTIONS
#define ee25xx_SetupPorts()		EE25XX_CS_DIR |= EE25XX_CS
#define ee25xx_Enable()  		EE25XX_CS_CLR =  EE25XX_CS 			// CS = 0 ativar EEPROM
#define ee25xx_Disable()  		EE25XX_CS_SET =  EE25XX_CS 			// CS = 1 desativar EEPROM

#endif
