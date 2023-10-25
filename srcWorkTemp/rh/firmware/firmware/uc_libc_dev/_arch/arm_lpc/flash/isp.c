// ####################################################################################################################
// INFORMAÇÕES
// ####################################################################################################################
// Baseado no lpc21isp versão 1.97 atualizado em 2014-01-09 por Martin Maurer

// Definiremos o host este dispositivo que vai enviar o firmware para o ARM a ser gravado na FLASH.
// No ARM alvo de gravação contém um bootcode que vai receber o firmware e gravá-lo na FLASH.

// O host vai enviar os bytes do firmware em parte de múltiplos de 256 bytes para RAM do ARM alvo,
// 		quando chegar o limite da RAM, o host envia o comando para bootcode gravar o conteúdo da RAM para a FLASH.
// 		O host repete o processo até que não tenha mais bytes do firmware a ser enviados.
//		O limite da RAM depende do ARM, se há 4096 bytes dispomivel no ARM esse será o limite

// Os bytes a serem enviados dever ser formatado no padrão UUencode.
// UUencoding pega um valor de 8 bits e converte em um valor equivalente ASCII.
// UUencode trabalha com grupo de 3 bytes de dados, se os bytes de dados não for em grupo de três,
//		bytes extras deven ser adicionados.

// Há um limite máximo de envio de dados formatados por vez para bootcode,
//  	não se pode exceder 61 caracteres de envio (que são 45 bytes de dados).
// Para controlar esse limite de envio, assumeremos que cada envio corresponde a uma linha.
// Então, uma linha de dados formatados deve ter no máximo de 45 bytes de dados.

// A cada 20 linhas enviadas deve ser enviado o checksum,
//		e se chegou ao fim do envio dos bytes e não completou as 20 linhas, envie o checksum também.
// O Checksum é a soma dos bytes transferidos e tem o seguinte formato: <valor checksum> <\r\n>
// A linha de dados formatados tem o seguinte formato:
//		<quantidade de caracteres> <caracteres formatados> <\r\n>
//			<quantidade de caracteres>: Quantidades de bytes formatado na linha de dados, a quantidade deve ser adicionado mais 32

// Comando para escrever na memória:
//		W <endereço da RAM> <quantidade de bytes>
//			<endereço da RAM>: Valor em decimal
//			<quantidade de bytes> deve ser multiplos de 4
//	Exemplo:
//		1 - O Host envia o comando W addr qtd
//		2 - O bootcode responde OK
//		3 - O Host envia os dados formatados
//		4 - O Host envia o checksum
//		5 - O bootcode responde OK ou RESEND

// Comando para o bootcode gravar o conteúdo da RAM para a FLASH
//		C <endereço da flash> <endereço da RAM> <quantidade de bytes>
//			<endereço da flash>: Valor em decimal e deve ter limite de 256 bytes
//			<endereço da RAM>: Valor em decimal
//			<quantidade de bytes>: Valores válidos são 256, 512, 1024 e 4096
//	Exemplo:
//		1 - O Host envia o comando para desbloquear para gravação: U 23130
//		2 - O bootcode responde OK
//		3 - O Host envia o comando para preparar o setor para gravação: P <setor> <setor>
//		4 - O bootcode responde OK
//		5 - O Host envia o comando C addr qtd
//		6 - O bootcode responde OK

// Podemos desligar o eco do bootcode para diminuir o tempo de gravação, para isto envie o comando: A 0
// ####################################################################################################################


#include <string.h>
#include "_config_lib_.h"
#include "isp.h"
#include "stdio_uc.h"
#include "_config_cpu_isp.h"

#define ispTIME_WAIT 1000

typedef enum {
  	CHIP_VARIANT_NONE,
  	CHIP_VARIANT_LPC43XX,
  	CHIP_VARIANT_LPC2XXX,
  	CHIP_VARIANT_LPC18XX,
  	CHIP_VARIANT_LPC17XX,
  	CHIP_VARIANT_LPC13XX,
  	CHIP_VARIANT_LPC11XX,
  	CHIP_VARIANT_LPC8XX
} CHIP_VARIANT;

typedef struct {
    const unsigned long  id;
    const unsigned long  id2;
    const unsigned int   EvalId2;
    const char *Product;
    const unsigned int   FlashSize;     /* in kiB, for informational purposes only */
    const unsigned int   RAMSize;       /* in kiB, for informational purposes only */
          unsigned int   FlashSectors;  /* total number of sectors */
          unsigned int   MaxCopySize;   /* maximum size that can be copied to Flash in a single command */
    const unsigned int  *SectorTable;   /* pointer to a sector table with constant the sector sizes */
    const CHIP_VARIANT   ChipVariant;
} LPC_DEVICE_TYPE;

// Used for LPC17xx devices
static const unsigned int SectorTable_17xx[] = {
     4096,  4096,  4096,  4096,  4096,  4096,  4096,  4096,
     4096,  4096,  4096,  4096,  4096,  4096,  4096,  4096,
    32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
    32768, 32768, 32768, 32768, 32768, 32768
};

// para outros dispositivos consulte isplpcdev.c
static LPC_DEVICE_TYPE LPCtypes[] = {
   // id,        id2,  use id2, name of product,          flash size, ram size, total number of sector, max copy size, sector table, chip variant
   { 0x25001118, 0x00000000, 0, "1751",                           32,   8,  8, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x25001121, 0x00000000, 0, "1752",                           64,  16, 16, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x281D3F47, 0x00000000, 0, "1788",                          512,  96, 30, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX }
};

/* LPC_RAMSTART, LPC_RAMBASE
*
* Used in NxpDownload() to decide whether to Flash code or just place in in RAM
* (works for .hex files only)
*
* LPC_RAMSTART - the Physical start address of the SRAM
* LPC_RAMBASE  - the base address where downloading starts.
*                Note that any code in the .hex file that resides in 0x4000,0000 ~ 0x4000,0200
*                will _not_ be written to the LPCs SRAM.
*                This is due to the fact that 0x4000,0040 - 0x4000,0200 is used by the bootrom.
*                Any interrupt vectors must be copied to 0x4000,0000 and remapped to 0x0000,0000
*                by the startup code.
*/
#define LPC_RAMSTART_LPC43XX    0x10000000L
#define LPC_RAMBASE_LPC43XX     0x10000200L

#define LPC_RAMSTART_LPC2XXX    0x40000000L
#define LPC_RAMBASE_LPC2XXX     0x40000200L

#define LPC_RAMSTART_LPC18XX    0x10000000L
#define LPC_RAMBASE_LPC18XX     0x10000200L

#define LPC_RAMSTART_LPC17XX    0x10000000L
#define LPC_RAMBASE_LPC17XX     0x10000200L

#define LPC_RAMSTART_LPC13XX    0x10000000L
#define LPC_RAMBASE_LPC13XX     0x10000300L

#define LPC_RAMSTART_LPC11XX    0x10000000L
#define LPC_RAMBASE_LPC11XX     0x10000300L

#define LPC_RAMSTART_LPC8XX     0x10000000L
#define LPC_RAMBASE_LPC8XX      0x10000270L

/* Return values used by NxpDownload(): reserving all values from 0x1000 to 0x1FFF */

#define NO_ANSWER_WDT       0x1000
#define NO_ANSWER_QM        0x1001
#define NO_ANSWER_SYNC      0x1002
#define NO_ANSWER_OSC       0x1003
#define NO_ANSWER_RBV       0x1004
#define NO_ANSWER_RPID      0x1005
#define ERROR_WRITE_DATA    0x1006
#define ERROR_WRITE_CRC     0x1007
#define ERROR_WRITE_CRC2    0x1008
#define PROGRAM_TOO_LARGE   0x1009

#define USER_ABORT_SYNC     0x100A   /* User aborted synchronisation process */

#define UNKNOWN_LPC         0x100B   /* Unknown LPC detected */

#define UNLOCK_ERROR        0x1100   /* return value is 0x1100 + NXP ISP returned value (0 to 255) */
#define WRONG_ANSWER_PREP   0x1200   /* return value is 0x1200 + NXP ISP returned value (0 to 255) */
#define WRONG_ANSWER_ERAS   0x1300   /* return value is 0x1300 + NXP ISP returned value (0 to 255) */
#define WRONG_ANSWER_WRIT   0x1400   /* return value is 0x1400 + NXP ISP returned value (0 to 255) */
#define WRONG_ANSWER_PREP2  0x1500   /* return value is 0x1500 + NXP ISP returned value (0 to 255) */
#define WRONG_ANSWER_COPY   0x1600   /* return value is 0x1600 + NXP ISP returned value (0 to 255) */
#define FAILED_RUN          0x1700   /* return value is 0x1700 + NXP ISP returned value (0 to 255) */
#define WRONG_ANSWER_BTBNK  0x1800   /* return value is 0x1800 + NXP ISP returned value (0 to 255) */

#define LPC_BSL_PIN         13
#define LPC_RESET_PIN       47
#define LPC_RESET(in)       NAsetGPIOpin(LPC_RESET_PIN, (in))
#define LPC_BSL(in)         NAsetGPIOpin(LPC_BSL_PIN,   (in))


/* LPC_FLASHMASK
*
* LPC_FLASHMASK - bitmask to define the maximum size of the Filesize to download.
*                 LoadFile() will check any new segment address record (03) or extended linear
*                 address record (04) to see if the addressed 64 kByte data block still falls
*                 in the max. flash size.
*                 LoadFile() will not load any files that are larger than this size.
*/
#define LPC_FLASHMASK  0xFFC00000 /* 22 bits = 4 MB */

typedef enum {
	stISP_NONE = 0, // faz nada
	stISP_INIT_BOOTCODE, // Ativa os sinais INT0 e RST do ARM para que o bootcode do mesmo inicie
	stISP_WAIT_BOOTCODE_READY, // Espera para que o bootcode esteja pronto para receber comandos
	stISP_PROGRAM_FLASH, // Estado para gravar o firmware na FLASH
	stISP_INIT_PROGMAIN, // Ativa os sinais INT0 e RST do ARM para que o programa principal seja iniciado
	stISP_WAIT_PROGMAIN_READY // Espera para que programa ser rodado
} tStISP;

typedef enum {
	stBOOT_SYNCHRONIZE = 0, // Fazer pedido para sincronizar
	stBOOT_SYNCHRONIZED, 	// Fazer pergunta se está sincronizado
	stBOOT_SET_OSC,			// Ajusta a frequencia do cristal em MHz
	stBOOT_UNLOCK,			// Destravar bootcode para gravação
	stBOOT_GETID,			// Ler o ID do ARM
	stBOOT_PRE_ERASEALL,	// Prepara para apagar toda a flash
	stBOOT_ERASEALL, 		// Apagar toda a flash
	stBOOT_PREPROG,
	stBOOT_PROG,
	stBOOT_FINISH			// Fim da gravação
} tStBOOT;

typedef struct {
	unsigned waitresponse:1;			// Sinaliza que estamos esperando pela resposta do bootcode do ARM
	unsigned programdone:1;				// Sinaliza que a programação na FLASH foi feita
	tStISP st;
	tStBOOT stBoot;

	void (*status)(int type, int progdone, int error, int progress);
	void (*setSpeed)(u32 bd);			// Ponteiro da função para ajustar a velocidade da UART
	int (*puts)(u8 *buffer, u16 count);	// Ponteiro da função para envio de bytes
	int (*getc)(u8 *ch);				// Ponteiro da função para recebimento de bytes
	int (*byteAvailable)(void);			// Ponteiro da função para checagem se há bytes já recebidos
	void(*flushRX)(void);				// Ponteiro da função para limpar o buffer de recepção

	pchar binaryContent;				// Ponteiro do código a ser gravado na FLASH
	unsigned long binaryLength;			// Tamanho do código a ser gravado na FLASH
	char answer[128];
	int error;							// Sinaliza qual erro conteceu durante o processo de gravação

	int deviceType;
	int idxListLPC;						// Indice do dispostivo em LPC_DEVICE_TYPE
	string oscilator;					// Oscilador em Khz

	volatile tTime waitreset;			// Tempo para que o pino RST do ARM fique ativo
	volatile tTime waitint0;			// Tempo para que o pino INT0 do ARM fique ativo
	volatile tTime waitboot;			// tempo para que o bootcode esteja pronto para receber comando
	volatile tTime timeout;				// Tempo para que o bootcode responda
} tISP, *pISP;
static tISP isp;

typedef enum {
	stCHECK_SECTOR = 0,
	stNEXT_SECTOR,
	stSEND_SECTOR,
	stSEND_BYTES_BLOCK,
	stSEND_BYTES,
	stPREPARE_PROG,
	stPROG_FLASH
} tStProg;
tStProg stProg;


static unsigned long ivt_CRC;          // CRC over interrupt vector table
static char uuencode_table[64];
static char s[128];
static uint sector;
static uint lenProg;
static uint binaryIndex; // Posição do byte atual do firmware
static uint offset; // Deslocamento do byte no setor atual
static uint chunk; // Salto em bytes do setor atual
static uint copyLen;
static uint byteBlock;
static uint checksum;
static int line;
static uint block;
static uint sendChecksum = 0;
static uint checkSumCheck = 0;

int programFlash(void);

void prog_Process(void);

// isp.status_func quando não necessáio colocar NULL
void isp_Init(
	void (*status_func)(int type, int progdone, int error, int bytesRecorded),
	void (*setSpeed_func)(u32 bd),
	int(*puts_func)(u8* buffer, u16 count),
	int(*getc_func)(u8* ch),
	int(*byte_available_func)(void),
	void(*flushRX_func)(void)
) {
	pinRESET_DIR |= pinRESET;
	pinRESET_OFF = pinRESET;
	pinINT0_DIR |= pinINT0;
	pinINT0_OFF = pinINT0;

	isp.status = status_func;
	isp.setSpeed = setSpeed_func;
	isp.puts = puts_func;
	isp.getc = getc_func;
	isp.byteAvailable = byte_available_func;
	isp.flushRX = flushRX_func;

	isp_Stop();
}

int isp_ProgramFlash(int deviceType, pchar addrCode, uint size) {
	isp.deviceType = deviceType;
	if (isp.deviceType == ispDEVICE_TYPE_WORKTEMP) {
		strcpy(isp.oscilator, "10000"); // valor em KHz
		isp.idxListLPC = 1;
		isp.setSpeed(115200);
	} else if (isp.deviceType == ispDEVICE_TYPE_PR111) {
		strcpy(isp.oscilator, "12000"); // valor em KHz
		isp.idxListLPC = 2;
		isp.setSpeed(57600);
	} else return errISP_DEVICE_TYPE;

	if (size > LPCtypes[isp.idxListLPC].FlashSize*1024) return errISP_PROGRAM_TOO_LARGE;

	isp.flushRX(); // limpa buffer de recepção
	isp.st = stISP_INIT_BOOTCODE;
	isp.stBoot = stBOOT_SYNCHRONIZE;
	isp.waitresponse = 0;
	isp.programdone = 0;
	isp.error = 0;
	isp.binaryContent = addrCode;
	isp.binaryLength = size;

	return pdPASS;
}

void isp_Stop(void) {
	isp.st = stISP_NONE;
	isp.waitint0 = 0;
	isp.waitreset = 0;
	isp.waitboot = 0;
}

int isp_Busy(void) {
	if (isp.st != stISP_NONE) return pdTRUE; else return pdFALSE;
}

int isp_ProgDone(void) {
	return isp.programdone;
}

int isp_CodeError(void) {
	return isp.error;
}

// esta função deve ser chamada a cada 1ms
void isp_Tick(void) {
	if (isp.waitint0) isp.waitint0--;
	if (isp.waitreset) isp.waitreset--;
	if (isp.waitboot) isp.waitboot--;
	if (isp.waitresponse) isp.timeout++;
}

#define timeWAIT_T1 50
#define timeWAIT_T2 150
#define timeWAIT_BOOT 500

// Processa os estados nivel 1 de programação na FLASH:
// Estado 1: Reinicia o ARM no modo que rode o seu bootcode
// Estado 2: Espera o tempo de inicialização do ARM para que bootcode esteja pronto para receber comandos
// Estado 3: Chama o procedimento para gravação na FLASH
// Estado 4: Se não houve erro na programação da FLASH vai para o próximo estado, senão fica na epsera de novos comandos
// Estado 5: Reinicia o ARM para rodar o firmware recém gravado
// Estado 6: Espera o tempo de inicialização do ARM
void isp_Process(void) {
	// Gerencia os pinos de controle
	if (isp.waitreset) pinRESET_ON = pinRESET; else pinRESET_OFF = pinRESET;
	if (isp.waitint0) pinINT0_ON = pinINT0; else pinINT0_OFF = pinINT0;

	switch (isp.st) {
	// aciona os pinos RST e INT0 para iniciar o bootcode
	case stISP_INIT_BOOTCODE:
		isp.st = stISP_WAIT_BOOTCODE_READY;
		isp.waitreset = timeWAIT_T1;
		isp.waitint0 = timeWAIT_T1+timeWAIT_T2;
		isp.waitboot = timeWAIT_BOOT+timeWAIT_T1+timeWAIT_T2;

		#if (ISP_USE_DEBUG == pdON)
		plognp("Init bootcode"CMD_TERMINATOR);
		#endif

		break;

	// Espera pelo acionamento dos sinais RST e INT0 para iniciar o bootcode e espera que o mesmo esteja pronot
	case stISP_WAIT_BOOTCODE_READY:
		if ( (isp.waitreset==0) && (isp.waitint0==0) && (isp.waitboot==0) ) { isp.st = stISP_PROGRAM_FLASH; }
		break;

	// Programa a flash
	case stISP_PROGRAM_FLASH:
		if (!isp.programdone)
			prog_Process(); // chama o procedimento de controle de estados para gravação na flash
		else {
			if (isp.error == 0) isp.st = stISP_INIT_PROGMAIN; else isp.st = stISP_NONE;
			if (isp.status) isp.status(stsPROG_DONE, isp.programdone, isp.error, binaryIndex);
		}
		break;

	// Aciona os pinos RST e INT0 para iniciar o programa
	case stISP_INIT_PROGMAIN:
		isp.st = stISP_WAIT_PROGMAIN_READY;
		isp.waitreset = timeWAIT_T1+timeWAIT_T2;
		isp.waitint0 = timeWAIT_T1;
		isp.waitboot = timeWAIT_BOOT+timeWAIT_T1+timeWAIT_T2;
		break;

	// Espera pelo acionamento dos sinais RST e INT0 para iniciar o porgrama
	case stISP_WAIT_PROGMAIN_READY:
		if ( (isp.waitreset==0) && (isp.waitint0==0) && (isp.waitboot==0) ) { isp.st = stISP_NONE; }
		break;

	// Faz nada neste estado
	case stISP_NONE:
		break;
	}
}

//static unsigned long ReturnValueLpcRamStart(void) {
//  	if (LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC43XX)
//  		return LPC_RAMSTART_LPC43XX;
//   	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC2XXX)
//    	return LPC_RAMSTART_LPC2XXX;
//  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC18XX)
//    	return LPC_RAMSTART_LPC18XX;
//  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC17XX)
//    	return LPC_RAMSTART_LPC17XX;
//  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC13XX)
//    	return LPC_RAMSTART_LPC13XX;
//  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC11XX)
//    	return LPC_RAMSTART_LPC11XX;
//  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC8XX)
//    	return LPC_RAMSTART_LPC8XX;
//
//  	#if (ISP_USE_DEBUG == pdON)
//  	plognp("Error in ReturnValueLpcRamStart (%d)"CMD_TERMINATOR, LPCtypes[isp.idxListLPC].ChipVariant);
//  	#endif
//  	return 0;
//}
//
static unsigned long ReturnValueLpcRamBase(void) {
  	if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC43XX)
    	return LPC_RAMBASE_LPC43XX;
  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC2XXX)
    	return LPC_RAMBASE_LPC2XXX;
  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC18XX)
    	return LPC_RAMBASE_LPC18XX;
  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC17XX)
    	return LPC_RAMBASE_LPC17XX;
  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC13XX)
    	return LPC_RAMBASE_LPC13XX;
  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC11XX)
    	return LPC_RAMBASE_LPC11XX;
  	else if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC8XX)
    	return LPC_RAMBASE_LPC8XX;

  	#if (ISP_USE_DEBUG == pdON)
  	plognp("Error in ReturnValueLpcRamBase (%d)"CMD_TERMINATOR, LPCtypes[isp.idxListLPC].ChipVariant);
  	#endif

  	return 1;
}



/***************************** FormatCommand ********************************/
/**  2013-06-28 Torsten Lang, Uwe Schneider GmbH
According to the various NXP user manuals the ISP bootloader commands should
be terminated with <CR><LF>, the echo and/or answer should have the same line
termination. So far for the theory...
In fact, the bootloader also accepts <LF> as line termination, but it may or
may not echo the linebreak character. Some bootloaders convert the character
into <CR><LF>, some leave the <LF> and append another one (<LF><LF> in the
answer). Furthermore, during uuencoded data transfer the bootloader may or
may not append an additional <LF> character at the end of the answer
(leading to a <CR><LF><LF> sequence).
A reliable way to handle these deviations from the UM is to strictly send the
commands according to the description in the UM and to re-format commands
and answers after a transfer.
FormatCommand works in a way that it drops any leading linefeeds which only
can be surplus <LF> characters from a previous answer. It then converts any
sequence of <CR> and <LF> into a single <LF> character.
FormatCommand can work in place, meaning that In==Out is allowed!
\param [in]  In  Pointer to input buffer.
\param [out] Out Pointer to output buffer.
*/

static void formatMSG(const char *in, char *out) {
  	int i, j;
  	for (i = 0, j = 0; in[j] != '\0'; i++, j++)  {
    	if ((in[j] == '\r') || (in[j] == '\n'))	{
	   		// Ignore leading line breaks (they must be leftovers from a previous answer)
	   		if (i > 0) out[i] = '\n'; else i--;
      		while ((in[j+1] == '\r') || (in[j+1] == '\n')) j++;
    	} else out[i] = in[j];
  	}

  	out[i] = '\0';
}


static uint idxAnswer;
static int CountTermination;

// Descrição: Fica na espera pelas respostas do bootcode
// parametros:
//		wantedTermination: quantidade de terminação devemos receber para assumir recepção completa
//		timeout: tempo máximo que devemos esperar pela resposta do codeboot
// retorna:
//		errISP_NO_RESPONSE se estorou o tempo de espera pela resposta do bootcode
// 		errBUFFER_FULL se estorou o tamanho do buffer
// 		pdPASS se recebeu as respostas esperadas
// 		pdNOT_READY se ainda está recebendo dados

// Atenção: A resposa sempre finaliza \r\n. Entretanto, entre as respostas o terminador depende
//	da terminação que enviamos com o comando, ou seja:
//		Se enviamos \r\n com o comando recebemos CMD\r\nRES1\r\nRES2\r\n
//		Se enviamos \n com o comando recebemos CMD\nRES1\nRES2\r\n
static int receiveCOM(int wantedTermination, tTime timeout) {
	if (idxAnswer > sizeof(isp.answer)) return errBUFFER_FULL;
	if (isp.timeout > timeout) return errISP_NO_RESPONSE;

	u8 ch;
	if (isp.getc(&ch)==pdPASS) {
		isp.timeout = 0;
		if (ch == '\r') CountTermination++;
		isp.answer[idxAnswer++] = ch; // somente vamos salvar a respostas com o terminador \n
	}

	// não esperamos mais a respostas caso recebemos a quantidade esperada
	if (CountTermination == wantedTermination) {
		#if (ISP_USE_DEBUG == pdON)
		if ((stProg == stSEND_BYTES) && (!checkSumCheck))
			plognp("codeboot answer SEND BYTES"CMD_TERMINATOR);
		else plognp("codeboot answer: %s", isp.answer);
		#endif
		formatMSG(isp.answer, isp.answer);
		isp.waitresponse = 0;
		return pdPASS;
	} else return pdNOT_READY;
}

// Descrição: Envia um comando para o bootcode
// parametro cmd: Comando a ser enviado, deve terminar com \r\n
static void sendCOM(pchar cmd) {
	isp.puts((u8*)cmd, strlen(cmd));
	isp.waitresponse = 1;
	isp.timeout = 0;
	memset(isp.answer, 0, sizeof(isp.answer));
	idxAnswer = 0;
	CountTermination = 0;
	#if (ISP_USE_DEBUG == pdON)
	if ((stProg == stSEND_BYTES) && (!checkSumCheck))
			plognp("Send BYTES lines[%d] checkSumCheck [%d] stProg[%d] "CMD_TERMINATOR, line, checkSumCheck, stProg);
	else 	plognp("Send command: %s", cmd);
	#endif
}

// Controla os estados de programação na FLASH
// Estado 1: Envia um comando para o bootcode de pedido de sincronização e espera pela resposta de confirmação
// Estado 2: Envia um comando para o bootcode de confirmação de sincronização e espera pela resposta de confirmação
// Estado 3: Envia um comando para o bootcode informando a frequencia do seu oscilador e espera pela resposta de confirmação
// Estado 4: Envia um comando para o bootcode desbloqueando para gravação da flash e espera pela resposta de confirmação
// Estado 5: Envia um comando para o bootcode para enviar o ID do ARM e fazemos a comparação se é mesmo o ARM alvo
// Estado 6: Envia um comando para o bootcode para preparar o apagamento completo da FLASH e espera pela resposta de confirmação
// Estado 7: Envia um comando para o bootcode para apagar toda a FLASH e espera pela resposta de confirmação
// Estado 8: Faz a preparação para gravação na FLASH
// Estado 9: Chama a função para enviar o firmware para o bootcode gravar na FLASH
void prog_Process(void) {
	int res = 0;

	if (isp.programdone) return;
	if (isp.error < 0) { isp.programdone = 1; return; } // se houve algum erro cancela tudo

	// enviar o comando para sincronizar baudrate,
	//	o codeboot deve responder "Synchronized\r\n"
	if (isp.stBoot == stBOOT_SYNCHRONIZE) {
		if (!isp.waitresponse) sendCOM("?\r\n");
		else {
			res = receiveCOM(1, ispTIME_WAIT);
			if (res == pdPASS) {
				if (!strcmp(isp.answer, "Synchronized\n") == 0) {
					res = errISP_SYNCHRONIZED;
				} else {
					isp.stBoot = stBOOT_SYNCHRONIZED;
					#if (ISP_USE_DEBUG == pdON)
					plognp("OK"CMD_TERMINATOR);
					#endif
				}
			}
		}
	}

	// enviar o comando de confirmação de sincronismo
	//	codeboot deve responder "Synchronized\rOK\r\n"
	else if (isp.stBoot == stBOOT_SYNCHRONIZED) {
		if (!isp.waitresponse) sendCOM("Synchronized\r\n");
		else {
			res = receiveCOM(2, ispTIME_WAIT);
			if (res == pdPASS) {
				if (!strcmp(isp.answer, "Synchronized\nOK\n") == 0) {
					res = errISP_SYNCHRONIZED;
				} else {
					isp.stBoot = stBOOT_SET_OSC;
					#if (ISP_USE_DEBUG == pdON)
					plognp("BOOTCODE SYNCHRONIZED "CMD_TERMINATOR);
					#endif
				}
			}
		}
	}

	// enviar o comando de ajuste do osilador do ARM em KHz
	//	codeboot deve responder "osc\rOK\r\n"
	else if (isp.stBoot == stBOOT_SET_OSC) {
		if (!isp.waitresponse) {
			sformat(s, "%s\r\n",isp.oscilator);
			sendCOM(s);
		}
		else {
			res = receiveCOM(2, ispTIME_WAIT);
			if (res == pdPASS) {
				sformat(s, "%s\nOK\n", isp.oscilator);
				if (!strcmp(isp.answer, s) == 0) {
					res = errISP_SET_OSCILATOR;
				} else {
					isp.stBoot = stBOOT_UNLOCK;
				}
			}
		}
	}

	// enviar o comando de desbloqueio para gravação
	//	codeboot deve responder "U 23130\r0\r\n"
	else if (isp.stBoot == stBOOT_UNLOCK) {
		if (!isp.waitresponse) sendCOM("U 23130\r\n");
		else {
			res = receiveCOM(2, ispTIME_WAIT);
			if (res == pdPASS) {
				if (!strcmp(isp.answer, "U 23130\n0\n") == 0) {
					res = errISP_UNLOCK;
				} else {
					isp.stBoot = stBOOT_GETID;
					#if (ISP_USE_DEBUG == pdON)
					plognp("BOOTCODE UNLOCED "CMD_TERMINATOR);
					#endif
				}
			}
		}
	}

	// enviar o comando para pegar o ID do ARM e conferir se confere com qual escolhido para gravação
	//	codeboot deve responder "J\r0\nID_ARM\r\n"
	// ATENÇÃO: Não estamos considerando o ID2 > 0, se isso ocorrer ver o código original para como procecer
	else if (isp.stBoot == stBOOT_GETID) {
		if (!isp.waitresponse) sendCOM("J\r\n");
		else {
			res = receiveCOM(3, ispTIME_WAIT);
			if (res == pdPASS) {
				sformat(s, "J\n0\n%d\n", LPCtypes[isp.idxListLPC].id);
				if (!strcmp(isp.answer, s) == 0) {
					res = errISP_DEVICE_TYPE;
				} else {
					isp.stBoot = stBOOT_PRE_ERASEALL;
					#if (ISP_USE_DEBUG == pdON)
					plognp("ARM type: lpc%s %dKB FLASH %d KB SRAM "CMD_TERMINATOR, LPCtypes[isp.idxListLPC].Product, LPCtypes[isp.idxListLPC].FlashSize, LPCtypes[isp.idxListLPC].RAMSize );
					#endif
				}
			}
		}
	}

	// Enviar o comando para preparar o apagamento completo da flash
	//	codeboot deve responder "P 0 maxSector\r0\r\n"
	else if (isp.stBoot == stBOOT_PRE_ERASEALL) {
		if (!isp.waitresponse) {
			if (LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC43XX ||
				LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC18XX)
					sformat(s, "P %d %d 0\r\n", 0, LPCtypes[isp.idxListLPC].FlashSectors-1);
        	else    sformat(s, "P %d %d\r\n", 0, LPCtypes[isp.idxListLPC].FlashSectors-1);
			sendCOM(s);
		} else {
			res = receiveCOM(2, ispTIME_WAIT);
			if (res == pdPASS) {
				if (LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC43XX ||
					LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC18XX)
						sformat(s, "P %d %d\n0\n", 0, LPCtypes[isp.idxListLPC].FlashSectors-1);
        		else    sformat(s, "P %d %d\n0\n", 0, LPCtypes[isp.idxListLPC].FlashSectors-1);
				if (!strcmp(isp.answer, s) == 0) {
					res = errISP_ERASE_FLASH_ALL;
				} else {
					isp.stBoot = stBOOT_ERASEALL;
					#if (ISP_USE_DEBUG == pdON)
					plognp("erase all flash, sector 0 at %d "CMD_TERMINATOR, LPCtypes[isp.idxListLPC].FlashSectors-1);
					#endif
				}
			}
		}
	}

	// Enviar o comando para fazer apagamento completo da flash
	//	codeboot deve responder "E 0 maxSector\r0\r\n"
	// ATENÇÃO: Para CHIP_VARIANT_LPC43XX ou CHIP_VARIANT_LPC18XX only bank A was wiped
	else if (isp.stBoot == stBOOT_ERASEALL) {
		if (!isp.waitresponse) {
			if (LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC43XX ||
				LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC18XX)
					sformat(s, "E %d %d 0\r\n", 0, LPCtypes[isp.idxListLPC].FlashSectors-1);
        	else    sformat(s, "E %d %d\r\n", 0, LPCtypes[isp.idxListLPC].FlashSectors-1);
			sendCOM(s);
		} else {
			res = receiveCOM(2, ispTIME_WAIT);
			if (res == pdPASS) {
				if (LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC43XX ||
					LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC18XX)
						sformat(s, "E %d %d\n0\n", 0, LPCtypes[isp.idxListLPC].FlashSectors-1);
        		else    sformat(s, "E %d %d\n0\n", 0, LPCtypes[isp.idxListLPC].FlashSectors-1);
				if (!strcmp(isp.answer, s) == 0) {
					res = errISP_ERASE_FLASH_ALL;
				} else {
					isp.stBoot = stBOOT_PREPROG;
					#if (ISP_USE_DEBUG == pdON)
					plognp("erase all flash, sector 0 at %d "CMD_TERMINATOR, LPCtypes[isp.idxListLPC].FlashSectors-1);
					#endif
				}
			}
		}
	}

	// preparação para gravação
	else if (isp.stBoot == stBOOT_PREPROG) {
		int offs;

		// Capturar a posição do CRC de acordo com o tipo de ARM
		if (
			LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC43XX ||
			LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC18XX ||
			LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC17XX ||
			LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC13XX ||
			LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC11XX ||
			LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC8XX)
		{
			offs = 0x1C;
			res = pdPASS;
		} else if (LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC2XXX) {
			offs = 0x14;
			res = pdPASS;
		} else {
			res = errISP_DEVICE_CHIP_VARIANT;
			#if (ISP_USE_DEBUG == pdON)
			plognp("Internal error: wrong chip variant %d (detected device %d)\n", LPCtypes[isp.idxListLPC].ChipVariant, isp.idxListLPC);
			#endif
		}

		if (res == pdPASS) {
			isp.stBoot = stBOOT_PROG;
			ivt_CRC = 0;

			// Build up uuencode table
			uuencode_table[0] = 0x60;           // 0x20 is translated to 0x60 !
			int i; for (i = 1; i < 64; i++) uuencode_table[i] = (char)(0x20 + i);

			// Patch 0x1C/0x14, otherwise it is not running and jumps to boot mode
			// Clear the vector at 0x1C/0x14 so it doesn't affect the checksum:
			for (i = 0; i < 4; i++) isp.binaryContent[i + offs] = 0;

			// Calculate a native checksum of the little endian vector table:
			for (i = 0; i < (4 * 8);) {
				ivt_CRC += isp.binaryContent[i++];
				ivt_CRC += isp.binaryContent[i++] << 8;
				ivt_CRC += isp.binaryContent[i++] << 16;
				ivt_CRC += isp.binaryContent[i++] << 24;
			}

			// Negate the result and place in the vector at 0x1C/0x14 as little endian again. The resulting vector table should checksum to 0
			ivt_CRC = (unsigned long) (0 - ivt_CRC);
			for (i = 0; i < 4; i++) isp.binaryContent[i + offs] = (unsigned char)(ivt_CRC >> (8 * i));


			stProg = stCHECK_SECTOR;
			sector = 0;
			lenProg = 0;
			binaryIndex = 0;
			offset = 0;
			chunk = 0;
			copyLen = 0;
			byteBlock = 0;
			checksum = 0;
			line = 0;

			#if (ISP_USE_DEBUG == pdON)
			plognp("Position 0x%x patched: ivt_CRC = 0x%08x"CMD_TERMINATOR, offset, ivt_CRC);
			plognp("Starting programming"CMD_TERMINATOR);
			#endif
		}
	}

	// faser leitura do firmware contido na RAM para a FLASH
	else if (isp.stBoot == stBOOT_PROG) {
		 if ( (res = programFlash()) == pdREADY) isp.programdone = 1;
	}

	if (res > 0) isp.error = 0; else isp.error = res;
	#if (ISP_USE_DEBUG == pdON)
	if (isp.error < 0) plognp("Error %d"CMD_TERMINATOR, res);
	#endif
}

// pdREADY
// pdNOT_READY
// errISP_PROGRAM_TOO_LARGE
// errISP_WRITE_RAM
// errISP_SEND_BYTES
// errISP_CHECSUM
// errISP_PREPARE_TO_PROG
// errISP_WRITE_FLASH
int programFlash(void) {
	// se estamos esperando uma resposta do bootcode
	if (isp.waitresponse) {
		int res = 0;
		if ( (stProg == stSEND_BYTES) && (!checkSumCheck))
				res = receiveCOM(1, ispTIME_WAIT);
		else 	res = receiveCOM(2, ispTIME_WAIT);

		if (res == pdPASS) {
			isp.waitresponse = 0;
			formatMSG(s,s);
			string sRes;
			if (stProg == stSEND_BYTES_BLOCK) {
				sformat(sRes, "%s0\n", s);
				if (!strcmp(isp.answer, sRes) == 0) {
					return errISP_WRITE_RAM;
				}
			} else if ((stProg == stSEND_BYTES) && (!checkSumCheck)) {
				sformat(sRes, "%s", s);
				if (!strcmp(isp.answer, sRes) == 0) {
					return errISP_SEND_BYTES;
				}
			} else if ( (stProg == stSEND_BYTES) || ( (stProg == stPREPARE_PROG) && line) ) {
				sformat(sRes, "%sOK\n", s);
				if (!strcmp(isp.answer, sRes) == 0) {
					return errISP_CHECSUM;
				}
			} else if (stProg == stPROG_FLASH) {
				sformat(sRes, "%s0\n", s);
				if (!strcmp(isp.answer, sRes) == 0) {
					return errISP_PREPARE_TO_PROG;
				}
			} else if (stProg == stNEXT_SECTOR) {
				sformat(sRes, "%s0\n", s);
				if (!strcmp(isp.answer, sRes) == 0) {
					return errISP_WRITE_FLASH;
				}
			}
		}

		if (res < 0) return res; else return pdNOT_READY;
	}

	switch(stProg) {
	case stCHECK_SECTOR:
		// pegar a quantidade de bytes para programação
        lenProg = LPCtypes[isp.idxListLPC].SectorTable[sector];
        if (lenProg > isp.binaryLength - binaryIndex)
            lenProg = isp.binaryLength - binaryIndex;
		#if (ISP_USE_DEBUG == pdON)
		plognp("check sector: %d, lenProg: %d, len sector: %d"CMD_TERMINATOR, sector, lenProg, LPCtypes[isp.idxListLPC].SectorTable[sector]);
		#endif

		// checa se a parte do código que vai ser gravado no setor atual são todos 0xFF
		// Se for todos os bytes 0xFF vamos para o próximo setor, isto porque quando
		//	foi apagada a flahs todos os setores assumem valores 0xFF
		for (offset = 0; offset < lenProg; ++offset)  {
        	if (isp.binaryContent[binaryIndex + offset] != 0xFF) break;
        }

        if (offset == lenProg) { // all data contents were 0xFFs
			#if (ISP_USE_DEBUG == pdON)
			plognp("Whole sector %d contents is 0xFFs, skipping programming"CMD_TERMINATOR, sector);
			#endif
			stProg = stNEXT_SECTOR;
        } else {
			offset = 0; // Posição incial do byte no novo setor
			copyLen = 0;
			stProg = stSEND_SECTOR;
		}
		break;

	// envia os bytes do firmware corresponde ao setor atual
	case stSEND_SECTOR:
		// se não mais espaço para o setor, vamos para o próximo setor
		if (offset >= lenProg) { stProg = stNEXT_SECTOR; break;}

		// If the Flash ROM sector size is bigger than the number of bytes
		// we can copy from RAM to Flash, we must "chop up" the sector and
		// copy these individually.
		// This is especially needed in the case where a Flash sector is
		// bigger than the amount of SRAM.
		chunk = lenProg - offset;
		if (chunk > (unsigned)LPCtypes[isp.idxListLPC].MaxCopySize)
			chunk = LPCtypes[isp.idxListLPC].MaxCopySize;

		// Write multiple of 45 * 4 Byte blocks to RAM, but copy maximum of on sector to Flash
		// In worst case we transfer up to 180 byte too much to RAM
		// but then we can always use full 45 byte blocks and length is multiple of 4
		copyLen = chunk;
        if(LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC2XXX ||
           LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC17XX ||
           LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC13XX ||
           LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC11XX ||
           LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC18XX ||
           LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC43XX)
		{
			if ((copyLen % (45 * 4)) != 0)  copyLen += ((45 * 4) - (copyLen % (45 * 4)));
		}

        // enviar o comando para o bootcode sinalizando quantos dados deve ser recebido
		sformat(s, "W %d %d\r\n", ReturnValueLpcRamBase(), copyLen);
		sendCOM(s);

		byteBlock = binaryIndex;
		checksum = 0;
    	line = 0;
		stProg = stSEND_BYTES_BLOCK;
		break;

	// Transfer blocks of 45 * 4 bytes to RAM
	case stSEND_BYTES_BLOCK:
		if (byteBlock >= binaryIndex + offset + copyLen) {
			stProg = stPREPARE_PROG;
			break;
		}

		block = 0;
		stProg = stSEND_BYTES;
		sendChecksum = 0;
		checkSumCheck = 0;
		if (isp.status) isp.status(stsPROGRESS, isp.programdone, isp.error, byteBlock);
		break;

	// enviar os bytes formatados para o bootcode
	case stSEND_BYTES:
		if (sendChecksum) {
			sformat(s, "%d\r\n", checksum);
        	checkSumCheck = 1;
        	sendCOM(s);
            sendChecksum = 0;
            line = 0;
            checksum = 0;
        	break;
		}

		// se enviou todos os bytes dos blocos vams
		if (block >= 4) {
			byteBlock += (45 * 4);
			stProg = stSEND_BYTES_BLOCK;
			break;
		}

	    uint blockOffset;
        uint sidx;
		int c;
		int k = 0;

		// Uuencode one 45 byte block
		sidx = 0;
        s[sidx++] = (char)(' ' + 45);        // Encode Length of block

        for (blockOffset = 0; blockOffset < 45; blockOffset++) {
			c = isp.binaryContent[byteBlock + block * 45 + blockOffset];
			checksum += c;
            k = (k << 8) + (c & 255);
			if ((blockOffset % 3) == 2) {  // Collecting always 3 Bytes, then do processing in 4 Bytes
				s[sidx++] = uuencode_table[(k >> 18) & 63];
            	s[sidx++] = uuencode_table[(k >> 12) & 63];
            	s[sidx++] = uuencode_table[(k >>  6) & 63];
                s[sidx++] = uuencode_table[ k        & 63];
            }
       	}

       	s[sidx++] = '\r';
       	s[sidx++] = '\n';
       	s[sidx++] = 0;
       	checkSumCheck = 0;
       	sendCOM(s);
		block++;
		line++;

		if (line == 20)	sendChecksum = 1; // se passou 20 linhas é para mandar o checksum

		break;

	case stPREPARE_PROG:
		// se ainda tem restante de linhas a ser transmitida cuja não chegou a 20 linhas
		//	vamos eviar o checksum
		if (line) {
			sformat(s, "%d\r\n", checksum);
        	sendCOM(s);
        	line = 0;
        	break;
		}

        // Prepare command must be repeated before every write
		if (LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC43XX ||
        	LPCtypes[isp.idxListLPC].ChipVariant == CHIP_VARIANT_LPC18XX)
            sformat(s, "P %d %d 0\r\n", sector, sector);
        else sformat(s, "P %d %d\r\n", sector, sector);

		sendCOM(s);

		offset += chunk;
		stProg = stPROG_FLASH;
		break;

	case stPROG_FLASH:
        // Round copyLen up to one of the following values: 512, 1024,
        // 4096, 8192; but do not exceed the maximum copy size (usually
        // 8192, but chip-dependent)

		if (copyLen < 512)  		copyLen = 512;
        else if (lenProg < 1024)    copyLen = 1024;
        else if (lenProg < 4096)	copyLen = 4096;
        else  						copyLen = 8192;

        if (copyLen > (unsigned)LPCtypes[isp.idxListLPC].MaxCopySize)
        	copyLen = LPCtypes[isp.idxListLPC].MaxCopySize;

		sformat(s, "C %d %d %d\r\n", binaryIndex, ReturnValueLpcRamBase(), copyLen);
		sendCOM(s);
		stProg = stNEXT_SECTOR;
		break;

	// se há ainda mais byte do firmware a ser gravado, vamos para o próximo setor, senão retorna sinalizando que está concluído
	case stNEXT_SECTOR:
		binaryIndex += LPCtypes[isp.idxListLPC].SectorTable[sector];
		sector++;

		if (sector >= LPCtypes[isp.idxListLPC].FlashSectors) {
			#if (ISP_USE_DEBUG == pdON)
			plognp("Program too large; running out of Flash sectors. Sector actual, total of sectors"CMD_TERMINATOR, sector, LPCtypes[isp.idxListLPC].FlashSectors);
			#endif
			return errISP_PROGRAM_TOO_LARGE;
		} else if ((binaryIndex) >= isp.binaryLength) {
			#if (ISP_USE_DEBUG == pdON)
			plognp("Finish programming FLASH [binaryIndex %d isp.binaryLength %d]"CMD_TERMINATOR, binaryIndex, isp.binaryLength);
			#endif
			return pdREADY;
		} else {
			stProg = stCHECK_SECTOR;
			#if (ISP_USE_DEBUG == pdON)
			plognp("new sector: %d len: %d binaryIndex: %d"CMD_TERMINATOR, sector, LPCtypes[isp.idxListLPC].SectorTable[sector], binaryIndex);
			#endif
        }
		break;
	}

	return pdNOT_READY;
}
