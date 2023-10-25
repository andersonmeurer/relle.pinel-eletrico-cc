#ifndef __BEEP_DRV_H
#define __BEEP_DRV_H

// TODO beep ainda não testado no AVR
#include "_config_cpu_.h"
#include "_config_cpu_device.h"

#define beep_SetupPorts()			\
	do {							\
		BUZZER_DIR |= _BV(BUZZER);	\
		cbi(BUZZER_PORT, BUZZER);	\
	}	while(0)

#define beep_BuzzerOFF() cbi(BUZZER_PORT, BUZZER);
#define beep_BuzzerON() sbi(BUZZER_PORT, BUZZER);

#endif

