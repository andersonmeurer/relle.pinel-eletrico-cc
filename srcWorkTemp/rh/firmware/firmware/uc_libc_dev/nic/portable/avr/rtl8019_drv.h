#ifndef __RTL8019_DRV_H
#define __RTL8019_DRV_H

#include <util\delay.h>
#include "_config_cpu_.h"
#include "_config_cpu_nic.h"

// #################################################################################################################
// #################################################################################################################
// COMANDO DE ESCRITA, LEITURA E RESET NA NIC
// #################################################################################################################
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escrita nos registradores na NIC
//				If using the External SRAM Interface, performs a write to
//            		address RTL8019_MEMORY_MAPPED_OFFSET + (RTL_ADDRESS<<8)
//            		The address is sent in the non-multiplxed upper address port so
//            		no latch is required.
//          	If using general I/O ports, the data port is left in the input
//            		state with pullups enabled
// Parametros: 	1. u8 RTL_ADDRESS - register offset of RTL register
//				2. u8 RTL_DATA - data to write to register
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Leitura nos registradores da NIC
//  			If using the External SRAM Interface, performs a read from
//            		address RTL_IOMAP_OFFSET + (RTL_ADDRESS<<8)
//            		The address is sent in the non-multiplxed upper address port so
//            		no latch is required.
//          	If using general I/O ports, the data port is assumed to already be
//            		an input, and is left as an input port when done
// Parametros: 	u8 RTL_ADDRESS - register offset of RTL register
// Retorna: 	Byte lido na NIC
//------------------------------------------------------------------------------------------------------------------
#if (RTL_CONNECTION == RTL_CONNECTION_IOMAP_HIGHADDR)
	#define rtl8019_Read(RTL_ADDRESS) (*(volatile u8 *) \
                       (RTL_IOMAP_OFFSET_HIGHADDR \
                       + (((u8)(RTL_ADDRESS)) << 8)) )

	#define rtl8019_Write(RTL_ADDRESS,RTL_DATA) do{ *(volatile u8 *) \
                             (RTL_IOMAP_OFFSET_HIGHADDR \
                             + (((u8)(RTL_ADDRESS)) << 8)) = \
                             (u8)(RTL_DATA); } while(0)
#elif (RTL_CONNECTION == RTL_CONNECTION_IOMAP)
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
	#error NÃO FOI DEFINIDO O TIPO DE CONEXÃO DA RTL. VEJA _CONFIG_NIC.H
#endif                       

//------------------------------------------------------------------------------------------------------------------
// Descrição:  	Emite um reset via Hardware para NIC
// Parametros: 	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
#define HARD_RESET_RTL() do{ RTL_RESET_PORT |= _BV(RTL_RESET_PIN);\
                                _delay_ms(10); \
                                RTL_RESET_PORT &= ~_BV(RTL_RESET_PIN);}\
                                while(0)

#define nic_Delay			_delay_ms
#define NIC_DELAY_INIT 		50
#define NIC_DELAY_SKYEYE 	255
#define NIC_DELAY_SKYEYE_2	2

void rtl8019_SetupPorts(void);

#endif
