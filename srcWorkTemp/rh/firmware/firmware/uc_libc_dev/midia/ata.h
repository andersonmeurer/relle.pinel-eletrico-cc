/*! \file ata.h \brief IDE-ATA hard disk interface driver. */
//*****************************************************************************
//
// File Name	: 'ata.h'
// Title		: IDE-ATA interface driver for hard disks
// Author		: Pascal Stang
// Date			: 11/22/2000
// Revised		: 4/2/2004
// Version		: 0.1
// Target MCU	: any
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#ifndef __ATA_H
#define __ATA_H

#include "_config_cpu_.h"
#include "ata_drv.h"

// typedefs
// drive info structure
struct disk_info_s {
	//DevBlock_t ataif;
	u32 sizeinsectors;
	u16 cylinders;
	u8 heads;
	u8 sectors;
	u8 LBAsupport;
	u8 driveno;
	char model[41];
} __attribute__((packed));

typedef struct disk_info_s disk_info_t;

int ata_Init(disk_info_t *disk, u8 driveno);



#define ATA_IDE_MASTER	0
#define ATA_IDE_SLAVE	1



// CONSTANTS
// ATA register offsets
// ACESSO AO CS0
#define ATA_REG_DATA			0x00
#define ATA_REG_ERROR			0x01
#define ATA_REG_SECCOUNT		0x02
#define ATA_REG_STARTSEC		0x03
#define ATA_REG_CYLLO			0x04
#define ATA_REG_CYLHI			0x05
#define ATA_REG_HDDEVSEL		0x06
#define ATA_REG_CMDSTATUS1		0x07

// ACESSO AO CS1
#define ATA_REG_CMDSTATUS2		0x08
#define ATA_REG_ACTSTATUS		0x09

#define DRIVE0		0

#define STANDBY		0
#define SLEEP		1
#define IDLE		2

// ATA status register bits
#define ATA_SR_BSY		0x80
#define ATA_SR_DRDY		0x40
#define ATA_SR_DF		0x20
#define ATA_SR_DSC		0x10
#define ATA_SR_DRQ		0x08
#define ATA_SR_CORR		0x04
#define ATA_SR_IDX		0x02
#define ATA_SR_ERR		0x01

// ATA error register bits
#define ATA_ER_UNC		0x40
#define ATA_ER_MC		0x20
#define ATA_ER_IDNF		0x10
#define ATA_ER_MCR		0x08
#define ATA_ER_ABRT		0x04
#define ATA_ER_TK0NF	0x02
#define ATA_ER_AMNF		0x01

// ATA head register bits
#define ATA_HEAD_USE_LBA	0x40

// ATA commands
#define ATA_CMD_READ			0x20
#define ATA_CMD_READNR			0x21
#define ATA_CMD_WRITE			0x30
#define ATA_CMD_WRITENR			0x31
#define ATA_CMD_IDENTIFY		0xEC
#define ATA_CMD_RECALIBRATE		0x10
#define ATA_CMD_SPINDOWN		0xE0	// spin down disk immediately
#define ATA_CMD_SPINUP			0xE1	// spin up disk immediately
#define ATA_CMD_STANDBY_5SU		0xE2	// spin down disk and set auto-power-down timer (sectorcount*5sec)
#define ATA_CMD_IDLE_5SU		0xE3	// keep disk spinning and set auto-power-down timer (sectorcount*5sec)
#define ATA_CMD_SLEEP			0xE6	// sleep disk (wakeup only on HW or SW reset)
#define ATA_CMD_STANDBY_01SU	0xF2	// spin down disk and set auto-power-down timer (sectorcount*0.1sec)
#define ATA_CMD_IDLE_01SU		0xF3	// keep disk spinning and set auto-power-down timer (sectorcount*0.1sec)


// ATA CHS disk parameters (examples, now we autodetect)
#define ATA_DISKPARM_CLYS		0x03A6	// number of cylinders per platter
#define ATA_DISKPARM_HEADS		0x10	// number of heads (usable plater sides)
#define ATA_DISKPARM_SECTORS	0x11	// number of sectors per head per cylinder

// ATA Identity fields
// all offsets refer to word offset (2 byte increments)
#define ATA_IDENT_DEVICETYPE	0		// specifies ATA/ATAPI, removable/non-removable
#define ATA_IDENT_CYLINDERS		1		// number of logical cylinders
#define ATA_IDENT_HEADS			3		// number of logical heads
#define ATA_IDENT_SECTORS		6		// number of sectors per track
#define ATA_IDENT_SERIAL		10		// drive serial number (20 characters)
#define ATA_IDENT_FIRMWAREREV	23		// drive firmware revision (8 characters)
#define ATA_IDENT_MODEL			27		// drive model name (40 characters)
#define ATA_IDENT_FIELDVALID	53		// indicates field validity of higher words (bit0: words54-58, bit1: words 64-70)
#define ATA_IDENT_LBASECTORS	60		// number of sectors in LBA translation mode

// drive mode defines (for ataSetDrivePowerMode() )
#define ATA_DISKMODE_SPINDOWN	0
#define ATA_DISKMODE_SPINUP		1
#define ATA_DISKMODE_SETTIMEOUT	2
#define ATA_DISKMODE_SLEEP		3
/*

// Prototypes
void ataInit(disk_info_t* disk, DevBlock_t ataif, u8 driveno);
void ataDriveInit(disk_info_t* disk);

void ataDriveSelect(disk_info_t* disk);
void ataSetDrivePowerMode(disk_info_t* disk, u08 mode, u08 timeout);

void ataShowRegisters(disk_info_t* disk);
u08  ataSWReset(disk_info_t* disk);
void ataDiskErr(disk_info_t* disk);
u08 ataStatusWait(disk_info_t* disk, u08 mask, u08 waitStatus);

// read and write routines for CHS based drives
u8 ataReadSectorsCHS(	disk_info_t* disk,
									u8 Head, 
									unsigned int Track,
									u8 Sector,
									unsigned int numsectors,
									u8 *Buffer);

u8 ataWriteSectorsCHS(	disk_info_t* disk,
									u8 Head, 
									unsigned int Track,
									u8 Sector,
									unsigned int numsectors,
									u8 *Buffer);

// read and write routines for LBA based drives
u8 ataReadSectorsLBA(	disk_info_t* disk,
									u32 lba,
									unsigned int numsectors,
                            		u8 *Buffer);

u8 ataWriteSectorsLBA(	disk_info_t* disk,
									u32 lba,
									unsigned int numsectors,
                            		u8 *Buffer);

// generic read and write routines using LBA
//   uses native or translated LBA addressing
//   given autodetected drive type
u8 ataReadSectors(	disk_info_t* disk, 
								u32 lba,
								unsigned int numsectors,
                            	u8 *Buffer);

u8 ataWriteSectors(	disk_info_t* disk, 
								u32 lba,
								unsigned int numsectors,
                            	u8 *Buffer);
*/
#endif
