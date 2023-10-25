#ifndef __VOLTIMETER_H
#define __VOLTIMETER_H

#include "_config_cpu_.h"
#include "_config_lib_sensor.h"

void volt_Init(uint tidx, double vcc, double rin, double rdown);
double volt_ValueVoltage(uint tidx, uint dec);
double volt_ConvertToVoltage(uint tidx, uint adc);

#endif
