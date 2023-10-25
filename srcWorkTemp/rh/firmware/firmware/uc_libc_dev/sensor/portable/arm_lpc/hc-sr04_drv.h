#ifndef __HCSR04_DRV_H
#define __HCSR04_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_us.h"

#define us04_SetupPortsModule1()				\
	do {										\
		US04_MODULE1_DIR |= US04_MODULE1_TRIGGER;	\
		US04_MODULE1_CLR = US04_MODULE1_TRIGGER;	\
	}	while(0)

#define us04_TriggerModule1(a)	{ if (a == pdON) US04_MODULE1_SET = US04_MODULE1_TRIGGER; else US04_MODULE1_CLR = US04_MODULE1_TRIGGER;}
#define us04_EchoModule1()		(US04_MODULE1_PIN&US04_MODULE1_ECHO)

#if (US04_N_MODULE > 1 )
#define us04_SetupPortsModule2()				\
	do {										\
		US04_MODULE2_DIR |= US04_MODULE2_TRIGGER;	\
		US04_MODULE2_CLR = US04_MODULE2_TRIGGER;	\
	} while(0)
#define us04_TriggerModule2(a)	{ if (a == pdON) US04_MODULE2_SET = US04_MODULE2_TRIGGER; else US04_MODULE2_CLR = US04_MODULE2_TRIGGER;}
#define us04_EchoModule2()		(US04_MODULE2_PIN&US04_MODULE2_ECHO)
#endif

#if (US04_N_MODULE > 2 )
#define us04_SetupPortsModule3()				\
	do {										\
		US04_MODULE3_DIR |= US04_MODULE3_TRIGGER;	\
		US04_MODULE3_CLR = US04_MODULE3_TRIGGER;	\
	} while(0)
#define us04_TriggerModule3(a)	{ if (a == pdON) US04_MODULE3_SET = US04_MODULE3_TRIGGER; else US04_MODULE3_CLR = US04_MODULE3_TRIGGER;}
#define us04_EchoModule3()		(US04_MODULE3_PIN&US04_MODULE3_ECHO)
#endif

#if (US04_N_MODULE > 3 )
#define us04_SetupPortsModule4()				\
	do {										\
		US04_MODULE4_DIR |= US04_MODULE4_TRIGGER;	\
		US04_MODULE4_CLR = US04_MODULE4_TRIGGER;	\
	} while(0)
#define us04_TriggerModule4(a)	{ if (a == pdON) US04_MODULE4_SET = US04_MODULE4_TRIGGER; else US04_MODULE4_CLR = US04_MODULE4_TRIGGER;}
#define us04_EchoModule4()		(US04_MODULE4_PIN&US04_MODULE4_ECHO)
#endif

// calcular o intervalo do timer para gerar ticks de 10us
#define US04_TICKS_PER_US (u32)((1e-6 * PCLK) + .5)
#define US04_TICKS_PER_10US 10*US04_TICKS_PER_US

#if ( US04_USE_TIMER == 2)
	#include "timer2.h"
	// Habilita timer sem prescaler e com interrupção em match0
	// Ajustar o timer chamar interrupção a cada 10us
	#define us04_TimeInit()														\
	do {																		\
		timer2_Init(1, T2TCR_ENABLE, T2MCR_MR0_INT_ENABLE|T2MCR_MR0_RESET );	\
		timer2_SetMatch0(US04_TICKS_PER_10US);									\
	} while(0)

	// Anexa uma função a interrupção
	#define us04_SetIntHandler(f) timer2_SetIntHandler(TIMER2_VECTOR_INT_MATCH0, f);
	#define us04_TimeStop()	timer2_Set(T2TCR_STOP)
	#define us04_TimeEnable()	timer2_Set(T2TCR_ENABLE)
#endif

#endif
