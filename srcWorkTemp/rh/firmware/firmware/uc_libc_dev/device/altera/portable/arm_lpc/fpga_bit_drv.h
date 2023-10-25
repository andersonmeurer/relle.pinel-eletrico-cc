#ifndef __FPGA_BIT_DRV_H
#define __FPGA_BIT_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_device.h"
//#include "_config_cpu_spi.h"

/*
#if (FPGA_USE_SPI == 0)
	#include "spi0.h"
	#define fpga_TransferByte		spi0_TransferByte
#elif (FPGA_USE_SPI == 1)
	#include "spi1.h"
	#define fpga_TransferByte		spi1_TransferByte
#else
	#error NÃO FOI ESPECIFICADO NO _config_cpu_spi.h A PORTA SPI A SER USADA PELA GRAVADORA FPGA
#endif
*/

#define fpga_SetupPorts() 	do { 	FPGA_CONFIG_DIR |= FPGA_CONFIG;				\
									FPGA_DCLK_DIR |= FPGA_DCLK;					\
									FPGA_DATA0_DIR |= FPGA_DATA0;				\
									FPGA_CONFIG_DONE_DIR &= ~FPGA_CONFIG_DONE;	\
									FPGA_STATUS_DIR &= ~FPGA_STATUS;			\
							}	while(0)

#define fpga_PinConfigSet() 	FPGA_CONFIG_SET = FPGA_CONFIG
#define fpga_PinConfigClr() 	FPGA_CONFIG_CLR = FPGA_CONFIG
#define fpga_PinDclk_Set() 		FPGA_DCLK_SET = FPGA_DCLK
#define fpga_PinDclk_Clr() 		FPGA_DCLK_CLR = FPGA_DCLK
#define fpga_PinData0Set()		FPGA_DATA0_SET = FPGA_DATA0
#define fpga_PinData0Clr()		FPGA_DATA0_CLR = FPGA_DATA0
#define fpga_Ready()			(FPGA_STATUS_PIN & FPGA_STATUS) 

#endif
