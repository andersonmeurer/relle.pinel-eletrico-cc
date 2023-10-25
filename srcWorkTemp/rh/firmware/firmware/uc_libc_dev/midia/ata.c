/*! \file ata.c \brief IDE-ATA hard disk interface driver. */
//*****************************************************************************
//
// File Name	: 'ata.c'
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
#include "ata.h"

#if defined(FREE_RTOS)
#include "systime.h"
#else
#include "delay.h"
#endif

#if (ATA_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

// global variables
u8 ata_buffer[512] __attribute__((aligned));

static int ata_DriveInit(disk_info_t *disk);
static u8 ata_StatusWait(u8 mask, u8 wait_status);
static int ata_ReadBlock(u8 *buffer, int numBytes);

// functions
int ata_Init(disk_info_t *disk, u8 driveno) {
	// set drive interface 
	//disk->ataif = ataif;	
	// set drive number (0=master, 1=slave)
	disk->driveno = driveno;
	// initialize interface
//	disk->ataif.Init();
	return ata_DriveInit(disk);
}

static int ata_DriveInit(disk_info_t *disk) {
	u8 i;
	
	u8 *buffer = ata_buffer;
	
	// read drive identity
	#if (ATA_USE_DEBUG == pdON)
	plog("ATA: INIT"CMD_TERMINATOR);
//	plog("Init      :"CMD_TERMINATOR); rprintfu32((u32)disk->ataif.Init); rprintfCRLF();
//	plog("ReadReg   :"CMD_TERMINATOR); rprintfu32((u32)disk->ataif.ReadReg); rprintfCRLF();
//	plog("WriteReg  :"CMD_TERMINATOR); rprintfu32((u32)disk->ataif.WriteReg); rprintfCRLF();
//	plog("ReadBlock :"CMD_TERMINATOR); rprintfu32((u32)disk->ataif.ReadBlock); rprintfCRLF();
//	plog("WriteBlock:"CMD_TERMINATOR); rprintfu32((u32)disk->ataif.WriteBlock); rprintfCRLF();
	plog("ATA: Scanning IDE interface..."CMD_TERMINATOR);
	#endif
		
	// Wait for drive to be ready
	ata_StatusWait(ATA_SR_BSY, ATA_SR_BSY);
	// issue identify command
	ata_WriteRegCS0(ATA_REG_CMDSTATUS1, 0xEC);
	// wait for drive to request data transfer
	ata_StatusWait(ATA_SR_DRQ, ATA_SR_DRQ);

	delay_ms(20); //timerPause(20);
	// read in the data
	ata_ReadBlock(buffer, 512);
	
	// set local drive info parameters
	disk->cylinders =		*( ((u16*) buffer) + ATA_IDENT_CYLINDERS );
	disk->heads =			*( ((u8*) buffer) + ATA_IDENT_HEADS );
	disk->sectors =			*( ((u8*) buffer) + ATA_IDENT_SECTORS );
	disk->LBAsupport =		*( ((u8*) buffer) + ATA_IDENT_FIELDVALID );
	disk->sizeinsectors =	*( ((u32*) (buffer + ATA_IDENT_LBASECTORS*2)) );

	// copy model string
	for(i=0; i<40; i+=2) {
		// correct for byte order
		disk->model[i  ] = buffer[(ATA_IDENT_MODEL*2) + i + 1];
		disk->model[i+1] = buffer[(ATA_IDENT_MODEL*2) + i    ];
	}
	// terminate string
	disk->model[40] = 0;

	// process and print info
	if(!disk->LBAsupport) {
		// CHS, no LBA support
		// calculate drive size
		disk->sizeinsectors = (u32) disk->cylinders*disk->heads*disk->sectors;
		
		#if (ATA_USE_DEBUG == pdON)
		plog("ATA: Drive 0: %dMB CHS mode C=%d H=%d S=%d -- MODEL:"CMD_TERMINATOR, disk->sizeinsectors/(1000000/512), disk->cylinders, disk->heads, disk->sectors );
		#endif

	} 
	#if (ATA_USE_DEBUG == pdON)
	else {
		// LBA support
		plog("ATA: Drive 0: %dMB LBA mode -- MODEL:"CMD_TERMINATOR, disk->sizeinsectors/(1000000/512) );
	}
		
	plog("ATA: disk cylinders     %u"CMD_TERMINATOR, disk->cylinders);
	plog("ATA: disk heads         %u"CMD_TERMINATOR, disk->heads);
	plog("ATA: disk sectors       %u"CMD_TERMINATOR, disk->sectors);
	plog("ATA: disk LBAsupport    %s"CMD_TERMINATOR, (disk->LBAsupport == 1)?"YES":"NO");
	plog("ATA: disk sizeinsectors %u"CMD_TERMINATOR, disk->sizeinsectors);
	plog("ATA: disk model         %s"CMD_TERMINATOR, disk->model); // print model information		
	
	#endif
	
	if (( !disk->cylinders) && (!disk->heads) && (!disk->sectors))
		return pdFAIL;
	else
		return pdPASS;
	

	// initialize local disk parameters
	//ataDriveInfo.cylinders = ATA_DISKPARM_CLYS;
	//ataDriveInfo.heads = ATA_DISKPARM_HEADS;
	//ataDriveInfo.sectors = ATA_DISKPARM_SECTORS;
}

/*
void ataDiskErr(disk_info_t* disk) {
	u8 b;

	b = ata_ReadRegCS0(ATA_REG_ERROR);	
	plog("ATA: Error:"); 
	rprintfu08(b); 
	rprintfCRLF();
}

void ataSetDrivePowerMode(disk_info_t* disk, u8 mode, u8 timeout)
{
	// select drive
	ataDriveSelect(disk);
	// Wait for drive to be ready
	ata_StatusWait(ATA_SR_BSY, ATA_SR_BSY);

	// set mode
	switch(mode)
	{
	case ATA_DISKMODE_SPINDOWN:
		ata_WriteRegCS0(ATA_REG_CMDSTATUS1, ATA_CMD_SPINDOWN);
		break;
	case ATA_DISKMODE_SPINUP:
		ata_WriteRegCS0(ATA_REG_CMDSTATUS1, ATA_CMD_SPINUP);
		break;
	case ATA_DISKMODE_SETTIMEOUT:
		ata_WriteRegCS0(ATA_REG_SECCOUNT, timeout);
		ata_WriteRegCS0(ATA_REG_CMDSTATUS1, ATA_CMD_IDLE_5SU);
		break;
	case ATA_DISKMODE_SLEEP:
		ata_WriteRegCS0(ATA_REG_CMDSTATUS1, ATA_CMD_SLEEP);
		break;
	default:
		break;
	}
}
*/


static u8 ata_StatusWait(u8 mask, u8 wait_status) {
	register int status;

	delay_us(100);

	#if (ATA_USE_DEBUG == pdON)
	plog("ATA: StatusWait"CMD_TERMINATOR);
	#endif
	// wait for desired status
	while( ((status = ata_ReadRegCS0(ATA_REG_CMDSTATUS1)) & mask) == wait_status );

	return status;
}

/*
u8 ataReadSectorsCHS(	disk_info_t* disk, 
									u8 Head, 
									u32 Track,
									u8 Sector,
									u32 numsectors,
									u8 *Buffer)
{
  	u8 temp;

	// Wait for drive to be ready
	temp = ata_StatusWait(ATA_SR_BSY, ATA_SR_BSY);

  	// Prepare parameters...
	ata_WriteRegCS0(ATA_REG_HDDEVSEL, 0xA0+(disk->driveno ? 0x10:00)+Head); // CHS mode/Drive/Head
	ata_WriteRegCS0(ATA_REG_CYLHI, Track>>8);  		// MSB of track
	ata_WriteRegCS0(ATA_REG_CYLLO, Track);     		// LSB of track
  	ata_WriteRegCS0(ATA_REG_STARTSEC, Sector);    	// sector
	ata_WriteRegCS0(ATA_REG_SECCOUNT, numsectors);	// # of sectors

	// Issue read sector command...
  	ata_WriteRegCS0(ATA_REG_CMDSTATUS1, 0x21);

	// Wait for drive to be ready
	temp = ata_StatusWait(ATA_SR_BSY, ATA_SR_BSY);

	if (temp & ATA_SR_ERR)
	{
		plog("ATA: RD ERR"CMD_TERMINATOR);
		return 1;
	}

	// Wait for drive to request data transfer
	ata_StatusWait(ATA_SR_DRQ, 0);

	// read data from drive
	ata_ReadBlock(Buffer, 512*numsectors);

	// Return the error bit from the status register...
	temp = ata_ReadRegCS0(ATA_REG_CMDSTATUS1);	// read status register

	return (temp & ATA_SR_ERR) ? 1:0;
}


u8 ataWriteSectorsCHS(	disk_info_t* disk,
									u8 Head, 
									u32 Track,
									u8 Sector,
									u32 numsectors,
									u8 *Buffer)
{
	u8 temp;

	// Wait for drive to be ready
	temp = ata_StatusWait(ATA_SR_BSY, ATA_SR_BSY);

  	// Prepare parameters...
	ata_WriteRegCS0(ATA_REG_HDDEVSEL, 0xA0+(disk->driveno ? 0x10:00)+Head); // CHS mode/Drive/Head
	ata_WriteRegCS0(ATA_REG_CYLHI, Track>>8);  		// MSB of track
	ata_WriteRegCS0(ATA_REG_CYLLO, Track);     		// LSB of track
  	ata_WriteRegCS0(ATA_REG_STARTSEC, Sector);    	// sector
	ata_WriteRegCS0(ATA_REG_SECCOUNT, numsectors);	// # of sectors

	// Issue write sector command
  	ata_WriteRegCS0(ATA_REG_CMDSTATUS1, 0x31);

	//delay_us(100);

	// Wait for drive to request data transfer
	ata_StatusWait(ATA_SR_DRQ, 0);

	// write data to drive
	disk->ataif.WriteBlock(Buffer, 512*numsectors);
	
	// Wait for drive to finish write
	temp = ata_StatusWait(ATA_SR_BSY, ATA_SR_BSY);

	// check for errors
	if (temp & ATA_SR_ERR)
	{
		plog("ATA: WR ERR"CMD_TERMINATOR);
		return 1;
	}

	// Return the error bit from the status register...
	return (temp & ATA_SR_ERR) ? 1:0;
}

u8 ataReadSectorsLBA(	disk_info_t* disk,
									u32 lba,
									u32 numsectors,
                            		u8 *Buffer)
{
  	u32 cyl, head, sect;
  	u8 temp;

#ifdef DEBUG_ATA
	plog("ATA: LBA read ");
	rprintfu32(lba); rprintfChar(' ');
	rprintfu16(numsectors); rprintfChar(' ');
	rprintfu16((u32)Buffer); 
	rprintfCRLF();
#endif

	sect = (int) ( lba & 0x000000ffL );
	lba = lba >> 8;
	cyl = (int) ( lba & 0x0000ffff );
	lba = lba >> 16;
	head = ( (int) ( lba & 0x0fL ) ) | ATA_HEAD_USE_LBA;

	temp = ataReadSectorsCHS( disk, head, cyl, sect, numsectors, Buffer );

	if(temp)
		ataDiskErr(disk);
	return temp;
}

u8 ataWriteSectorsLBA(	disk_info_t* disk,
									u32 lba,
									u32 numsectors,
                            		u8 *Buffer)
{
	u32 cyl, head, sect;
	u8 temp;

#ifdef DEBUG_ATA
	plog("ATA: LBA write ");
	rprintfu32(lba); rprintfChar(' ');
	rprintfu16(numsectors); rprintfChar(' ');
	rprintfu16((u32)Buffer); 
	rprintfCRLF();
#endif

	sect = (int) ( lba & 0x000000ffL );
	lba = lba >> 8;
	cyl = (int) ( lba & 0x0000ffff );
	lba = lba >> 16;
	head = ( (int) ( lba & 0x0fL ) ) | ATA_HEAD_USE_LBA;

	temp = ataWriteSectorsCHS( disk, head, cyl, sect, numsectors, Buffer );

	if(temp)
		ataDiskErr(disk);
	return temp;
}                            		


u8 ataReadSectors(	disk_info_t* disk,
								u32 lba,
								u32 numsectors,
                            	u8 *Buffer)
{
  	u32 cyl, head, sect;
  	u8 temp;

	// check if drive supports native LBA mode
	if(disk->LBAsupport)
	{
		// drive supports using native LBA
		temp = ataReadSectorsLBA(disk, lba, numsectors, Buffer);
	}
	else
	{
		// drive required CHS access
		#ifdef DEBUG_ATA
			// do this defore destroying lba
			plog("ATA: LBA for CHS read: ");
			plog("ATA: LBA="); rprintfu32(lba); rprintfChar(' ');
		#endif

		// convert LBA to pseudo CHS
		// remember to offset the sector count by one
		sect = (u8) (lba % disk->sectors)+1;
		lba = lba / disk->sectors;
		head = (u8) (lba % disk->heads);
		lba = lba / disk->heads;
		cyl = (u16) lba;

		#ifdef DEBUG_ATA
			plog("ATA: C:H:S=");
			rprintfu16(cyl); rprintfChar(':');
			rprintfu08(head); rprintfChar(':');
			rprintfu08(sect); rprintfCRLF();
		#endif

		temp = ataReadSectorsCHS( disk, head, cyl, sect, numsectors, Buffer );
	}

	if(temp)
		ataDiskErr(disk);
	return temp;
}


u8 ataWriteSectors(	disk_info_t* disk,
								u32 lba,
								u32 numsectors,
                            	u8 *Buffer)
{
  	u32 cyl, head, sect;
  	u8 temp;

	// check if drive supports native LBA mode
	if(disk->LBAsupport)
	{
		// drive supports using native LBA
		temp = ataWriteSectorsLBA(disk, lba, numsectors, Buffer);
	}
	else
	{
		// drive required CHS access
		#ifdef DEBUG_ATA
			// do this defore destroying lba
			plog("ATA: LBA for CHS write: ");
			plog("ATA: LBA="); rprintfu32(lba); rprintfChar(' ');
		#endif

		// convert LBA to pseudo CHS
		// remember to offset the sector count by one
		sect = (u8) (lba % disk->sectors)+1;
		lba = lba / disk->sectors;
		head = (u8) (lba % disk->heads);
		lba = lba / disk->heads;
		cyl = (u16) lba;

		#ifdef DEBUG_ATA
			plog("ATA: C:H:S=");
			rprintfu16(cyl); rprintfChar(' ');
			rprintfu08(head); rprintfChar(' ');
			rprintfu08(sect); rprintfCRLF();
		#endif

		temp = ataWriteSectorsCHS( disk, head, cyl, sect, numsectors, Buffer );
	}

	if(temp)
		ataDiskErr(disk);
	return temp;
}                            		

void ataDriveSelect(disk_info_t* disk)
{
	ata_WriteRegCS0(ATA_REG_HDDEVSEL, 0xA0+(disk->driveno ? 0x10:00)); // Drive selection
}

void ataShowRegisters(disk_info_t* disk) 
{ 
	ata_WriteRegCS0(ATA_REG_HDDEVSEL, 0xA0 + (disk->driveno ? 0x10:0x00)); // Select drive
	
	plog("ATA: R0: DATA     = 0x");	rprintfu08(ata_ReadRegCS0(ATA_REG_DATA		));		plog(CMD_TERMINATOR);
	plog("ATA: R1: ERROR    = 0x");	rprintfu08(ata_ReadRegCS0(ATA_REG_ERROR	));		plog(CMD_TERMINATOR);
	plog("ATA: R2: SECT CNT = 0x");	rprintfu08(ata_ReadRegCS0(ATA_REG_SECCOUNT));		plog(CMD_TERMINATOR);
	plog("ATA: R3: SECT NUM = 0x");	rprintfu08(ata_ReadRegCS0(ATA_REG_STARTSEC));		plog(CMD_TERMINATOR);
	plog("ATA: R4: CYL LOW  = 0x");	rprintfu08(ata_ReadRegCS0(ATA_REG_CYLLO	));		plog(CMD_TERMINATOR);
	plog("ATA: R5: CYL HIGH = 0x");	rprintfu08(ata_ReadRegCS0(ATA_REG_CYLHI	));		plog(CMD_TERMINATOR);
	plog("ATA: R6: HEAD/DEV = 0x");	rprintfu08(ata_ReadRegCS0(ATA_REG_HDDEVSEL));		plog(CMD_TERMINATOR);
	plog("ATA: R7: CMD/STA  = 0x");	rprintfu08(ata_ReadRegCS0(ATA_REG_CMDSTATUS1));	plog(CMD_TERMINATOR);
}

u8 ataSWReset(disk_info_t* disk)
{
	ata_WriteRegCS0(ATA_REG_HDDEVSEL, 0x06);	// SRST and nIEN bits
	delay_us(10);	// 10uS delay_us
	ata_WriteRegCS0(ATA_REG_HDDEVSEL, 0x02);	// nIEN bits
	delay_us(10);	// 10uS delay_us
   
	while( (ata_ReadRegCS0(ATA_REG_CMDSTATUS1) & 0xC0) != 0x40 ); // Wait for DRDY and not BSY
    
	return ata_ReadRegCS0(ATA_REG_CMDSTATUS1) + ata_ReadRegCS0(ATA_REG_ERROR);
}
*/















/*
u8 ATA_Idle(u8 Drive)
{

  WriteBYTE(CMD, 6, 0xA0 + (Drive ? 0x10:0x00)); // Select drive
  WriteBYTE(CMD,7, 0xE1);

  while ((ReadBYTE(CMD,7) & 0xC0)!=0x40); // Wait for DRDY & NOT BUSY 

  // Return the error register...
  return ReadBYTE(CMD, 1);
}
*/
//----------------------------------------------------------------------------
// Set drive mode (STANDBY, IDLE)
//----------------------------------------------------------------------------
/*#define STANDBY 0
#define IDLE    1
#define SLEEP   2 
*/ 

/*
u8 SetMode(u8 DriveNo, u8 Mode, u8 PwrDown) 
{
  WriteBYTE(CMD, 6, 0xA0 + (DriveNo ? 0x10:0x00)); // Select drive
  WriteBYTE(CMD, 2, (PwrDown ? 0x01:0x00)); // Enable automatic power down
  switch (Mode) 
  {
    case STANDBY: WriteBYTE(CMD,7, 0xE2); break;
    case IDLE:    WriteBYTE(CMD,7, 0xE3); break;
    // NOTE: To recover from sleep, either issue a soft or hardware reset !
    // (But not on all drives, f.ex seagate ST3655A it's not nessecary to reset
    // but only to go in Idle mode, But on a Conner CFA170A it's nessecary with
    // a reset)
    case SLEEP:   WriteBYTE(CMD,7, 0xE6); break;
  }
  Timer10mSec=10000;
  while ((ReadBYTE(CMD,7) & 0xC0)!=0x40 && Timer10mSec); // Wait for DRDY & NOT BUSY 
  if (Timer10mSec==0) return 0xFF;                       //   or timeout
 
  // Return the error register...
  return ReadBYTE(CMD, 1);
}

*/


// read data from IDE-ATA device
static int ata_ReadBlock(u8 *buffer, int numBytes) {
	register int i;

	u16* buf = (u16*)buffer;

	// LÊ DADOS NO DISCO EM 16 EM 16 bytes
	for (i=0; i<(numBytes/16); i++) {
		// optimize by reading 16 bytes in-line before looping
		*buf++ = ata_ReadRegCS0(ATA_REG_DATA);
		*buf++ = ata_ReadRegCS0(ATA_REG_DATA);
		*buf++ = ata_ReadRegCS0(ATA_REG_DATA);
		*buf++ = ata_ReadRegCS0(ATA_REG_DATA);
		*buf++ = ata_ReadRegCS0(ATA_REG_DATA);
		*buf++ = ata_ReadRegCS0(ATA_REG_DATA);
		*buf++ = ata_ReadRegCS0(ATA_REG_DATA);
		*buf++ = ata_ReadRegCS0(ATA_REG_DATA);
	}
	
	return numBytes;
}

/*
// write data to IDE-ATA device
int ata_WriteData(u08 *buffer, int numBytes) { 
	register int i;

	u16* buf = (u16*)buffer;

	// write data to drive
	for (i=0; i<(numBytes/16); i++) {
		// optimize by writing 16 bytes in-line before looping
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
		membusWrite(ATA_REG_BASE_CS1+ATA_REG_DATA, *buf++);
	}
	return numBytes;
}
*/

