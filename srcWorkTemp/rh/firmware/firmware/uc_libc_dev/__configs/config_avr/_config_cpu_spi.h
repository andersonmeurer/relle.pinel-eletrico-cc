#ifndef __CONFIG_CPU_SPI_H
#define __CONFIG_CPU_SPI_H

#include "uc_libdefs.h"
#include "_config_cpu_.h"

// ###################################################################################################################
// CONFIGURA��O PARA ACESSO A FPGA VIA SPI (LVDS ou n�o)
#define FPGA_USE_SPI	0 		// Selecione qual a SPI deve ser usada para acessar o FPGA tanto para configura��o ou troca de dados
								//	0 = Utiliza SPI0, 1 = Utiliza SP1 e assim sucessivamente

// ###################################################################################################################
// CONFIGURA��O PARA MEM�RIA FLASH m25pXX
#define M25PXX_USE_SPI	0 		// Selecione qual a SPI deve ser usada para a flash. 0 = Utiliza SPI0, 1 = Utiliza SP1 e assim sucessivamente
#define M25PXX_USE_DUMP	pdON 	// pdON - habilita a fun��o de dump na mem�ria. Isto requer a biblioteca utils.c com a fun��o put_dump liberada (UTILS_USE_DUMP = pdON)

// ###################################################################################################################
// CONFIGURA��O PARA MEM�RIA EEPROM 25XX
#define EE25XX_USE_SPI	1 		// Selecione qual a SPI deve ser usada para a mem�ria eeprom . 0 = Utiliza SPI0, 1 = Utiliza SP1 e assim sucessivamente
#define EE25XX_USE_DUMP			pdOFF // pdON - habilita a fun��o de dump na mem�ria. Isto requer a biblioteca utils.c com a fun��o put_dump liberada (UTILS_USE_DUMP = pdON)

#define M25PXX_CS_DDR  			DDRB
#define M25PXX_CS_PORT			PORTB
#define M25PXX_CS  				PB0

// ###################################################################################################################
// CONFIGURA��O PARA CART�O MMCSD
#define MMCSD_USE_SPI				1 			// Selecione qual a porta SPP deve ser usada para O MMC. 0 = Utiliza SPP0, 1 = Utiliza SPP1 e assim sucessivamente
#define MMCSD_USE_HARD_CONTROL 		pdOFF		// Ajusta se � para usar controle a n�vel de hardware para prote��o contra grava��o e presen��o do cart�o MMC

// ###################################################################################################################
// CONFIGURA��O PARA ENC28J60
#define ENC28J60_USE_SPI			1 			// Define qual barramento SPI ser� usado para a NIC. 0=SPI0, 1=SPI1 e assim sucessivamente

#define ENC28J60_SPI_PORT  		PORTB
#define ENC28J60_SPI_DDR    	DDRB
#define ENC28J60_SPI_CS     	PB4

// ###################################################################################################################
// CONFIGURA��O PARA DECODER MP3
#define STA_USE_SPI					0

// ###################################################################################################################
// CONFIGURA��O DA PORTA SPI
#define SPI_PORT_CONFIG 		DDRB
#define SPI_PORT_OUT  			PORTB
#define SPI_SCK 				PB1
#define SPI_MISO 				PB3
#define SPI_MOSI 				PB2

#endif
