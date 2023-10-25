#ifndef __RH_HIH4000_H
#define __RH_HIH4000_H

#include "_config_cpu_.h"
#include "_config_lib_sensor.h"

void ihi4000_Init(uint sidx, double svcc, uint r1, uint r2, double adcvcc, uint adcmax);
int ihi4000_ConvertToRH(uint sidx, uint adc, double temperature);

#endif
