//---------------------------------------------------------------------
// protocolo
//---------------------------------------------------------------------
//...
/*

ddr protocol: subida e descida

sample data: 0x55

sdo	--- \_7_/-6-\_5_/-4-\_3_/-2-\_1_/-0--- ---
sck	___ __/---\___/---\___/---\___/---\___ ___
msb ??? ??\______7_______5_______3_______1 ???
lsb ??? ??/------6-------4-------2-------0 ???

duração do bit: 100ns

nada de crc, comunicação direto

	TX (arm->fpga): [CMD, ADDR, T0, T1, ...]
	RX (arm<-fpga): [???, ????, R0, R1, ...]

CMD: comando
[0]: 0=tx, 1=rx
[1]: 0=no inc, 1=inc addr

============================

exemplo do dado 0x55
bit		 7766554433221100
sck	____ _H_H_H_H_H_H_H_H ____
sdi	xxxx __HH__HH__HH__HH xxxx

a comunicação precisa ser em sequencia e sem interrupções, pois um delay
de apenas 500ns eh o que reseta a comunicação com o fpga

	pacote:
	TX (arm->fpga): [CMD, ADDR, TXD0, CRC]
	RX (arm<-fpga): [RXD0, RXD1]

	CMD: comando
		bit[0]=write operation (generate strobe)
		bit[1]=read operation (generate strobe)
		bit[2]=correction package

	[escrita]
	cpu envia pacote para fpga, e recebe a resposta.

*/

//#include <stdio.h>
//#include <utils/xstdio.h>
//#include <port/xdelay.h>
//#include "cpu.h"
//#include "pio.h"
/*
//---------------------------------------------------------------------
// variáveis
//---------------------------------------------------------------------
tFPGA fpga;

//---------------------------------------------------------------------
// LINK LVDS
//---------------------------------------------------------------------

int lvds_error(int id, cpchar msg) {
	fpga.error[id]++;
	fpga.enabled = 0;
	showmsg(msg);
	return 1;
}



//---------------------------------------------------------------------
// uarts
//---------------------------------------------------------------------
//...
char fpgaPUTu(char a, u8 c) {
	altwb(a+fraUART_TXRXREG,c);
	return 0;
}

char fpgaGETu(char a, u8 * c) {
	*c = altrb(a+fraUART_TXRXREG);
	return 0;
}

char fpgaSETBAUDu(char a, long baud) {
	altww(a+fraUART_BAUDL,100000000l/baud-1);
	return 0;
}

char fpgaRESETu(char a) {
	altwb(a+fraUART_STATUS,0); //desliga a serial
	altwb(a+fraUART_STATUS,1); //liga novamente
	return 0;
}

char fpgaRXDAu(char a) {
	return altUART_RXIF(a);
}

char fpgaSTATUSu(char a) {
	return altrb(a+fraUART_STATUS);
}

void fpga_uart_init(void) {
	const int uartbase[] = {fraUART_CG, fraUART_CL, fraUART_RES, fraUART_MT, fraUART_MP};
	int i;
	//inicia a serial
	for (i=0;i<5;i++) {
		fpgaSETBAUDu(uartbase[i],1250000);
		fpgaRESETu(uartbase[i]);
	}
}

//---------------------------------------------------------------------
// disco
//---------------------------------------------------------------------
//calcula a velocidade de comunicação
//chamar esta rotina pela interrupção com intervalos de 1ms
void fpga_spd(void) {
	static int time=0; //conta 1s
	static int tx=0, rx=0; //registra o ultimo valor
	//deu 1 segundo?
	if (time++<1000) return;
	time = 0;
	//calcula a diferenca
	fpga.txspd = fpga.txcnt - tx;
	fpga.rxspd = fpga.rxcnt - rx;
	//salva
	tx = fpga.txcnt;
	rx = fpga.rxcnt;
}

//n: tipo do crc, 0=reset, 1=write, 2=read, 3=address
int fpga_crc(int n) {
	if (n==0) {
		fpga.crcw = altCRCW();
		fpga.crcr = altCRCR()*2;
		fpga.crca = altCRCA();
		fpga.check = 0;
		return 0;
	}
	if (n==1) return (u8)(altCRCW() - fpga.crcw);
	if (n==2) return (u8)(altCRCR()*2 - fpga.crcr);
	if (n==3) return (u8)(altCRCA() - fpga.crca);
	return 0;
}

//verifica o check do altera
void fpga_check(void) {
	static tTIME t = 0;
	//controla a frequencia de verificação
	if (Now()<t) return;
	t = Now() + 100;
	//deu erro?
	if (fpga_crc(1)) fpga.check = 1;
	if (fpga_crc(2)) fpga.check = 1;
	if (fpga_crc(3)) fpga.check = 1;
}
*/























#include "fpga.h"
#include "fpga_drv.h"

// Gerar o arquivo de configurações executando make_fpga_file.bat da pasta app
// Leia o documento make_fpga_file.txt para mais detalhes
// O arquvio gerado ja deve vir pronto para ser enviado pela spi,
// 	ou seja, ja deve vir com o byte no modo big-endian
#if (FPGA_USE_APP == 0)
static u8* altera_bin;
#elif (FPGA_USE_APP == 1)
#include "fpga_app_test.h"
#elif  (FPGA_USE_APP == 2)
#include "fpga_app_pr112v2.h"
#else
#error NÃO FOI DEFINIDO O ARQUIVO DE CONFIGURAÇÃO DO FPGA VER _config_lib_fpga.h
#endif

static void fpga_BlockRW(u8 a, u8 * d, u8 write);
static void fpga_rw(u8 cmd, u8 a, u8 * x, u8 write);

int fpga_StartUp(void) {
	u32 i;
//
//	fpga.enabled = 1;
	// TODO - Colocar opção para escolher qual SPI
	for (i = 0; i < sizeof(altera_bin); i++)
		spi_TransferByte(altera_bin[i]);

//	delayms(50);
//
//	if ((altVER()!=50)||(altREV()!=15))
//		showmsg("E107: Versao Altera (%d.%d)",altVER(),altREV()); //xxx E107 versao alt
//
//	fpga_uart_init();
//	altLARGENCMAX(255);
//	fpga_crc(0);

	return pdPASS;
}


int fpga_BlockRead(u8* data, u8 addr, u8 len) {
	while (len--) fpga_BlockRW(addr++, data++, 0);
	return pdPASS;
}

int fpga_BlockWrite(u8* data, u8 addr, u8 len) {
	while (len--) fpga_BlockRW(addr++, data++, 1);
	return pdPASS;
}


static void fpga_BlockRW(u8 addr, u8* data, u8 write) {
	u8 cmd = 0;
	//desligado?

//	if (!fpga.enabled) { *d = 0; return; }
//	fpga.crca += a;
//	if (write==1) {
//		fpga.crcw += *d;
//		fpga.txcnt++;
//	}

	if (write) cmd = BIT_0; else cmd = BIT_1;
	fpga_rw(cmd, addr, data, write);
//	if (write==0) {
//		fpga.crcr += *d;
//		fpga.rxcnt++;
//	}
}

//a: address
//d: buffer data
//write: 1=write, 0=read
static void fpga_rw(u8 cmd, u8 a, u8 * x, u8 write) {
	/*
	u8 d[4], sde, c;
	//ainda esta ativado? da um tempo para resetar...
	if (pinLVDS_SDE) {
		delayus(1);
		if (pinLVDS_SDE) {
			lvds_error(0,"E104: LVDS sde"); //xxx E104 lvds sde
			return;
		}
	}
	c = 0;
	u8 crc = -(a + *x + cmd);
	do {
		//executa a operação
		DI();
		SSP0DR = cmd;
		SSP0DR = a;
		SSP0DR = *x;
		SSP0DR = crc;
		if (!write) {
			SSP0DR = 0;
			SSP0DR = 0;
			SSP0DR = 0;
			SSP0DR = 0;
		}
		while (SSP0SR_bit.BSY);
		sde = pinLVDS_SDE;
		d[0] = SSP0DR;
		d[0] = SSP0DR;
		d[0] = SSP0DR;
		d[0] = SSP0DR;
		if (!write) {
			d[0] = SSP0DR;
			d[1] = SSP0DR;
			d[2] = SSP0DR;
			d[3] = SSP0DR;
		}
		EI();
		if (c>0) fpga.error[1]++;
	}while ((sde==0)&&(c++<5));
	//apenas escrita?
	if (write) return;
	//deu erro?
	if (sde==0) {
		 lvds_error(1,"E105: LVDS write"); //xxx E105 lvds write
		return;
	}
	//algum deu certo?
	if (d[0]==d[1]) { *x = d[0]; return; } fpga.error[2]++;
	if (d[1]==d[2]) { *x = d[1]; return; } fpga.error[2]++;
	if (d[2]==d[3]) { *x = d[2]; return; }
	//nenhum deu certo...
	lvds_error(2,"E106: LVDS read"); //xxx E106 lvds read
	printf("read %d %d %d",d[0],d[1],d[2]);
	*/
}


// ###########################################################################################
// DEVIO
#if (FPGA_USE_DEVIO == pdON)
int devio_fpga(u8 op, void *data, u32 addr, u32 len) {
    u8 r = pdFAIL;
    switch (op) {
        case doREAD     : r = fpga_BlockRead(data, addr, len);	break;
        case doWRITE    : r = fpga_BlockWrite(data, addr, len);    break;
        case doSTARTUP  : r = fpga_StartUp(); break;
        default: break;
    }
    return r;
}
#endif
