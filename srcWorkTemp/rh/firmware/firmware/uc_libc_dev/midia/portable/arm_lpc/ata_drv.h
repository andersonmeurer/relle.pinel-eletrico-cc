#ifndef __ATA_LIB_DRV_H
#define __ATA_LIB_DRV_H
   
#include "_config_cpu_.h"
#include "_config_iomap.h"

#define ata_ReadRegCS0(reg)	 	(*(volatile u8*) (IOMAP_IDE0_CS0 + reg))
#define ata_ReadRegCS1(reg)	 	(*(volatile u8*) (IOMAP_IDE0_CS1 + (reg - 8)))
#define ata_WriteRegCS0(reg , data)	 do { *(volatile u8*) (IOMAP_IDE0_CS0 + reg) = data; } while(0)	
#define ata_WriteRegCS1(reg , data)	 do { *(volatile u8*) (IOMAP_IDE0_CS1 + reg) = data; } while(0)

#endif


