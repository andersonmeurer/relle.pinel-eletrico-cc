#ifndef __CONFIG_CPU_DEVICE_H
#define __CONFIG_CPU_DEVICE_H

#include "_config_cpu_.h"

// ###########################################################################################################################################
// CONFIGURAÇÕES DO HX711 sensor de peso
// clock de saída
#define HX711_PDSCK_DIR		GPIO2_DIR
#define HX711_PDSCK_HIGH	GPIO2_SET
#define HX711_PDSCK_LOW		GPIO2_CLR
#define HX711_PDSCK			BIT_9

// dados de entrada
#define HX711_DOUT_PIN		GPIO2_PIN
#define HX711_DOUT			BIT_8

// ###################################################################################################################
// CONFIGURAÇÃO PARA O DRIVER TLC5917
#define TLC5917_CLK_DIR  			GPIO1_DIR
#define TLC5917_CLK_CLR				GPIO1_CLR
#define TLC5917_CLK_SET				GPIO1_SET
#define TLC5917_CLK					(1<<1)

#define TLC5917_SDI_DIR  			GPIO1_DIR
#define TLC5917_SDI_CLR				GPIO1_CLR
#define TLC5917_SDI_SET				GPIO1_SET
#define TLC5917_SDI					(1<<0)

#define TLC5917_SDO_DIR  			GPIO0_DIR
#define TLC5917_SDO_PIN				GPIO0_PIN
#define TLC5917_SDO					(1<<4)

#define TLC5917_LE_DIR  			GPIO1_DIR
#define TLC5917_LE_CLR				GPIO1_CLR
#define TLC5917_LE_SET				GPIO1_SET
#define TLC5917_LE					(1<<4)

#define TLC5917_OE_DIR  			GPIO1_DIR
#define TLC5917_OE_CLR				GPIO1_CLR
#define TLC5917_OE_SET				GPIO1_SET
#define TLC5917_OE					(1<<8)


// ###################################################################################################################
// CONFIGURAÇÃO PARA GRAVADORA DO FPGA ALTERA CYCLONE II
// PINOS DE SAÍDA
#define FPGA_DCLK_DIR  				GPIO0_DIR
#define FPGA_DCLK_CLR				GPIO0_CLR
#define FPGA_DCLK_SET				GPIO0_SET
#define FPGA_DCLK					(1<<7)

#define FPGA_DATA0_DIR  			GPIO0_DIR
#define FPGA_DATA0_CLR				GPIO0_CLR
#define FPGA_DATA0_SET				GPIO0_SET
#define FPGA_DATA0					(1<<8)

#define FPGA_CONFIG_DIR  			GPIO0_DIR
#define FPGA_CONFIG_CLR				GPIO0_CLR
#define FPGA_CONFIG_SET				GPIO0_SET
#define FPGA_CONFIG					(1<<5)

// PINOS DE ENTRADA
#define FPGA_STATUS_DIR  			GPIO0_DIR
#define FPGA_STATUS_PIN				GPIO0_PIN
#define FPGA_STATUS					(1<<6)

#define FPGA_CONFIG_DONE_DIR  		GPIO0_DIR
#define FPGA_CONFIG_DONE_PIN		GPIO0_PIN
#define FPGA_CONFIG_DONE			(1<<9)


#endif

