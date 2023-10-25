#ifndef __SPI_M25PXX_DRV_H
#define __SPI_M25PXX_DRV_H

#include "_config_cpu_spi.h"

#if (EEPROM_USE_SPI == 1)
	#include "spi1.h"
	#define m25pxx_TransferByte(c)	spi1_TransferByte(c)

#elif (EEPROM_USE_SPI == 0)
	#include "spi0.h"
	#define m25pxx_TransferByte(c)	spi0_TransferByte(c)
#else
	#error NÃO FOI DEFINIDO A PORTA SPI PARA A FLASH, AJUESTE EM _config_cpu_spi.h
#endif

// FUNCTIONS
#define m25pxx_SetupPorts()		M25PXX_CS_DIR |= M25PXX_CS
#define m25pxx_Enable()  		M25PXX_CS_CLR =  M25PXX_CS 			// CS = 0 ativar EEPROM
#define m25pxx_Disable()  		M25PXX_CS_SET =  M25PXX_CS 			// CS = 1 desativar EEPROM

#endif
