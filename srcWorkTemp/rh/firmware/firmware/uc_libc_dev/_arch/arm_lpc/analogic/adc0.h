#ifndef __ADC0_H 
#define __ADC0_H

#include "_config_cpu_.h"

void adc0_Init(u32 adc_clk, u32 bit);
void adc0_Start(u8 channel, u32 start, u32 edge);
void adc0_Stop(void);

#if defined(LPC2468) || defined(cortexm3)
	int adc0_Read(u8 channel, u16 *value);
	int adc0_ReadNoWait(u8 channel, u16 *value);
#else
	int adc0_Read(u16 *value);
	int adc0_ReadNoWait(u16 *value);
#endif

#endif

