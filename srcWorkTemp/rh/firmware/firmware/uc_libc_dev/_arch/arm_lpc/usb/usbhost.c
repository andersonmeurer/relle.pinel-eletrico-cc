#include "usbhost.h"
#include "_config_cpu_usb.h"
#include "vic.h"
#include "utils.h"
#if defined(cortexm3)
#include "arm.h"
#endif
#if defined(FREE_RTOS)
#include "sys_time.h"
#else
#include "delay.h"
#endif

#if (USB_USE_DEBUG == pdON)
#include"stdio_uc.h"
#endif

extern int usbHost_MS_ParseConfiguration (void);

static int gUSBConnected;
volatile u32 HOST_RhscIntr = 0;         	// Root Hub Status Change interrupt. Sinaliza qual dispositivo está conectado
volatile u32 HOST_WdhIntr  = 0;         	// Semaphore to wait until the TD is submitted
volatile u8  HOST_TDControlStatus = 0;
volatile HCED *EDCtrl;                    // Control endpoint descriptor structure
volatile HCED *EDBulkIn;                  // BulkIn endpoint descriptor  structure
volatile HCED *EDBulkOut;                 // BulkOut endpoint descriptor structure
volatile HCTD *TDHead;                    // Head transfer descriptor structure
volatile HCTD *TDTail;                    // Tail transfer descriptor structure
volatile HCCA *Hcca;                      // Host Controller Communications Area structure
volatile u8  *TDBuffer;                  // Current Buffer Pointer of transfer descriptor
volatile u8  *FATBuffer;                 // Buffer used by FAT file system
volatile u8  *UserBuffer;                // Buffer used by application

static void  usbHost_HCCAInit (volatile  HCCA  *hcca);
static void usbHost_EDInit (volatile HCED *ed);
static void usbHost_TDInit (volatile  HCTD *td);
void usbHost_ISR (void);

int usbHost_Init (void) {
	// Liga a USB no controle de potencia.
	//	Tem que ser antes da configuração, pois após ligar o periféricos,
	//	seus regs assumem valores padrões
	PCONP |= PCUSB;

	// Habilitar o clock para acessar os registradores de controle.
	// Este procedimento não preciso ser repetido toda vez que o software for
	//	acessar os registradores, isto porque o registrador USBClkCtrl fica inalterado
	// Configura os pinos do ARM para a função USBD1 e USBD2
	#if defined(arm7tdmi)
	// TODO - USB valor 0x8 (não funcionou no lpc2468), valor 0x1 funcionou (testado por + tempo no lpc2468) ou o valor 0x9
	USBClkCtrl = 0x00000009;
	// Quando habilitando clock com USBClkCtrl, devemos monitorar o bit correspondente no
	//  USBClkSt, quando o bit for 1 os registradores podem se acessados
	while ((USBClkSt & 0x00000009) == 0);

	OTGStCtrl = USB_PORT_FUNC;

	USB1_PINSEL = (USB1_PINSEL & ~USB1_PINMASK) | USB1_PINS;
	USB2_PINSEL = (USB2_PINSEL & ~USB2_PINMASK) | USB2_PINS;
	#if (USB_USE_DEBUG == pdON)
	plog("USB1: PINSEL [0x%x] 0x%x"CMD_TERMINATOR, &USB1_PINSEL, USB1_PINSEL);
	plog("USB2: PINSEL [0x%x] 0x%x"CMD_TERMINATOR, &USB2_PINSEL, USB1_PINSEL);
	#endif

	#endif

	#if defined(cortexm3)
	USBClkCtrl = 0x0000001f; // USBClkCtrl = 0x00000009;
	// Quando habilitando clock com USBClkCtrl, devemos monitorar o bit correspondente no
	//  USBClkSt, quando o bit for 1 os registradores podem se acessados
	while ((USBClkSt & 0x0000001f) == 0);//while ((USBClkSt & 0x00000001) == 0);

	OTGStCtrl = USB_PORT_FUNC;

	pinsel_cfg_t pinCFG;
	pinCFG.addrPort = &USB1_PINSEL; // Endereço da porta USB
	pinCFG.numPin = USB1_DP_NUMPIN;	// Número do pino do ARM para o D+
	pinCFG.nunFunc = USB1_NUNFUNC;	// Ajustado a função do pino para USB
	pinCFG.openDrain = 0;			// Dreno aberto desligado
	pinCFG.pinMode = 0;				// Sem resistor de pull
	pinsel_configPin(&pinCFG);		// Aplica configurações
	pinCFG.numPin = USB1_DM_NUMPIN;	// Número do pino do ARM para o D-
	pinsel_configPin(&pinCFG);		// Aplica configurações
	#endif

    // Variáveis de controle utilizando os 16KB da SRAM interna do ARM dedicado para USB
	Hcca       = (volatile  HCCA *)(USB_RAM_BASE+0x000);
	TDHead     = (volatile  HCTD *)(USB_RAM_BASE+0x100);
    TDTail     = (volatile  HCTD *)(USB_RAM_BASE+0x110);
    EDCtrl     = (volatile  HCED *)(USB_RAM_BASE+0x120);
    EDBulkIn   = (volatile  HCED *)(USB_RAM_BASE+0x130);
    EDBulkOut  = (volatile  HCED *)(USB_RAM_BASE+0x140);
    TDBuffer   = (volatile  u8 *)(USB_RAM_BASE+0x150);
    FATBuffer  = (volatile  u8 *)(USB_RAM_BASE+0x1D0);
    UserBuffer = (volatile  u8 *)(USB_RAM_BASE+0x1000);

    // Initialize all the TDs, EDs and HCCA to 0
    usbHost_EDInit(EDCtrl);
    usbHost_EDInit(EDBulkIn);
    usbHost_EDInit(EDBulkOut);
    usbHost_TDInit(TDHead);
    usbHost_TDInit(TDTail);
    usbHost_HCCAInit(Hcca);

    delay_ms(50);           // Wait 50 ms before apply reset
    HcControl = 0;      	// HARDWARE RESET
    HcControlHeadED = 0;    // Initialize Control list head to Zero
    HcBulkHeadED = 0;    // Initialize Bulk list head to Zero

    // SOFTWARE RESET
    HcCommandStatus = HC_OR_CMD_STATUS_HCR;
    HcFmInterval = USB_DEFAULT_FMINTERVAL; // Write Fm Interval and Largest Data Packet Counter
    // Put HC in operational state
    HcControl  = (HcControl & (~HC_OR_CONTROL_HCFS)) | HC_OR_CONTROL_HC_OPER;
    HcRhStatus = HC_OR_RH_STATUS_LPSC; // Set Global Power

    HcHCCA = (u32)Hcca;
    HcInterruptStatus 	|= HcInterruptStatus; // Clear Interrrupt Status

    // Enable  interrupts
    HcInterruptEnable  =
    		HC_OR_INTR_ENABLE_MIE |
    		HC_OR_INTR_ENABLE_WDH |
    		HC_OR_INTR_ENABLE_RHSC;

	#if defined (arm7tdmi)
    // Instalar o serviço de interrupção nos vetores de interrupções
    if (!irq_Install(VIC_USB, usbHost_ISR, USB_PRIO_LEVEL))
    	return errIRQ_INSTALL;
    #endif

    #if defined (cortexm3)
    /* Enable the USB Interrupt */
	nvic_enableIRQ(VIC_USB);               /* enable USB interrupt */
	nvic_setPriority (VIC_USB, USB_PRIO_LEVEL);			/* highest priority */
	#endif

	#if (USB_USE_LED ==	pdON)
    USB_LED_DEVCNT1_DIR |= USB_LED_DEVCNT1; // Colocar o pino do led para saida
    USB_LED_DEVCNT1_OFF = USB_LED_DEVCNT1;
	#endif

    #if (USB_USE_DEBUG == pdON)
    plog("USB: INIT OK"CMD_TERMINATOR);
	#endif

    return pdPASS;
}




/*
**************************************************************************************************************
*                                     PROCESS TRANSFER DESCRIPTOR
*
* Description: This function processes the transfer descriptor
*
* Arguments  : ed            Endpoint descriptor that contains this transfer descriptor
*              token         SETUP, IN, OUT
*              buffer        Current Buffer Pointer of the transfer descriptor
*              buffer_len    Length of the buffer
*
* Returns    : OK       if TD submission is successful
*              ERROR    if TD submission fails
*
**************************************************************************************************************
*/

// ret
//pdPASS;
//errUSB_TD_FAIL;

int usbHost_ProcessTD (
	volatile  HCED *ed,
    volatile  u32  token,
    volatile  u8 *buffer,
    u32 buffer_len)
{
	volatile u32 td_toggle;

    if (ed == EDCtrl) {
        if (token == USB_TD_SETUP) {
            td_toggle = USB_TD_TOGGLE_0;
        } else {
            td_toggle = USB_TD_TOGGLE_1;
        }
    } else {
        td_toggle = 0;
    }

    TDHead->Control 	= ( USB_TD_ROUNDING | token | USB_TD_DELAY_INT(0) | td_toggle | USB_TD_CC );
    TDTail->Control 	= 0;
    TDHead->CurrBufPtr  = (u32) buffer;
    TDTail->CurrBufPtr  = 0;
    TDHead->Next        = (u32) TDTail;
    TDTail->Next        = 0;
    TDHead->BufEnd      = (u32)(buffer + (buffer_len - 1));
    TDTail->BufEnd      = 0;

    ed->HeadTd  = (u32)TDHead | ((ed->HeadTd) & 0x00000002);
    ed->TailTd  = (u32)TDTail;
    ed->Next    = 0;

    if (ed == EDCtrl) {
    	HcControlHeadED = (u32)ed;
    	HcCommandStatus |= HC_OR_CMD_STATUS_CLF;
        HcControl 	|= HC_OR_CONTROL_CLE;
    } else {
    	HcBulkHeadED	= (u32)ed;
    	HcCommandStatus 	|= HC_OR_CMD_STATUS_BLF;
        HcControl  	|= HC_OR_CONTROL_BLE;
    }

    usbHost_WDHWait();

    if (!HOST_TDControlStatus)
        	return pdPASS;
    else  	return errUSB_TD_FAIL;
}


/*
**************************************************************************************************************
*                                       ENUMERATE THE DEVICE
*
* Description: This function is used to enumerate the device connected
*
* Arguments  : None
*
* Returns    : None
*
**************************************************************************************************************
*/
int usbHost_EnumDev (void) {
	int rc;

	#if (USB_USE_DEBUG == pdON)
	plog("USB: Connect a Mass Storage device"CMD_TERMINATOR);
	#endif

	// Se o dispositivo já foi enumerado não há necessidade de refaze-lo
	if (!HOST_RhscIntr) return errUSB_DEVICE_NO_PRESENT;

    delay_ms(100); // USB 2.0 spec says atleast 50ms delay before port reset

//	if ( HOST_RhscIntr & 0x01 ) {
//	  HC_RH_PORT_STAT1 = HC_OR_RH_PORT_PRS; // Initiate port reset
//	  while (HC_RH_PORT_STAT1 & HC_OR_RH_PORT_PRS)
//		; // Wait for port reset to complete...
//	  HC_RH_PORT_STAT1 = HC_OR_RH_PORT_PRSC; // ...and clear port reset signal
//		#if (USB_USE_DEBUG == pdON)
//	  	plog("USB: Connected in USB1"CMD_TERMINATOR);
//		#endif
//
//	}
//
	//if ( HOST_RhscIntr & 0x02 )	{
    HcRhPortStatus = HC_OR_RH_PORT_PRS; // Initiate port reset
	  while (HcRhPortStatus & HC_OR_RH_PORT_PRS)
		; // Wait for port reset to complete...
	  HcRhPortStatus = HC_OR_RH_PORT_PRSC; // ...and clear port reset signal
		#if (USB_USE_DEBUG == pdON)
	  	  plog("USB: Connected in USB2"CMD_TERMINATOR);
		#endif

	//}
    delay_ms(200);                                                 // Wait for 100 MS after port reset

    EDCtrl->Control = 8 << 16;                                         // Put max pkt size = 8
                                                                       // Read first 8 bytes of device desc
    rc = USB_HOST_GET_DESCRIPTOR(USB_DESCRIPTOR_TYPE_DEVICE, 0, TDBuffer, 8);
    if (rc != pdPASS) {
		#if (USB_USE_DEBUG == pdON)
    	plog("USB: error [%d]"CMD_TERMINATOR, rc);
		#endif
        return rc;
    }

    EDCtrl->Control = TDBuffer[7] << 16;                               // Get max pkt size of endpoint 0
    rc = USB_HOST_SET_ADDRESS(1);                                          // Set the device address to 1
    if (rc != pdPASS) {
		#if (USB_USE_DEBUG == pdON)
    	plog("USB: error [%d]"CMD_TERMINATOR, rc);
		#endif
        return rc;
    }
    delay_ms(2);

    EDCtrl->Control = (EDCtrl->Control) | 1;                          // Modify control pipe with address 1
    // Get the configuration descriptor
    rc = USB_HOST_GET_DESCRIPTOR(USB_DESCRIPTOR_TYPE_CONFIGURATION, 0, TDBuffer, 9);
    if (rc != pdPASS) {
		#if (USB_USE_DEBUG == pdON)
    	plog("USB: error [%d]"CMD_TERMINATOR, rc);
		#endif
        return rc;
    }

    rc = USB_HOST_GET_DESCRIPTOR(USB_DESCRIPTOR_TYPE_CONFIGURATION, 0, TDBuffer, read_LE16u(&TDBuffer[2]));
    if (rc != pdPASS) {
		#if (USB_USE_DEBUG	== pdON)
    	plog("USB: error [%d]"CMD_TERMINATOR, rc);
		#endif
        return rc;
    }

    rc = usbHost_MS_ParseConfiguration();                                      // Parse the configuration
    if (rc != pdPASS) {
		#if (USB_USE_DEBUG	== pdON)
    	plog("USB: error [%d]"CMD_TERMINATOR, rc);
		#endif
        return rc;
    }

    rc = USBH_SET_CONFIGURATION(1);                                    // Select device configuration 1
    if (rc != pdPASS) {
		#if (USB_USE_DEBUG	== pdON)
    	plog("USB: error [%d]"CMD_TERMINATOR, rc);
		#endif
    }

    delay_ms(100);                                               // Some devices may require this delay
    return rc;
}


/*
**************************************************************************************************************
*                                        RECEIVE THE CONTROL INFORMATION
*
* Description: This function is used to receive the control information
*
* Arguments  : bm_request_type
*              b_request
*              w_value
*              w_index
*              w_length
*              buffer
*
* Returns    : OK       if Success
*              ERROR    if Failed
*
**************************************************************************************************************
*/

int usbHost_CtrlRecv (
	u8 bm_request_type,
    u8 b_request,
    u16   w_value,
    u16   w_index,
    u16   w_length,
    volatile  u8  *buffer
) {
	int  rc;
	(void)buffer; // Somente para instruir ao compilador de não emitir um aviso de não uso desta variável

    usbHost_FillSetup(bm_request_type, b_request, w_value, w_index, w_length);
    rc = usbHost_ProcessTD(EDCtrl, USB_TD_SETUP, TDBuffer, 8);
    if (rc == pdPASS) {
        if (w_length) {
            rc = usbHost_ProcessTD(EDCtrl, USB_TD_IN, TDBuffer, w_length);
        }
        if (rc == pdPASS) {
            rc = usbHost_ProcessTD(EDCtrl, USB_TD_OUT, NULL, 0);
        }
    }

    return rc;
}


/*
**************************************************************************************************************
*                                         SEND THE CONTROL INFORMATION
*
* Description: This function is used to send the control information
*
* Arguments  : None
*
* Returns    : OK		              if Success
*              ERR_INVALID_BOOTSIG    if Failed
*
**************************************************************************************************************
*/
int usbHost_CtrlSend (
	u8 bm_request_type,
    u8 b_request,
    u16 w_value,
    u16 w_index,
    u16 w_length,
    volatile  u8  *buffer
) {
    int  rc;
    (void)buffer; // Somente para instruir ao compilador de não emitir um aviso de não uso desta variável

    usbHost_FillSetup(bm_request_type, b_request, w_value, w_index, w_length);

    rc = usbHost_ProcessTD(EDCtrl, USB_TD_SETUP, TDBuffer, 8);
    if (rc == pdPASS) {
        if (w_length) {
            rc = usbHost_ProcessTD(EDCtrl, USB_TD_OUT, TDBuffer, w_length);
        }
        if (rc == pdPASS) {
            rc = usbHost_ProcessTD(EDCtrl, USB_TD_IN, NULL, 0);
        }
    }
    return rc;
}


/*
**************************************************************************************************************
*                                          FILL SETUP PACKET
*
* Description: This function is used to fill the setup packet
*
* Arguments  : None
*
* Returns    : OK		              if Success
*              ERR_INVALID_BOOTSIG    if Failed
*
**************************************************************************************************************
*/
void usbHost_FillSetup (
	u8 bm_request_type,
    u8 b_request,
    u16 w_value,
    u16 w_index,
    u16 w_length
) {
	int i;
	for (i=0;i<w_length;i++)
		TDBuffer[i] = 0;

    TDBuffer[0] = bm_request_type;
    TDBuffer[1] = b_request;
    write_LE16u(&TDBuffer[2], w_value);
    write_LE16u(&TDBuffer[4], w_index);
    write_LE16u(&TDBuffer[6], w_length);
}


/*
**************************************************************************************************************
*                                         INITIALIZE THE TRANSFER DESCRIPTOR
*
* Description: This function initializes transfer descriptor
*
* Arguments  : Pointer to TD structure
*
* Returns    : None
*
**************************************************************************************************************
*/

static void usbHost_TDInit (volatile  HCTD *td) {
    td->Control    = 0;
    td->CurrBufPtr = 0;
    td->Next       = 0;
    td->BufEnd     = 0;
}

/*
**************************************************************************************************************
*                                         INITIALIZE THE ENDPOINT DESCRIPTOR
*
* Description: This function initializes endpoint descriptor
*
* Arguments  : Pointer to ED strcuture
*
* Returns    : None
*
**************************************************************************************************************
*/

static void usbHost_EDInit (volatile HCED *ed) {
    ed->Control = 0;
    ed->TailTd  = 0;
    ed->HeadTd  = 0;
    ed->Next    = 0;
}

/*
**************************************************************************************************************
*                                 INITIALIZE HOST CONTROLLER COMMUNICATIONS AREA
*
* Description: This function initializes host controller communications area
*
* Arguments  : Pointer to HCCA
*
* Returns    :
*
**************************************************************************************************************
*/
static void usbHost_HCCAInit (volatile  HCCA  *hcca) {
    u32  i;
    for (i = 0; i < 32; i++) {
        hcca->IntTable[i] = 0;
        hcca->FrameNumber = 0;
        hcca->DoneHead    = 0;
    }
}



/*
**************************************************************************************************************
*                                         WAIT FOR WDH INTERRUPT
*
* Description: This function is infinite loop which breaks when ever a WDH interrupt rises
*
* Arguments  : None
*
* Returns    : None
*
**************************************************************************************************************
*/
void usbHost_WDHWait (void) {
	while (!HOST_WdhIntr);
	HOST_WdhIntr = 0;
}



//###################################################################################################################
// USB SERVIÇO DE INTERRUPÇÕES
// 		Descrição: 	Esta função implementa o ISR da USB1
//###################################################################################################################
//###################################################################################################################
void usbHost_ISR (void) {
	ISR_ENTRY;																// Procedimento para entrada da interrupção

	u32 int_status;
	u32 ie_status;

	int_status = HcInterruptStatus; 	//Read Interrupt Status
	ie_status = HcInterruptEnable; 		//Read Interrupt enable status

	if (!(int_status & ie_status)) return;
	int_status = int_status & ie_status;

	if (int_status & HC_OR_INTR_STATUS_RHSC) { //Root hub status change interrupt
		if (HcRhPortStatus & HC_OR_RH_PORT_CSC) {
			if (HcRhStatus & HC_OR_RH_STATUS_DRWE) {
				// When DRWE is on, Connect Status Change means a remote wakeup event.
				HOST_RhscIntr = 1;
			} else {
				// When DRWE is off, Connect Status Change is NOT a remote wakeup event
    			if (HcRhPortStatus & HC_OR_RH_PORT_CCS) {
    				if (!gUSBConnected) {
    					HOST_TDControlStatus = 0;
    					HOST_WdhIntr = 0;
    					HOST_RhscIntr = 1;
    					gUSBConnected = 1;
    				}
					#if (USB_USE_DEBUG == pdON)
    				else plog("USB: Spurious status change (connected)?"CMD_TERMINATOR);
    				#endif
    			} else {
    				if (gUSBConnected) {
    					HcInterruptEnable = 0; // why do we get multiple disc. rupts???
    					HOST_RhscIntr = 0;
    					gUSBConnected = 0;
    				}
    				#if (USB_USE_DEBUG == pdON)
    				else plog("USB: Spurious status change (disconnected)?"CMD_TERMINATOR);
    				#endif
    			}
    		}
			HcRhPortStatus = HC_OR_RH_PORT_CSC;
    	}
    	if (HcRhPortStatus & HC_OR_RH_PORT_PRSC) {
    		HcRhPortStatus = HC_OR_RH_PORT_PRSC;
    	}
    }

    if (int_status & HC_OR_INTR_STATUS_WDH) {                  /* Writeback Done Head interrupt        */
    	HOST_WdhIntr = 1;
    	HOST_TDControlStatus = (TDHead->Control >> 28) & 0xf;
    }

    HcInterruptStatus = int_status;                         /* Clear interrupt status register      */

    #if (USB_USE_LED ==	pdON)
    if (HOST_RhscIntr)
    		USB_LED_DEVCNT1_ON = USB_LED_DEVCNT1;
    else	USB_LED_DEVCNT1_OFF = USB_LED_DEVCNT1;
	#endif

    // Limpa interrupção
    #if defined (arm7tdmi)
    VICSoftIntClr = (1<<VIC_USB);
	#endif
	#if defined (cortexm3)
    nvic_clearPendingIRQ(VIC_USB);
    #endif

    ISR_EXIT;																// Procedimento para saida da interrupção
}
