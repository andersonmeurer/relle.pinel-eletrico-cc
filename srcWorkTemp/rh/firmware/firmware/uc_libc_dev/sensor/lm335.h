#ifndef __LM335_H
#define __LM335_H

#include "_config_cpu_.h"
#include "_config_lib_sensor.h"

int lm335_ConvertToDegC(int adcVal);
int lm335_ConvertToDegF(int adcVal);

#endif
