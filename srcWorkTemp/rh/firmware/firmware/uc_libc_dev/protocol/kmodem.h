#ifndef __KMODEM_H
#define __KMODEM_H

#include "_config_cpu_.h"

#define KMODEM_SIZE_HEADER			11 // tamanho do cabeçaho do protocolo

#define KMODEM_TIMEROUT_1SEC		1000000L
#define KMODEM_TIMEROUT_2SEC		2000000L
#define KMODEM_TIMEROUT_3SEC		3000000L
#define KMODEM_TIMEROUT_4SEC		4000000L
#define KMODEM_TIMEROUT_5SEC		5000000L
#define KMODEM_TIMEROUT_6SEC		6000000L
#define KMODEM_TIMEROUT_7SEC		7000000L
#define KMODEM_TIMEROUT_8SEC		8000000L
#define KMODEM_TIMEROUT_9SEC		9000000L


#if defined(FREE_RTOS)
#include "systime.h"
#else
#include "delay.h"
#endif

#define KMODEM_CMD_NACK 				0
#define KMODEM_CMD_ACK 					0xFF
#define KMODEM_CMD_GET_SIZE_FILE 		0x1
#define KMODEM_CMD_GET_SIZE_DATA	 	0x2
#define KMODEM_CMD_GET_DATA				0x4
#define KMODEM_CMD_FINISH 				0x8
#define KMODEM_CMD_CANCEL 				0x10

void kmodem_Init(
	int (*sendbyte_func)(n16 c),
	void (*flush_TX)(void),
	int (*getbyte_func)(u8 *ch),
	void (*flush_RX)(void),
	u32 timerout, u16 packageSize);
void kmodem_SetPackageSize( u16 packageSize);
void kmodem_SetTimerOut( u32 timerout);
int kmodem_Receive(u8 *dest, u32 sizedest, u32 *sizerx);
int kmodem_Transmit(u8 *buf, u32 sizebuf);


#endif
