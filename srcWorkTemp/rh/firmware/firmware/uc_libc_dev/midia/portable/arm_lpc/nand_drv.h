#ifndef __NANDFLASH_DRV_H
#define __NANDFLASH_DRV_H

#include "_config_cpu_.h"

#if (NAND_CONNECTION ==	NAND_CONNECTION_PIO)

// Ajustar os pinos de saida, os demais pinos são de entrada por padrão
#define nand_SetupPorts() 			\
	{								\
		NAND_BUS_DIR |= NAND_CE;	\
		NAND_BUS_DIR |= NAND_RE;	\
		NAND_BUS_DIR |= NAND_CLE;	\
		NAND_BUS_DIR |= NAND_ALE;	\
		NAND_BUS_DIR |= NAND_WE;	\
		NAND_BUS_DIR |= NAND_WP;	\
		NAND_BUS_SET = NAND_CE;		\
		NAND_BUS_SET = NAND_WE;		\
		NAND_BUS_SET = NAND_WP;		\
		NAND_BUS_SET = NAND_RE;		\
		NAND_BUS_CLR = NAND_CLE;	\
		NAND_BUS_CLR = NAND_ALE;	\
	}

#define nand_SetCE()     		do { NAND_BUS_SET = NAND_CE; 	nand_Delay(); } while(0)
#define nand_ClrCE()       		do { NAND_BUS_CLR = NAND_CE; 	nand_Delay(); } while(0)
#define nand_SetCLE()       	do { NAND_BUS_SET = NAND_CLE;  	nand_Delay(); } while(0)
#define nand_ClrCLE()      		do { NAND_BUS_CLR = NAND_CLE;  	nand_Delay(); } while(0)
#define nand_SetRE()       		do { NAND_BUS_SET = NAND_RE; 	nand_Delay(); } while(0)
#define nand_ClrRE()       		do { NAND_BUS_CLR = NAND_RE; 	nand_Delay(); } while(0)
#define nand_SetALE()       	do { NAND_BUS_SET = NAND_ALE;   nand_Delay(); } while(0)
#define nand_ClrALE()       	do { NAND_BUS_CLR = NAND_ALE;   nand_Delay(); } while(0)
#define nand_SetWE()       		do { NAND_BUS_SET = NAND_WE;    nand_Delay(); } while(0)
#define nand_ClrWE()      		do { NAND_BUS_CLR = NAND_WE;    nand_Delay(); } while(0)
#define nand_SetWP()       		do { NAND_BUS_SET = NAND_WP;    nand_Delay(); } while(0)
#define nand_ClrWP()      		do { NAND_BUS_CLR = NAND_WP;    nand_Delay(); } while(0)
#define nand_BusDataSetOut() 	NAND_BUS_DIR |= 0xFF
#define nand_BusDataSetIn()    	NAND_BUS_DIR &= ~0xFF
#define nand_BusDataWR(data)	NAND_BUSDATA_PIN = data
#define nand_BusDataRD()		NAND_BUSDATA_PIN
#define nand_Busy()      		(NAND_BUS_PIN & NAND_RB) == 0


#else
#error NÃO HÁ SUPORTE PARA ESSE TIPO DE CONEXÃO PARA A MEMÓRIA NAND FLASH EXTERNA
#endif

#endif
