//
//  $Id: disk.c 42 2008-10-04 18:40:36Z jcw $
//  $Revision: 42 $
//  $Author: jcw $
//  $Date: 2008-10-04 14:40:36 -0400 (Sat, 04 Oct 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/fatfs/disk.c $
//

// FATFs acessa o disco físico usando as rotinas do diskio com as funcs disk_initizalise, disk_status, disk_read, disk_write e disk_ioctl
// para USB preencher com usbHost_bulkRecv e usbHost_BulkSend

#include <stdio.h> 
#include "diskio.h"
#include "stdio_uc.h"

static volatile tDStatus gDiskStatus[_VOLUMES]; // Salva o status de cada drive de disco

#if (VOLUME_MMCSD >= 0)
#if (_MAX_SS < 1024)
#error _MAX_SS em _config_lib_fs.h não deve ser menor que 1024 para usar o MMCSD
#endif

#include "mmcsd.h"

static tMediaStatus mediaStatus[_VOLUMES];
static tDiskStatus diskStatus[_VOLUMES];		// Guarda os dados do tipo de cartão
static void disk_initializeMMCSD (u8 drv);
static tDiskResult disk_ioctlMMCSD (u8 drv, u8 ctrl, void *buff);
#endif

#if (VOLUME_USBMS >=0)
extern volatile  u8  *FATBuffer;                 // Buffer used by FAT file system
#include "usbhost.h"
#include "usbhost_ms.h"

u32 numBlks, blkSize;
u8 inquiryResult[USB_INQUIRY_LENGTH];
static void disk_initializeUSBMS (u8 drv);
static tDiskResult disk_ioctlUSBMS (u8 drv, u8 ctrl, void *buff);
#endif

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o sistema de acesso a discos de armazenamento de dados
// Parametros: 	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void disk_Init(void) {
	u8 y;
	for (y=0; y<_VOLUMES;y++)
		gDiskStatus[y] = DSTATUS_NOINIT;
	//memset(gDiskStatus, DSTATUS_NOINIT, sizeof(gDiskStatus));

	#if (DISKIO_USE_DEBUG == pdON)
	plog("DISKIO: Init %u volumes"CMD_TERMINATOR, _VOLUMES);
	#endif
}

#if (USE_STRING_MESSAGE == pdON)
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com a string de erro de acordo com o seu respecitivo código
// Parametros: 	Código do erro
// Retorna: 	String do erro
//------------------------------------------------------------------------------------------------------------------
const char *disk_errorlookup(tDiskResult res) {
	u8 i;
	typedef struct {
		tDiskResult dres;
		const char *str;
	} errorStrings_t;

	static const errorStrings_t errorStrings [] = {
		{ DSTATUS_NOINIT,			"Driver não inicializado. Reconecte o dispositivo"},
		{ DSTATUS_NODISK,			"Não há disco conectado"},
		{ DSTATUS_PROTECT,			"Driver protegido contra gravação"},
		{ DSTATUS_CLUSTER_INVALID,	"Tamanho do cluster não suportado - Ajuste corretamente _MAX_SS em _config_lib_fs.h"},
	};

	for (i=0; i<arrsizeof(errorStrings); i++ )
		if (errorStrings[i].dres == res) return errorStrings[i].str;

	return "(Sem descrição do erro)";
}

void disk_printerror(tDiskResult res) {
	lprintf("%s [%u]"CMD_TERMINATOR, disk_errorlookup(res), res);
}
#endif

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o disco de armazenamento de dados.
// Parametros: 	Identificador de disco
// Retorna: 	Código da status da operação
//					DSTATUS_INIT			Drive initialized
//					DSTATUS_NOINIT			Drive not initialized
//					DSTATUS_NODISK			No media in the drive
//					DSTATUS_PROTECT			Write protected
//					DSTATUS_CLUSTER_INVALID	Tamanho do cluster do disco é maior suportado pelo sistema
//------------------------------------------------------------------------------------------------------------------
tDStatus disk_initialize (u8 drv) {
	#if (DISKIO_USE_DEBUG == pdON)
	plog("DISKIO: iniciando disco drv %u"CMD_TERMINATOR, drv);
	#endif

	switch (drv) {

	#if (VOLUME_MMCSD >=0 )
  	case VOLUME_MMCSD:
  		disk_initializeMMCSD(drv);
  		break;
  	#endif

  	#if (VOLUME_USBMS >=0 )
	case VOLUME_USBMS:
		disk_initializeUSBMS(drv);
		break;
	#endif

	default:
		gDiskStatus[drv] = DSTATUS_NODISK;
		break;
  	}

  	return gDiskStatus[drv];
}


void disk_finalize (u8 drv) {
	#if (DISKIO_USE_DEBUG == pdON)
	plog("DISKIO: finalizando disco drv %u"CMD_TERMINATOR, drv);
	#endif
	gDiskStatus[drv] = DSTATUS_NOINIT;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Retorna com o status do drive de disco
// Parametros: 	Identificado do drive do disco
// Retorna: 	Código da status da operação
//					DSTATUS_INIT			Drive initialized
//					DSTATUS_NOINIT			Drive not initialized
//					DSTATUS_NODISK			No media in the drive
//					DSTATUS_PROTECT			Write protected
//------------------------------------------------------------------------------------------------------------------
tDStatus disk_status (u8 drv) {
  	return gDiskStatus[drv];
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Função de leitura ao disco usado pela FatFS
//------------------------------------------------------------------------------------------------------------------
tDiskResult disk_read (u8 drv, u8 *buff, u32 sector, u8 count) {
	tDiskResult res = DRESULT_UKNOW;

	#if (DISKIO_USE_DEBUG == pdON)
	plog("DISKIO: disk vol [%u] read sector %u count %u"CMD_TERMINATOR, drv, sector, count);
  	plog("DISKIO: addr buff 0x%x"CMD_TERMINATOR, buff);
	#endif

  	if (gDiskStatus[drv] & DSTATUS_NOINIT)
    	return DRESULT_NOTRDY;
  	if (!count) 
    	return DRESULT_PARERR;


  	switch (drv) {

  	#if (VOLUME_MMCSD >=0 )
  	case VOLUME_MMCSD:
  		res = (mmcsd_ReadSectors(buff, sector, count) == MMCSD_OK) ? DRESULT_OK : DRESULT_ERROR;
		#if (DISKIO_USE_DEBUG == pdON)
  		plog("DISKIO: disk read [%u][%u] MMC [%s - %d]"CMD_TERMINATOR, sector, count, (res == DRESULT_OK)?"OK\0":"FAIL\0",res);
		#endif

  		break;
	#endif

  	#if (VOLUME_USBMS >=0 )
  	case VOLUME_USBMS:
		if ( usbHost_MS_BulkRecv(sector, count, FATBuffer ) == pdPASS ) {
			u16 y;
			volatile u8 *ptr = FATBuffer;
			for(y=0; y<_MAX_SS; y++) buff[y] = *ptr++;

			// memcpy(buff, FATBuffer, _MAX_SS); // Copiar conteudo RAM da USB para a FAT
			res = DRESULT_OK;
		} else  res = DRESULT_ERROR;
		#if (DISKIO_USE_DEBUG == pdON)
		plog("DISKIO: disk read [%u][%u] USB MS [%s - %d]"CMD_TERMINATOR, sector, count, (res == DRESULT_OK)?"OK\0":"FAIL\0", res);
		#endif

  		break;
	#endif

  	default:
		res = DRESULT_DRVNOTPRESET;
		#if (DISKIO_USE_DEBUG == pdON)
		plog("DISKIO: ioctl device not set"CMD_TERMINATOR);
		#endif
		break;
  	}

  	return res;
}


//------------------------------------------------------------------------------------------------------------------
// Descrição:	Função de escrita ao disco usado pela FatFS
//------------------------------------------------------------------------------------------------------------------
/* The FatFs module will issue multiple sector transfer request
 /  (count > 1) to the disk I/O layer. The disk function should process
 /  the multiple sector transfer properly Do. not translate it into
 /  multiple single sector transfers to the media, or the data read/write
 /  performance may be drastically decreased. */

#if _FS_READONLY == 0
tDiskResult disk_write (u8 drv, const u8 *buff, u32 sector, u8 count) {
	tDiskResult res = DRESULT_UKNOW;

  	#if (DISKIO_USE_DEBUG == pdON)
  	plog("DISKIO: disk[%u] write sector %u qtd %u"CMD_TERMINATOR, drv, sector, count);
	#endif

  	if (gDiskStatus[drv] & DSTATUS_NOINIT)
    	return DRESULT_NOTRDY;
  	if (gDiskStatus[drv] & DSTATUS_PROTECT)
    	return DRESULT_WRPRT;
  	if (!count) 
    	return DRESULT_PARERR;

  	switch (drv) {

	#if (VOLUME_MMCSD >= 0)
	case VOLUME_MMCSD:
		res = (mmcsd_WriteSectors(buff, sector, count) == MMCSD_OK) ?  DRESULT_OK :  DRESULT_ERROR;
		#if (DISKIO_USE_DEBUG == pdON)
		plog("DISKIO: disk write [%u][%u] MMC [%s - %d]"CMD_TERMINATOR, sector, count, (res == DRESULT_OK)?"OK\0":"FAIL\0",res);
		#endif
		break;
	#endif

	#if (VOLUME_USBMS >=0 )
	case VOLUME_USBMS: {
		u16 y;
		volatile u8 *ptr = FATBuffer;
		for(y=0; y<_MAX_SS; y++) *ptr++ = buff[y];

		//memcpy(FATBuffer, buff, _MAX_SS);
		if ( usbHost_MS_BulkSend( sector, count,  FATBuffer) == pdPASS ) {
			res = DRESULT_OK;
		} else  res = DRESULT_ERROR;
		#if (DISKIO_USE_DEBUG == pdON)
		plog("DISKIO: disk write [%u][%u] USB MS [%s - %d]"CMD_TERMINATOR, sector, count, (res == DRESULT_OK)?"OK\0":"FAIL\0",res);
		#endif

		break;
	}
	#endif

	default:
		res = DRESULT_DRVNOTPRESET;
		#if (DISKIO_USE_DEBUG == pdON)
		plog("DISKIO: ioctl device not set"CMD_TERMINATOR);
		#endif
		break;
  	}

	return res;
}
#endif

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Função de controle do disco usada pela FatFS
//------------------------------------------------------------------------------------------------------------------
//The FatFs module uses only device independent commands described below. Any device dependent function is not used.
//Command Description
//CTRL_SYNC Make sure that the disk drive has finished pending write process. When the disk I/O module has a write back cache, flush the dirty sector immediately. This command is not required in read-only configuration.
//GET_SECTOR_SIZE Returns sector size of the drive into the WORD variable pointed by Buffer. This command is not required in single sector size configuration, _MAX_SS is 512.
//GET_SECTOR_COUNT Returns total sectors on the drive into the DWORD variable pointed by Buffer. This command is used by only f_mkfs function to determine the volume size to be created.
//GET_BLOCK_SIZE Returns erase block size of the flash memory in unit of sector into the DWORD variable pointed by Buffer. This command is used by only f_mkfs function and it attempts to align data area to the erase block boundary. The allowable value is 1 to 32768 in power of 2. Return 1 if the erase block size is unknown or disk devices.
tDiskResult disk_ioctl (u8 drv, u8 ctrl, void *buff) {
	tDiskResult res;

	#if (DISKIO_USE_DEBUG == pdON)
  	plog("DISKIO: ioctl [%d][%d]"CMD_TERMINATOR, drv);
	#endif

  	if (gDiskStatus[drv] & DSTATUS_NOINIT)
    	return DRESULT_NOTRDY;

  	switch (drv) {
  	//switch (registerVolume[drv]) {

	#if (VOLUME_MMCSD >=0 )
  	case VOLUME_MMCSD:
  		res = disk_ioctlMMCSD(drv, ctrl, buff);
		#if (DISKIO_USE_DEBUG == pdON)
  		plog("DISKIO: ioctl MMC [%u][%s][%d]"CMD_TERMINATOR, ctrl, (res == DRESULT_OK)?"OK\0":"FAIL\0",res);
		#endif

  		break;
	#endif

	#if (VOLUME_USBMS >=0 )
  	case VOLUME_USBMS:
  		res = disk_ioctlUSBMS(drv, ctrl, buff);
		#if (DISKIO_USE_DEBUG == pdON)
  		plog("DISKIO: ioctl USB MS [%u][%s][%d]"CMD_TERMINATOR, ctrl, (res == DRESULT_OK)?"OK\0":"FAIL\0",res);
		#endif
  		break;
	#endif

  	default:
		res = DRESULT_DRVNOTPRESET;
		#if (DISKIO_USE_DEBUG == pdON)
		plog("DISKIO: ioctl device not set"CMD_TERMINATOR);
		#endif
		break;
  	}

  	return res;
}


// #########################################################################################
// DISKIO PARA O MMC/SD
// #########################################################################################
#if (VOLUME_MMCSD >=0 )
static void disk_initializeMMCSD (u8 drv) {
	gDiskStatus[drv] = DSTATUS_NOINIT;
	switch (mmcsd_Init(&diskStatus[drv])) {
	case MMCSD_OK :
		mediaStatus[drv].statusCode = DSC_COMMANDPASS;
		mediaStatus[drv].mediaChanged = pdTRUE;

		// CHECA SE O CLUSTER DO CARTÃO É MAIOR QUE O DEFINIDO PELO SISTEMA
		//	RETORNA ERRO SE O SISTEMA NÃO ESTÁ CONFIGURADO OU NÃO SUPORTA O TAMANHO DO CLUSTER
		if (diskStatus[drv].block_size > _MAX_SS) {
			gDiskStatus[drv] = DSTATUS_CLUSTER_INVALID;
			break;
		}

		gDiskStatus[drv] = DSTATUS_INIT;
		if (mmcsd_WriteProtect ()) {
			mediaStatus[drv].writeProtect = pdTRUE;
			gDiskStatus[drv] |= DSTATUS_PROTECT;
		}

		#if (DISKIO_USE_DEBUG == pdON)
		plog("DISKIO: MMC init - cluster size %u"CMD_TERMINATOR, diskStatus[drv].block_size);
		#endif

		break;

	case MMCSD_CARD_ERROR :
	case MMCSD_DATA_ERROR :
		mediaStatus[drv].statusCode = DSC_NOTREADY;
		gDiskStatus[drv] = DSTATUS_NOINIT;
		if (mmcsd_WriteProtect ()) {
			mediaStatus[drv].writeProtect = pdTRUE;
			gDiskStatus[drv] |= DSTATUS_PROTECT;
		}
		#if (DISKIO_USE_DEBUG == pdON)
		plog("DISKIO: MMCSD protected"CMD_TERMINATOR);
		#endif

		break;

	default:
		mediaStatus[drv].statusCode = DSC_NOTPRESENT;
		gDiskStatus[drv] = DSTATUS_NODISK;
		break;
	}
}

static tDiskResult disk_ioctlMMCSD (u8 drv, u8 ctrl, void *buff) {
	tDiskResult res;
	u8 n, csd[16], *ptr = buff;
	u16 csize;
	u32 *dp, st, ed;


	res = DRESULT_ERROR;

	switch (ctrl) {
	case IOCTL_CTRL_SYNC :		/* Wait for end of internal write process of the drive */
		if (mmcsd_Enable()) {
			mmcsd_Disable();
			res = DRESULT_OK;
		}
		break;

	case IOCTL_GET_SECTOR_COUNT :	/* Get drive capacity in unit of sector (u32) */
		if ((mmcsd_SendCommand(CMD9, 0) == 0) && mmcsd_ReceiveDataBlock(csd, 16)) {
			if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
				csize = csd[9] + ((u16)csd[8] << 8) + 1;
				*(u32*)buff = (u32)csize << 10;
			} else {					/* SDC ver 1.XX or MMC ver 3 */
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
				*(u32*)buff = (u32)csize << (n - 9);
			}
			res = DRESULT_OK;
		}
		break;

	case IOCTL_GET_SECTOR_SIZE :	/* Get sector size in unit of byte (u16) */
		*(u16*)buff = 512;
		res = DRESULT_OK;
		break;

	case IOCTL_GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (u32) */
		if (diskStatus[drv].disk_type == DISK_TYPE_SD2) {	/* SDC ver 2.00 */
			if (mmcsd_SendCommand(ACMD13, 0) == 0) {	/* Read SD status */
				mmcsd_TransferByte(0xFF);
				if (mmcsd_ReceiveDataBlock(csd, 16)) {				/* Read partial block */
					for (n = 64 - 16; n; n--) mmcsd_TransferByte(0xFF);	/* Purge trailing data */
					*(u32*)buff = 16UL << (csd[10] >> 4);
					res = DRESULT_OK;
				}
			}
		} else {					/* SDC ver 1.XX or MMC */
			if ((mmcsd_SendCommand(CMD9, 0) == 0) && mmcsd_ReceiveDataBlock(csd, 16)) {	/* Read CSD */
				if (diskStatus[drv].disk_type == DISK_TYPE_SD1) {	/* SDC ver 1.XX */
					*(u32*)buff = (((csd[10] & 63) << 1) + ((u16)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else {					/* MMC */
					*(u32*)buff = ((u16)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = DRESULT_OK;
			}
		}
		break;

	case IOCTL_CTRL_ERASE_SECTOR :	/* Erase a block of sectors (used when _USE_ERASE == 1) */
		if (!(diskStatus[drv].disk_type == DISK_TYPE_SDC)) break;				/* Check if the card is SDC */
		if (disk_ioctl(drv, IOCTL_MMCSD_GET_CSD, csd)) break;	/* Get CSD */
		if (!(csd[0] >> 6) && !(csd[10] & 0x40)) break;	/* Check if sector erase can be applied to the card */
		dp = buff; st = dp[0]; ed = dp[1];				/* Load sector block */
		if (!(diskStatus[drv].disk_type == DISK_TYPE_SD_BLOCK)) {
			st *= 512; ed *= 512;
		}
		if (mmcsd_SendCommand(CMD32, st) == 0 && mmcsd_SendCommand(CMD33, ed) == 0 && mmcsd_SendCommand(CMD38, 0) == 0 && mmcsd_WaitReady())	/* Erase sector block */
			res = DRESULT_OK;	/* FatFs does not check result of this command */
		break;

	/* Following command are not used by FatFs module */

	case IOCTL_MMCSD_GET_TYPE :		/* Get MMC/SDC type (u8) */
		*ptr = diskStatus[drv].disk_type;
		res = DRESULT_OK;
		break;

	case IOCTL_MMCSD_GET_CSD :		/* Read CSD (16 bytes) */
		if (mmcsd_SendCommand(CMD9, 0) == 0		/* READ_CSD */
			&& mmcsd_ReceiveDataBlock(ptr, 16))
			res = DRESULT_OK;
		break;

	case IOCTL_MMCSD_GET_CID :		/* Read CID (16 bytes) */
		if (mmcsd_SendCommand(CMD10, 0) == 0		/* READ_CID */
			&& mmcsd_ReceiveDataBlock(ptr, 16))
			res = DRESULT_OK;
		break;

	case IOCTL_MMCSD_GET_OCR :		/* Read OCR (4 bytes) */
		if (mmcsd_SendCommand(CMD58, 0) == 0) {	/* READ_OCR */
			for (n = 4; n; n--) *ptr++ = mmcsd_TransferByte(0xFF);
			res = DRESULT_OK;
		}
		break;

	case IOCTL_MMCSD_GET_SDSTAT :	/* Read SD status (64 bytes) */
		if (mmcsd_SendCommand(ACMD13, 0) == 0) {	/* SD_STATUS */
			mmcsd_TransferByte(0xFF);
			if (mmcsd_ReceiveDataBlock(ptr, 64))
				res = DRESULT_OK;
		}
		break;

	default:
		res = DRESULT_PARERR;
	}

	mmcsd_Disable();

	return res;
}
#endif

// #########################################################################################
// DISKIO PARA O USB MASS STORE
// #########################################################################################
#if (VOLUME_USBMS >=0 )
static void disk_initializeUSBMS (u8 drv) {
	/* USB host init and enumeration */
	if ( gDiskStatus[drv] & DSTATUS_NOINIT ) {
		if (usbHost_Init() != pdPASS) return;

		#if (DISKIO_USE_DEBUG == pdON)
		plog("DISKIO: USB enum"CMD_TERMINATOR);
		#endif

		if ( usbHost_EnumDev() == pdPASS ) {
			#if (DISKIO_USE_DEBUG == pdON)
			plog("DISKIO: USB MS INIT"CMD_TERMINATOR);
			#endif

			// mass-storage init
			if ( usbHost_MS_Init(&blkSize, &numBlks, inquiryResult) == pdPASS )
				gDiskStatus[drv] = DSTATUS_INIT;

			#if (DISKIO_USE_DEBUG == pdON)
			plog("DISKIO: disk[%u] USB init [%u][%u]"CMD_TERMINATOR, drv, blkSize, numBlks);
			u8 x1;
			for (x1=0;x1<USB_INQUIRY_LENGTH;x1++)
				plognp("   inquiryResult[%u] = %u"CMD_TERMINATOR, x1, inquiryResult[x1]);
			#endif
		}
	}
}

static tDiskResult disk_ioctlUSBMS (u8 drv, u8 ctrl, void *buff) {
	tDiskResult res;

	if (gDiskStatus[drv] & DSTATUS_NOINIT) return DRESULT_NOTRDY;
	res = DRESULT_OK;
	switch(ctrl) {
	case IOCTL_CTRL_SYNC: /* Make sure that no pending write process */
		res = DRESULT_OK;
		break;
	case IOCTL_GET_SECTOR_SIZE: /* Get R/W sector size (WORD) */
		usbHost_MS_ReadCapacity(&numBlks, &blkSize);
		*(u16*)buff = blkSize; //512;
		break;
	case IOCTL_GET_SECTOR_COUNT: /* Get number of sectors on the disk (DWORD) */
		usbHost_MS_ReadCapacity(&numBlks, &blkSize);
		*(u32*)buff = numBlks;
		break;
	case IOCTL_GET_BLOCK_SIZE: /* Get erase block size in unit of sector (DWORD) */
		usbHost_MS_ReadCapacity(&numBlks, &blkSize);
		*(u32*)buff = blkSize;
		break;
	default:
		res = DRESULT_PARERR;
		break;
	}

	return res;
}
#endif
