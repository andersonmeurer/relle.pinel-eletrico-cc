#ifndef __MMCSD_H
#define __MMCSD_H

#include "mmcsd_drv.h"

//	CÓDIGOS DE OPERAÇÕES DO RETORNO DAS FUNÇÕES DE ACESSO AO CARTÃO MMC/SD
typedef enum {
  	MMCSD_OK = 0,
  	MMCSD_NO_PRESENT,
  	MMCSD_NO_RESPONSE,
  	MMCSD_CARD_ERROR,
  	MMCSD_DATA_ERROR,
  	MMCSD_UKNOW_ERROR,
  	MMCSD_PARAMETER_ERROR,
  	MMCSD_MIS_COMPARE,
  	MMCSD_DISK_TYPE_UNKNOWN
} mmcsd_sts_t;


#define MMCSD_DATA_TOKEN      0xfe
#define MMCSD_DATA_ERR_TOKEN  0x1f
#define MMCSD_STOP_TRAN       0xfd


// COMANDOS DE ACESSO AO CARTÃO MMC/SD
#define CMD0	(0)			// Resets the MultiMediaCard
#define CMD1	(1)			// Activates the card initialization process (MMC)
#define CMD8	(8)			// SEND_IF_COND
#define CMD9	(9)			// Asks the selected card to send its card-specific data (CSD)
#define CMD10	(10)		// Asks the selected card to send its card identification (CID)
#define CMD12	(12)		// Stop transmission on multiple block read
#define	CMD13	(13)   	 	// Asks the selected card to send its status register
#define CMD16	(16)		// Selects a block length (in bytes) for all following block commands (read and write)
#define CMD17	(17)		// Reads a block of the size selected by the SET_BLOCKLEN command
#define CMD18	(18)		// Continuously transfers data blocks from card to host until interrupted by a Stop command or the requested number of data blocks transmitted
#define CMD23	(23)		// SET_BLOCK_COUNT (MMC)
#define CMD24	(24)		// Writes a block of the size selected by the SET_BLOCKLEN command
#define CMD25	(25)		// Continuously writes blocks of data until a "Stop Tran" token or the requested number of blocks received
#define CMD27   (27) 		// Programming of the programmable bits of the CSD
#define CMD28   (28) 		// If the card has write protection features, this command sets the write protection bit of the addressed group. The properties of write protection are coded in the card specific data (WP_GRP_SIZE).
#define CMD29   (29) 		// If the card has write protection features, this command clears the write protection bit of the addressed group
#define CMD30   (30) 		// If the card has write protection features, this command asks the card to send the status of the write protection bits
#define CMD32	(32)		// Sets the address of the first sector of the erase group
#define CMD33	(33)		// Sets the address of the last sector in a continuous range within the selected erase group, or the address of a single sector to be selected for erase
#define CMD34	(34)		// Removes one previously selected sector from the erase selection
#define CMD35 	(35)		// Sets the address of the first erase group within a range to be selected for erase
#define CMD36 	(36)		// Sets the address of the last erase group within a continuous range to be selected for erase
#define CMD37 	(37)		// Removes one previously selected erase group from the erase selection.
#define CMD38	(38)		// Erases all previously selected sectors
#define CMD42 	(42)		// Used to set/reset the password or lock/unlock the card. The size of the Data Block is defined by the SET_BLOCK_LEN command
#define CMD55	(55)		// Notifies the card that the next command is an application specific command rather than a standard command.
#define CMD56 	(56)		// Used either to transfer a Data Block to the card or to get a Data Block from the card for general purpose/application specific commands. The size of the Data Block is defined with SET_BLOCK_LEN command
#define CMD58	(58)		// Reads the OCR register of a card
#define CMD59 	(59)		// Turns the CRC option on or off. A ‘1’ in the CRC option bit will turn the option on, a ‘0’ will turn it off
#define	ACMD23	(0x80+23)	// SET_WR_BLK_ERASE_COUNT (SDC)
#define ACMD13	(0x80+13)	// SD_STATUS (SDC)
#define	ACMD41	(0x80+41)	// Activates the card’s initialization process (Only for SD)


mmcsd_sts_t mmcsd_Init (tDiskStatus *disk_sts);
bool mmcsd_WriteProtect (void);
u32 mmcsd_SetBlockLen (u32 length);
mmcsd_sts_t mmcsd_ReadCardInfo (u8 *buffer);
mmcsd_sts_t mmcsd_ReadSectors ( u8 *buff, u32 sector, u8 count);
mmcsd_sts_t mmcsd_WriteSectors ( const u8 *buff, u32 sector, u8 count);
void mmcsd_Disable (void);
int mmcsd_Enable (void);
u8 mmcsd_SendCommand (u8 cmd, u32 arg);
int mmcsd_ReceiveDataBlock (u8 *buff, u16 btr);
int mmcsd_SendDataBlock (const u8 *buff, u8 token);


#if (MMCSD_USE_DEBUG == pdON)
void mmcsd_DecodeCSD (void);
#endif
#endif 
