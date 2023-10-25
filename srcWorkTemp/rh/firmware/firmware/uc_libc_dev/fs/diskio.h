//
//  $Id: disk.h 50 2008-10-05 03:21:54Z jcw $
//  $Revision: 50 $
//  $Author: jcw $
//  $Date: 2008-10-04 23:21:54 -0400 (Sat, 04 Oct 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/fatfs/disk.h $
//

/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file  R0.04a   (C)ChaN, 2007
/-----------------------------------------------------------------------*/

#ifndef __DISKIO_H
#define __DISKIO_H

#include "_config_cpu_.h"
#include "_config_lib_fs.h"

// #####################################################################################
//  Results of Disk Functions 
typedef enum {
	DRESULT_OK = 0,		/* 0: Successful */
	DRESULT_ERROR,		/* 1: R/W Error */
	DRESULT_WRPRT,		/* 2: Write Protected */
	DRESULT_NOTRDY,		/* 3: Not Ready */
	DRESULT_PARERR,		/* 4: Invalid Parameter */
	DRESULT_DRVNOTPRESET,		/* 5: Driver not present */
	DRESULT_UKNOW
} tDiskResult;

// #####################################################################################
//  Status of Disk Functions 
typedef u8 tDStatus;

//  Disk Status Bits (tDStatus)
#define DSTATUS_INIT			0		/* Drive initialized */
#define DSTATUS_NOINIT			0x01	/* Drive not initialized */
#define DSTATUS_NODISK			0x02	/* No media in the drive */
#define DSTATUS_PROTECT			0x04	/* Write protected */
#define DSTATUS_CLUSTER_INVALID	0x08	// Tamanho do cluster do disco é maior suportado pelo sistema

/* Command code for disk_ioctrl fucntion */

/* Generic command (defined for FatFs) */
#define IOCTL_CTRL_SYNC				0	/* Flush disk cache (for write functions) */
#define IOCTL_GET_SECTOR_COUNT		1	/* Get media size (for only f_mkfs()) */
#define IOCTL_GET_SECTOR_SIZE		2	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define IOCTL_GET_BLOCK_SIZE		3	/* Get erase block size (for only f_mkfs()) */
#define IOCTL_CTRL_ERASE_SECTOR		4	/* Force erased a block of sectors (for only _USE_ERASE) */

/* Generic command */
#define IOCTL_CTRL_POWER			5	/* Get/Set power status */
#define IOCTL_CTRL_LOCK				6	/* Lock/Unlock media removal */
#define IOCTL_CTRL_EJECT			7	/* Eject media */

/* MMC/SDC specific ioctl command */
#define IOCTL_MMCSD_GET_TYPE		10	/* Get card type */
#define IOCTL_MMCSD_GET_CSD			11	/* Get CSD */
#define IOCTL_MMCSD_GET_CID			12	/* Get CID */
#define IOCTL_MMCSD_GET_OCR			13	/* Get OCR */
#define IOCTL_MMCSD_GET_SDSTAT		14	/* Get SD status */

/* ATA/CF specific ioctl command */
#define IOCTL_ATA_GET_REV			20	/* Get F/W revision */
#define IOCTL_ATA_GET_MODEL			21 	/* Get model name */
#define IOCTL_ATA_GET_SN			22	/* Get serial number */

/* NAND specific ioctl command */
#define IOCTL_NAND_FORMAT			30	/* Create physical format */
//
//
//
#if (VOLUME_MMCSD >= 0)
typedef enum {
  	DSC_COMMANDPASS = 0,
  	DSC_NOTREADY,
  	DSC_NOTPRESENT,
  	DSC_PARAMETERSERROR,
  	DSC_MISCOMPAREERROR,
  	DSC_DISKCHANGED,
  	DSC_UNKNOWNERROR,
} eDiskStatusCode;

typedef struct {
	eDiskStatusCode statusCode;
	bool writeProtect;
	bool mediaChanged;
} tMediaStatus;
#endif

// ---------------------------------------
// Prototypes for disk control functions
void disk_Init(void);
#if (USE_STRING_MESSAGE == pdON)
const char *disk_errorlookup(tDiskResult res);
void disk_printerror(tDiskResult res);
#endif
tDStatus disk_initialize (u8);
void disk_finalize (u8 drv);
tDStatus disk_status (u8);
tDiskResult disk_read (u8, u8 *, u32, u8);
#if _FS_READONLY == 0
tDiskResult disk_write (u8, const u8 *, u32, u8);
#endif
tDiskResult disk_ioctl (u8, u8, void *);

#endif
