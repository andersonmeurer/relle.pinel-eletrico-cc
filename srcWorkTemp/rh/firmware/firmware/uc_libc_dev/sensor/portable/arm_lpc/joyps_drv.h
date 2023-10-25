#ifndef __JOYPS_DRV_H
#define __JOYPS_DRV_H

#include "_config_cpu_spi.h"

#if (JOYPS_USE_SPI == 1)
	#if defined(cortexm3)
	#include "ssp1.h"
	#define joyps_TransferByte(c)	ssp1_TransferByte(c)
	#else
	#include "spi1.h"
	#define joyps_TransferByte(c)	spi1_TransferByte(c)
	#endif

#elif (JOYPS_USE_SPI == 0)
	#if defined(cortexm3)
	#include "ssp0.h"
	#define joyps_TransferByte(c)	ssp0_TransferByte(c)
	#else
	#include "spi0.h"
	#define joyps_TransferByte(c)	spi0_TransferByte(c)
	#endif
#else
	#error NÃO FOI DEFINIDO A PORTA SPI PARA A EEPROM, AJUESTE EM _config_cpu_spi.h
#endif

// FUNCTIONS

#define joyps_SetupPorts()	DEV0_CS_DIR |= DEV0_CS
#define joyps_Enable() 		DEV0_CS_CLR =  DEV0_CS 			// CS = 0 ativar tsc2046
#define joyps_Disable()  	DEV0_CS_SET =  DEV0_CS 			// CS = 1 desativar tsc2046

#endif
