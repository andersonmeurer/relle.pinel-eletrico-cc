#ifndef __RTL8019_H
#define __RTL8019_H

#include "rtl8019_drv.h"
#include "_config_lib_net.h"


int rtl8019_Init(u8 mac0, u8 mac1, u8 mac2, u8 mac3, u8 mac4, u8 mac5);
int rtl8019_CableConnected(void);
void rtl8019_BeginPacketSend(u16 packetLength);
void rtl8019_SendPacketData(u8 * localBuffer, u16 length);
void rtl8019_EndPacketSend(void);
u16 rtl8019_BeginPacketRetreive(void);
void rtl8019_RetreivePacketData(u8 * localBuffer,u16 length);
void rtl8019_EndPacketRetreive(void);
void rtl8019_ProcessInterrupt(void);
u8 rtl8019_CountOverrun(void);
void rtl8019_ClearCountOverrun(void);

// #################################################################################################################
// #################################################################################################################
// REGISTRADORES DE CONTROLE DA RTL
// #################################################################################################################
#define RTL_REG_CR		 	0x00
	#define  RTL_PS1			0x80 
	#define  RTL_PS0			0x40 
	#define  RTL_RD2			0x20 
	#define  RTL_RD1			0x10 
	#define  RTL_RD0			0x08 
	#define  RTL_TXP			0x04 
	#define  RTL_START			0x02 
	#define  RTL_STOP			0x01 
#define RTL_REG_PSTART		0x01
#define RTL_REG_PAR0      	0x01    // Page 1
#define RTL_REG_CR9346    	0x01    // Page 3
#define RTL_REG_PSTOP		0x02
#define RTL_REG_BNRY		0x03
#define RTL_REG_TSR			0x04
#define RTL_REG_TPSR		0x04
#define RTL_REG_TBCR0		0x05
#define RTL_REG_NCR			0x05
#define RTL_REG_TBCR1		0x06
#define RTL_REG_ISR  		0x07
	#define  RTL_RST 			0x80
	#define  RTL_RDC 			0x40
	#define  RTL_OVW 			0x10
	#define  RTL_RXE 			0x08
	#define  RTL_TXE 			0x04
	#define  RTL_PTX 			0x02
	#define  RTL_PRX			0x01
#define RTL_REG_CURR		0x07   // Page 1
#define RTL_REG_RSAR0		0x08
#define RTL_REG_CRDA0		0x08
#define RTL_REG_RSAR1		0x09
#define RTL_REG_CRDA1		0x09
#define RTL_REG_RBCR0		0x0A
#define RTL_REG_RBCR1		0x0B
#define RTL_REG_RSR			0x0C
#define RTL_REG_RCR			0x0C
	#define  RTL_MON			0x20
	#define  RTL_PRO			0x10
	#define  RTL_AM				0x08
	#define  RTL_AB				0x04
	#define  RTL_AR				0x02
	#define  RTL_SEP			0x01
#define RTL_REG_TCR			0x0D
	#define  RTL_FDU			0x80	// full duplex
	#define  RTL_PD				0x40	// pad disable
	#define  RTL_RLO			0x20	// retry of late collisions
	#define  RTL_LB1			0x04	// loopback 1
	#define  RTL_LB0			0x02	// loopback 0
	#define  RTL_CRC			0x01	// generate CRC

#define RTL_REG_CNTR0		0x0D
#define RTL_REG_DCR			0x0E
#define RTL_REG_CNTR1		0x0E
#define RTL_REG_IMR			0x0F
#define RTL_REG_CNTR2		0x0F
#define RTL_REG_RDMAPORT  	0x10
#define RTL_REG_RSTPORT   	0x18
#define RTL_REG_CONFIG0     0x03    // page 3
	#define RTL_BNC				0x04
#define RTL_REG_CONFIG1     0x04    // page 3
#define RTL_REG_CONFIG2     0x05    // page 3
#define RTL_REG_CONFIG3     0x06    // page 3
#define RTL_REG_EECR    	0x01    // page 3
	#define RTL_EEM1			0x80
	#define RTL_EEM0			0x40
	#define RTL_EECS			0x08
	#define RTL_EESK			0x04
	#define RTL_EEDI			0x02
	#define RTL_EEDO			0x01

#endif
