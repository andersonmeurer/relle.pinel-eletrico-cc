#ifndef __CONFIG_LIB_H
#define __CONFIG_LIB_H

#include "uc_libdefs.h"
#include "arch.h"

// ###################################################################################################################
// CONFIGURA��ES ENTRADAS E SAIDAS PADR�ES DE CARACTERES
#define USE_STRING_MESSAGE 		pdON		// pdON = Disponibiliza mensagens em formato de string, sen�o coloque pdOFF
#define CMD_TERMINATOR			"\n"		// Finalizador de nova linha no envio de mensages : "\n" ou "\r\n"
#define PROMPT 					"$> "		// O que ser� exibido no prompt de comando
#define PROMPT_LOG 				"LOG > "

// ###################################################################################################################
// UTILIZAR OU N�O O CONCENTRADOR DE ACESSOS AOS DISPOSITIVOS
#define FPGA_USE_DEVIO				pdON
#define NAND_USE_DEVIO				pdON
#define EEPROM_USE_DEVIO			pdON

// ###################################################################################################################
// HABILITA FUN��ES DE DEPURA��O NAS BIBLIOTECAS GEN�RICAS
// Para habilitar depura��o coloque pdON, sen�o coloque pdOFF

#define VICERR_USE_DEBUG		pdON // Habilita a mensagem de erro de vic quando algum interrup��o inesperada acontecer
#define VIC_USE_DEBUG 			pdOFF
#define STARTUP_USE_DEBUG 		pdOFF
#define	USB_USE_DEBUG			pdOFF
#define	USB_MS_USE_DEBUG		pdOFF
#define IAP_USE_DEBUG 			pdOFF
#define FLASH_USE_DEBUG 		pdOFF
#define MODEM_USE_DEBUG 		pdOFF

#define TASK_UIP_USE_DEBUG  	pdOFF
#define I2C0_USE_DEBUG			pdOFF
#define I2C2_USE_DEBUG			pdOFF
#define SPI0_USE_DEBUG			pdOFF
#define SPI1_USE_DEBUG			pdOFF
#define SSP0_USE_DEBUG			pdOFF
#define SSP1_USE_DEBUG			pdOFF
#define ADC0_USE_DEBUG			pdOFF
#define DAC_USE_DEBUG			pdOFF
#define EE_USE_DEBUG 			pdOFF

#define SPI_EEPROM_USE_DEBUG 	pdOFF
#define SPI_FLASH_USE_DEBUG 	pdOFF
#define SPI_TSC2016_USE_DEBUG 	pdOFF

#define XMODEM_USE_DEBUG 		pdOFF
#define KMODEM_USE_DEBUG 		pdOFF
#define MODBUS_USE_DEBUG 		pdOFF // SLAVE
#define MODBUSM_USE_DEBUG 		pdOFF // MASTER

#define NIC_USE_DEBUG  			pdOFF
#define CDCE_USE_DEBUG 			pdOFF
#define STA_USE_DEBUG			pdOFF

#define DISKIO_USE_DEBUG   		pdOFF
#define ATA_USE_DEBUG 			pdOFF
#define FS_USE_DEBUG 			pdOFF
#define MMCSD_USE_DEBUG   		pdOFF
#define NAND_USE_DEBUG 			pdOFF

#define UIP_USE_DEBUG			pdOFF
#define UIPAPP_USE_DEBUG		pdOFF
#define UIPPOLL_USE_DEBUG		pdOFF

#define DISP7SEG_USE_DEBUG 		pdOFF
#define LCD_USE_DEBUG  			pdOFF
#define CANVAS_USE_DEBUG 		pdOFF
#define CANVASOBJ_USE_DEBUG 	pdOFF
#define CANVAS_USE_TESTBENCH 	pdOFF
#define TOUCH_USE_DEBUG 		pdOFF
#define TOUCH_USE_DEBUG_SCAN	pdOFF 	// ATEN��O COLOQUE O TICK DE LEITURA BEM LENTO ex: systime_SetIntHandler(callsytem, 1000);
#define DIGITALIZER_USE_DEBUG  	pdOFF

#define US04_USE_DEBUG 			pdOFF	// pdON liga a depura��o do US e o gerenciador de ultrasom ficar� fazendo leituras continuas
										// Este procediemento pode at� ser usado como medi��es de modo continuo
										// pdOFF desliga a depura��o do US e gerenciador requer um comando para fazer cada leitura

#define ISP_USE_DEBUG 			pdOFF
#define PCA9685_USE_DEBUG		pdON

//###################################################################################################################
// CONFIGURA��O DE USO DE RAM PARA AS LIBS
#define USE_MALLOC pdOFF	// pdON = As posi��o de RAMs s�o geradas pelo malloc. Util caso deseja usar a posi��o da heap na mem�ria externa


// ###################################################################################################################
// DETERMINA QUE ATITUDE DEVE SER TOMADAS EM LOOPS DE ESPERA NA FUN��ES DAS BIBLIOTECAS
#if defined(RTOS_UCLINUX)
	#define CRITICAL_WAIT	swap()		// For�a a troca de contexto
#else
	#define CRITICAL_WAIT	continue	// Faz nada, espero no loop
#endif

#if defined(FREE_RTOS)
	#define ENTER_CRITICAL() 	taskENTER_CRITICAL()
	#define EXIT_CRITICAL() 	taskEXIT_CRITICAL()
#else
	#define ENTER_CRITICAL()
	#define EXIT_CRITICAL()
#endif

#endif
