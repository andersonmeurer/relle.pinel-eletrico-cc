#ifndef __PROG_FLASH_H
#define __PROG_FLASH_H

#include "_config_cpu_.h"

#if defined(FREE_RTOS)
#include "sys_time.h"
#else
#include "delay.h"
#endif

void modem_Init(
	int (*sendbyte_func)(n16 c),
	int (*getbyte_func)(u8 *ch),
	u32 timerout
);

void modem_SetTimerOut( u32 timerout);
int modem_Receive(void);
void modem_cmdAck(void);
void modem_cmdCancel(void);

#endif
