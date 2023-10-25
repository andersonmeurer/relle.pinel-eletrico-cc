#ifndef __TERMISTOR_H
#define __TERMISTOR_H

#include "_config_cpu_.h"
#include "_config_lib_sensor.h"

#define tempCELSIUS		0
#define tempFAHRENHEIT	1
#define tempKELVIN 		2

void term_Init(uint tidx, double vcc, double rs, double a, double b, double c);
double term_ValueVoltage(uint tidx, uint dec);
double term_ValueRes(uint tidx, double vs);
double term_SteinhartHart(uint tidx, double res);
double term_ConvertToTemp(uint tidx, uint adc, uint retTemp);
void term_ConvertToTempS(uint tidx, uint adc, pchar s, uint retTemp);
void term_TempRound(double* temp);

#define convKelvin2Celcius(t) 		((double)(t-273.15))
#define convCelcius2Kelvin(t) 		((double)(t+273.15))
#define convKelvin2Fahrenheit(t) 	((double)( (t-273.15)*1.8+32))
#define convCelcius2Fahrenheit(t) 	((double)( t*1.8+32) )
#define convFahrenheit2Celcius(t) 	((double)( (t-32)/1.8))

#endif
