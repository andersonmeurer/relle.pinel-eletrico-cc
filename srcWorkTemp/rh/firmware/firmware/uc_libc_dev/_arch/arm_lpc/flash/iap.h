#ifndef __IAP_H_
#define __IAP_H_

#include "_config_cpu_.h"

#define IAP_RESULT_CMD_SUCCESS						( 0 )
#define IAP_RESULT_INVALID_COMMAND      			( 1 )
#define IAP_RESULT_SRC_ADDR_ERROR       			( 2 )
#define IAP_RESULT_DST_ADDR_ERROR       			( 3 )
#define IAP_RESULT_SRC_ADDR_NOT_MAPPED 				( 4 )
#define IAP_RESULT_DST_ADDR_NOT_MAPPED 				( 5 )
#define IAP_RESULT_COUNT_ERROR          			( 6 )
#define IAP_RESULT_INVALID_SECTOR       			( 7 )
#define IAP_RESULT_SECTOR_NOT_BLANK     			( 8 )
#define IAP_RESULT_SECTOR_NOT_PREPARED 				( 9 )
#define IAP_RESULT_COMPARE_ERROR        			( 10 )
#define IAP_RESULT_BUSY                 			( 11 )

int iap_ClearSectors (u32 startingSector, u32 endingSector);
int iap_WriteSectors (u32 address, u8 *buffer, u16 bufferLen);
int iap_FillSectors (u32 startingSector, u32 endingSector, u8 byte);

int iap_PrepareSectors (u32 startingSector, u32 endingSector);
int iap_CopyRAMtoFLASH (u32 address, u8 *buffer, u16 bufferLen);
int iap_EraseSectors (u32 startingSector, u32 endingSector);
int iap_IsSectorsBlank (u32 startingSector, u32 endingSector);
u32 iap_ReadPartID (void);
u32 iap_ReadBootCodeVersion (void);
int iap_Compare (u32 address, u8 *buffer, u16 bufferLen);

int iap_IsSafeSector (u32 sector);
int iap_IsValidSector (u32 sector);
int iap_AddressToSector (u32 address, u32 *sector);
int iap_SectorToAddress (u32 sectorNumber, u32 *address, u32 *sectorSize);

// DEFINIÇÃO DOS SETORES DA MEMÓRIA FLASH DO ARM
typedef struct flashSectorToAddress_s {
	u32 address;
  	u16 sizeInBytes;
} flashSectorToAddress_t;

#endif
