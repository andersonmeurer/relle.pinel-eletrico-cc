#ifndef __SPI_M25PXX_H
#define __SPI_M25PXX_H

#include "_config_cpu_spi.h"
#include "spi_m25pxx_drv.h"

typedef enum {
	SPI_FLASH_NONE = 0,
	AT25DF041,		// 4 megabit Serial Flash Memory
	ST25P40, 		// 4 megabit Serial Flash Memory 40 MHz
	ST25P32 		// 32 megabit Serial Flash Memory 40 Mhz
} SPI_FLASH_DEV;


SPI_FLASH_DEV m25pxx_Init (void);
int m25pxx_SeftTest(u8 idx);
void m25pxx_Signature(u8 idx, u8 *sig, u8 size_sig);
u32 m25pxx_GetID (void);
int m25pxx_Erase (void);
int m25pxx_SectorErase(u32 sector);
u8 m25pxx_ReadByte(u32 addr);
void m25pxx_ReadBuffer (u32 addr , u8 *data, u32 nbytes);
int m25pxx_WriteByte (u32 addr, u8 data);
int m25pxx_WriteBuffer (u32 addr, u8 *data, u32 nbytes);

#if (M25PXX_USE_DUMP == pdON)
void m25pxx_dump(u32 addr, u32 qtd);
#endif

#endif
