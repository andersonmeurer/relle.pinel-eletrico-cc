#ifndef __USBHOST_H
#define __USBHOST_H

#include "_config_cpu_.h"

// ########################################################################################
// DEFINIÇÕES DE TIPOS
// ########################################################################################
typedef struct hcEd { /* ----------- HostController EndPoint Descriptor ------------- */
	volatile u32 Control; /* Endpoint descriptor control                              */
	volatile u32 TailTd; /* Physical address of tail in Transfer descriptor list     */
	volatile u32 HeadTd; /* Physcial address of head in Transfer descriptor list     */
	volatile u32 Next; /* Physical address of next Endpoint descriptor             */
} HCED;

typedef struct hcTd { /* ------------ HostController Transfer Descriptor ------------ */
	volatile u32 Control; /* Transfer descriptor control                              */
	volatile u32 CurrBufPtr; /* Physical address of current buffer pointer               */
	volatile u32 Next; /* Physical pointer to next Transfer Descriptor             */
	volatile u32 BufEnd; /* Physical address of end of buffer                        */
} HCTD;

typedef struct hcca { /* ----------- Host Controller Communication Area ------------  */
	volatile u32 IntTable[32]; /* Interrupt Table                                          */
	volatile u32 FrameNumber; /* Frame Number                                             */
	volatile u32 DoneHead; /* Done Head                                                */
	volatile u8 Reserved[116]; /* Reserved for future use                                  */
	volatile u8 Unknown[4]; /* Unused                                                   */
} HCCA;


// ########################################################################################
// TRANSFER DESCRIPTOR CONTROL FIELDS
// ########################################################################################

#define  USB_TD_ROUNDING        (u32)(0x00040000)        /* Buffer Rounding                             */
#define  USB_TD_SETUP           (u32)(0)                  /* Direction of Setup Packet                   */
#define  USB_TD_IN              (u32)(0x00100000)         /* Direction In                                */
#define  USB_TD_OUT             (u32)(0x00080000)         /* Direction Out                               */
#define  USB_TD_DELAY_INT(x)    (u32)((x) << 21)          /* Delay Interrupt                             */
#define  USB_TD_TOGGLE_0        (u32)(0x02000000)         /* Toggle 0                                    */
#define  USB_TD_TOGGLE_1        (u32)(0x03000000)         /* Toggle 1                                    */
#define  USB_TD_CC              (u32)(0xF0000000)         /* Completion Code                             */


// ########################################################################################
// DEFINIÇÃO PADRÃO DE REQUISITO DE COMUNICAÇÃO
// ########################################################################################

#define  USB_DESCRIPTOR_TYPE_DEVICE                     1
#define  USB_DESCRIPTOR_TYPE_CONFIGURATION              2
#define  USB_DESCRIPTOR_TYPE_STRING                     3
#define  USB_DESCRIPTOR_TYPE_INTERFACE                  4
#define  USB_DESCRIPTOR_TYPE_ENDPOINT                   5
                                                    /*  ----------- Control RequestType Fields  ----------- */
#define  USB_DEVICE_TO_HOST         0x80
#define  USB_HOST_TO_DEVICE         0x00
#define  USB_REQUEST_TYPE_CLASS     0x20
#define  USB_RECIPIENT_DEVICE       0x00
#define  USB_RECIPIENT_INTERFACE    0x01

/* -------------- USB Standard Requests  -------------- */
#define  USB_SET_ADDRESS                 5
#define  USB_GET_DESCRIPTOR              6
#define  USB_SET_CONFIGURATION           9
#define  USB_SET_INTERFACE              11


// ########################################################################################
// FUNÇÕES
// ########################################################################################
int usbHost_Init (void);
int usbHost_EnumDev (void);
int usbHost_ProcessTD (
	volatile  HCED *ed,
	volatile  u32  token,
	volatile  u8 *buffer,
	u32  buffer_len
);
int usbHost_CtrlRecv (
	u8 bm_request_type,
    u8 b_request,
    u16   w_value,
    u16   w_index,
    u16   w_length,
    volatile  u8  *buffer
);
int usbHost_CtrlSend (
	u8 bm_request_type,
    u8 b_request,
    u16 w_value,
    u16 w_index,
    u16 w_length,
    volatile  u8  *buffer
);
void usbHost_FillSetup (
	u8 bm_request_type,
    u8 b_request,
    u16 w_value,
    u16 w_index,
    u16 w_length
);

void usbHost_WDHWait (void);


#define  USB_HOST_GET_DESCRIPTOR(descType, descIndex, data, length)                      	\
         usbHost_CtrlRecv(USB_DEVICE_TO_HOST | USB_RECIPIENT_DEVICE, USB_GET_DESCRIPTOR,    \
         (descType << 8)|(descIndex), 0, length, data)

#define  USB_HOST_SET_ADDRESS(new_addr)                                                  	\
         usbHost_CtrlSend(USB_HOST_TO_DEVICE | USB_RECIPIENT_DEVICE, USB_SET_ADDRESS,       \
         new_addr, 0, 0, NULL)

#define  USBH_SET_CONFIGURATION(configNum)                                           		\
         usbHost_CtrlSend(USB_HOST_TO_DEVICE | USB_RECIPIENT_DEVICE, USB_SET_CONFIGURATION, \
         configNum, 0, 0, NULL)

#define  USBH_SET_INTERFACE(ifNum, altNum)                                           		\
         usbHost_CtrlSend(USB_HOST_TO_DEVICE | USB_RECIPIENT_INTERFACE, USB_SET_INTERFACE,  \
         altNum, ifNum, 0, NULL)

#endif
