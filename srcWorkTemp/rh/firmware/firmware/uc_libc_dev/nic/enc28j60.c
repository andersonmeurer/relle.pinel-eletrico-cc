// AINDA NÃO FOI TESTADO COM FREE RTOS


//
//  $Id: enc28j60.c 320 2008-11-08 05:05:26Z jcw $
//  $Revision: 320 $
//  $Author: jcw $
//  $Date: 2008-11-08 00:05:26 -0500 (Sat, 08 Nov 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/uip/hardware/enc28j60.c $
//
//
// File Name    : 'enc38j60.c'
// Title        : Driver for the Microchip ENC28J60 Ethernet controller
// Author       : Bernard Debbasch - Copyright (C) 2005
// Created      : 2005.08.17
// Revised      : 2005.08.17
// Version      : 1.0
// Target MCU   : Philips LPC213x series
// Editor Tabs  : 2
//
//
//
// Copyright (C) 2005, Bernard Debbasch
// All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met: 
// 1. Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in the  
//    documentation and/or other materials provided with the distribution. 
// 3. The name of the author may not be used to endorse or promote
//    products derived from this software without specific prior
//    written permission.  
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "enc28j60.h"

#if (NIC_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

static void enc28j60_SoftReset(void);
#if (ENC28J60_USE_INTERRUPT == pdON)
static void enc28j60_int(void);
#endif

//  GLOBAL VARIABLE 
static u16 packetRxPointer = 0;

#if defined(FREE_RTOS)
xSemaphoreHandle xENC28J60Semaphore = NULL;
#endif


// #################################################################################################################
// #################################################################################################################
// PROCEDIMENTOS PARA INCIAR A NIC
// #################################################################################################################
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa a o controlador ethernet
// Parametros: 	Bytes do endereço MAC
// Retorna: 	pdPASS
//				errNIC_NO_RESPONSE
//------------------------------------------------------------------------------------------------------------------
int enc28j60_Init(u8 mac0, u8 mac1, u8 mac2, u8 mac3, u8 mac4, u8 mac5) {
  	u32 t;
  	int ret;

	#if defined(FREE_RTOS)
  	volatile portTickType xTicks;

  	//  It'd probably be better if the EINT initialization code took a pointer to
  	//  the semaphore, rather than just "knowing" about it, but it's hard to make
  	//  that sort of thing generic, since an interrupt routine may not really
  	//  want to use a semaphore, and instead use a queue or somesuch.  
  	//
  	if (!xENC28J60Semaphore) {
    	vSemaphoreCreateBinary (xENC28J60Semaphore);

    	if (!xENC28J60Semaphore)
      		return errSEMAPHORE_CREATE_ERROR;
  	}

  	xSemaphoreTake (xENC28J60Semaphore, 0);
	//#else
	//volatile u32 xTicks;
	#endif

  	enc28j60_SetupPorts();
  	enc28j60_Deselect();

	#if (ENC28J60_USE_INTERRUPT == pdON)
  	eint_Init(EXTMODE_EDGE, EXTPOLAR_LOW);
  	eint_SetIntHandler(enc28j60_int);
	#endif

  	//enc28j60_Reset ();

  	//
  	//  Now hold part in reset for 100ms
  	//
  	///ENC28J60_Reset ();

   	// RESET the entire ENC28J60, clearing all registers
	// Also wait for CLKRDY to become set.
	// Bit 3 in ESTAT is an unimplemented bit.  If it reads out as '1' that
	// means the part is in RESET or there is something wrong with the SPI
	// connection.  This loop makes sure that we can communicate with the
	// ENC28J60 before proceeding.
  	#if (NIC_USE_DEBUG == pdON)
	plog("NIC: POR delay"CMD_TERMINATOR);
	#endif

  	delay_ms(200); // O original eral 100 ms, mas aumentei porque estava dando erro no kit arm video

  	///vTaskDelay (200 / portTICK_RATE_MS);
  	///ENC28J60_Unreset ();
  	///vTaskDelay (200 / portTICK_RATE_MS);

  	//  Give the part 1 second for the PHY to become ready (CLKRDY == 1).  If it
  	//  doesn't, return an error to the user. 
  	//
  	//  Note that we also check that bit 3 is 0.  The data sheet says this is
  	//  unimplemented and will return 0.  We use this as a sanity check for the
  	//  ENC28J60 actually being present, because the MISO line typically floats
  	//  high.  If we only checked the CLKRDY, it will likely return 1 for when no
  	//  ENC28J60 is present.
  	#if (NIC_USE_DEBUG == pdON)
	plog("NIC: Reset and wait"CMD_TERMINATOR);
	#endif
	
	#if defined(FREE_RTOS)
  	xTicks = systime_GetTick();	
  	while (((enc28j60_ReadEthReg(ESTAT) & (ESTAT_UNIMP | ESTAT_CLKRDY)) != ESTAT_CLKRDY)) {
    	if ((systime_GetTick() - xTicks) > ms2tick(1000)) return errNIC_NO_RESPONSE;
    }
  	#else
  		// TIMEROUT DE 1S
  		t = 50;
  		while (t--) {
  			//  SEND A SOFT RESET TO THE CHIP
  			enc28j60_SoftReset();
  			delay_ms(20);
  			if ((enc28j60_ReadEthReg(ESTAT) & (ESTAT_UNIMP | ESTAT_CLKRDY)) == ESTAT_CLKRDY) break;
  		}

  		if (t == 0) return errNIC_NO_RESPONSE;
  	#endif
  
	enc28j60_BankSelect(BANK0);
  	packetRxPointer = ENC28J60_RXSTART;             		// Initialize the local RX pointer...
  	enc28j60_WriteReg16(ERXSTL, ENC28J60_RXSTART);    // ...and configure the base address 
  	enc28j60_WriteReg16(ERXRDPTL, ENC28J60_RXSTART);  // Read pointer is also at the base address for now
  	enc28j60_WriteReg16(ERXNDL, ENC28J60_RXEND);      // Configure the H/W with the end of memory location
  	enc28j60_WriteReg16(ETXSTL, ENC28J60_TXSTART);    // Configure the H/W with the TX base address
  
  	enc28j60_BankSelect(BANK1);
  	enc28j60_WriteReg16(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);

  	enc28j60_BankSelect(BANK2);
	//enc28j60_WriteReg(MACON1, MACON1_MARXEN);                    // Enable RX
	enc28j60_WriteReg(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS); // Enable TX pause, RX pause and RX
	enc28j60_WriteReg(MACON2, 0);  			// Remove all reset conditions

	//#if (ENC28J60_USE_FULL_DUPLEX == pdOFF)
  		//enc28j60_WriteReg(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN);  // Automatic padding, CRC generation
		enc28j60_WriteReg(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);  //	enable automatic padding and CRC operations
	//#else
  		//enc28j60_WriteReg(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FULDPX);  // enable automatic padding and CRC operations Full-Duplex
		//enc28j60_WriteReg(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);  // Automatic padding, CRC generation, Full-Duplex
	//#endif

	enc28j60_WriteReg(MABBIPG, 0x12);            // Set back to back inter-packet gap

	// Late collisions occur beyond 63+8 bytes (8 bytes for preamble/start of frame delimiter)
	// 55 is all that is needed for IEEE 802.3, but ENC28J60 B5 errata for improper link pulse
	// collisions will occur less often with a larger number.
	enc28j60_WriteReg(MACLCON2, 63);

	enc28j60_WriteReg16(MAIPGL, 0x0C12);         			// Set inter-packet gap to 9.6us
	enc28j60_WriteReg16(MAMXFLL, ETHERNET_MAX_FRAME_LEN);  // Set the maximum packet size which the controller will accept

  	#if (NIC_USE_DEBUG == pdON)
	plog("NIC: setting MAC = 0x%02X-0x%02X-0x%02X-0x%02X-0x%02X-0x%02X"CMD_TERMINATOR,mac0, mac1, mac2, mac3, mac4, mac5);
	#endif

  	enc28j60_BankSelect(BANK3);
  	enc28j60_WriteReg(MAADR1, mac0);
  	enc28j60_WriteReg(MAADR2, mac1);
  	enc28j60_WriteReg(MAADR3, mac2);
  	enc28j60_WriteReg(MAADR4, mac3);
  	enc28j60_WriteReg(MAADR5, mac4);
  	enc28j60_WriteReg(MAADR6, mac5);

	// Disable the CLKOUT output to reduce EMI generation
	enc28j60_WriteReg(ECOCON, 0);	// Output off (0V)

	if ((ret=enc28j60_WritePHYReg(PHCON2, PHCON2_HDLDIS)) != pdPASS) return ret;

	//#if (ENC28J60_USE_FULL_DUPLEX == pdOFF)
	if ((ret=enc28j60_WritePHYReg(PHCON1, 0)) != pdPASS) return ret;
	//#else
  	//enc28j60_WritePHYReg(PHCON1, PHCON1_PDPXMD);
	//#endif

  	u16 phyreg;
	if ((ret=enc28j60_ReadPHYReg(PHID1, &phyreg)) != pdPASS) return ret;          // Read a PHY register

	// EFEITO DE PISCA OS LEDS
	int i;
	for(i=0;i<5;i++) {
		enc28j60_WritePHYReg(PHLCON, 0x3990);	//off
		delay_ms(50);
		enc28j60_WritePHYReg(PHLCON, 0x3880);	//on
		delay_ms(50);
	}

	// DISPLAY LINK STATUS (bits 11:8 = 0x34) AND TRANSMIT/RECEIVE ACTIVITY (ALWAYS STRETCHED) (bits 7:4 = 0x72)
	if ((ret=enc28j60_WritePHYReg(PHLCON, 0x3472)) != pdPASS) return ret;

	#if (ENC28J60_USE_INTERRUPT == pdON)
	// HABILITANDO INTERRUPÇÃO PARA ACIONAMENTO DO PINO EXTERNO DA NIC
	// 	Isto para gerar um sinal no pino quando ocorrer algum evento de interrupção
		// EIE: ETHERNET INTERRUPT ENABLE REGISTER
			// INTIE: Bit para habilitar a interrupção global
			// PKTIE: Bit para habilitar a interrupçaõ de pacotes recebidos ainda pendente para leitura
	enc28j60_BFSReg(EIE, EIE_INTIE|EIE_PKTIE);
	#endif

	// ENABLE PACKET RECEPTION
	enc28j60_BFSReg(ECON1, ECON1_RXEN);

	// AJUSTAR TIPOS DE FILTROS
	// enc28j60_WriteReg(ERXFCON, ERXFCON_CRCEN|ERXFCON_ANDOR );

  	#if (NIC_USE_DEBUG == pdON)
	u16 reg;
	enc28j60_ReadPHYReg(PHLCON, &reg);
	plog("NIC: PHLCON=0x%x"CMD_TERMINATOR, reg);
	enc28j60_ReadPHYReg(PHCON1, &reg);
	plog("NIC: PHCON1=0x%x"CMD_TERMINATOR, reg);

	plog("NIC: Init OK"CMD_TERMINATOR);
	#endif

  	return pdPASS;
}

// #################################################################################################################
// #################################################################################################################
// PROCEDIMENTOS PARA ENVIAR E RECEBER PACOTES NA REDE
// #################################################################################################################
//------------------------------------------------------------------------------------------------------------------
// Descrição:
// Parametros:
// Retorna:
//------------------------------------------------------------------------------------------------------------------
void enc28j60_PacketSend(u8 *packet, u16 length) {
	#if (NIC_USE_DEBUG == pdON)
	plog ("NIC: TX len = %u"CMD_TERMINATOR, length);
	#endif

  	//  ENABLE PACKET TRANSMISSION
  	//  	enc28j60_BFCReg(EIR, EIR_TXIF);
  	enc28j60_BFSReg(ECON1, ECON1_TXRST);         // Errata for B5
  	enc28j60_BFCReg(ECON1, ECON1_TXRST);         // Errata for B5
  	enc28j60_BFCReg(EIR, EIR_TXERIF | EIR_TXIF); // Errata for B5

  	// SET THE WRITE POINTER TO START OF TRANSMIT BUFFER AREA
  	enc28j60_BankSelect(BANK0);
  	enc28j60_WriteReg16(EWRPTL, ENC28J60_TXSTART);

  	// SET THE TXND POINTER TO CORRESPOND TO THE PACKET SIZE GIVEN
  	enc28j60_WriteReg16(ETXNDL, (ENC28J60_TXSTART + length));

  	// WRITE THE PER PACKET OPTION BYTE WE USE THE DEFAULT VALUES SET IN CHIP EARLIER
  	enc28j60_MACwrite(0);

  	// COPY THE PACKET INTO THE TRANSMIT BUFFER
  	enc28j60_MACwriteBulk(packet, length);

  	// SEND THE CONTENTS OF THE TRANSMIT BUFFER ONTO THE NETWORK
  	enc28j60_BFSReg(ECON1, ECON1_TXRTS);
}


//------------------------------------------------------------------------------------------------------------------
// Descrição:
// Parametros:
// Retorna:
//------------------------------------------------------------------------------------------------------------------
u16 enc28j60_PacketReceive(u8 *packet, u16 maxlen) {
  	u16 len = 0;
  	u16 rxstat;

	#if (ENC28J60_USE_INTERRUPT == pdON)
    	//  CHECK IF AT LEAST ONE PACKET HAS BEEN RECEIVED AND IS WAITING
  		if ((enc28j60_ReadEthReg (EIR) & EIR_PKTIF) == 0) return 0;
	#else
  		//  SANITY CHECK THAT AT LEAST ONE FULL PACKET IS PRESENT (SPURIOUS INTERRUPTS?)
  		enc28j60_BankSelect(BANK1);
  		if (!enc28j60_ReadEthReg (EPKTCNT)) return 0;
	#endif

  	// SET THE READ POINTER TO THE START OF THE RECEIVED PACKET
  	enc28j60_BankSelect(BANK0);
  	enc28j60_WriteReg16(ERDPTL, packetRxPointer);

  	// READ THE NEXT PACKET POINTER
  	packetRxPointer = enc28j60_MACread();
  	packetRxPointer += enc28j60_MACread() << 8;

  	// READ THE PACKET LENGTH
  	len = enc28j60_MACread();
  	len += enc28j60_MACread() << 8;

  	// READ THE RECEIVE STATUS
  	rxstat = enc28j60_MACread();
  	rxstat |= enc28j60_MACread() << 8;

	// LIMIT RETRIEVE LENGTH
	// (we reduce the MAC-reported length by 4 to remove the CRC)
  	len = min(len, maxlen);

  	// COPY THE PACKET FROM THE RECEIVE BUFFER
  	enc28j60_MACreadBulk(packet, len);

	// MOVE THE RX READ POINTER TO THE START OF THE NEXT RECEIVED PACKET
	// This frees the memory we just read out
  	enc28j60_BankSelect(BANK0);
  	enc28j60_WriteReg16(ERXRDPTL, packetRxPointer);

    // ERRATA WORKAROUND #13. Make sure ERXRDPT is odd
    u16 rs,re;
    rs = enc28j60_ReadEthReg(ERXSTH);
    rs <<= 8;
    rs |= enc28j60_ReadEthReg(ERXSTL);
    re = enc28j60_ReadEthReg(ERXNDH);
    re <<= 8;
    re |= enc28j60_ReadEthReg(ERXNDL);

    if (packetRxPointer - 1 < rs || packetRxPointer - 1 > re)
    		enc28j60_WriteReg16(ERXRDPTL, re);
    else 	enc28j60_WriteReg16(ERXRDPTL, (packetRxPointer-1));

	// decrement the packet counter indicate we are done with this packet
	// clear the PKTIF: Receive Packet Pending Interrupt Flag bit
  	enc28j60_BFSReg(ECON2, ECON2_PKTDEC);

	#if (NIC_USE_DEBUG == pdON)
	plog ("NIC: RX len = %u"CMD_TERMINATOR, len);
	#endif

  	//  RETURN THE LENGTH
  	return len;
}


//------------------------------------------------------------------------------------------------------------------
// Descrição:
// Parametros:
// Retorna:
//------------------------------------------------------------------------------------------------------------------
#if defined(FREE_RTOS)
signed int enc28j60_WaitForData(portTickType delay) {
  	int semStat;

	#if (ENC28J60_USE_INTERRUPT == pdON)
  	enc28j60_BFCReg(EIR, EIR_PKTIF);
  	enc28j60_WriteReg(EIE, EIE_INTIE | EIE_PKTIE);
	#endif

  	if ((semStat = xSemaphoreTake (xENC28J60Semaphore, delay)) == pdPASS) {
		#if (ENC28J60_USE_INTERRUPT == pdON)
    	enc28j60_BFCReg(EIE, EIE_INTIE);
		#endif
  	}

  	return semStat;
}
#endif

// #################################################################################################################
// #################################################################################################################
// PROCEDIMENTOS PARA ACESSAR A NIC
// #################################################################################################################
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Manda um comando SRC (System Reset Command), reset via solftware
// Parametros: 	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
static void enc28j60_SoftReset (void) {
  	enc28j60_Select();
  	enc28j60_TransferByte(SRC); 
  	enc28j60_Deselect();
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê o valor do endereço
// Parametros: 	Endereço do registrador para leitura
// Retorna: 	Valor lido do registrador
//------------------------------------------------------------------------------------------------------------------
u8 enc28j60_ReadEthReg(u8 address) {
  	u8 value;

  	//  Tcss (/CS setup time) is 50ns, Tcsh (/CS hold time) is 10ns ETH registers
  	//  and memory buffer, 210ns MAC and MII registers.
  	enc28j60_Select();

  	enc28j60_TransferByte(RCR | address);  
  	value = enc28j60_TransferByte(0);

  	enc28j60_Deselect();

  	return value;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Lê o registrador na NIC
// Parametros:	Endereço do registrador a ser lido
// Retorna:		Retorna o valor lido do registrador
//------------------------------------------------------------------------------------------------------------------
u8 enc28j60_ReadMacReg (u8 address)	{
  	u8 value;

  	enc28j60_Select();

  	enc28j60_TransferByte(RCR | address);  
  	enc28j60_TransferByte(0);              //Send a dummy byte
  	value = enc28j60_TransferByte(0);

  	enc28j60_Deselect();

  	return value;
}
 
//------------------------------------------------------------------------------------------------------------------
// Descrição:		Grava dados de 8 bits no registrador
// Pre-requisito:	O banco de registradores deve esta selecionado corretamente
// Parametros:		Endereço do registrador e o valor a ser gravado
// Retorna:			Nada
//------------------------------------------------------------------------------------------------------------------
void enc28j60_WriteReg(u8 address, u8 data) {
  	enc28j60_Select();

  	enc28j60_TransferByte(WCR | address);
  	enc28j60_TransferByte(data);

  	enc28j60_Deselect();
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:		Ajusta o dados do registrador na NIC
// Pre-requisito:	O banco de registradores deve esta selecionado corretamente
// Parametros:		Endereço do registrador e dados a ser gravados
// Retorna:			Nada
//------------------------------------------------------------------------------------------------------------------
void enc28j60_BFSReg(u8 address, u8 data) {
  	enc28j60_Select();

  	enc28j60_TransferByte(BFS | address);
  	enc28j60_TransferByte(data);

  	enc28j60_Deselect();
}


//------------------------------------------------------------------------------------------------------------------
// Descrição:		Clear the data field in the address register
// Pre-requisito:	O banco de registradores deve esta selecionado corretamente
// Parametros:		address and data fiels
// Retorna:			Nada
//------------------------------------------------------------------------------------------------------------------
 void enc28j60_BFCReg(u8 address, u8 data) {
  	enc28j60_Select();

  	enc28j60_TransferByte(BFC | address);
  	enc28j60_TransferByte(data);

  	enc28j60_Deselect();
}
 
//------------------------------------------------------------------------------------------------------------------
// Descrição:		Grava dados de 16 bits no registrador
// Pre-requisito:	O banco de registradores deve esta selecionado corretamente
// Parametros:		Endereço e o dados a ser gravado
// Retorna:			Nada
//------------------------------------------------------------------------------------------------------------------
void enc28j60_WriteReg16(u8 address, u16 data) {
  	enc28j60_WriteReg(address,   (u8) (data & 0xff));
  	enc28j60_WriteReg(address+1, (u8) (data >> 8));
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	
// Parametros:	Endereço do registrador e dados a ser escrito	
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
int enc28j60_WritePHYReg(u8 address, u16 data) {
	u32 t = 1000;
	enc28j60_BankSelect(BANK2);

  	enc28j60_WriteReg(MIREGADR, address);
  	enc28j60_WriteReg16(MIWRL, data);

  	enc28j60_BankSelect(BANK3);
  	while(t--) {
  		if ((enc28j60_ReadMacReg(MISTAT) & MISTAT_BUSY) == 0) break;
  		delay_us(1);
  	}

  	if (t) return pdPASS;
  	else 	return errNIC_NO_RESPONSE;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Lê um registrador de 16 bits na NIC
// Parametros:	Endereço do registrador a ser lido
// Retorna:		Valor lido do registrador
//------------------------------------------------------------------------------------------------------------------
int enc28j60_ReadPHYReg(u8 address, u16 *value) {
//  	u16 value;
	u32 t=1000;

  	enc28j60_BankSelect(BANK2);

  	enc28j60_WriteReg(MIREGADR, address);
  	enc28j60_WriteReg(MICMD, MICMD_MIIRD);

  	enc28j60_BankSelect(BANK3);

  	while ( t--) {
  		if ( (enc28j60_ReadMacReg(MISTAT) & MISTAT_BUSY) == 0) break;
  		delay_us(1);
	}

	if (t==0) return errNIC_NO_RESPONSE;

  	enc28j60_BankSelect(BANK2);
  	enc28j60_WriteReg(MICMD, 0);

  	*value  = enc28j60_ReadMacReg(MIRDH) << 8;
  	*value += enc28j60_ReadMacReg(MIRDL);

  	return pdPASS;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Seleciona o banco de registradores na NIC
// Parametros:	O banco de registradores desejado
//				BANK0, BANK1, BANK2 ou BANK4
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void enc28j60_BankSelect(u8 bank) {
  	enc28j60_BFCReg(ECON1, ECON1_BSEL1 | ECON1_BSEL0);
  	enc28j60_BFSReg(ECON1, bank);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:		Escreve um byte na memória MAC com a intenção de mandar um frame mais tarde
// Pre-requisito:	EWRPT deve apontar para a localização a ser escrito
// Parametros:		Byte a ser escrito
// Retorna:			Nada
//------------------------------------------------------------------------------------------------------------------
void enc28j60_MACwrite(u8 data) {
  	enc28j60_Select();

  	enc28j60_TransferByte(WBM);
  	enc28j60_TransferByte(data);

  	enc28j60_Deselect();
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:		Escreve um buffer na na memória MAC com a intenção de mandar mais frames mais tarde
// Pre-requisito:	EWRPT deve apontar para a localização a ser escrito
// Parametros:		O buffer e seu tamanho
// Retorna:			Nada
//------------------------------------------------------------------------------------------------------------------
void enc28j60_MACwriteBulk(u8 *buffer, u16 length) {
  	enc28j60_Select();

  	enc28j60_TransferByte(WBM);
  	
  	while (length--) enc28j60_TransferByte(*buffer++);  	  		
  	
  	enc28j60_Deselect();
}


//------------------------------------------------------------------------------------------------------------------
// Descrição:		Lê um byte da memória MAC
// Pre-requisito:	ERDPT deve apontar para a localização a ser escrito
// Parametros:		Nenhum
// Retorna:			Leitura do dado na memória MAC
//------------------------------------------------------------------------------------------------------------------
u8 enc28j60_MACread(void) {
  	u8 value;

  	enc28j60_Select();
	
  	enc28j60_TransferByte(RBM);
  	value = enc28j60_TransferByte(0);

  	enc28j60_Deselect();

  	return value;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:		Lê dados da memória MAC e grava em um buffer
// Pre-requisito:	ERDPT deve apontar para a localização a ser escrito
// Parametros:		O endereço do buffer e seu tamanho
// Retorna:			Nada
//------------------------------------------------------------------------------------------------------------------
void enc28j60_MACreadBulk(u8 *buffer, u16 length) {
  	enc28j60_Select();
  	
  	enc28j60_TransferByte(RBM);	
	while (length--) *buffer++ = enc28j60_TransferByte(0);
  	
  	enc28j60_Deselect();
}

#if (ENC28J60_USE_INTERRUPT == pdON)
static void enc28j60_int(void) {
	#if (NIC_USE_DEBUG == pdON)
	plog("NIC:  ====> INT <===="CMD_TERMINATOR);
	#endif


	#if defined(FREE_RTOS)

  	int xHigherPriorityTaskWoken = pdFALSE;

  	xSemaphoreGiveFromISR (xENC28J60Semaphore, &xHigherPriorityTaskWoken);

	if (xHigherPriorityTaskWoken)
		portYIELD_FROM_ISR ();
	#endif
}
#endif

#if (NIC_USE_DEBUG == pdON)
void enc28j60_RegDump(void) {
            
    
/*
    plog("RevID: 0x%x"CMD_TERMINATOR, enc28j60_Read(EREVID));
    plog("Cntrl: ECON1 ECON2 ESTAT  EIR  EIE"CMD_TERMINATOR);
    plog("%02x",enc28j60_Read(ECON1));
   	plog("    ");
    plog("%02x",enc28j60_Read(ECON2));
   	plog("    ");
    plog("%02x",enc28j60_Read(ESTAT));
   	plog("    ");
    plog("%02x",enc28j60_Read(EIR));
   	plog("    ");
	plog("%02x",enc28j60_Read(EIE));
   	plog(CMD_TERMINATOR);

    plog("MAC  : MACON1  MACON2  MACON3  MACON4  MAC-Address"CMD_TERMINATOR));
   plog("        0x"));
    plog("%02x",enc28j60_Read(MACON1));
   plog("    0x"));
    plog("%02x",enc28j60_Read(MACON2));
   plog("    0x"));
    plog("%02x",enc28j60_Read(MACON3));
   plog("    0x"));
    plog("%02x",enc28j60_Read(MACON4));
   plog("    ");
    plog("%02x",enc28j60_Read(MAADR5));
    plog("%02x",enc28j60_Read(MAADR4));
    plog("%02x",enc28j60_Read(MAADR3));
    plog("%02x",enc28j60_Read(MAADR2));
    plog("%02x",enc28j60_Read(MAADR1));
    plog("%02x",enc28j60_Read(MAADR0));
   plog(CMD_TERMINATOR);

    plog("Rx   : ERXST  ERXND  ERXWRPT ERXRDPT ERXFCON EPKTCNT MAMXFL"CMD_TERMINATOR));
   plog("       0x"));
    plog("%02x",enc28j60_Read(ERXSTH));
    plog("%02x",enc28j60_Read(ERXSTL));
   plog(" 0x"));
    plog("%02x",enc28j60_Read(ERXNDH));
    plog("%02x",enc28j60_Read(ERXNDL));
   plog("  0x"));
    plog("%02x",enc28j60_Read(ERXWRPTH));
    plog("%02x",enc28j60_Read(ERXWRPTL));
   plog("  0x"));
    plog("%02x",enc28j60_Read(ERXRDPTH));
    plog("%02x",enc28j60_Read(ERXRDPTL));
   plog("   0x"));
    plog("%02x",enc28j60_Read(ERXFCON));
   plog("    0x"));
    plog("%02x",enc28j60_Read(EPKTCNT));
   plog("  0x"));
    plog("%02x",enc28j60_Read(MAMXFLH));
    plog("%02x",enc28j60_Read(MAMXFLL));
   plog(CMD_TERMINATOR);

    plog("Tx   : ETXST  ETXND  MACLCON1 MACLCON2 MAPHSUP"CMD_TERMINATOR));
   plog("       0x"));
    plog("%02x",enc28j60_Read(ETXSTH));
    plog("%02x",enc28j60_Read(ETXSTL));
   plog(" 0x"));
    plog("%02x",enc28j60_Read(ETXNDH));
    plog("%02x",enc28j60_Read(ETXNDL));
   plog("   0x"));
    plog("%02x",enc28j60_Read(MACLCON1));
   plog("     0x"));
    plog("%02x",enc28j60_Read(MACLCON2));
   plog("     0x"));
    plog("%02x",enc28j60_Read(MAPHSUP));
	plog(CMD_TERMINATOR);
*/
}
#endif
