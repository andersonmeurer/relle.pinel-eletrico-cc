/*****************************************************************************
* vim:sw=8:ts=8:si:et
*
* Title        : Microchip ENC28J60 Ethernet Interface Driver
* Author        : Pascal Stang (c)2005
* Modified by Guido Socher
* Copyright: GPL V2
*
*This driver provides initialization and transmit/receive
*functions for the Microchip ENC28J60 10Mb Ethernet Controller and PHY.
*This chip is novel in that it is a full MAC+PHY interface all in a 28-pin
*chip, using an SPI interface to the host processor.
*
*
*****************************************************************************/
//@{

#ifndef __STA015_DRV_H
#define __STA015_DRV_H


#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/twi.h>
#include "_config_cpu_.h"
#include "_config_cpu_sta.h"

#if (I2C_USE_IO == pdON)
	#include "_i2c_io.h"
#else
	#include "_i2c.h"
#endif

#define i2c_MasterSend 				 i2c_SendBuffer
#define i2c_MasterReceive 			 i2c_ReceiverBuffer




// Ajusta pino do reset como saida
// Ajusta pino do SCKR como saida
// Ajusta pino do SDI como saida
// Ajusta pino do DataReq como entrada    
// Clock em baixa
#define sta_SetupPorts() do { sbi(STA_SCKR_DDR, STA_SCKR);	sbi(STA_SDI_DDR, STA_SDI);	cbi(STA_DATAREQ_DDR, STA_DATAREQ);	cbi(STA_SCKR_PORT, STA_SCKR); } while(0)

/*
#include <inttypes.h>
#include <util/delay.h>
#include "_config_cpu_.h"
#include "_config_cpu_nic.h"

// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address	(bits 0-4)
#define ADDR_MASK	0x1F

// SPI operation codes
#define ENC28J60_READ_CTRL_REG	0x00
#define ENC28J60_READ_BUF_MEM	0x3A
#define ENC28J60_WRITE_CTRL_REG	0x40
#define ENC28J60_WRITE_BUF_MEM	0x7A
#define ENC28J60_BIT_FIELD_SET	0x80
#define ENC28J60_BIT_FIELD_CLR	0xA0
#define ENC28J60_SOFT_RESET		0xFF


#define ENC28J60_CSACTIVE 	ENC28J60_SPI_PORT&=~(1<<ENC28J60_SPI_CS) 	// CS = 0 ativar NIC
#define ENC28J60_CSPASSIVE 	ENC28J60_SPI_PORT|=(1<<ENC28J60_SPI_CS) 	// CS = 1 desativar NIC
#define waitspi() while(!(SPSR&(1<<SPIF)))								// Esperar a finalização da transmissão no barramento
#define delay_ms	_delay_ms
#define delay_us	_delay_us

void enc28j60_SetupPorts(void);
void enc28j60_WriteBuffer(u16 len, u8 *data);
void enc28j60_WriteOp(u8 op, u8 address, u8 data);
void enc28j60_ReadBuffer(u16 len, u8 *data);
u8 enc28j60_ReadOp(u8 op, u8 address);
*/

#endif
