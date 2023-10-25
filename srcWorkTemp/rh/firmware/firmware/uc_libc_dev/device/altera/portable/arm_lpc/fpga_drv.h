#ifndef __FPGA_DRV_H
#define __FPGA_DRV_H

#include "_config_lib_fpga.h"

#if (FPGA_USE_SPI == 0)
	#if defined(cortexm3)
	#include "ssp0.h"
	#define spi_TransferByte(c) ssp0_TransferByte(c)
	#else
	#include "spi0.h"
	#define spi_TransferByte(c) spi0_TransferByte(c)
	#endif
#elif (FPGA_USE_SPI == 1)

	#if defined(cortexm3)
	#include "ssp1.h"
	#define spi_TransferByte(c) ssp0_TransferByte(c)
	#else
	#include "spi1.h"
	#define spi_TransferByte(c) spi0_TransferByte(c)
	#endif
#else
	#error NÃO FOI DEFINIDO A PORTA DE COMUNICAÇÃO COM O FPGA
#endif

#endif
