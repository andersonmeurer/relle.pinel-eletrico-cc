#ifndef __USBH_FAT_H__
#define __USBH_FAT_H__

#include "_config_cpu_.h"

#define    FAT_16                   1

#define    LAST_ENTRY               1
#define    FREE_ENTRY               2
#define    LFN_ENTRY                3
#define    SFN_ENTRY                4

#define    RDONLY                   1
#define    RDWR                     2


#define  MATCH_FOUND               	0
#define  MATCH_NOT_FOUND         	-200
#define  ERR_OPEN_LIMIT_REACHED  	-202
#define  ERR_FAT_NOT_SUPPORTED   	-201
#define  ERR_INVALID_BOOT_SIG    	-203
#define  ERR_INVALID_BOOT_SEC    	-204
#define  ERR_ROOT_DIR_FULL       	-205


typedef struct boot_sec {
    u32    BootSecOffset;             /* Offset of the boot sector from sector 0                     */
    u16    BytsPerSec;                /* Bytes per sector                                            */
    u8    SecPerClus;                /* Sectors per cluster                                         */
    u32    BytsPerClus;               /* Bytes per cluster                                           */
    u16    RsvdSecCnt;                /* Reserved sector count                                       */
    u8    NumFATs;                   /* Number of FAT copies                                        */
    u16    RootEntCnt;                /* Root entry count                                            */
    u16    TotSec16;                  /* Total sectors in the disk. !=0 if TotSec32 = 0              */
    u32    TotSec32;                  /* Total sectors in the disk. !=0 if TotSec16 = 0              */
    u16    FATSz16;                   /* Sectors occupied by single FAT table                        */
    u8    FATType;                   /* File system type                                            */
    u32    RootDirSec;                /* Sectors occupied by root directory                          */
    u32    RootDirStartSec;           /* Starting sector of the root directory                       */
    u32    FirstDataSec;              /* Starting sector of the first data cluster                   */
} BOOT_SEC;


typedef  struct  file_entry {
    u32  FileSize;                    /* Total size of the file                                      */
    u16  CurrClus;                    /* Current cluster of the cluster offset                       */
    u32  CurrClusOffset;              /* Current cluster offset                                      */
    u32  EntrySec;                    /* Sector where the file entry is located                      */
    u32  EntrySecOffset;              /* Offset in the entry sector from where the file is located   */
    u8  Status;                      /* File's open status                                          */
} FILE_ENTRY;


int FAT_Init (void);
u8  FAT_GetFATType (void);
int FILE_Open (u8 *file_name, u8 flags);
int FAT_FindEntry (u8  *ent_name_given, FILE_ENTRY  *entry);
int  FAT_CreateEntry (u8  *ent_name_given, FILE_ENTRY  *entry);
void FAT_PutSFN (u8  *ent_name_given, FILE_ENTRY  *entry);
int FAT_GetFreeEntry (FILE_ENTRY  *entry);
u32  FAT_ChkEntType (volatile u8 *ent);
void FAT_GetSFN (volatile  u8  *entry, u8  *name);
void  FAT_GetSfnName (volatile  u8  *entry, u8  *name);
void  FAT_GetSfnExt (volatile  u8  *entry, u8  *ext_ptr);
int  FAT_StrCaseCmp (u8  *str1, u8  *str2);
u32  FILE_Read (s32 fd, volatile  u8  *buffer, u32   num_bytes);
u16 FAT_GetNextClus (u16  clus_no);
u32  FAT_ClusRead (u16 curr_clus, u32 clus_offset, volatile u8  *buffer, u32 num_bytes);
u32  FILE_Write ( s32 fd, volatile  u8  *buffer, u32  num_bytes);
u32 FAT_ClusWrite (u16 curr_clus, u32 clus_offset, volatile u8 *buffer, u32 num_bytes);
u16 FAT_GetFreeClus (void);
void FAT_UpdateFAT (u16 curr_clus, u16  value);
void FAT_UpdateEntry (FILE_ENTRY  *entry);
void FILE_Close (s32  fd);
u16  FAT_GetEndClus (u16  clus_no);
#endif
