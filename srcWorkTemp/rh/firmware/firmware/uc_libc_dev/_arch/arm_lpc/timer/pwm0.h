#ifndef __PWM0_H
#define __PWM0_H

#include "_config_cpu_.h"

#if !defined (cortexm3)
#error NÃO FOI IMPLEMENTADO PWM PARA ESSA CPU OU A MESMA NÃO TEM SUPORTE
#endif

int pwm0_Init(void);
void pwm0_Off(void);
void pwm0_On(void);
void pwm0_SetPrescale(u32 prescale);
int pwm0_Frequency (int frequency);
//int pwm0_PulseWidthInMicroseconds (int us);
int pwm0_DutyCycle (int percentage);

#endif
