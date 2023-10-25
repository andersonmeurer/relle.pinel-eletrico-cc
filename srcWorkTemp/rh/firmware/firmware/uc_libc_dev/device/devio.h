#ifndef __DEVIO_H
#define __DEVIO_H

#include "_config_cpu_.h"

// lista das operações realizadas pelos dispositivos
//---------------------------------------------------------------------
#define doWRITE			100	//gravação em disco
#define doREAD			101 //leitura de disco
#define doRESET			102	//resetar o dispositivo
#define doSTATUS		103 //device status
#define doELECSIG		104	//Eletronic Signature, Man/Dev
#define doGETSIZE		105 //tamanho do disco
#define doFREESPACE 	106 //espaço livre
#define doREADS 		107 //spare data (so usa com a NAND FLASH)
#define doWRITES 		108 //spare data (so usa com a NAND FLASH)
#define doERASE			109 //limpa regiao de memoria (setores)
#define doSTARTUP		110 //inicialização, como iniciar o altera
#define doEXEC			111 //compatibilidade com programas antigos
#define doSWAP			112 //compatibilidade com programas antigos

// lista padrão de dispositivos
//---------------------------------------------------------------------
#define devEEPROM		10	//serial eeprom (25LC640T)
#define devFLASH		11	//serial flash (M25P40)
#define devSDCARD		12	//sdcard
#define devNAND			13	//main nand flash
#define devFPGA			14	//altera
#define devRTC			15	//rtc
#define devSSD			16	//ssd1963

// Os dispositivo a ser registrados precisam seguir a padrão:
typedef int (*devioctl)(u8 op, void *data, u32 addr, u32 len);

int devioreg(int id, devioctl ctrl);
//	Exemplo de uma sequencia de registro:
//
//    devioreg(devFPGA,		devio_fpga);
//    devioreg(devNAND, 	devio_nand);
//    devioreg(devEEPROM,	devio_eeprom);
//    devioreg(devRTC,		devio_rtc);
//    devioreg(devSSD,		devio_ssd);

int devio(u8 dev, u8 op, void *data, u32 addr, u32 len);

// facilitadores de acesso a tipos de dados simples
#define devr(dev, data, addr, len)		devio(dev, doREAD, data, addr, len)
#define devw(dev, data, addr, len)		devio(dev, doWRITE, data, addr, len)

u8  	devrb(u8 dev, u32 adrr);			//byte
u16 	devrw(u8 dev, u32 addr);			//word
u32	 	devrd(u8 dev, u32 addr);			//double word
void 	devwb(u8 dev, u32 addr, u8 data);		//byte
void 	devww(u8 dev, u32 addr, u16 data);	//word
void 	devwd(u8 dev, u32 addr, u32 data);	//double word

#endif
