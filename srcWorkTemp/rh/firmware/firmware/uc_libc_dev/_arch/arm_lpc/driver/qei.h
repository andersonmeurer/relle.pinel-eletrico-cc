#ifndef __QEI_H
#define __QEI_H

#include "_config_lib_.h"

#define qeiVALUE_ABSOLUTE 	0
#define qeiVALUE_IN_US 		1

void qei_Init(void);
void qei_Config(uint config);
void qei_Reset(uint resetType);
void qei_SetFilter(uint samplingPulse, int reloadType);
void qei_SetTimerReload(uint reloadValue, int reloadType);
void qei_SetMaxPosition(uint maxPos);
void qei_SetPositionComp(u8 posCompCh, u32 value);
void qei_SetIndexComp(uint value);
void qei_SetVelocityComp(uint value);
uint qei_GetIndex(void);
uint qei_GetTimer(void);
uint qei_GetVelocity(void);
uint qei_GetVelocityCap(void);
int qei_GetDirection(void);
uint qei_GetPosition(void);
uint qei_CalculateRPM(uint velCapValue, uint ppr);
void qei_SetIntHandler(void (*intFunc) (int), uint intType);
void qei_ClearIntHandler(void);
int qei_GetIntStatus(uint ints, uint intType);
void qei_IntClear(uint intType);
void qei_IntSet(uint intType);


#endif // #ifndef __QEI_H
