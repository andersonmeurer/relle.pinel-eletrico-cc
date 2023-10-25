#ifndef __FPGA_H
#define __FPGA_H
#include "_config_cpu_.h"

/*
//---------------------------------------------------------------------
// FPGA Register address
//---------------------------------------------------------------------
//		name				addr  w/r	size	description
#define fraID				0	//r		1		id do circuito
#define fraREV				1	//r		1		revisão do fpga
#define fraTEST				2	//r/w	1		porta de teste/cao de guarda
#define fraPORTA			3	//r		1		entradas PORTA
#define fraPORTB			4	//r		1		entradas PORTB
#define fraPORTC			5	//r		1		entradas PORTC
#define fraALTCRCW			6	//r		1		crc write
#define fraALTCRCR			7	//r		1		crc read
#define fraALTCRCA			8	//r		1		crc address

#define fraERROENC			10	//r		1		erro do encoder
#define fraROTACAO			12	//r		2		?

//inst_encoder
#define frbENCODER			16	//encoder
#define fraTIMERENC			(frbENCODER+0)		//r/w	4		tick count do encoder
#define fraLARGENC			(frbENCODER+4)		//r/w	4/1		largura do pulso do encoder
#define fraPOSENC			(frbENCODER+8)		//r		2		posição do encoder
#define fraLARGENCZ			(frbENCODER+12)		//r		4		largura sinal z

#define fraUART_CG			32	//		4		base address
#define fraUART_CL			36	//		4		base address
#define fraUART_RES			40	//		4		base address
#define fraUART_MT			44	//		4		base address
#define fraUART_MP			48	//		4		base address

#define fraMOTZ_VEL			64	//w		4		velocidade do motor z
#define fraMOTX_POS			80	//w		4		config pos setup
#define fraMOTY_POS			96	//w		4		config pos setup

//campos dos registros da serial
#define fraUART_BAUDL		0	//w		baud rate lsb
#define fraUART_BAUDH		1	//w		baud rate msb
#define fraUART_TXRXREG		2	//r/w	r=rxreg, w=txreg
#define fraUART_STATUS		3	//r		[0]enable,[1]rxif,[2]rxer,[3]trmt

//---------------------------------------------------------------------
// macros de acesso ao altera
//---------------------------------------------------------------------
//...
//macro de acesso as portas do altera
#define altwb(a,d)				devwb(devFPGA,a,d)
#define altww(a,d)				devww(devFPGA,a,d)
#define altwd(a,d)				devwd(devFPGA,a,d)
#define altrb(a)				devrb(devFPGA,a)
#define altrw(a)				devrw(devFPGA,a)
#define altrd(a)				devrd(devFPGA,a)
#define altrbit(a,mask)			((devrb(devFPGA,a)&mask)==mask)

//---------------------------------------------------------------------
// portas do altera
//---------------------------------------------------------------------
//...
//ja implementados
#define altVER()				altrb(fraID) //pior8_id
#define altREV()				altrb(fraREV) //pior8_rev
#define altCAO(x)				altwb(fraTEST, x)
#define	altUART_TXREG(base,c)	altwb(base+fraUART_TXRXREG, c)
#define altUART_RXREG(base)		altrb(base+fraUART_TXRXREG)
#define altPORTA()				altrb(fraPORTA) //pior8_porta
#define altPORTB()				altrb(fraPORTB) //pior8_portb
#define altPORTC()				altrb(fraPORTC) //pior8_portc
#define altTIMERENC()			altrd(fraTIMERENC) //inst_encoder
#define altTIMERENCCLR()		altwb(fraTIMERENC, 0)
#define altPOSENC()				altrw(fraPOSENC) //lpm_encoder6
#define altLARGENC()			altrd(fraLARGENC) //inst_encoder.inst_mlpsem
#define altLARGENCZ()			altrd(fraLARGENCZ) //inst_encoder.inst_cntx
#define altLARGENCMAX(x)		altwb(fraLARGENC, x) //lpm_multiplicador
#define altMOTZ_VEL(x) 			altwd(fraMOTZ_VEL, x) //inst_velmotz.
#define altMOTX_POS(x)			altwb(fraMOTX_POS, x) //inst_ctrlpos_motx
#define altMOTY_POS(x)			altwb(fraMOTY_POS, x) //inst_ctrlpos_moty
#define altCRCW()				altrb(fraALTCRCW) //inst_crcw
#define altCRCR()				altrb(fraALTCRCR) //inst_crcr
#define altCRCA()				altrb(fraALTCRCA) //inst_crca

//ainda nao implementados
#define altENCER()				altrb(fraERROENC)	//lpm_encoder6.erro_enc

//---------------------------------------------------------------------
// bitfield test macros
//---------------------------------------------------------------------
//...
//PORTA
#define altFLAGCABG()			altrbit(fraPORTA,BIT0)
#define altFLAGCABL()			altrbit(fraPORTA,BIT1)
#define altSWITCH1()			altrbit(fraPORTA,BIT2)
#define altSWITCH2()			altrbit(fraPORTA,BIT3)
#define altJUMP4()				altrbit(fraPORTA,BIT4)
#define altJUMP5()				altrbit(fraPORTA,BIT5)
#define altJUMP6()				altrbit(fraPORTA,BIT6)
//PORTAB
#define altALMPM()				altrbit(fraPORTB,BIT0)
#define altALMX()				altrbit(fraPORTB,BIT1)
#define altALMY()				altrbit(fraPORTB,BIT2)
#define altALMZ()				altrbit(fraPORTB,BIT3)
//PORTC
#define altENCSA()				altrbit(fraPORTC,BIT0)
#define altENCSB()				altrbit(fraPORTC,BIT1)
#define altENCSZ()				altrbit(fraPORTC,BIT2)
//UART
#define altUART_ENA(base)		altrbit(base+fraUART_STATUS,BIT0)
#define altUART_RXIF(base)		altrbit(base+fraUART_STATUS,BIT1)
#define altUART_RXER(base)		altrbit(base+fraUART_STATUS,BIT2)
#define altUART_TXIF(base)		altrbit(base+fraUART_STATUS,BIT3)
#define altCABG_RXIF()			altUART_RXIF(fraUART_CG)
#define altCABL_RXIF()			altUART_RXIF(fraUART_CL)

//---------------------------------------------------------------------
// uart
//---------------------------------------------------------------------
//...
char fpgaPUTu(char a, U8 c);
char fpgaGETu(char a, U8 * c);
char fpgaSETBAUDu(char a, long baud);
char fpgaRESETu(char a);
char fpgaRXDAu(char a);
char fpgaSTATUSu(char a);

//---------------------------------------------------------------------
// variáveis
//---------------------------------------------------------------------
//...

typedef struct {
	unsigned enabled: 1;
	unsigned check: 1;
	int error[3];
	int txcnt,rxcnt; //quantidade de dados transmitidos
	int txspd,rxspd; //dados no ultimo segundo
	U8 crcw,crcr,crca; //crc dos dados lidos/escritos/endereco
}tFPGA;

extern tFPGA fpga;

void lvds_rw(U8 a, U8 * d, U8 write);
void fpga_spd(void);
void fpga_check(void);
int fpga_crc(int n);
*/

int fpga_StartUp(void);
int fpga_BlockRead(u8* data, u8 addr, u8 len);
int fpga_BlockWrite(u8* data, u8 addr, u8 len);

#if (FPGA_USE_DEVIO == pdON)
#include "devio.h"
int devio_fpga(u8 op, void *data, u32 addr, u32 len);
#endif

#endif

