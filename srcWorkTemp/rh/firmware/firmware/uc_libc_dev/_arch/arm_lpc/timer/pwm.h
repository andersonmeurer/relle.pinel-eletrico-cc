#ifndef __PWM_H
#define __PWM_H

#include "_config_cpu_.h"
#include "vic.h"


#if !defined (PWM_PRIO_LEVEL)
#error N�O FOI DEFINIDO O N�VEL DE INTERRUP��O DO PWM NO _config_cpu_.h
#endif

int pwm_Init(void);
void pwm_Set(u32 Cycle, u32 OffSet);
void pwm_Start( void );
void pwm_Stop( void );

#endif

