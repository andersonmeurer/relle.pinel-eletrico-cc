#ifndef __CONFIG_CPU_SPI_H
#define __CONFIG_CPU_SPI_H

#include "uc_libdefs.h"
#include "_config_cpu_.h"

// ###################################################################################################################
// CONFIGURAÇÃO PARA A CONTROLADORA JOYSTICK de PlayStation
#define JOYPS_USE_SPI		1 		// Selecione qual a SPI deve ser usada para a flash. 0 = Utiliza SPI0, 1 = Utiliza SP1 e assim sucessivamente

// ###################################################################################################################
// CONFIGURAÇÃO PARA A CONTROLADORA TOUCH SCREEN tsc2046
#define TSC2046_USE_SPI		0 		// Selecione qual a SPI deve ser usada para a flash. 0 = Utiliza SPI0, 1 = Utiliza SP1 e assim sucessivamente

// ###################################################################################################################
// CONFIGURAÇÃO PARA ACESSO A FPGA VIA SPI (LVDS ou não)
#define FPGA_USE_SPI	0 		// Selecione qual a SPI deve ser usada para acessar o FPGA tanto para configuração ou troca de dados
								//	0 = Utiliza SPI0, 1 = Utiliza SP1 e assim sucessivamente

// ###################################################################################################################
// CONFIGURAÇÃO PARA MEMÓRIA FLASH m25pXX
#define M25PXX_USE_SPI	0 		// Selecione qual a SPI deve ser usada para a flash. 0 = Utiliza SPI0, 1 = Utiliza SP1 e assim sucessivamente
#define M25PXX_USE_DUMP	pdON 	// pdON - habilita a função de dump na memória. Isto requer a biblioteca utils.c com a função put_dump liberada (UTILS_USE_DUMP = pdON)

// ###################################################################################################################
// CONFIGURAÇÃO PARA MEMÓRIA EEPROM 25XX
#define EE25XX_USE_SPI	1 		// Selecione qual a SPI deve ser usada para a memória eeprom . 0 = Utiliza SPI0, 1 = Utiliza SP1 e assim sucessivamente
#define EE25XX_USE_DUMP			pdOFF // pdON - habilita a função de dump na memória. Isto requer a biblioteca utils.c com a função put_dump liberada (UTILS_USE_DUMP = pdON)

// ###################################################################################################################
// CONFIGURAÇÃO PARA CARTÃO MMCSD
#define MMCSD_USE_SPI				1 			// Selecione qual a porta SPP deve ser usada para O MMC. 0 = Utiliza SPP0, 1 = Utiliza SPP1 e assim sucessivamente
#define MMCSD_USE_HARD_CONTROL 		pdOFF		// Ajusta se é para usar controle a nível de hardware para proteção contra gravação e presenção do cartão MMC

// ###################################################################################################################
// CONFIGURAÇÃO PARA ENC28J60
#define ENC28J60_USE_SPI			1 			// Define qual barramento SPI será usado para a NIC. 0=SPI0, 1=SPI1 e assim sucessivamente

// ###################################################################################################################
// CONFIGURAÇÃO PARA DECODER MP3
#define STA_USE_SPI					0

#endif
