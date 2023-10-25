#ifndef DISPLAY7SEG_DRV_H_
#define DISPLAY7SEG_DRV_H_

#include "_config_cpu_.h"
#include "_config_cpu_display.h"

void disp7seg_SetupPorts(void);
void disp7seg_WriteSeg(int value);
void disp7seg_SetModule(int module);

#endif /* DISPLAY7SEG_H_ */
