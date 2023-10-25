#ifndef UIP_NIC_H
#define UIP_NIC_H

/*****************************************************************************
*  Module Name:       NIC Driver Interface for uIP-AVR Port
*  
*  Created By:        Louis Beaudoin (www.embedded-creations.com)
*
*  Original Release:  November 16, 2003 
*
*  Module Description:  
*  Provides three functions to interface with a NIC driver
*  These functions can be called directly from the main uIP control loop
*  to send packets from uip_buf and uip_appbuf, and store incoming packets to
*  uip_buf
*
*
*****************************************************************************/

#include "uip.h"
#include "uip_arp.h"
#include "_config_lib_uip.h"


#if (UIP_USE_NIC == UIP_USE_NIC_AX88796)
	#include "ax88796.h"
	#define NICInit 				ax88796_Init
	#define NICBeginPacketSend 		ax88796_BeginPacketSend
	#define NICSendPacketData 		ax88796_SendPacketData
	#define NICEndPacketSend 		ax88796_EndPacketSend
	#define NICBeginPacketRetreive 	ax88796_BeginPacketRetreive
	#define NICRetreivePacketData 	ax88796_RetreivePacketData
	#define NICEndPacketRetreive 	ax88796_EndPacketRetreive
#elif (UIP_USE_NIC == UIP_USE_NIC_ENC28J60)
	#include "enc28j60.h"
	#define NICInit 				enc28j60_Init
	#if defined(FREE_RTOS)
		#define nic_WaitForData 	enc28j60_WaitForData
	#endif
#elif (UIP_USE_NIC == UIP_USE_NIC_RTL8019)
	#include "rtl8019.h"
	#define NICInit 				rtl8019_Init
	#define NICCableConnected		rtl8019_CableConnected
	#define NICBeginPacketSend 		rtl8019_BeginPacketSend
	#define NICSendPacketData 		rtl8019_SendPacketData
	#define NICEndPacketSend 		rtl8019_EndPacketSend
	#define NICBeginPacketRetreive 	rtl8019_BeginPacketRetreive
	#define NICRetreivePacketData 	rtl8019_RetreivePacketData
	#define NICEndPacketRetreive 	rtl8019_EndPacketRetreive
	#define NICCountOverrun			rtl8019_CountOverrun
	#define NICClearCountOverrun	rtl8019_ClearCountOverrun
#else
	#error NÃO FOI ESCOLHIDO O TIPO DE NIC. ESPECIFIQUE NO _config_lib_uip.h
#endif

int nic_Init(struct uip_eth_addr mac);
void nic_Send(void);
u16 nic_Poll(void);

#if (UIP_USE_NIC != UIP_USE_NIC_ENC28J60)
	n8 nic_CableConnected(void);
	u8 nic_CountOverrun(void);
	void nic_ClearCountOverrun(void);
#endif

#endif
