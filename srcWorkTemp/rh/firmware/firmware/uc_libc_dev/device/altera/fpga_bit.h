#ifndef __FPGA_BIT_H
#define __FPGA_BIT_H

#include "fpga_bit_drv.h"

// TAMANHOS EM BYTES DA SRAM INTERNA DE CONFIGURAÇÃO DE CADA DO FPGA
#define EP2C5_DATA_SIZE  	152998
#define EP2C8_DATA_SIZE  	247974
#define EP2C15_DATA_SIZE  	486562
#define EP2C20_DATA_SIZE  	486562
#define EP2C35_DATA_SIZE  	857332
#define EP2C50_DATA_SIZE  	1245424
#define EP2C70_DATA_SIZE	1789902


void fpga_Init ( void);
int fpga_Reset ( void );
int fpga_SendByte ( u8 data );

#endif
