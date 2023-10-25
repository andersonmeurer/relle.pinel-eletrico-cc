#ifndef __HCSR04_H
#define __HCSR04_H

#include "_config_cpu_.h"
#include "servus_drv.h"

#if (SERVO_N_MODULE <= 0 ) || (SERVO_N_MODULE > 6 )
#error O GERENCIADOR CONTROLA DE 1 A 6 SERVOS
#endif


#if (US04_USES == pdON)
#if (US04_N_MODULE <= 0 ) || (US04_N_MODULE > 4 )
#error O GERENCIADOR CONTROLA DE 1 A 4 MÓDULOS HC-SR04
#endif

#if (US04_TIME_MEASURE < 50)
#error O TEMPO DE MEDIÇÃO DO MÓDULO HC-SR04 NÃO PODE SER INFERIOR A 50mS
#else
#define US04_END_MEASURE (50+10)*100
#endif

#endif //#if (US04_USES == pdON)

void servus_Init(void);
void servus_Finish(void);
int servo_SetPos(uint servo, int us);
int servo_Move(uint servo, uint en);

#if (US04_USES == pdON)
void us04_StartMeasure(uint modeCont);
void us04_StopMeasure(void);
int us04_ReadMeasureCM(uint module);
#endif

#endif
