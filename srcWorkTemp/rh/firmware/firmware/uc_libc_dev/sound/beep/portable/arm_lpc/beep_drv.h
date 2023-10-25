#ifndef __BEEP_DRV_H
#define __BEEP_DRV_H

#include "_config_cpu_.h"

#define beep_SetupPorts()		\
	do {						\
		BUZZER_DIR |= BUZZER;	\
		BUZZER_OFF = BUZZER;	\
	}	while(0)

#define beep_BuzzerOFF() BUZZER_OFF = BUZZER
#define beep_BuzzerON() BUZZER_ON = BUZZER

#endif
