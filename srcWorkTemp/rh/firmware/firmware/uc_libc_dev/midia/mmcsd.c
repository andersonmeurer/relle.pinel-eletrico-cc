/*------------------------------------------------------------------------*/
/* LPCXpresso176x: MMCv3/SDv1/SDv2 (SPI mode) control module              */
/*------------------------------------------------------------------------*/
/*
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "mmcsd.h"
#if (MMCSD_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

static tDiskStatus mmcsd_DiskStatus;		// Guarda os dados do tipo de cartão
static u8 mmcsd_csd[16];
#define CSD_GET_TRAN_SPEED_EXP()      (mmcsd_csd[ 3] & 0x07)
#define CSD_GET_TRAN_SPEED_MANT()    ((mmcsd_csd[ 3] & 0xF8) >> 3)
#define CSD_GET_NSAC()                (mmcsd_csd[ 2])
#define CSD_GET_TAAC_EXP()            (mmcsd_csd[ 1] & 0x7)
#define CSD_GET_TAAC_MANT()          ((mmcsd_csd[ 1] & 0xF8) >> 3)
#define CSD_GET_R2W_FACTOR()         ((mmcsd_csd[12] & 0x1C) >> 2)
#define CSD_GET_READ_BL_LEN()         (mmcsd_csd[ 5] & 0x0F)
#define CSD_GET_C_SIZE()            (((mmcsd_csd[ 6] & 0x03) << 10) + (mmcsd_csd[7] << 2) + ((mmcsd_csd[8] & 0xc0) >> 6))
#define CSD_GET_C_SIZE_MULT()       (((mmcsd_csd[ 9] & 0x03) << 1) + ((mmcsd_csd[10] & 0x80) >> 7))
#define CSD_GET_PERM_WRITE_PROTECT() ((mmcsd_csd[14] & 0x20) >> 5)
#define CSD_GET_TMP_WRITE_PROTECT()  ((mmcsd_csd[14] & 0x10) >> 4)


// #####################################################################################
// DECLARAÇÃO DE PROTÓTIPOS
static void mmcsd_PowerOff (void);

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Inicializa o cartão MMC ou SD
// Parametros:	Ponteiro para a estrutura de status de disco
// Retorna:		Retorna o código da operação
//					MMCSD_OK = Cartão inicalizado com sucesso
//					MCSD_DISK_TYPE_UNKNOWN
//					MMCSD_CARD_ERROR
//					MMCSD_NO_RESPONSE = Cartão não responde
//------------------------------------------------------------------------------------------------------------------
mmcsd_sts_t
mmcsd_Init (tDiskStatus *disk_sts) {
	mmcsd_sts_t res;
	u8 n, cmd, ocr[4];
	mmcsd_DiskStatus.block_number = mmcsd_DiskStatus.block_size = 0;
	mmcsd_DiskStatus.disk_type = DISK_TYPE_UNKNOWN;

	mmcsd_SetupPorts();

	#if (MMC_USE_HARD_CONTROL == pdON)
	if (!mccHardwareCardPresent())								// Checa se o cartão está conectado
		return(MMCCD_NO_PRESENT);
	#endif

	mmcsd_Deselect();
	mmcsd_slowClock(); // Ajusta o clock baixo do barramento para configuração

	// MANDAR 74 CICLOS DE CLOCK PARA O CARTÃO(POWER UP CYCLES). ISTO É NECESSÁRIO APÓS LIGAR O CARTÃO
	for (n = 10; n; n--) mmcsd_TransferByte(0xFF);	// Send 80 dummy clocks

	#if (MMCSD_USE_DEBUG == pdON)
	plog("MMCSD: reset"CMD_TERMINATOR);
	#endif

	res = MMCSD_NO_RESPONSE;

	if (mmcsd_SendCommand(CMD0, 0) == 1) {			// Mandar o comando de reset, colocando o cartão em modo spi e estdo ocioso
		#if (MMCSD_USE_DEBUG == pdON)
		plog("MMCSD: Read card type"CMD_TERMINATOR);
		#endif

		if (mmcsd_SendCommand(CMD8, 0x1AA) == 1) {	// SDv2?
			for (n = 0; n < 4; n++) ocr[n] = mmcsd_TransferByte(0xFF);	// Get 32 bit return value of R7 resp
			res = MMCSD_CARD_ERROR;
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				// Is the card supports vcc of 2.7-3.6V?
				while (mmcsd_SendCommand(ACMD41, 1UL << 30)) ;	// Wait for end of initialization with ACMD41(HCS)
				if (mmcsd_SendCommand(CMD58, 0) == 0) {		// Check CCS bit in the OCR
					for (n = 0; n < 4; n++) ocr[n] = mmcsd_TransferByte(0xFF);
					mmcsd_DiskStatus.disk_type = (ocr[0] & 0x40) ? DISK_TYPE_SD2 | DISK_TYPE_SD_BLOCK : DISK_TYPE_SD2;	// Card id SDv2
					res = MMCSD_OK;
				}
			}
		} else {	// Not SDv2 card
			res = MMCSD_OK;
			if (mmcsd_SendCommand(ACMD41, 0) <= 1) 	{	// SDv1 or MMC?
				mmcsd_DiskStatus.disk_type = DISK_TYPE_SD1; cmd = ACMD41;	// SDv1 (ACMD41(0))
			} else {
				mmcsd_DiskStatus.disk_type = DISK_TYPE_MMC; cmd = CMD1;	// MMCv3 (CMD1(0))
			}
			while (mmcsd_SendCommand(cmd, 0)) ;		// Wait for end of initialization
			if (mmcsd_SendCommand(CMD16, 512) != 0)	 {// Set block length: 512
				mmcsd_DiskStatus.disk_type = DISK_TYPE_UNKNOWN;
				res = MMCSD_DISK_TYPE_UNKNOWN;
			}
		}

		if (res == MMCSD_OK)  {
			mmcsd_fastClock();			// Set fast clock

			//mmcsd_CSDImplement(); //  Implement CSD data, and set block size
			//mmcsd_SetBlockLen(mmc_disk_status.block_size);

		  	if ((res = mmcsd_ReadCardInfo (mmcsd_csd)) == MMCSD_OK) {

			  	mmcsd_DiskStatus.block_size = 1 << CSD_GET_READ_BL_LEN ();
			  	mmcsd_DiskStatus.block_number = (CSD_GET_C_SIZE () + 1) * (4 << CSD_GET_C_SIZE_MULT ());
			  	#if (MMCSD_USE_HARD_CONTROL == pdON)
			  	mmcsd_DiskStatus.write_protect = mmcsd_HardwareWriteProtected() | CSD_GET_PERM_WRITE_PROTECT () | CSD_GET_TMP_WRITE_PROTECT ();
			  	#else
			  	mmcsd_DiskStatus.write_protect = CSD_GET_PERM_WRITE_PROTECT () | CSD_GET_TMP_WRITE_PROTECT ();
			  	#endif

		  		#if (MMCSD_USE_DEBUG == pdON)

		  		plog("MMCSD: block_size %u"CMD_TERMINATOR, mmcsd_DiskStatus.block_size);
		  		plog("MMCSD: block_number %u"CMD_TERMINATOR, mmcsd_DiskStatus.block_number);
		  		plog("MMCSD: write_protect %s"CMD_TERMINATOR, (mmcsd_DiskStatus.write_protect == pdTRUE)?"TRUE":"FALSE");

		  		mmcsd_DecodeCSD();
		  		plog("MMCSD: Init OK"CMD_TERMINATOR);
				#endif
		    } else
		    	mmcsd_PowerOff();
		}
	}

	mmcsd_Disable();
	*disk_sts = mmcsd_DiskStatus;

	return res;
}


//------------------------------------------------------------------------------------------------------------------
// Descrição:
// Parametros:
// Retorna:
//------------------------------------------------------------------------------------------------------------------
bool mmcsd_WriteProtect (void) {
	return mmcsd_DiskStatus.write_protect;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Ajusta o tamanho do bloco
// Parametros:	Tamanho do buffer
// Retorna:		Código da operação vindo do dispostivo
//------------------------------------------------------------------------------------------------------------------
u32
mmcsd_SetBlockLen (u32 length) {
  	u32 res = mmcsd_SendCommand(CMD16, length);
  	mmcsd_Deselect();
  	return res;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:	Read CSD or CID  registers depending on command
// Parametros:	TIPO COMANDO command - CMD9, CMD10 are only allowed
// Retorna:
//------------------------------------------------------------------------------------------------------------------
mmcsd_sts_t
mmcsd_ReadCardInfo (u8 *buffer) {
  	u32 i;
  	u8 res;

  	mmcsd_Deselect();
  	mmcsd_TransferByte(0xff);

	#if (MMCSD_USE_DEBUG == pdON)
  	plog("MMCSD: read card info"CMD_TERMINATOR);
	#endif

  	if (mmcsd_SendCommand (CMD9, 0) == 0) {
  		for (i = 8; i ; --i) {
  			if (((res = mmcsd_TransferByte(0xff)) | MMCSD_DATA_ERR_TOKEN) == MMCSD_DATA_ERR_TOKEN) {
				#if (MMCSD_USE_DEBUG == pdON)
      			plog("MMCSD: data error"CMD_TERMINATOR);
				#endif

      			return MMCSD_DATA_ERROR;
      		} else if (res == MMCSD_DATA_TOKEN) {
      			for (i = 0; i <16 ; ++i)
      				*buffer++ = mmcsd_TransferByte(0xff);

      			// CRC receive
      			mmcsd_TransferByte(0xff);
      			mmcsd_TransferByte(0xff);
      			mmcsd_Deselect();
      			return MMCSD_OK;
      		}
    	}
  	}

  	mmcsd_Deselect();
  	return MMCSD_CARD_ERROR;
}


// READ SECTOR(S)
mmcsd_sts_t
mmcsd_ReadSectors (
	u8 *buff,		/* Pointer to the data buffer to store read data */
	u32 sector,	/* Start sector number (LBA) */
	u8 count		/* Number of sectors to read (1..128) */
) {
	if (!(mmcsd_DiskStatus.disk_type == DISK_TYPE_SD_BLOCK)) sector *= 512;	/* LBA ot BA conversion (byte addressing cards) */

	if (count == 1) {	/* Single sector read */
		if ((mmcsd_SendCommand(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& mmcsd_ReceiveDataBlock(buff, 512))
			count = 0;
	}
	else {				/* Multiple sector read */
		if (mmcsd_SendCommand(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!mmcsd_ReceiveDataBlock(buff, 512)) break;
				buff += 512;
			} while (--count);
			mmcsd_SendCommand(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	mmcsd_Disable();

	return count ? MMCSD_DATA_ERROR : MMCSD_OK;	/* Return result */
}


// WRITE SECTOR(S)
mmcsd_sts_t
mmcsd_WriteSectors (
	const u8 *buff,	/* Ponter to the data to write */
	u32 sector,		/* Start sector number (LBA) */
	u8 count			/* Number of sectors to write (1..128) */
) {

	// TODO: ainda não foi testado

	if (!(mmcsd_DiskStatus.disk_type == DISK_TYPE_SD_BLOCK)) sector *= 512;	/* LBA ==> BA conversion (byte addressing cards) */

	if (count == 1) {	/* Single sector write */
		if ((mmcsd_SendCommand(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& mmcsd_SendDataBlock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple sector write */
		if (mmcsd_DiskStatus.disk_type == DISK_TYPE_SDC) mmcsd_SendCommand(ACMD23, count);	/* Predefine number of sectors */
		if (mmcsd_SendCommand(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!mmcsd_SendDataBlock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!mmcsd_SendDataBlock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	mmcsd_Disable();

	return count ? MMCSD_DATA_ERROR : MMCSD_OK;	/* Return result */
}


// SELECT CARD AND WAIT FOR READY
int
mmcsd_Enable (void)
{
	mmcsd_Select();
	mmcsd_TransferByte(0xFF);	// Dummy clock (force DO enabled)

	if (mmcsd_WaitReady()) return pdPASS;
	mmcsd_Disable();
	return pdFAIL;	// Timeout
}

// DESELECT CARD AND RELEASE SPI
void
mmcsd_Disable (void) {
	mmcsd_Deselect();
	mmcsd_TransferByte(0xFF);	// Dummy clock (force DO hi-z for multiple slave SPI)
}


// SEND A COMMAND PACKET TO THE MMC                                      */
u8
mmcsd_SendCommand (		/* Return value: R1 resp (bit7==1:Failed to send) */
	u8 cmd,		/* Command index */
	u32 arg		/* Argument */
)
{
	u8 n, res;


	if (cmd & 0x80) {	/* Send a CMD55 prior to ACMD<n> */
		cmd &= 0x7F;
		res = mmcsd_SendCommand(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select card */
	mmcsd_Disable();
	if (!mmcsd_Enable()) return 0xFF;

	/* Send command packet */
	mmcsd_TransferByte(0x40 | cmd);				/* Start + command index */
	mmcsd_TransferByte((u8)(arg >> 24));		/* Argument[31..24] */
	mmcsd_TransferByte((u8)(arg >> 16));		/* Argument[23..16] */
	mmcsd_TransferByte((u8)(arg >> 8));			/* Argument[15..8] */
	mmcsd_TransferByte((u8)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	mmcsd_TransferByte(n);

	/* Receive command resp */
	if (cmd == CMD12) mmcsd_TransferByte(0xFF);	/* Diacard following one byte when CMD12 */
	n = 10;								/* Wait for response (10 bytes max) */
	do
		res = mmcsd_TransferByte(0xFF);
	while ((res & 0x80) && --n);

	return res;							/* Return received response */
}

// RECEIVE A DATA PACKET FROM THE MMCSD
int
mmcsd_ReceiveDataBlock (	/* 1:OK, 0:Error */
	u8 *buff,			/* Data buffer */
	u16 btr			/* Data block length (byte) */
)
{
	u8 token;
	do {							/* Wait for DataStart token in timeout of 200ms */
		token = mmcsd_TransferByte(0xFF);
		/* This loop will take a time. Insert rot_rdq() here for multitask envilonment. */
	} while ((token == 0xFF));
	if(token != 0xFE) return 0;		/* Function fails if invalid DataStart token or timeout */

	mmcsd_ReceiveBlock(buff, btr);		/* Store trailing data to the buffer */
	mmcsd_TransferByte(0xFF); mmcsd_TransferByte(0xFF);			/* Discard CRC */

	#if (MMCSD_USE_DEBUG == pdON)
	plog("MMC: read block size %u"CMD_TERMINATOR, btr);
	#endif

	return 1;						/* Function succeeded */
}


// SEND A DATA PACKET TO THE MMC
int
mmcsd_SendDataBlock (	/* 1:OK, 0:Failed */
	const u8 *buff,	/* Ponter to 512 byte data to be sent */
	u8 token			/* Token */
) {
	// TODO: ainda não foi testado

	u8 resp;
	if (!mmcsd_WaitReady()) return 0;		/* Wait for card ready */

	mmcsd_TransferByte(token);					/* Send token */
	if (token != 0xFD) {				/* Send data if token is other than StopTran */
		mmcsd_SendBlock(buff, 512);		/* Data */
		mmcsd_TransferByte(0xFF); mmcsd_TransferByte(0xFF);	/* Dummy CRC */

		resp = mmcsd_TransferByte(0xFF);				/* Receive data resp */
		if ((resp & 0x1F) != 0x05)		/* Function fails if the data packet was not accepted */
			return 0;
	}

	#if (MMCSD_USE_DEBUG == pdON)
	plog("MMC: send packet - token %u"CMD_TERMINATOR, token);
	#endif
	return 1;
}


// #################################################################################################################################################################
// #################################################################################################################################################################
// FUNCÕES INTERNAS
static void
mmcsd_PowerOff (void) {		/* Disable SPI function */
	mmcsd_Enable();				/* Wait for card ready */
	mmcsd_Disable();
}




/*
//------------------------------------------------------------------------------------------------------------------
// Descrição:	Implemet data from CSD
// Parametros:
// Retorna:
//------------------------------------------------------------------------------------------------------------------
static void mmc_CSDImplement (void) {
  	u32 frequency;

  	// Calculate SPI max clock
  	frequency = mmcTransfExp [CSD_GET_TRAN_SPEED_EXP ()] * mmcCsdMant [CSD_GET_TRAN_SPEED_MANT ()];

  	//if ( (frequency > 20000000) || (frequency == 0) )
  	if (frequency > 20000000)
    	frequency = 20000000;

   	#if (MMCSD_USE_DEBUG == pdON)
	plog("MMCSD: frequency set %u"CMD_TERMINATOR, frequency);
	#endif

  	frequency = mmc_SetClockFreq (frequency);

  	if (mmc_disk_status.disk_type == DISK_TYPE_MMC) {
    	t_nac = mmmcAccessTime [CSD_GET_TAAC_EXP ()] * mmcAccessTimeMant [CSD_GET_TAAC_MANT ()];
    	t_nac = frequency / t_nac;
    	t_nac += 1 << (CSD_GET_NSAC () + 4);
    	t_nac *= 10;
    	t_wr   = t_nac * CSD_GET_R2W_FACTOR ();
  	} else {
    	t_nac = frequency / SD_READ_TIME_OUT;
    	t_wr  = frequency / SD_WRITE_TIME_OUT;
  	}

  	mmc_disk_status.block_size = 1 << CSD_GET_READ_BL_LEN ();
  	mmc_disk_status.block_number = (CSD_GET_C_SIZE () + 1) * (4 << CSD_GET_C_SIZE_MULT ());
  	#if (MMC_USE_HARD_CONTROL == pdON)
  	mmc_disk_status.write_protect = mccHardwareWriteProtected() | CSD_GET_PERM_WRITE_PROTECT () | CSD_GET_TMP_WRITE_PROTECT ();
  	#else
  	mmc_disk_status.write_protect = CSD_GET_PERM_WRITE_PROTECT () | CSD_GET_TMP_WRITE_PROTECT ();
  	#endif

   	#if (MMCSD_USE_DEBUG == pdON)
	plog("MMCSD: frequency %u"CMD_TERMINATOR, frequency);
	plog("MMCSD: t_nac %u"CMD_TERMINATOR, t_nac);
	plog("MMCSD: t_wr %u"CMD_TERMINATOR, t_wr);
	plog("MMCSD: block_size %u"CMD_TERMINATOR, mmc_disk_status.block_size);
	plog("MMCSD: block_number %u"CMD_TERMINATOR, mmc_disk_status.block_number);
	plog("MMCSD: write_protect %s"CMD_TERMINATOR, (mmc_disk_status.write_protect == pdTRUE)?"TRUE":"FALSE");
	#endif
}
*/

#if (MMCSD_USE_HARD_CONTROL == pdON)
//  Return 1 if MMC card present, 0 if not
//------------------------------------------------------------------------------------------------------------------
// Descrição:
// Parametros:
// Retorna:
//------------------------------------------------------------------------------------------------------------------
inline int mmcsd_HardwareCardPresent (void) {
  	return (MMCSD_PRESENT_PIN & MMCSD_PRESENT) ? 0 : 1;
}

//  Return 1 if card write protected (or not present), 0 if not write protected
//------------------------------------------------------------------------------------------------------------------
// Descrição:
// Parametros:
// Retorna:
//------------------------------------------------------------------------------------------------------------------
inline int mmcsd_HardwareWriteProtected (void) {
  	if (!mmcsd_HardwareCardPresent ())
    	return 1;

  	return (MMCSD_WR_PROTECT_PIN & MMCSD_WR_PROTECT) ? 1 : 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição:
// Parametros:
// Retorna:
//------------------------------------------------------------------------------------------------------------------
bool mmcsd_CardPresent (void) {
  	return mmcsd_HardwareCardPresent();
}
#endif


// #################################################################################################################################################################
// DEBUG

#if (MMCSD_USE_DEBUG == pdON)
typedef struct csd_11_s {
  unsigned int not_used_1          : 2;   // 0..1  [121:120]   0x44
  unsigned int mmc_prot            : 4;   // 2..5  [125:122]
  unsigned int csd_structure       : 2;   // 6..7  [127:126]

  unsigned int taac_exp            : 3;   // 0..2  [114:112]   0x26
  unsigned int taac_value          : 4;   // 3..6  [118:115]
  unsigned int taac_rsvd           : 1;   // 7..7  [119:119]

  unsigned int nsac                : 8;   // 0..7  [111:104]   0x00

  unsigned int tran_speed_exp      : 3;   // 0..2  [98:96]     0x2a
  unsigned int tran_speed_value    : 4;   // 3..6  [102:99]
  unsigned int tran_speed_rsvd     : 1;   // 7..7  [103:103]

  unsigned int ccc_hi              : 8;   // 0..7  [95:88]     0x1f

  unsigned int read_bl_len         : 4;   // 0..3  [83:80]     0xf9
  unsigned int ccc_lo              : 4;   // 4..7  [87:84]

  unsigned int c_size_hi           : 2;   // 0..1  [73:72]     0x83
  unsigned int not_used_2          : 2;   // 2..3  [75:74]
  unsigned int dsr_imp             : 1;   // 4..4  [76:76]
  unsigned int read_blk_misalign   : 1;   // 5..5  [77:77]
  unsigned int write_blk_misalign  : 1;   // 6..6  [78:78]
  unsigned int read_bl_partial     : 1;   // 7..7  [79:79]

  unsigned int c_size_mid          : 8;   // 0..7  [71:64]     0xd3

  unsigned int vdd_r_curr_max      : 3;   // 0..1  [58:56]     0xe3
  unsigned int vdd_r_curr_min      : 3;   // 2..5  [61:59]
  unsigned int c_size_lo           : 2;   // 6..7  [63:62]

  unsigned int c_size_mult_hi      : 2;   // 0..1  [49:48]     0x91
  unsigned int vdd_w_curr_max      : 3;   // 2..4  [52:50]
  unsigned int vdd_w_curr_min      : 3;   // 4..7  [55:53]

  unsigned int erase_grp_size_lo   : 2;   // 0..1  [41:40]     0x83
  unsigned int erase_grp_mult      : 5;   // 2..6  [46:42]
  unsigned int c_size_mult_lo      : 1;   // 7..7  [47:47]

  unsigned int wp_grp_size         : 5;   // 0..4  [36:32]     0xff
  unsigned int erase_grp_size_hi   : 3;   // 5..7  [39:37]

  unsigned int write_bl_len_hi     : 2;   // 0..1  [25:24]     0x92
  unsigned int r2w_factor          : 3;   // 2..4  [28:26]
  unsigned int default_ecc         : 2;   // 5..6  [30:29]

  unsigned int wp_grp_enable       : 1;   // 7..7  [31:31]
  unsigned int not_used_3          : 5;   // 0..4  [20:16]     0x40

  unsigned int write_bl_partial    : 1;   // 5..5  [21:21]
  unsigned int write_bl_len_lo     : 2;   // 6..7  [23:22]

  unsigned int ecc                 : 2;   // 0..1  [9:8]       0x40
  unsigned int not_used_5          : 2;   // 2..3  [11:10]
  unsigned int tmp_write_protect   : 1;   // 4..4  [12:12]
  unsigned int perm_write_protect  : 1;   // 5..5  [13:13]
  unsigned int copy                : 1;   // 6..6  [14:14]
  unsigned int not_used_4          : 1;   // 7..7  [15:15]

  unsigned int notused_6           : 1;   // 0..0  [0:0]       0x67
  unsigned int crc                 : 7;   // 1..7  [7:1]
} __attribute__ ((packed)) csd_11_t;

static const unsigned int tran_exp [] = {
  10000,    100000,   1000000,  10000000,
  0,        0,        0,        0
};

static const unsigned char tran_mant [] = {
  0,  10, 12, 13, 15, 20, 25, 30,
  35, 40, 45, 50, 55, 60, 70, 80,
};

static const unsigned int taac_exp [] = {
  1,  10, 100,  1000, 10000,  100000, 1000000, 10000000,
};

static const unsigned int taac_mant [] = {
  0,  10, 12, 13, 15, 20, 25, 30,
  35, 40, 45, 50, 55, 60, 70, 80,
};

static const unsigned int vdd_values [] = {
  500, 1000, 5000, 10000, 25000, 35000, 60000, 100000
};

//------------------------------------------------------------------------------------------------------------------
// Descrição:
// Parametros:
// Retorna:
//------------------------------------------------------------------------------------------------------------------
void mmcsd_DecodeCSD (void) {
  	u8 mmcCSD [16];

  	csd_11_t *p = (csd_11_t *) mmcCSD;

  	if (mmcsd_ReadCardInfo (mmcCSD) == MMCSD_OK) {
    	plog("--------------------------------------------------"CMD_TERMINATOR);
    	plog("MMCSD: sizeof (csd_11_t)     : %lu"CMD_TERMINATOR,        sizeof (csd_11_t));
    	plog("MMCSD: CSD structure version : 1.%d"CMD_TERMINATOR,       p->csd_structure);
    	plog("MMCSD: MMC protocol version  : %d"CMD_TERMINATOR,         p->mmc_prot);
    	plog("MMCSD: TAAC                  : %d"CMD_TERMINATOR,         (taac_exp [p->tran_speed_exp] * taac_mant [p->tran_speed_value] + 9) / 10);
    	plog("MMCSD: NSAC                  : %d"CMD_TERMINATOR,         p->nsac * 100);
    	plog("MMCSD: TRAN_SPEED            : %d Mhz"CMD_TERMINATOR,     (tran_exp [p->tran_speed_exp] * tran_mant [p->tran_speed_value]));
    	plog("MMCSD: CCC                   : 0x%03x"CMD_TERMINATOR,     (p->ccc_hi << 4) | p->ccc_lo);
    	plog("MMCSD: READ_BL_LEN           : %d (%d)"CMD_TERMINATOR,    p->read_bl_len, 1 << p->read_bl_len);
    	plog("MMCSD: READ_BL_PARTIAL       : %d"CMD_TERMINATOR,         p->read_bl_partial);
    	plog("MMCSD: WRITE_BL_LEN          : %d (%d)"CMD_TERMINATOR,    (p->write_bl_len_hi << 2) | p->write_bl_len_lo, 1 << ((p->write_bl_len_hi << 2) | p->write_bl_len_lo));
    	plog("MMCSD: WRITE_BL_PARTIAL      : %d"CMD_TERMINATOR,         p->write_bl_partial);
    	plog("MMCSD: WRITE_BLK_MISALIGN    : %d"CMD_TERMINATOR,         p->write_blk_misalign);
    	plog("MMCSD: READ_BLK_MISALIGN     : %d"CMD_TERMINATOR,         p->read_blk_misalign);
    	plog("MMCSD: DSR_IMP               : %d"CMD_TERMINATOR,         p->dsr_imp);
    	plog("MMCSD: VDD_R_CURR_MIN        : %d (%d ua)"CMD_TERMINATOR, p->vdd_r_curr_min, vdd_values [p->vdd_r_curr_min]);
    	plog("MMCSD: VDD_R_CURR_MAX        : %d (%d ua)"CMD_TERMINATOR, p->vdd_r_curr_max, vdd_values [p->vdd_r_curr_max]);
    	plog("MMCSD: VDD_W_CURR_MIN        : %d (%d ua)"CMD_TERMINATOR, p->vdd_w_curr_min, vdd_values [p->vdd_w_curr_min]);
    	plog("MMCSD: VDD_W_CURR_MAX        : %d (%d ua)"CMD_TERMINATOR, p->vdd_w_curr_max, vdd_values [p->vdd_w_curr_max]);
    	plog("MMCSD: ERASE_GRP_SIZE        : %d"CMD_TERMINATOR,         ((p->erase_grp_size_hi << 2) | p->erase_grp_size_lo) + 1);
    	plog("MMCSD: ERASE_GRP_MULT        : %d"CMD_TERMINATOR,         p->erase_grp_mult + 1);
    	plog("MMCSD: WP_GRP_SIZE           : %d"CMD_TERMINATOR,         p->wp_grp_size + 1);
    	plog("MMCSD: WP_GRP_ENABLE         : %d"CMD_TERMINATOR,         p->wp_grp_enable);
    	plog("MMCSD: DEFAULT_ECC           : %d"CMD_TERMINATOR,         p->default_ecc);
    	plog("MMCSD: R2W_FACTOR            : %d (%d:1)"CMD_TERMINATOR,  p->r2w_factor, 1 << p->r2w_factor);
    	plog("MMCSD: COPY                  : %d"CMD_TERMINATOR,         p->copy);
    	plog("MMCSD: PERM_WRITE_PROTECT    : %d"CMD_TERMINATOR,         p->perm_write_protect);
    	plog("MMCSD: TMP_WRITE_PROTECT     : %d"CMD_TERMINATOR,         p->tmp_write_protect);
    	plog("MMCSD: ECC                   : %d"CMD_TERMINATOR,         p->ecc);
    	plog("MMCSD: C_SIZE                : %d"CMD_TERMINATOR,         (p->c_size_hi << 10) | (p->c_size_mid << 2) | p->c_size_lo);
    	plog("MMCSD: C_SIZE_MULT           : %d"CMD_TERMINATOR,         (p->c_size_mult_hi << 1) | p->c_size_mult_lo);
    	plog("MMCSD: MEDIA_SIZE            : %u"CMD_TERMINATOR,         (u32) (((p->c_size_hi << 10) | (p->c_size_mid << 2) | p->c_size_lo) + 1) *
        	                                            (u32) (4 << ((p->c_size_mult_hi << 1) | p->c_size_mult_lo)) *
            	                                        (u32) (1 << (p->read_bl_len)));
       	plog("--------------------------------------------------"CMD_TERMINATOR);
  	} else
    	plog("CSD read error"CMD_TERMINATOR);
}
#endif
