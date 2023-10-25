#ifndef __STA015_DRV_H
#define __STA015_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_sta.h"

#if (STA_USE_I2C == 0)
	#include "i2c0.h"
	#define sta_WriteBuffer 		 	i2c0_WriteBuffer
	#define sta_ReadBuffer 			 	i2c0_ReadBuffer	
#else
	#error SELECIONE QUAL O BARRAMENTO I2C NO _config_cpu_spi.h
#endif

#if (STA_USE_GPIO_BITSTREAM == pdOFF) //Utiliza SPI para mandar bistream mp3 para o sta
	#if (STA_USE_SPI == 0)
		#include "spi0.h"
		#define sta_TransferByte			spi0_TransferByte
	#else
		#error NÃO FOI ESPECIFICADO NO _config_cpu_i2c.h A PORTA SPI A SER USADA PELO STA
	#endif

#endif

// AJUSTA PINO DO DATAREQ COMO ENTRADA    
#if (STA_USE_GPIO_BITSTREAM == pdOFF) 	// utiliza spi para mandar bitstream mp3
#define sta_SetupPorts() 	do { STA_DATAREQ_IODIR &= ~STA_DATAREQ; } while(0)
#else					 				// utiliza pinos io para mandar bitstream mp3			
#define sta_SetupPorts() \
	do {	\
	  	STA_SCKR_IODIR 	|= STA_SCKR; STA_SDI_IODIR	|= STA_SDI; STA_DATAREQ_IODIR &= ~STA_DATAREQ; STA_SCKR_IOCLR = STA_SCKR; \
	 } while(0)
#endif


#define sta_dara_req()	(STA_DATAREQ_PIN & STA_DATAREQ)?(pdTRUE):(pdFALSE) // Se o STA_DATAREQ = 1 então STA solicita dados
#define sta_set_data()	(STA_SDI_IOSET = STA_SDI)
#define sta_clr_data()	(STA_SDI_IOCLR = STA_SDI)
#define sta_set_clk()	(STA_SCKR_IOSET = STA_SCKR)
#define sta_clr_clk()	(STA_SCKR_IOCLR = STA_SCKR)


#endif
