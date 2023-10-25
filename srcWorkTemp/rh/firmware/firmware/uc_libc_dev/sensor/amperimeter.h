#ifndef __AMPERIMETER_H
#define __AMPERIMETER_H

#include "_config_cpu_.h"
#include "_config_lib_sensor.h"

void amp_Init(uint tidx, double vcc, double rs, double rl);
double amp_ValueVoltage(uint tidx, uint dec);
double amp_ConvertToAmperage(uint tidx, uint adc);

#endif
