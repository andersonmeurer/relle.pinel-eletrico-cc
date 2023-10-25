#ifndef __HCSR04_H
#define __HCSR04_H

#include "_config_cpu_.h"
#include "_config_cpu_us.h"
#include "hc-sr04_drv.h"

#if (US04_N_MODULE <= 0 ) || (US04_N_MODULE > 4 )
#error O GERENCIADOR CONTROLA DE 1 A 4 M�DULOS HC-SR04
#endif

#if (US04_TIME_MEASURE < 50)
#error O TEMPO DE MEDI��O DO M�DULO HC-SR04 N�O PODE SER INFERIOR A 50mS
#else
#define US04_END_MEASURE (50+10)*100
#endif


typedef struct {
	volatile unsigned measure:1; 				// Sinaliza quando come�ar as medi��es
	volatile unsigned done:1;					// Sinaliza quando as medi��es est�o prontas
	//volatile uint cont;							// Flag do m�quina de estado de trabalho de gerenciador
	//volatile int module;						// Guarda o m�dulo atual para medi��o
	volatile uint measurements[US04_N_MODULE];
} tUs04, *pUs04;

void us04_Init( pUs04 us );
void us04_StartMeasure(void);
void us04_Trigger(uint module, uint action);
uint us04_Echo(uint module);
uint us04_ReadMeasureCM(int module);

#endif
