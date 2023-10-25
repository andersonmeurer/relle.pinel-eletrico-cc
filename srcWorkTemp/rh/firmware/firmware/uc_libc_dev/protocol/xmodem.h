#ifndef __XMODEM_H
#define __XMODEM_H

#include "_config_cpu_.h"

#if defined(FREE_RTOS)
#include "systime.h"
#else
#include "delay.h"
#endif

// DEFINIÇÃO DE QUANTO TEMPO A FUNÇÃO DE RECEPÇÃO DE DADOS ESPERA PELO DADO
#define XMODEM_TIMEROUT_1SEC	1000000L
#define XMODEM_TIMEROUT_2SEC	2000000L
#define XMODEM_TIMEROUT_3SEC	3000000L
#define XMODEM_TIMEROUT_4SEC	4000000L
#define XMODEM_TIMEROUT_5SEC	5000000L
#define XMODEM_TIMEROUT_6SEC	6000000L
#define XMODEM_TIMEROUT_7SEC	7000000L
#define XMODEM_TIMEROUT_8SEC	8000000L
#define XMODEM_TIMEROUT_9SEC	9000000L

void xmodem_Init(int (*sendbyte_func)(n16 c), int (*getbyte_func)(u8 *ch), u32 timerout, u8 maxRetrans);
int xmodem_Receive(u8 *dest, int destsz);
int xmodem_Transmit(u8 *src, int srcsz);


#endif
