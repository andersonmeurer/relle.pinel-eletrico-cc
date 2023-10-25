#ifndef __HX711_H
#define __HX711_H

#include "_config_cpu_.h"
#include "_config_lib_sensor.h"

#define HX711_SET_CHANNEL_A_128	25
#define HX711_SET_CHANNEL_A_64	26
#define HX711_SET_CHANNEL_B_32	27

void hx711_Init(uint tm);
void hx711_Reset(void);
int hx711_Config(int nPulse);
int hx711_GetValue(s32 *data);
void hx711_SetOffSet(s32 ofs);
s32 hx711_GetOffSet(void);
void hx711_SetScale(float scl);
float hx711_GetScale(void);
int hx711_ZeroAdjustment(void);
void hx711_Tick(void);
#if (HX711_USE_PROCESS == pdON)
void hx711_Process(void);
#endif

#endif
