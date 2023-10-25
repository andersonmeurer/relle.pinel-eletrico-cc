#ifndef __RTL8019_DRV_H
#define __RTL8019_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_nic.h"
#include "delay.h"

#if (RTL_CONNECTION == RTL_CONNECTION_IOMAP)
	#define rtl8019_Read(RTL_ADDRESS) (*(volatile u8 *) \
                       (RTL_IOMAP_OFFSET \
                       + (u8)(RTL_ADDRESS)) )

	#define rtl8019_Write(RTL_ADDRESS,RTL_DATA) do{ *(volatile u8 *) \
                             (RTL_IOMAP_OFFSET \
                             + (u8)(RTL_ADDRESS)) = \
                             (u8)(RTL_DATA); } while(0)
#elif (RTL_CONNECTION == RTL_CONNECTION_GPIO)
	u8 rtl8019_Read(u8 address);
	void rtl8019_Write(u8 address, u8 data);
#else
	#error NÃO FOI DEFINIDO O TIPO DE CONEXÃO DA RTL. VEJA _config_nic.h
#endif                       

//------------------------------------------------------------------------------------------------------------------
// Descrição:  	Emite um reset via Hardware para NIC
// Parametros: 	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
#define HARD_RESET_RTL() do { 	RTL_PORT_SET = RTL_PIN_RST;				\
                          			delay_us(10000);					\
                            		RTL_PORT_CLR = RTL_PIN_RST;			\
                           	} while(0)

void rtl8019_SetupPorts(void);

#define nic_Delay			delay_us
#define NIC_DELAY_INIT 		50000
#define NIC_DELAY_SKYEYE 	25500
#define NIC_DELAY_SKYEYE_2	2000

#endif
