#ifndef __TLC5917_DRV_H
#define __TLC5917_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_device.h"

// Ajustar os pinos para saída ou entrada
//	Pinos CLK, SDI, LE e OE como saída
//	Pino SDO como entrada
#define tlc5917_SetupPorts()				\
	do {									\
		TLC5917_CLK_DIR |= TLC5917_CLK;		\
		TLC5917_SDI_DIR |= TLC5917_SDI; 	\
		TLC5917_LE_DIR  |= TLC5917_LE;		\
		TLC5917_OE_DIR  |= TLC5917_OE;		\
		TLC5917_SDO_DIR &= ~TLC5917_SDO;	\
	}	while(0)

#define tlc5917_clk(v)	(v==pdTRUE)?(TLC5917_CLK_SET = TLC5917_CLK):(TLC5917_CLK_CLR = TLC5917_CLK)
#define tlc5917_sdi(v)	(v==pdTRUE)?(TLC5917_SDI_SET = TLC5917_SDI):(TLC5917_SDI_CLR = TLC5917_SDI)
#define tlc5917_sdo()	(TLC5917_SDO_PIN & TLC5917_SDO)?(pdTRUE):(pdFALSE)
#define tlc5917_le(v)	(v==pdTRUE)?(TLC5917_LE_SET = TLC5917_LE):(TLC5917_LE_CLR = TLC5917_LE)
#define tlc5917_oe(v)	(v==pdTRUE)?(TLC5917_OE_SET = TLC5917_OE):(TLC5917_OE_CLR = TLC5917_OE)

#endif
