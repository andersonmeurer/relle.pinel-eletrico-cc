#ifndef __SPI_H
#define __SPI_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "_config_cpu_.h"
#include "_config_cpu_spi.h"

void spi_Init(u8, u8, u8);
u8 spi_TransferByte(u8);
	
#endif
