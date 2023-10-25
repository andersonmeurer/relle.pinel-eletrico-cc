#ifndef __DISPLAY_7SEG_DRV_H
#define __DISPLAY_7SEG_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_display.h"
#include <util/delay.h>

#define delay_ms _delay_ms
#define delay_us _delay_us

#if (DISP_7SEG_INVERTED == pdOFF)
	#define disp_7SegLedOn(port, led)			sbi(port, led)
	#define disp_7SegLedOff(port, led)			cbi(port, led)
	#define disp_7SegModuleOn(port, module)		sbi(port, module)
	#define disp_7SegModuleOff(port, module)	cbi(port, module)
#else
	#define disp_7SegLedOn(port, led)			cbi(port, led)
	#define disp_7SegLedOff(port, led)			sbi(port, led)
	#define disp_7SegModuleOn(port, module)		cbi(port, module)
	#define disp_7SegModuleOff(port, module)	sbi(port, module)
#endif

#define disp_7SegSetOut(port, led)	sbi(port, led)

#endif
