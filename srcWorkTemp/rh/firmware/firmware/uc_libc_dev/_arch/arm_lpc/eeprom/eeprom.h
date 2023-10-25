#ifndef __CPU_EEPROM_H
#define __CPU_EEPROM_H

#include "_config_cpu_.h"

// Macro defines for EEPROM command register
#define EEPROM_CMD_8_BIT_READ			(0)
#define EEPROM_CMD_16_BIT_READ			(1)
#define EEPROM_CMD_32_BIT_READ			(2)
#define EEPROM_CMD_8_BIT_WRITE			(3)
#define EEPROM_CMD_16_BIT_WRITE			(4)
#define EEPROM_CMD_32_BIT_WRITE			(5)
#define EEPROM_CMD_ERASE_PRG_PAGE		(6)

#define EEPROM_CMD_RDPREFETCH			(1<<3)

#define EEPROM_PAGE_SIZE				64
#define EEPROM_PAGE_NUM					64

// Macro defines for EEPROM address register
#define EEPROM_PAGE_OFFSET(n)			(n&0x3F)
#define EEPROM_PAGE_ADRESS(n)			((n&0x3F)<<6)

// Macro defines for EEPROM write data register
#define	EEPROM_WDATA_8_BIT(n)			(n&0x000000FF)
#define EEPROM_WDATA_16_BIT(n)			(n&0x0000FFFF)
#define EEPROM_WDATA_32_BIT(n)			(n&0xFFFFFFFF)

// Macro defines for EEPROM read data register
#define	EEPROM_RDATA_8_BIT(n)			(n&0x000000FF)
#define EEPROM_RDATA_16_BIT(n)			(n&0x0000FFFF)
#define EEPROM_RDATA_32_BIT(n)			(n&0xFFFFFFFF)

// Macro defines for EEPROM power down register
#define EEPROM_PWRDWN					(1<<0)

#define EEPROM_ENDOF_RW					(26)
#define EEPROM_ENDOF_PROG				(28)


// Public Types ---------------------------------------------------------------
typedef enum {
	MODE_8_BIT = 0,
	MODE_16_BIT,
	MODE_32_BIT
}tEE_ModeType;


void eeprom_Init(void);
void eeprom_Write(u16 page_offset, u16 page_address, void* data, tEE_ModeType mode, u32 count);
void eeprom_Read(u16 page_offset, u16 page_address, void* data, tEE_ModeType mode, u32 count);
void eeprom_Erase(u32 address);
void eeprom_PowerDown(u8 NewState);

#if (EEPROM_USE_DEVIO == pdON)
#include "devio.h"
// facilitador para acesso a dados simples na eeprom
#define eeprom_wb(addr, data)	devwb(devEEPROM, addr, data) 	// addr e data deve ser em byte
#define eeprom_ww(addr, data)	devww(devEEPROM, (addr)*2, data)	// addr e data deve ser em word (2 bytes)
#define eeprom_wd(addr, data)	devwd(devEEPROM, (addr)*4, data)	// addr e data deve ser em dword (4 bytes)

#define eeprom_rb(addr)	devrb(devEEPROM, addr) 		// addr deve ser em byte
#define eeprom_rw(addr)	devrw(devEEPROM, (addr)*2)	// addr deve ser em word (2 bytes)
#define eeprom_rd(addr)	devrd(devEEPROM, (addr)*4)	// addr  deve ser em dword (4 bytes)

int devio_eeprom(u8 op, void* data, u32 addr, u32 len);

#endif

#endif
