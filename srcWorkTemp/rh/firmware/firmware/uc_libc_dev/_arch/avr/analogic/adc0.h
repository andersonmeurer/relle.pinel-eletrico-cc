#ifndef __ADC0_H
#define __ADC0_H

#include <avr/interrupt.h>
#include "_config_cpu_.h"
#include "_config_cpu_adc.h"

void adc0_Init(u8 prescale, u8 reference, u8 result);
void adc0_Off(void);
void adc0_SetPrescaler(u8 prescale);
void adc0_SetReference(u8 ref);
void adc0_SetChannel(u8 ch);
void adc0_Start(u8 Channel);
int adc0_ReadNoWait(u16 *value);

#if (AD0_USE_INT_FUNCTIONS == pdON)
	void adc0_ClearIntHandler(void);
	void adc0_SetIntHandler(void(*Func)(u16));
#endif

#endif

