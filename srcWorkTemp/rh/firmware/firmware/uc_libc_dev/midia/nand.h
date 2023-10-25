#ifndef __NANDFLASH_H
#define __NANDFLASH_H

#include "_config_cpu_.h"

u8 nand_StartUp(void);
u8 nand_GetCode(u8* code);
u8 nand_ReadBlock(u8* data, u32 addr, u32 len, u8 cmd);
u8 nand_ReadSpare(u8* data, u32 n, u16 len);
u8 nand_WriteBlock(u8* data, u32 addr, u32 len, u8 cmd);
u8 nand_Erase(u32 n);

#if (NAND_USE_DEVIO == pdON)
#include "devio.h"
u8 nand_ReadData(u8* data, u32 addr, u16 len);
u8 nand_WriteData(u8* data, u32 addr, u16 len);
int devio_nand(u8 op, void* data, u32 addr, u32 len);
#endif

#endif
