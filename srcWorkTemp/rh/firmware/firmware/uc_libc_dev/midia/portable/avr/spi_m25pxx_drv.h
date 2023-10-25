#ifndef __SPI_M25PXX_DRV_H
#define __SPI_M25PXX_DRV_H

#include "_config_cpu_spi.h"
#include "spi.h"
#define m25pxx_TransferByte	spi_TransferByte

// FUNCTIONS
#define m25pxx_SetupPorts()		sbi(M25PXX_CS_DDR, M25PXX_CS)
#define m25pxx_Enable()  		cbi(M25PXX_CS_PORT, M25PXX_CS) 			// CS = 0 ativar FLASH
#define m25pxx_Disable()  		sbi(M25PXX_CS_PORT, M25PXX_CS) 			// CS = 1 desativar FLASH

#endif
