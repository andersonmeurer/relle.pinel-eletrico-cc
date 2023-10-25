#include "usbhost_fat.h"
#include "utils.h"
#include "usbhost_ms.h"

#if (FS_USE_DEBUG	== pdON)
#include"stdio_uc.h"
#endif

#define MAX_FILE_DESCRIPTORS 2

extern volatile  u8  *FATBuffer;                 // Buffer used by FAT file system

static  BOOT_SEC    FAT_BootSec;
static  FILE_ENTRY  FAT_FileEntry[MAX_FILE_DESCRIPTORS];

int FAT_Init (void) {
    u16  boot_sig;
    int  rc;
    s32 i;
    FILE_ENTRY	*entry;


    for (i=0;i<MAX_FILE_DESCRIPTORS;i++) {
    	entry = &FAT_FileEntry[i];
        entry->CurrClus       = 0;
        entry->CurrClusOffset = 0;
        entry->FileSize       = 0;
        entry->EntrySec       = 0;
        entry->EntrySecOffset = 0;
        entry->Status         = 0;
    }

    usbHost_MS_BulkRecv(0, 1, FATBuffer);
    boot_sig = read_LE16u(&FATBuffer[510]);
    if (boot_sig != 0xAA55) {
        rc = ERR_INVALID_BOOT_SIG;
    } else {
        if (FATBuffer[0] != 0xEB && FATBuffer[0] != 0xE9) {
            FAT_BootSec.BootSecOffset = read_LE32u(&FATBuffer[454]);
            usbHost_MS_BulkRecv(FAT_BootSec.BootSecOffset, 1, FATBuffer);
        }
        FAT_BootSec.BytsPerSec      = read_LE16u(&FATBuffer[11]);          // Bytes per cluster
        FAT_BootSec.SecPerClus      = FATBuffer[13];                      // Sectors per cluster
                                                                          // Reserved sector count
        FAT_BootSec.RsvdSecCnt      = read_LE16u(&FATBuffer[14]) + FAT_BootSec.BootSecOffset;
        FAT_BootSec.NumFATs         = FATBuffer[16];                      // Number of FAT copies
        FAT_BootSec.RootEntCnt      = read_LE16u(&FATBuffer[17]);          // Root entry count
        FAT_BootSec.TotSec16        = read_LE16u(&FATBuffer[19]);          // Total FAT16 sectors
        FAT_BootSec.TotSec32        = read_LE32u(&FATBuffer[32]);          // Total FAT32 sectors
        FAT_BootSec.FATSz16         = read_LE16u(&FATBuffer[22]);          // Size of the FAT table
                                                                          // Bytes per cluster
        FAT_BootSec.BytsPerClus     = (FAT_BootSec.BytsPerSec * FAT_BootSec.SecPerClus);
                                                                          // Root directory starting sector
        FAT_BootSec.RootDirStartSec = FAT_BootSec.RsvdSecCnt + (FAT_BootSec.FATSz16 * FAT_BootSec.NumFATs);
                                                                      // Sectors occupied by root directory
        FAT_BootSec.RootDirSec      = ((FAT_BootSec.RootEntCnt * 32) + (FAT_BootSec.BytsPerSec - 1)) /
                                       (FAT_BootSec.BytsPerSec);
                                                                          // First data sector
        FAT_BootSec.FirstDataSec    = FAT_BootSec.RootDirStartSec + FAT_BootSec.RootDirSec;
        FAT_BootSec.FATType         = FAT_GetFATType();                   // Type of FAT
        if (FAT_BootSec.FATType == FAT_16) {
            rc = pdPASS;
        } else {
            rc = ERR_FAT_NOT_SUPPORTED;
			#if (FS_USE_DEBUG	== pdON)
            plog("FAT: error [%d]"CMD_TERMINATOR, rc);
			#endif
        }
    }

    return rc;
}

//
//**************************************************************************************************************
//*                                         GET FILE SYSTEM TYPE
//*
//* Description: This function returns the file system type with which the disk is formatted
//*
//* Arguments  : None
//*
//* Returns    : FAT16           On Success
//*              ERR_FAT_TYPE    On Failure
//*
//**************************************************************************************************************
u8  FAT_GetFATType (void) {
    u8  fat_type;
    u32  tot_sec;
    u32  tot_data_clus;

    if (FAT_BootSec.TotSec16 != 0) {                             // Get total sectors in the disk
        tot_sec = FAT_BootSec.TotSec16;
    } else {
        tot_sec = FAT_BootSec.TotSec32;
    }

    tot_data_clus = tot_sec / (FAT_BootSec.SecPerClus);          // Get total data clusters in the disk
                                                                 // If total data clusters >= 4085 and
                                                                 // < 65525, then it is FAT16 file system
    if (tot_data_clus >= 4085 && tot_data_clus < 65525) {
        fat_type = FAT_16;
    } else {
        fat_type = 0;
    }

    return (fat_type);
}

//**************************************************************************************************************
//*                                                OPENING A FILE
//*
//* Description: This function stores the attributes of a file, such as starting cluster, file size,
//*              sector where the file entry is stored and offset of the entry in that sector
//*
//* Arguments  : file_name    Name of the file. The file must be in root directory.
//*
//* Returns    : pointer to the entry    On Success
//*              NULL                    On Failure
//*
//* Modifed 6/9/08 WCB returns a file descriptor which is the INDEX+1 to the entry.
//* returning the entry address itself could be interpreted as a negative number --
//* and thus an error -- for memory locations of 0x80000000 and above.  We return
//* INDEX+1 instead of just the index to avoid returning a file descriptor of zero,
//* which could be potentially confused with an error.
//*
//**************************************************************************************************************
int FILE_Open (u8 *file_name, u8 flags) {

	int   rc;
	FILE_ENTRY  *entry = 0;
	s32  fd = -1;

    do {
    	if (FAT_FileEntry[++fd].Status == 0)
    		entry = &FAT_FileEntry[fd];
    } while ((entry == 0) && (fd < MAX_FILE_DESCRIPTORS-1));
    if (entry == 0) {
        return (ERR_OPEN_LIMIT_REACHED);
    }
    if (flags == RDONLY) {                       // Search for a file. If it doesn't exist, don't create it
        rc = FAT_FindEntry(file_name, entry);
        if (rc == MATCH_FOUND) {
            entry->Status = 1;
            rc = fd+1;
        }
    } else {                                     // Search for a file. If it doesn't exist, create it
        rc = FAT_CreateEntry(file_name, entry);
        if (rc == MATCH_FOUND) {
            entry->Status = 1;
            rc = fd+1;
        }
    }

    return rc;
}

//**************************************************************************************************************
//*                                                 FINDING AN ENTRY
//*
//* Description: This function searches for a file name in the root directory
//*
//* Arguments  : ent_name_given    Pointer to the file name to be searched.
//*              entry             Pointer to the entry structure. The attributes of the file are stored in this
//*                                structure if the file was found in the root directory.
//*
//* Returns    : MATCH_FOUND       if the file was found in the root directory.
//*              MATCH_NOT_FOUND   if the file was not found in the root directory.
//*
//**************************************************************************************************************
int FAT_FindEntry (u8  *ent_name_given, FILE_ENTRY  *entry) {
	u32   sec_num;
    volatile u8  *buf;
    u8   ent_type;
    u8   ent_name_read[13];

    for (sec_num = FAT_BootSec.RootDirStartSec;              // For all the sectors in root directory
         sec_num < (FAT_BootSec.RootDirStartSec + FAT_BootSec.RootDirSec);
         sec_num++) {


        usbHost_MS_BulkRecv(sec_num, 1, FATBuffer);                  // Read one sector
        buf = FATBuffer;
        while (buf < (FATBuffer + FAT_BootSec.BytsPerSec)) {
            ent_type = FAT_ChkEntType(buf);                  // Check for the entry type
            if (ent_type == SFN_ENTRY) {                     // If it is short entry get short file name
                FAT_GetSFN(buf, ent_name_read);
                                                    // Compare given name with this name case insensitively
                if (FAT_StrCaseCmp(ent_name_given, ent_name_read) == MATCH_FOUND) {
                    entry->CurrClus = read_LE16u(&buf[26]);   // If they are same, get starting cluster
                    entry->FileSize  = read_LE32u(&buf[28]);  // Get file size
                    entry->EntrySec = sec_num;           // Get sector number where the filename is located
                                                 // Get offset in this sector where the filename is located
                    entry->EntrySecOffset = buf - FATBuffer;
                    return (MATCH_FOUND);
                }
            }
            if (ent_type == LAST_ENTRY) {    // If it is the last entry, no more entries will exist. Return
                return (MATCH_NOT_FOUND);
            }
            buf = buf + 32;                                  // Move to the next entry
        }
    }
    return (MATCH_NOT_FOUND);
}

//**************************************************************************************************************
//*                                              CREATING AN ENTRY
//*
//* Description: This function creates a file entry in the root directory if the file does not exist
//*
//* Arguments  : ent_name_given    The file name with which the entry is to be created
//*              entry             Pointer to FILE ENTRY structure
//*
//* Returns    : OK                If the entry already exists or successfully created if it doesn't exists
//*              ERROR             If failed to create the entry
//*
//**************************************************************************************************************
int  FAT_CreateEntry (u8  *ent_name_given, FILE_ENTRY  *entry) {
	int rc;

    rc = FAT_FindEntry(ent_name_given, entry);        // Find for the given file name in the root directory
    if (rc == MATCH_FOUND) {                          // If match found, return
        return (rc);
    } else {
        rc = FAT_GetFreeEntry(entry);                 // Else get a free entry from the root directory
        if (rc != pdPASS) {
            return (rc);
        } else {
            FAT_PutSFN(ent_name_given, entry);        // Store the given short file name in that entry
            return (rc);
        }
    }
}

//**************************************************************************************************************
//*                                              PUT SHORT FILE NAME
//*
//* Description: This function fills the file entry with the short file name given by the user
//*
//* Arguments  : ent_name_given    File name given by the user
//*              entry             Pointer to the FILE_ENTRY structure
//*
//* Returns    : None
//*
//**************************************************************************************************************
void FAT_PutSFN (u8  *ent_name_given, FILE_ENTRY  *entry) {
    u32  idx;
                                           // Read the sector from root directory containing the free entry
    usbHost_MS_BulkRecv(entry->EntrySec, 1, FATBuffer);
    for (idx = 0; idx < 8; idx++) {        // Fill the first eight charecters of the entry with file name
        if (*ent_name_given == '.') {
            while (idx < 8) {
                FATBuffer[entry->EntrySecOffset + idx] = 0x20;
                idx++;
            }
            ent_name_given++;
        } else {
            FATBuffer[entry->EntrySecOffset + idx] = *ent_name_given;
            ent_name_given++;
        }
    }

    for (idx = 8; idx < 11; idx++) {                      // Fill the next 3 charecters with file extension
        if (*ent_name_given == '.') {
            while (idx < 11) {
                FATBuffer[entry->EntrySecOffset + idx] = 0x20;
                idx++;
            }
        } else {
            FATBuffer[entry->EntrySecOffset + idx] = *ent_name_given;
            ent_name_given++;
        }
    }
    FATBuffer[entry->EntrySecOffset + idx] = 0x20;
    for (idx = 12; idx < 32; idx++) {                           // Fill all the remaining bytes with 0's
        FATBuffer[entry->EntrySecOffset + idx] = 0;
    }
    usbHost_MS_BulkSend(entry->EntrySec, 1, FATBuffer);                 // Write the sector into the root directory
}

//**************************************************************************************************************
//*                                                GET GREE ENTRY
//*
//* Description: This function searches for a free entry in the root directory. If a free entry is found, the
//*              sector number and sector offset where the entry is located will be stored
//*
//* Arguments  : entry    Pointer to FILE_ENTRY structure
//*
//* Returns    : OK       If a free entry is found
//*              ERROR    If no free entry is found
//*
//**************************************************************************************************************
int FAT_GetFreeEntry (FILE_ENTRY  *entry) {
    u32   sec_num;
    volatile  u8  *buf;
    u8   ent_type;


    for (sec_num = FAT_BootSec.RootDirStartSec;
         sec_num < (FAT_BootSec.RootDirStartSec + FAT_BootSec.RootDirSec);
         sec_num++) {

        usbHost_MS_BulkRecv(sec_num, 1, FATBuffer);
        buf = FATBuffer;
        while (buf < (FATBuffer + FAT_BootSec.BytsPerSec)) {
            ent_type = FAT_ChkEntType(buf);
            if (ent_type == FREE_ENTRY) {
                entry->EntrySec       = sec_num;
                entry->EntrySecOffset = buf - FATBuffer;
                return (pdPASS);
            }
            if (ent_type == LAST_ENTRY) {
                return (ERR_ROOT_DIR_FULL);
            } else {
                buf += 32;
            }
        }
    }
    return (ERR_ROOT_DIR_FULL);
}

//**************************************************************************************************************
//*                                       CHECK TYPE OF THE ENTRY
//*
//* Description: This function checks the type of file entry.
//*
//* Arguments  : ent           Pointer to the buffer containing the entry
//*
//* Returns    : LAST_ENTRY    if the entry is last entry
//*              FREE_ENTRY    if the entry is free entry
//*              LFN_ENTRY     if the entry is long file name entry
//*              SFN_ENTRY     if the entry is short file name entry
//*
//**************************************************************************************************************
u32  FAT_ChkEntType (volatile u8 *ent) {
    if (ent[0] == 0x00) {                              // First byte is 0 means it is the last entry
        return (LAST_ENTRY);
    }

    if (ent[0] == 0xE5) {                              // First byte is 0xE5 means it is the free entry
        return (FREE_ENTRY);
    }

    if (0x0F == ent[11]) {                             // If 11th byte of an entry is 0x0F, it is LFN
        return (LFN_ENTRY);

    } else {
        return (SFN_ENTRY);                            // Else it is the SFN
    }
}

//**************************************************************************************************************
//*                                       GET SHORT FILE NAME AND EXTENSION OF A FILE
//*
//* Description: This function reads the short file name and extension corresponding to a file
//*
//* Arguments  : ent_buf    buffer which contains the 32 byte entry of a file
//*              name       buffer to store the file name and extension of a file
//*
//* Returns    : None
//*
//**************************************************************************************************************
void FAT_GetSFN (volatile  u8  *entry, u8  *name) {
    u8   ext[4];                          // Buffer to store the extension of a file
    u8  *ext_ptr;
    ext_ptr = ext;

    FAT_GetSfnName(entry, name);                  // Get file name into "name" buffer
    FAT_GetSfnExt(entry, ext_ptr);                // Get extension into "ext" buffer

    while (*name) {                               // Goto the end of the filename
        name++;
    }
    if (*ext_ptr) {                               // If the extension exists, put a '.' charecter
        *name = '.';
        name++;
    }
    while (*ext_ptr) {                            // Append the extension to the file name
        *name = *ext_ptr;
        name++;
        ext_ptr++;
    }
    *name = '\0';
}

//**************************************************************************************************************
//*                                          GET SHORT FILE NAME OF A FILE
//*
//* Description: This function reads the short file name of a file
//*
//* Arguments  : ent_buf  buffer which contains the 32 byte entry of a file
//*              name     buffer to store the short file name of a file
//*
//* Returns    : None
//*
//**************************************************************************************************************
void  FAT_GetSfnName (volatile  u8  *entry, u8  *name) {
    u32  cnt;

    cnt = 0;
    while (cnt < 8) {
        *name = *entry;                     // Get first 8 charecters of an SFN entry
        name++;
        entry++;
        cnt++;
    }
    *name = 0;
    name--;
    while (*name == 0x20) {                 // If any spaces exist after the file name, replace them with 0
        *name = 0;
        name--;
    }
}

//**************************************************************************************************************
//*                                       GET EXTENSION OF A FILE
//*
//* Description: This function reads the extension of a file
//*
//* Arguments  : ent_buf    buffer which contains the 32 byte entry of a file
//*              ext_ptr    buffer to store the extension of a file
//*
//* Returns    : None
//*
//**************************************************************************************************************
void  FAT_GetSfnExt (volatile  u8  *entry, u8  *ext_ptr) {
    u32  cnt;


    cnt = 0;
    while (cnt < 8) {                  // Goto the beginning of the file extension
        entry++;
        cnt++;
    }
    cnt = 0;
    while (cnt < 3) {                  // Get 3 charecters from there
        *ext_ptr = *entry;
        ext_ptr++;
        entry++;
        cnt++;
    }
    *ext_ptr = 0;
    ext_ptr--;
    while (*ext_ptr == ' ') {          // If any spaces exist after the file extension, replace them with 0
        *ext_ptr = 0;
        ext_ptr--;
    }
}

//**************************************************************************************************************
//*                                       CASE INSENSITIVE COMPARISION OF STRINGS
//*
//* Description: This function compares two strings case insensitively
//*
//* Arguments  : str1               Pointer to the first string
//*              str2               Pointer to the second string
//*
//* Returns    : MATCH_FOUND        if both the strings are same
//*              NATCH_NOT_FOUND    if both the strings are different
//*
//**************************************************************************************************************
int  FAT_StrCaseCmp (u8  *str1, u8  *str2) {
    while (*str1 && *str2) {
        if (*str1 == *str2 || *str1 == (*str2 + 32) || *str1 == (*str2 - 32)) {
            str1++;
            str2++;
            continue;
        } else {
            return (MATCH_NOT_FOUND);
        }
    }
    if (*str1 == 0 && *str2 == 0) {
        return (MATCH_FOUND);
    } else {
        return (MATCH_NOT_FOUND);
    }
}

//**************************************************************************************************************
//*                                       READ DATA REQUESTED BY THE USER
//*
//* Description: This function reads data requested by the application from the file pointed by file descriptor.
//*
//* Arguments  : fd                  file descriptor that points to a file
//*              buffer              buffer into which the data is to be read
//*              num_bytes           number of bytes requested by the application
//*
//* Returns    : total_bytes_read    Total bytes actually read.
//*
//**************************************************************************************************************
u32  FILE_Read (s32 fd, volatile  u8  *buffer, u32   num_bytes) {
    u32   total_bytes_to_read;            // Total bytes requested by the application
    u32   total_bytes_read;               // Total bytes read
    u32   bytes_read;                     // Bytes read from one cluster
    u32   bytes_to_read;                  // Bytes to be read in one cluster
    FILE_ENTRY  *entry;                          // Entry that contains the file attribute information
    u16   next_clus;                     // Next cluster of the current cluster in the cluster chain

    entry = &FAT_FileEntry[fd-1];                // Get file entry from file descriptor
    total_bytes_read = 0;

    if (entry->FileSize == 0) {
        return (0);
    }
    if (num_bytes < entry->FileSize) {
        total_bytes_to_read = num_bytes;
    } else {
        total_bytes_to_read = entry->FileSize;
    }
    do {
        next_clus = FAT_GetNextClus(entry->CurrClus);     // Get next cluster
        if (next_clus == 0) {                             // If the current cluster is the last cluster
                                                          // If the offset is at the end of the file
            if (entry->CurrClusOffset == (entry->FileSize % FAT_BootSec.BytsPerClus)) {
                return (0);                               // No more bytes to read
            }                               // If requested number is > remaining bytes in the last cluster
            if (total_bytes_to_read > ((entry->FileSize % FAT_BootSec.BytsPerClus) - entry->CurrClusOffset)) {
                total_bytes_to_read = (entry->FileSize % FAT_BootSec.BytsPerClus) - entry->CurrClusOffset;
            }
            bytes_to_read = total_bytes_to_read;
                                         // If requested number is > remaining bytes in the current cluster
        } else if (total_bytes_to_read > (FAT_BootSec.BytsPerClus - entry->CurrClusOffset)) {
            bytes_to_read = FAT_BootSec.BytsPerClus - entry->CurrClusOffset;
        } else {
            bytes_to_read = total_bytes_to_read;
        }
        bytes_read = FAT_ClusRead(entry->CurrClus,       // Read bytes from a single cluster
                                  entry->CurrClusOffset,
                                  buffer,
                                  bytes_to_read);
        buffer              += bytes_read;
        total_bytes_read    += bytes_read;
        total_bytes_to_read -= bytes_read;
                                             // If the cluster offset reaches end of the cluster, make it 0
        if (entry->CurrClusOffset + bytes_read == FAT_BootSec.BytsPerClus) {
            entry->CurrClusOffset = 0;
        } else {
            entry->CurrClusOffset += bytes_read;        // Else increment the cluster offset
        }
        if (entry->CurrClusOffset == 0) {
            entry->CurrClus = (next_clus > 0) ? next_clus : entry->CurrClus;
        }
    } while (total_bytes_to_read);
    return (total_bytes_read);
}

//**************************************************************************************************************
//*                                              GET NEXT CLUSTER
//*
//* Description: This function returns next cluster of the current cluster in the cluster chain. If the current
//*              cluster is the last cluster then this function returns 0
//*
//* Arguments  : clus_no    The cluster number for which the next cluster to be found
//*
//* Returns    : next_clus  if the current cluster is not the last cluster
//*              0          if the current cluster is the last cluster
//*                         Note: In practical cluster number 0 doesn't exist
//*
//**************************************************************************************************************
u16 FAT_GetNextClus (u16  clus_no) {
    u32  sec_num;
    u32  ent_offset;
    u16  next_clus;
                                          // Get the sector number in the FAT that contains current cluster
    sec_num = FAT_BootSec.RsvdSecCnt + ((clus_no * 2) / FAT_BootSec.BytsPerSec);
                                   // Get the sector offset in the FAT where the current cluster is located
    ent_offset = (clus_no * 2) % FAT_BootSec.BytsPerSec;
    usbHost_MS_BulkRecv(sec_num, 1, FATBuffer);                          // Read that sector
    next_clus = read_LE16u(&FATBuffer[ent_offset]);               // Read the next cluster
    if (next_clus >= 0xFFF8 && next_clus <= 0xFFFF) {      // If that value is in between 0xFFF8 and 0xFFFF
        next_clus = 0;                                     // Current cluster is the end cluster
    }
    return (next_clus);

}

//**************************************************************************************************************
//*                                                 READ FROM ONE CLUSTER
//*
//* Description: This function reads the data from a single cluster.
//*
//* Arguments  : curr_clus         Current cluster from which the data has to read
//*              clus_offset       Position in the current cluster from which the data has to read
//*              buffer            Buffer into which the data has to read
//*              num_bytes         Number of bytes to read
//*
//* Returns    : tot_bytes_read    Total bytes read from the current cluster
//*
//**************************************************************************************************************
u32  FAT_ClusRead (u16 curr_clus, u32 clus_offset, volatile u8  *buffer, u32 num_bytes) {
    u32  tot_bytes_read;                              // total bytes read in the current cluster
    u32  n_bytes;                                     // Bytes to read in the current sector
    u32  start_sec;                                   // Starting sector of the current cluster
    u32  sec_num;                                     // Current sector number
    u16  num_sec;                                     // Number of sectors to be read
    u32  sec_offset;                                  // Offset in the current sector
    u32  cnt;


    tot_bytes_read = 0;
    start_sec  = ((curr_clus - 2) * FAT_BootSec.SecPerClus) + FAT_BootSec.FirstDataSec;
    sec_num    = start_sec + (clus_offset / FAT_BootSec.BytsPerSec);
    num_sec    = num_bytes / FAT_BootSec.BytsPerSec;
    sec_offset = clus_offset % FAT_BootSec.BytsPerSec;

    if (sec_offset) {                                 // If the sector offset is at the middle of a sector
        usbHost_MS_BulkRecv(sec_num, 1, FATBuffer);           // Read the first sector
        n_bytes = (FAT_BootSec.BytsPerSec - sec_offset <= num_bytes) ?
                  (FAT_BootSec.BytsPerSec - sec_offset) : num_bytes;
        for (cnt = sec_offset; cnt < sec_offset + n_bytes; cnt++) {
            *buffer = FATBuffer[cnt];                 // Copy the required bytes to user buffer
             buffer++;
        }
        tot_bytes_read += n_bytes;
        clus_offset    += n_bytes;
        num_bytes      -= n_bytes;
        sec_num++;
    }

    if (num_bytes / FAT_BootSec.BytsPerSec) {         // Read all the remaining full sectors

        num_sec = num_bytes / FAT_BootSec.BytsPerSec;
        usbHost_MS_BulkRecv(sec_num, num_sec, buffer);
        buffer         += (num_sec * FAT_BootSec.BytsPerSec);
        tot_bytes_read += (num_sec * FAT_BootSec.BytsPerSec);
        clus_offset    += (num_sec * FAT_BootSec.BytsPerSec);
        num_bytes      -= (num_sec * FAT_BootSec.BytsPerSec);
        sec_num        += num_sec;
    }

    if (num_bytes) {                                  // Read the last sector for the remaining bytes
        usbHost_MS_BulkRecv(sec_num, 1, FATBuffer);
        for (cnt = 0; cnt < num_bytes; cnt++) {
            *buffer = FATBuffer[cnt];                 // Copy the required bytes to user buffer
             buffer++;
        }
        tot_bytes_read += num_bytes;
    }

    return (tot_bytes_read);
}

//**************************************************************************************************************
//*                                       WRITE THE DATA REQUESTED BY THE USER
//*
//* Description: This function writes data requested by the application to the file pointed by file descriptor
//*
//* Arguments  : fd                     file descriptor that points to a file
//*              buffer                 buffer from which the data is to be written
//*              num_bytes              number of bytes requested by the application
//*
//* Returns    : total_bytes_written    Total bytes actually written
//*
//**************************************************************************************************************
u32  FILE_Write ( s32 fd, volatile  u8  *buffer, u32  num_bytes) {
    u32   total_bytes_to_write;                      // Total bytes requested by application
    u32   total_bytes_written;                       // Total bytes written
    u32   bytes_written;                             // Bytes written in a single cluster
    u32   bytes_to_write;                            // Bytes to write in a single cluster
    FILE_ENTRY  *entry;                               // Entry that contains the file attribute information
    u16   free_clus;                                 // Free cluster available in the disk

    entry = &FAT_FileEntry[fd-1];                           // Get file entry from file descriptor
    total_bytes_written  = 0;
    total_bytes_to_write = num_bytes;

    if (num_bytes) {
        if (entry->FileSize == 0) {
            free_clus = FAT_GetFreeClus();
            FAT_UpdateFAT(free_clus, 0xFFFF);
            entry->CurrClus  = free_clus;
            usbHost_MS_BulkRecv(entry->EntrySec, 1, FATBuffer);
            write_LE16u(&FATBuffer[(entry->EntrySecOffset) + 26], free_clus);
            usbHost_MS_BulkSend(entry->EntrySec, 1, FATBuffer);
        }
    } else {
        return (0);
    }
    entry->CurrClus = FAT_GetEndClus(entry->CurrClus);           // Make the current cluster as end cluster
    entry->CurrClusOffset = entry->FileSize % FAT_BootSec.BytsPerClus;   // Move cluster offset to file end
    do {
        if (total_bytes_to_write > FAT_BootSec.BytsPerClus - entry->CurrClusOffset) {
            bytes_to_write = FAT_BootSec.BytsPerClus - entry->CurrClusOffset;
        } else {
            bytes_to_write = total_bytes_to_write;
        }
        bytes_written = FAT_ClusWrite(entry->CurrClus,
                                      entry->CurrClusOffset,
                                      buffer,
                                      bytes_to_write);
        buffer               += bytes_written;
        total_bytes_written  += bytes_written;
        total_bytes_to_write -= bytes_written;
        entry->FileSize      += bytes_written;
        if (entry->CurrClusOffset + bytes_written == FAT_BootSec.BytsPerClus) {
            entry->CurrClusOffset = 0;
        } else {
            entry->CurrClusOffset += bytes_written;
        }
        if (entry->CurrClusOffset == 0) {
            free_clus = FAT_GetFreeClus();
            if (free_clus == 0) {
                return (total_bytes_written);
            }
            FAT_UpdateFAT(entry->CurrClus, free_clus);
            FAT_UpdateFAT(free_clus, 0xFFFF);
            entry->CurrClus = free_clus;
        }
    } while (total_bytes_to_write);
    return (total_bytes_written);
}

//**************************************************************************************************************
//*                                               WRITE TO ONE CLUSTER
//*
//* Description: This function writes the data to a single cluster.
//*
//* Arguments  : curr_clus         Current cluster into which the data has to write
//*              clus_offset       Position in the current cluster from which the data has to write
//*              buffer            Buffer from which the data has to write
//*              num_bytes         Number of bytes to write
//*
//* Returns    : tot_bytes_read    Total bytes written into the current cluster
//*
//**************************************************************************************************************
u32 FAT_ClusWrite (u16 curr_clus, u32 clus_offset, volatile u8 *buffer, u32 num_bytes) {
    u32  tot_bytes_written;
	u32  n_bytes;
	u32  start_sec;
	u32  sec_num;
	u16  num_sec;
	u32  sec_offset;
	u32  cnt;


	tot_bytes_written = 0;
	start_sec  = ((curr_clus - 2) * FAT_BootSec.SecPerClus) + FAT_BootSec.FirstDataSec;
	sec_num    = start_sec + (clus_offset / FAT_BootSec.BytsPerSec);
	num_sec    = num_bytes / FAT_BootSec.BytsPerSec;
	sec_offset = clus_offset % FAT_BootSec.BytsPerSec;

	if (sec_offset) {
        usbHost_MS_BulkRecv(sec_num, 1, FATBuffer);
		n_bytes = (FAT_BootSec.BytsPerSec - sec_offset <= num_bytes) ?
                  (FAT_BootSec.BytsPerSec - sec_offset) : num_bytes;
		for (cnt = sec_offset; cnt < (sec_offset + n_bytes); cnt++) {
            FATBuffer[cnt] = *buffer;
			buffer++;
		}
        usbHost_MS_BulkSend(sec_num, 1, FATBuffer);
		tot_bytes_written += n_bytes;
		clus_offset       += n_bytes;
        num_bytes         -= n_bytes;
		sec_num++;
	}
	if (num_bytes / FAT_BootSec.BytsPerSec) {
		num_sec = num_bytes / FAT_BootSec.BytsPerSec;
		usbHost_MS_BulkSend(sec_num, num_sec, buffer);
		buffer            += (num_sec * FAT_BootSec.BytsPerSec);
		tot_bytes_written += (num_sec * FAT_BootSec.BytsPerSec);
		clus_offset       += (num_sec * FAT_BootSec.BytsPerSec);
        num_bytes         -= (num_sec * FAT_BootSec.BytsPerSec);
		sec_num           += num_sec;
	}
	if (num_bytes) {
        usbHost_MS_BulkRecv(sec_num, 1, FATBuffer);

		for (cnt = 0; cnt < num_bytes; cnt++) {
			FATBuffer[cnt] = *buffer;
			buffer++;
		}
        usbHost_MS_BulkSend(sec_num, 1, FATBuffer);
		tot_bytes_written += num_bytes;
	}
	return (tot_bytes_written);
}

//**************************************************************************************************************
//*                                               GET FREE CLUSTER
//*
//* Description: This function returns the free cluster if available
//*
//* Arguments  : None
//*
//* Returns    : free_clus    if available
//*              0            if not available(means the disk is full)
//*
//**************************************************************************************************************
u16 FAT_GetFreeClus (void) {
    u32  num_sec;
    u32  cnt;
    u32  sec_num;
    u16  free_clus;

    sec_num = FAT_BootSec.RsvdSecCnt;
    num_sec = FAT_BootSec.FATSz16;
    while (sec_num < (FAT_BootSec.RsvdSecCnt + num_sec)) {
        usbHost_MS_BulkRecv(sec_num, 1, FATBuffer);
        for (cnt = 0; cnt < FAT_BootSec.BytsPerSec; cnt += 2) {
            if (read_LE16u(&FATBuffer[cnt]) == 0) {
                free_clus = (((sec_num - FAT_BootSec.RsvdSecCnt) * FAT_BootSec.BytsPerSec) + cnt) / 2;
                return (free_clus);
            }
        }
        sec_num++;
    }
    return (0);
}

//**************************************************************************************************************
//*                                       UPDATE FILE ALLOCATION TABLE
//*
//* Description: This function updates the file allocation table
//*
//* Arguments  : curr_clus    Offset of the current cluster number in the file allocation table
//*              value        Value with which this offset to be updated
//*
//* Returns    : None
//*
//**************************************************************************************************************
void FAT_UpdateFAT (u16 curr_clus, u16  value) {
    u32  sec_num;
    u32  sec_offset;

    sec_num = FAT_BootSec.RsvdSecCnt + (curr_clus * 2) / FAT_BootSec.BytsPerSec;
    sec_offset = (curr_clus * 2) % FAT_BootSec.BytsPerSec;

    usbHost_MS_BulkRecv(sec_num, 1, FATBuffer);
    write_LE16u(&FATBuffer[sec_offset], value);
    usbHost_MS_BulkSend(sec_num, 1, FATBuffer);
}
//
//**************************************************************************************************************
//*                                              UPDATE THE FILE ENTRY
//*
//* Description: This function updates the file entry that is located in the root directory
//*
//* Arguments  : entry    Pointer to the FILE ENTRY structure which contains the information about the file
//*
//* Returns    : None
//*
//**************************************************************************************************************
void FAT_UpdateEntry (FILE_ENTRY  *entry) {
    u32  sec_num;
    u32  offset;

    sec_num = entry->EntrySec;
    offset  = entry->EntrySecOffset;
    usbHost_MS_BulkRecv(sec_num, 1, FATBuffer);
    write_LE32u(&FATBuffer[offset + 28], entry->FileSize);
    usbHost_MS_BulkSend(sec_num, 1, FATBuffer);
}

//**************************************************************************************************************
//*                                                  FILE CLOSE
//*
//* Description: This function closes the opened file by making all the elements of FILE_ENTRY structure to 0
//*
//* Arguments  : fd    File descriptor which points to the file to be closed
//*
//* Returns    : None
//*
//**************************************************************************************************************
void FILE_Close (s32  fd) {
    FILE_ENTRY  *entry;

    entry = &FAT_FileEntry[fd-1];
    usbHost_MS_BulkRecv(entry->EntrySec, 1, FATBuffer);
    write_LE32u(&FATBuffer[entry->EntrySecOffset + 28], entry->FileSize);    // Update the file size
    usbHost_MS_BulkSend(entry->EntrySec, 1, FATBuffer);
    entry->CurrClus       = 0;
    entry->CurrClusOffset = 0;
    entry->FileSize       = 0;
    entry->EntrySec       = 0;
    entry->EntrySecOffset = 0;
    entry->Status         = 0;
}

//**************************************************************************************************************
//*                                               GET END CLUSTER
//*
//* Description: This function end cluster in the cluster chain of a cluster
//*
//* Arguments  : clus_no    Starting cluster of the cluster chain in which end cluster to be found
//*
//* Returns    : End cluster in the cluster chain
//*
//**************************************************************************************************************
u16  FAT_GetEndClus (u16  clus_no) {
    u16  next_clus;

    next_clus = clus_no;
    while (next_clus) {
        next_clus = FAT_GetNextClus(clus_no);
        if (next_clus) {
            clus_no = next_clus;
        }
    }
    return (clus_no);
}
