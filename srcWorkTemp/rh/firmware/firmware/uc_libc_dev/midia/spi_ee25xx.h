#ifndef __SPI_EE25XX_H
#define __SPI_EE25XX_H

#include "spi_ee25xx_drv.h"
#define EE25XX_PAGESIZE			32		// Cada página ocupa 32 byte

// DEVICE COMMANDS
#define EE25XX_CMD_WREN			0x06	// write enable
#define EE25XX_CMD_WRDI			0x04	// write disable
#define EE25XX_CMD_READ			0x03	// read
#define EE25XX_CMD_WRITE		0x02	// write
#define EE25XX_CMD_WRSR			0x01	// write status register
#define EE25XX_CMD_RDSR			0x05	// read status register
	#define EE25XX_STATUS_WIP		0x01	// busy, write in progress
	#define EE25XX_STATUS_WEL		0x02	// write enable latch
	#define EE25XX_STATUS_BP0		0x04	// block protect 0
	#define EE25XX_STATUS_BP1		0x08	// block protect 1
	#define EE25XX_STATUS_WPEN		0x80	// write protect enabled

void ee25xx_Init (void);
u8 ee25xx_readStatus(void);
u8 ee25xx_ReadByte(u32 addr);
void ee25xx_ReadBuffer(u32 addr, u8 *data, u32 nbytes);
void ee25xx_WriteByte(u32 addr, u8 data);
void ee25xx_WriteBuffer(u32 addr, u8 *data, u32 nbytes);

#if (EE25XX_USE_DUMP == pdON)
void ee25xx_dump(u32 addr, u32 qtd);
#endif

#endif
