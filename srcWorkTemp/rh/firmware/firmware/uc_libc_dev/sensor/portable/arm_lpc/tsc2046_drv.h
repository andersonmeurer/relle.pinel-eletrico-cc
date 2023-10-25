#ifndef __TSC2046_DRV_H
#define __TSC2046_DRV_H

#include "_config_cpu_spi.h"

#if (TSC2046_USE_SPI == 1)
	#if defined(cortexm3)
	#include "ssp1.h"
	#define tsc2046_TransferByte(c)	ssp1_TransferByte(c)
	#else
	#include "spi1.h"
	#define tsc2046_TransferByte(c)	spi1_TransferByte(c)
	#endif

#elif (TSC2046_USE_SPI == 0)
	#if defined(cortexm3)
	#include "ssp0.h"
	#define tsc2046_TransferByte(c)	ssp0_TransferByte(c)
	#else
	#include "spi0.h"
	#define tsc2046_TransferByte(c)	spi0_TransferByte(c)
	#endif
#else
	#error NÃO FOI DEFINIDO A PORTA SPI PARA A EEPROM, AJUESTE EM _config_cpu_spi.h
#endif

// FUNCTIONS
#define tsc2046_SetupPorts()	TSC2046_CS_DIR |= TSC2046_CS
#define tsc2046_Enable() 		TSC2046_CS_CLR =  TSC2046_CS 			// CS = 0 ativar tsc2046
#define tsc2046_Disable()  		TSC2046_CS_SET =  TSC2046_CS 			// CS = 1 desativar tsc2046

#endif
