#ifndef __PWM1_H
#define __PWM1_H

#include "_config_cpu_.h"

#if !defined (cortexm3)
#error NÃO FOI IMPLEMENTADO PWM PARA ESSA CPU OU A MESMA NÃO TEM SUPORTE
// testado no lpc1752
#endif

int pwm1_Init(void);
void pwm1_Off(void);
void pwm1_On(void);
void pwm1_SetPrescale(u32 prescale);
int pwm1_Frequency (int frequency);
//int pwm1_PulseWidthInMicroseconds (int us);
int pwm1_DutyCycle (int percentage);

#endif
