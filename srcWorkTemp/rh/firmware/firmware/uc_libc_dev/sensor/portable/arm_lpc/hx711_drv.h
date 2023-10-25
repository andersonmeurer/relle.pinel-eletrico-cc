#ifndef __HX711_DRV_H
#define __HX711_DRV_H

#include "_config_cpu_device.h"

#define hx711_SetupPorts()		\
	do {						\
		HX711_PDSCK_DIR |= HX711_PDSCK;	\
		HX711_PDSCK_LOW = HX711_PDSCK;	\
	}	while(0)

#define hx711_DOUT() 		((HX711_DOUT_PIN & HX711_DOUT)>0)?1:0
#define hx711_ClockHigh() 	HX711_PDSCK_HIGH = HX711_PDSCK
#define hx711_ClockLow() 	HX711_PDSCK_LOW = HX711_PDSCK

#endif
