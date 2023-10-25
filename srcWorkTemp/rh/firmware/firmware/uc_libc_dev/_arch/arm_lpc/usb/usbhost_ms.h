#ifndef __USBHOST_MS_H
#define __USBHOST_MS_H

#include "_config_cpu_.h"

#define USB_INQUIRY_LENGTH                36

// MASS STORAGE SPECIFIC DEFINITIONS
#define USB_MS_GET_MAX_LUN_REQ          0xFE
#define USB_MS_STORAGE_CLASS            0x08
#define USB_MS_STORAGE_SUBCLASS_SCSI    0x06
#define USB_MS_STORAGE_PROTOCOL_BO      0x50

// SCSI SPECIFIC DEFINITIONS
#define  USB_CBW_SIGNATURE               0x43425355
#define  USB_CSW_SIGNATURE               0x53425355
#define  USB_CBW_SIZE                      31
#define  USB_CSW_SIZE                      13
#define  USB_CSW_CMD_PASSED              0x00
#define  USB_SCSI_CMD_REQUEST_SENSE      0x03
#define  USB_SCSI_CMD_TEST_UNIT_READY    0x00
#define  USB_SCSI_CMD_INQUIRY            0x12
#define  USB_SCSI_CMD_READ_10            0x28
#define  USB_SCSI_CMD_READ_CAPACITY      0x25
#define  USB_SCSI_CMD_WRITE_10           0x2A


typedef enum  ms_data_dir {
    MS_DATA_DIR_IN     = 0x80,
    MS_DATA_DIR_OUT    = 0x00,
    MS_DATA_DIR_NONE   = 0x01
} USB_MS_DATA_DIR;


int usbHost_MS_Init (u32 *blkSize, u32 *numBlks, u8 *inquiryResult);
int usbHost_MS_ParseConfiguration (void);
int usbHost_MS_GetMaxLUN (void);
int usbHost_MS_TestUnitReady (void);
int usbHost_MS_GetSenseInfo (void);
int usbHost_MS_ReadCapacity (u32 *numBlks, u32 *blkSize);
int usbHost_MS_Inquire (u8 *response);
int usbHost_MS_BulkRecv (
	u32   block_number,
    u16   num_blocks,
    volatile  u8  *user_buffer
);
int  usbHost_MS_BulkSend (
	u32 block_number,
    u16 num_blocks,
    volatile u8 *user_buffer
);

#endif
