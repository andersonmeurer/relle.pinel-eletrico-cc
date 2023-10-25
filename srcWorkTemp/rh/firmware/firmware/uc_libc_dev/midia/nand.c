// CONTROLE DA MEMÓRIA NAND FLASH EXTERNA
//
//    Programa baseado nas informações contidas no datasheet:
//
//   H27US08121B-TPCB.pdf"
//
//	Modelos atualmente conhecidos
//	Part Number     Manufacture Code    Device Code Bus Width   Size (MBytes)	Manufactur
//	HY27US08121M    ADh                 76h         x8          64				hynix
//	NAND512W3A2S 	20h					76h			x8			64				numonix


#include "nand.h"
#include "nand_drv.h"

#if (NAND_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

#if defined(FREE_RTOS)
#include "systime.h"
signed int enc28j60_WaitForData(portTickType delay);
#else
#include "delay.h"
#endif

static void nand_Read(u8* data);
static void nand_Write(u8 data);
static void nand_WriteCMD(u8 cmd);
static void nand_WrAddr(u32 addr, u8 f);
static void nand_Wait(void);
static u8 nand_Status(void);

u8 nand_StartUp(void) {
	u8 s[2]; //s[0]=fabricante, s[1]=device

	nand_SetupPorts();
	nand_GetCode(s);

	#if (NAND_USE_DEBUG == pdON)
	plog("NAND: Startup Intel sig 0x%x, 0x%x"CMD_TERMINATOR, s[0], s[1]);
	#endif

	//hynix
	if ((s[0]==0xAD)&&(s[1]==0x76)) {
		#if (NAND_USE_DEBUG == pdON)
		plog("NAND: umonix NAND512W3A2S de 64MB"CMD_TERMINATOR);
		#endif

		return 0;
	}

	//st ou numonix
	if ((s[0]==0x20)&&(s[1]==0x76)) {
		#if (NAND_USE_DEBUG == pdON)
		plog("NAND: st ou numonix"CMD_TERMINATOR);
		#endif

		return 0;
	}

	#if (NAND_USE_DEBUG == pdON)
	plog("NAND: defeito"CMD_TERMINATOR);
	#endif

	return 1;
}

u8 nand_GetCode(u8* code) {
    return nand_ReadBlock(code, 0, 2, 0x90);
}


//cmd: 0=data, 0x50=spare data, 0x70=status, 0x90=man code
u8 nand_ReadBlock(u8* data, u32 addr, u32 len, u8 cmd) {
	nand_ClrCE();
	nand_WriteCMD(cmd);
	nand_WrAddr(addr, 1);
    nand_Wait();
    while (len--) nand_Read(data++);
    nand_SetCE();
    return 0;
}

//esta função le os 16 spare bytes da pagina selecionada
//n=numero da pagina, c=numero de paginas
u8 nand_ReadSpare(u8* data, u32 n, u16 len) {
    long a = n*512L;
    while (len--) {
        if (nand_ReadBlock(data, a, 16, 0x50)) return 1;
        a += 512;
        data += 16;
    }
    return 0;
}


//cmd: 0x00=data
u8 nand_WriteBlock(u8* data, u32 addr, u32 len, u8 cmd) {
    nand_SetWP();
    nand_ClrCE();
    nand_WriteCMD(cmd);
    nand_WriteCMD(0x80);
    nand_WrAddr(addr, 1);
    while (len--) nand_Write(*data++);
    nand_WriteCMD(0x10);
    nand_Wait();
    nand_SetCE();
    nand_ClrWP();
    return (nand_Status() & BIT_0);
}

//Rotina está prepara para limpar blocos de 16K (32 páginas de 512)
//n=numero do setor
u8 nand_Erase(u32 n) {
    long a = n*16384L;
    nand_SetWP();
    nand_ClrCE();
    nand_WriteCMD(0x60);
    nand_WrAddr(a,0);
    nand_WriteCMD(0xD0);
    nand_Wait();
    nand_SetCE();
    nand_ClrWP();
    return (nand_Status()&BIT_0);
}


//le dado enviado pela nand
static void nand_Read(u8* data) {
	nand_ClrRE();
    *data = nand_BusDataRD();
	nand_SetRE();
}

//grava um dado na porta
static void nand_Write(u8 data) {
	nand_BusDataWR(data);
	nand_BusDataSetOut();
	nand_ClrWE();
	nand_SetWE();
	nand_BusDataSetIn();
}

//grava um comando
static void nand_WriteCMD(u8 cmd) {
	nand_SetCLE();
    nand_Write(cmd);
    nand_ClrCLE();
}

//grava o endereco da nand
//f: full address
static void nand_WrAddr(u32 addr, u8 f) {
	nand_SetALE();
    if (f) nand_Write(addr);
    nand_Write(addr>>9);
    nand_Write(addr>>17);
    nand_Write(addr>>25);
	nand_ClrALE();
}

//aguarda o sinal de busy subir
static void nand_Wait(void) {
    int i;
    //timeout de 5ms
    for (i=0;i<1000;i++) {
        delay_us(5);
        if (!nand_Busy()) return;
    }
}

//BIT7    Write Protection
//        '1' Not Protected
//        '0' Protected
//BIT6    Program/Erase/Read Controller
//        '1' P/E/R C Inactive, device ready
//        '0' P/E/R C active, device busy
//BIT0    Generic Error
//        '1' Error - Operation failed
//        '0' No Error - Operation successful

//le o registrador de status da memória
static u8 nand_Status(void) {
    u8 r;
    nand_Wait();
    nand_ClrCE();
    nand_WriteCMD(0x70);
    nand_Read(&r);
    nand_SetCE();
    return r;
}


// ###########################################################################################
// DEVIO
#if (NAND_USE_DEVIO == pdON)

u8 nand_ReadData(u8* data, u32 addr, u16 len) {
    return nand_ReadBlock(data, addr, len, 0x00);
}

u8 nand_WriteData(u8* data, u32 addr, u16 len) {
    return nand_WriteBlock(data, addr, len, 0x00);
}


int devio_nand(u8 op, void* data, u32 addr, u32 len) {
	int r = 1;
    switch (op) {
        case doREAD     : r = nand_ReadData(data , addr, len);		break;
        case doWRITE    : r = nand_WriteData(data , addr, len);		break;
        case doREADS    : r = nand_ReadSpare(data , addr, len);		break;
        case doERASE    : r = nand_Erase(addr);						break;
        case doELECSIG	: r = nand_GetCode(data);					break;
        case doSTARTUP	: r = nand_StartUp();						break;
        default : break;
    }
    return r;
}

#endif

