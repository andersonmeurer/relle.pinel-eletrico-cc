#ifndef __ENC28J60_DRV_H
#define __ENC28J60_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_nic.h"
#include "_config_cpu_spi.h"

#if (ENC28J60_USE_SPI == 0)
	#if defined (cortexm3)
	#include "ssp0.h"
	#define enc28j60_TransferByte		ssp0_TransferByte
	#else
	#include "spi0.h"
	#define enc28j60_TransferByte		spi0_TransferByte
	#endif
#elif (ENC28J60_USE_SPI == 1)
	#if defined (cortexm3)
	#include "ssp1.h"
	#define enc28j60_TransferByte		ssp1_TransferByte
	#else
	#include "spi1.h"
	#define enc28j60_TransferByte		spi1_TransferByte
	#endif

#else
	#error NÃO FOI ESPECIFICADO NO _config_cpu_spi.h A PORTA SPI A SER USADA PELA ENC28J60
#endif

#if (ENC28J60_USE_INTERRUPT == pdON)
#if (ENC28J60_USE_EINT == 0)
	#include "eint0.h"
	#define	eint_Init			eint0_Init
	#define eint_SetIntHandler eint0_SetIntHandler
#elif (ENC28J60_USE_EINT == 1)
	#include "eint1.h"
	#define	eint_Init		eint1_Init
	#define eint_SetIntHandler eint1_SetIntHandler
#elif (ENC28J60_USE_EINT == 2)
	#include "eint2.h"
	#define	eint_Init		eint2_Init
	#define eint_SetIntHandler eint2_SetIntHandler
#elif (ENC28J60_USE_EINT == 3)
	#include "eint3.h"
	#define	eint_Init		eint3_Init
	#define eint_SetIntHandler eint3_SetIntHandler
#else
	#error NÃO FOI ESPECIFICADO NO _config_cpu_spi.h O PINO DE INTERRUPÇÃO A SER USADA PELA ENC28J60
#endif
#endif

#if defined(FREE_RTOS)
	#include "FreeRTOS.h"
	#include "task.h"
	#include "semphr.h"
	#include "_config_lib_freertos.h"
#endif

#define enc28j60_SetupPorts() 	ENC28J60_CS_DIR |= ENC28J60_CS
#define enc28j60_Select()  		ENC28J60_CS_CLR =  ENC28J60_CS 		// CS = 0 ativar NIC
#define enc28j60_Deselect()  	ENC28J60_CS_SET =  ENC28J60_CS 		// CS = 1 desativar NIC


#endif
