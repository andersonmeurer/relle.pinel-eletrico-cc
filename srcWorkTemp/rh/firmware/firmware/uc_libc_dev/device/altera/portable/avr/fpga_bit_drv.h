#ifndef __FPGA_BIT_DRV_H
#define __FPGA_BIT_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_device.h"

#define fpga_SetupPorts() 	do { 	sbi(FPGA_DCLK_DIR, FPGA_DCLK);					\
									sbi(FPGA_DATA0_DIR, FPGA_DATA0);				\
									sbi(FPGA_CONFIG_DIR, FPGA_CONFIG);				\
									cbi(FPGA_STATUS_DIR, FPGA_STATUS);				\
									cbi(FPGA_CONFIG_DONE_DIR, FPGA_CONFIG_DONE);	\
							}	while(0)

#define fpga_PinConfigSet() 	sbi(FPGA_CONFIG_PORT, FPGA_CONFIG) 
#define fpga_PinConfigClr() 	cbi(FPGA_CONFIG_PORT, FPGA_CONFIG)
#define fpga_PinDclk_Set() 		sbi(FPGA_DCLK_PORT, FPGA_DCLK)
#define fpga_PinDclk_Clr() 		cbi(FPGA_DCLK_PORT, FPGA_DCLK)
#define fpga_PinData0Set()		sbi(FPGA_DATA0_PORT, FPGA_DATA0) 
#define fpga_PinData0Clr()		cbi(FPGA_DATA0_PORT, FPGA_DATA0)
#define fpga_Ready()			(FPGA_STATUS_PIN & _BV(FPGA_STATUS)) 

#endif
