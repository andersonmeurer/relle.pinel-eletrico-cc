#include  "usbhost.h"
#include  "usbhost_ms.h"
#include "utils.h"

#if (USB_MS_USE_DEBUG	== pdON)
#include"stdio_uc.h"
#endif

static u32 usb_MS_BlkSize;

extern volatile  u8  *TDBuffer;                  /* Current Buffer Pointer of transfer descriptor          */
extern volatile  HCED *EDBulkOut;                 /* BulkOut endpoint descriptor structure                  */
extern volatile  HCED *EDBulkIn;                  /* BulkIn endpoint descriptor  structure                  */

static void  usbHost_MS_fillCommand (
	u32   block_number,
	u32   block_size,
    u16   num_blocks,
    USB_MS_DATA_DIR  direction,
    u8   scsi_cmd,
    u8   scsi_cmd_len
);

//                                      INITIALIZE MASS STORAGE INTERFACE
//
// Description: This function initializes the mass storage interface
//
// Arguments  : None
//
// Returns    : OK		              if Success
//              ERR_INVALID_BOOTSIG    if Failed
//
int usbHost_MS_Init (u32 *blkSize, u32 *numBlks, u8 *inquiryResult) {
    u8  retry;
    int  rc;

    usbHost_MS_GetMaxLUN();                                                    // Get maximum logical unit number
    retry = 80;
    while(retry) {
        rc = usbHost_MS_TestUnitReady();                                       // Test whether the unit is ready
        if (rc == pdPASS) {
            break;
        }
        usbHost_MS_GetSenseInfo();                                             // Get sense information
        retry--;
    }

    #if (USB_MS_USE_DEBUG	== pdON)
    plog("USB MS: Init retry [%u]"CMD_TERMINATOR, retry);
	#endif

    if (rc != pdPASS) {
		#if (USB_MS_USE_DEBUG	== pdON)
    	plog("USB MS: Init error [%d]"CMD_TERMINATOR, rc);
		#endif
        return (rc);
    }

    rc = usbHost_MS_ReadCapacity(numBlks, blkSize);                         // Read capacity of the disk
    usb_MS_BlkSize = *blkSize;						// Set global
    rc = usbHost_MS_Inquire (inquiryResult);

    #if (USB_MS_USE_DEBUG	== pdON)
    plog("USB MS: init OK [%u][%d]"CMD_TERMINATOR, usb_MS_BlkSize, rc);
	#endif

    return rc;
}



//                                         PARSE THE CONFIGURATION
//
// Description: This function is used to parse the configuration
//
// Arguments  : None
//
// Returns    : OK		              if Success
//              ERR_INVALID_BOOTSIG    if Failed
//

int usbHost_MS_ParseConfiguration (void) {
    volatile u8 *desc_ptr;
    u8 ms_int_found;

    desc_ptr = TDBuffer;
    ms_int_found = 0;

    if (desc_ptr[1] != USB_DESCRIPTOR_TYPE_CONFIGURATION) {    
        return (errUSB_MS_BAD_CONFIGURATION);
    }

    desc_ptr += desc_ptr[0];

    while (desc_ptr != TDBuffer + read_LE16u(&TDBuffer[2])) {
        switch (desc_ptr[1]) {

        case USB_DESCRIPTOR_TYPE_INTERFACE:                       // If it is an interface descriptor
        	if (desc_ptr[5] == USB_MS_STORAGE_CLASS &&             // check if the class is mass storage
        		desc_ptr[6] == USB_MS_STORAGE_SUBCLASS_SCSI &&     // check if the subclass is SCSI
                desc_ptr[7] == USB_MS_STORAGE_PROTOCOL_BO)			// check if the protocol is Bulk only
        	{
                ms_int_found = 1;
                desc_ptr += desc_ptr[0];                      // Move to next descriptor start
            }
            break;

        case USB_DESCRIPTOR_TYPE_ENDPOINT:                        // If it is an endpoint descriptor
        	if ((desc_ptr[3] & 0x03) == 0x02) {                  // If it is Bulk endpoint
        		if (desc_ptr[2] & 0x80) {                        // If it is In endpoint
        			EDBulkIn->Control =  1                |      // USB address
        					((desc_ptr[2] & 0x7F) << 7)   |      // Endpoint address
                            (2 << 11)                     |      // direction
                            (read_LE16u(&desc_ptr[4]) << 16);     // MaxPkt Size
                    desc_ptr += desc_ptr[0];                     // Move to next descriptor start
        		} else {                                         // If it is Out endpoint
        			EDBulkOut->Control = 1                |      // USB address
        					((desc_ptr[2] & 0x7F) << 7)   |      // Endpoint address
                            (1 << 11)                     |      // direction
                            (read_LE16u(&desc_ptr[4]) << 16);     // MaxPkt Size
        			desc_ptr += desc_ptr[0];                     // Move to next descriptor start
                 }
        	} else {                                             // If it is not bulk end point
        		desc_ptr += desc_ptr[0];                         // Move to next descriptor start
            }
            break;

        default:                                 // If the descriptor is neither interface nor endpoint
        	desc_ptr += desc_ptr[0];                             // Move to next descriptor start
            break;
        }
    }

    if (ms_int_found) {
		#if (USB_MS_USE_DEBUG	== pdON)
    	plog("USB MS: Mass Storage device connected"CMD_TERMINATOR);
		#endif

        return pdPASS;
    } else {
		#if (USB_MS_USE_DEBUG	== pdON)
    	plog("USB MS: Not a Mass Storage device"CMD_TERMINATOR);
		#endif
        return errUSB_MS_NO_INTERFACE;
    }
}

//                                         GET MAXIMUM LOGICAL UNIT
//
// Description: This function returns the maximum logical unit from the device
//
// Arguments  : None
//
// Returns    : OK		              if Success
//              ERR_INVALID_BOOTSIG    if Failed

int  usbHost_MS_GetMaxLUN (void) {
	int  rc;

    rc = usbHost_CtrlRecv(
    		USB_DEVICE_TO_HOST | USB_REQUEST_TYPE_CLASS | USB_RECIPIENT_INTERFACE,
    		USB_MS_GET_MAX_LUN_REQ,
            0,
            0,
            1,
            TDBuffer
    );
    return rc;
}


//                                          GET SENSE INFORMATION
//
// Description: This function is used to get sense information from the device
//
// Arguments  : None
//
// Returns    : OK       if Success
//              ERROR    if Failed
//
int  usbHost_MS_GetSenseInfo (void) {
	int  rc;
	#if (USB_MS_USE_DEBUG	== pdON)
	plog("USB MS: GetSenseInfo"CMD_TERMINATOR);
	#endif

    usbHost_MS_fillCommand(0, 0, 0, MS_DATA_DIR_IN, USB_SCSI_CMD_REQUEST_SENSE, 6);
    rc = usbHost_ProcessTD(EDBulkOut, USB_TD_OUT, TDBuffer, USB_CBW_SIZE);
    if (rc == pdPASS) {
        rc = usbHost_ProcessTD(EDBulkIn, USB_TD_IN, TDBuffer, 18);
        if (rc == pdPASS) {
            rc = usbHost_ProcessTD(EDBulkIn, USB_TD_IN, TDBuffer, USB_CSW_SIZE);
            if (rc == pdPASS) {
                if (TDBuffer[12] != 0) {
                    rc = errUSB_MS_CMD_FAILED;
                }
            }
        }
    }
    return rc;
}

//                                           TEST UNIT READY
//
// Description: This function is used to test whether the unit is ready or not
//
// Arguments  : None
//
// Returns    : OK       if Success
//              ERROR    if Failed
//

int usbHost_MS_TestUnitReady (void) {
	int  rc;

	#if (USB_MS_USE_DEBUG	== pdON)
	plog("USB MS: TestUnitReady"CMD_TERMINATOR);
	#endif

	usbHost_MS_fillCommand(0, 0, 0, MS_DATA_DIR_NONE, USB_SCSI_CMD_TEST_UNIT_READY, 6);
    rc = usbHost_ProcessTD(EDBulkOut, USB_TD_OUT, TDBuffer, USB_CBW_SIZE);
    if (rc == pdPASS) {
        rc = usbHost_ProcessTD(EDBulkIn, USB_TD_IN, TDBuffer, USB_CSW_SIZE);
        if (rc == pdPASS) {
            if (TDBuffer[12] != 0) {
                rc = errUSB_MS_CMD_FAILED;
            }
        }
    }
    return rc;
}


//                                            READ CAPACITY

// Description: This function is used to read the capacity of the mass storage device
//
// Arguments  : None
//
// Returns    : OK       if Success
//              ERROR    if Failed
int usbHost_MS_ReadCapacity (u32 *numBlks, u32 *blkSize) {
	int  rc;

	#if (USB_MS_USE_DEBUG	== pdON)
	plog("USB MS: ReadCapacity"CMD_TERMINATOR);
	#endif

	usbHost_MS_fillCommand(0, 0, 0, MS_DATA_DIR_IN, USB_SCSI_CMD_READ_CAPACITY, 10);
    rc = usbHost_ProcessTD(EDBulkOut, USB_TD_OUT, TDBuffer, USB_CBW_SIZE);
    if (rc == pdPASS) {
        rc = usbHost_ProcessTD(EDBulkIn, USB_TD_IN, TDBuffer, 8);
        if (rc == pdPASS) {
            if (numBlks)
            	*numBlks = read_BE32u(&TDBuffer[0]);
            if (blkSize)
            	*blkSize = read_BE32u(&TDBuffer[4]);
            rc = usbHost_ProcessTD(EDBulkIn, USB_TD_IN, TDBuffer, USB_CSW_SIZE);
            if (rc == pdPASS) {
                if (TDBuffer[12] != 0) {
                    rc = errUSB_MS_CMD_FAILED;
                }
            }
        }
    }
    return rc;
}

int usbHost_MS_Inquire (u8 *response) {
    int rc;
	u32 i;

	#if (USB_MS_USE_DEBUG	== pdON)
	plog("USB MS: Inquire"CMD_TERMINATOR);
	#endif

    usbHost_MS_fillCommand(0, 0, 0, MS_DATA_DIR_IN, USB_SCSI_CMD_INQUIRY, 6);
    rc = usbHost_ProcessTD(EDBulkOut, USB_TD_OUT, TDBuffer, USB_CBW_SIZE);
    if (rc == pdPASS) {
        rc = usbHost_ProcessTD(EDBulkIn, USB_TD_IN, TDBuffer, USB_INQUIRY_LENGTH);
        if (rc == pdPASS) {
            if (response) {
				for ( i = 0; i < USB_INQUIRY_LENGTH; i++ )
					*response++ = *TDBuffer++;
#if 0
            	MemCpy (response, TDBuffer, USB_INQUIRY_LENGTH);
	        	StrNullTrailingSpace (response->vendorID, SCSI_INQUIRY_VENDORCHARS);
	        	StrNullTrailingSpace (response->productID, SCSI_INQUIRY_PRODUCTCHARS);
	        	StrNullTrailingSpace (response->productRev, SCSI_INQUIRY_REVCHARS);
#endif
            }
            rc = usbHost_ProcessTD(EDBulkIn, USB_TD_IN, TDBuffer, USB_CSW_SIZE);
            if (rc == pdPASS) {
                if (TDBuffer[12] != 0) {	// bCSWStatus byte
                    rc = errUSB_MS_CMD_FAILED;
                }
            }
        }
    }
	#if (USB_MS_USE_DEBUG	== pdON)
    plog("USB MS: inquire [%d]"CMD_TERMINATOR, rc);
	#endif

    return (rc);
}

//                                         RECEIVE THE BULK DATA
//
// Description: This function is used to receive the bulk data
//
// Arguments  : None
//
// Returns    : OK		              if Success
//              ERR_INVALID_BOOTSIG    if Failed
int usbHost_MS_BulkRecv (
	u32   block_number,
    u16   num_blocks,
    volatile u8 *user_buffer
) {
	int rc;
	unsigned int i;
    volatile u8 *c = user_buffer;
    for (i=0; i<usb_MS_BlkSize*num_blocks; i++)
    	*c++ = 0;

    usbHost_MS_fillCommand(block_number, usb_MS_BlkSize, num_blocks, MS_DATA_DIR_IN, USB_SCSI_CMD_READ_10, 10);

    rc = usbHost_ProcessTD(EDBulkOut, USB_TD_OUT, TDBuffer, USB_CBW_SIZE);
    if (rc == pdPASS) {
        rc = usbHost_ProcessTD(EDBulkIn, USB_TD_IN, user_buffer, usb_MS_BlkSize * num_blocks);
        if (rc == pdPASS) {
            rc = usbHost_ProcessTD(EDBulkIn, USB_TD_IN, TDBuffer, USB_CSW_SIZE);
            if (rc == pdPASS) {
                if (TDBuffer[12] != 0) {
                    rc = errUSB_MS_CMD_FAILED;
                }
            }
        }
    }

	#if (USB_MS_USE_DEBUG	== pdON)
    plog("USB MS: BulkRecv [%u][%u][0x%x][%d]"CMD_TERMINATOR, block_number, num_blocks, user_buffer, rc);
	#endif

    return rc;
}


//                                         SEND BULK DATA
//
// Description: This function is used to send the bulk data
//
// Arguments  : None
//
// Returns    : OK		              if Success
//              ERR_INVALID_BOOTSIG    if Failed
int  usbHost_MS_BulkSend (
	u32 block_number,
    u16 num_blocks,
    volatile u8 *user_buffer
) {
	int  rc;

	usbHost_MS_fillCommand(block_number, usb_MS_BlkSize, num_blocks, MS_DATA_DIR_OUT, USB_SCSI_CMD_WRITE_10, 10);

    rc = usbHost_ProcessTD(EDBulkOut, USB_TD_OUT, TDBuffer, USB_CBW_SIZE);
    if (rc == pdPASS) {
        rc = usbHost_ProcessTD(EDBulkOut, USB_TD_OUT, user_buffer, usb_MS_BlkSize * num_blocks);
        if (rc == pdPASS) {
            rc = usbHost_ProcessTD(EDBulkIn, USB_TD_IN, TDBuffer, USB_CSW_SIZE);
            if (rc == pdPASS) {
                if (TDBuffer[12] != 0) {
                    rc = errUSB_MS_CMD_FAILED;
                }
            }
        }
    }

	#if (USB_MS_USE_DEBUG	== pdON)
    plog("USB MS: BulkSend [%u][%u][0x%x][%d]"CMD_TERMINATOR, block_number, num_blocks, user_buffer, rc);
	#endif

    return rc;
}

//                                         FILL MASS STORAGE COMMAND
//
// Description: This function is used to fill the mass storage command
//
// Arguments  : None
//
// Returns    : OK		              if Success
//              ERR_INVALID_BOOTSIG    if Failed
void usbHost_MS_fillCommand (
	u32   block_number,
    u32   block_size,
    u16   num_blocks,
    USB_MS_DATA_DIR  direction,
    u8   scsi_cmd,
    u8   scsi_cmd_len
) {
   u32 data_len;
   static u32 tag_cnt = 0;
   u32 cnt;

   for (cnt = 0; cnt < USB_CBW_SIZE; cnt++) {
	   TDBuffer[cnt] = 0;
   }

   switch(scsi_cmd) {
   case USB_SCSI_CMD_TEST_UNIT_READY:
       data_len = 0;
       break;
   case USB_SCSI_CMD_READ_CAPACITY:
       data_len = 8;
       break;
   case USB_SCSI_CMD_REQUEST_SENSE:
       data_len = 18;
       break;
   case USB_SCSI_CMD_INQUIRY:
       data_len = 36;
       break;
   default:
	   data_len = block_size * num_blocks;
       break;
   }

   write_LE32u(TDBuffer, USB_CBW_SIGNATURE);
   write_LE32u(&TDBuffer[4], tag_cnt);
   write_LE32u(&TDBuffer[8], data_len);

   TDBuffer[12] = (direction == MS_DATA_DIR_NONE) ? 0 : direction;
   TDBuffer[14] = scsi_cmd_len;                                   // Length of the CBW
   TDBuffer[15] = scsi_cmd;

   if ((scsi_cmd == USB_SCSI_CMD_REQUEST_SENSE) || (scsi_cmd == USB_SCSI_CMD_INQUIRY)) {
	   TDBuffer[19] = (u8)data_len;
   } else {
	   write_BE32u(&TDBuffer[17], block_number);
   }

   write_BE16u(&TDBuffer[22], num_blocks);
}

