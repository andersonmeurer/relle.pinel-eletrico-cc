#ifndef __LM35_H
#define __LM35_H

#include "_config_cpu_.h"
#include "_config_lib_sensor.h"

int lm35_ConvertToDegC(int adcVal);
int lm35_ConvertToDegF(int adcVal);

#endif
