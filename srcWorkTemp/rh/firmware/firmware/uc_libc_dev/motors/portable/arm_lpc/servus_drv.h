#ifndef __SERVUS_DRV_H
#define __SERVUS_DRV_H

#include "_config_cpu_.h"
#include "_config_cpu_servus.h"

// calcular o intervalo do timer para gerar ticks de 10us
#define SERVUS_TICKS_PER_US (u32)((1e-6 * PCLK) + .5)
#define SERVUS_TICKS_PER_10US 10*SERVUS_TICKS_PER_US

#if (SERVUS_USE_TIMER == 1)
	#include "timer1.h"
	// Habilita timer sem prescaler e com interrupção em match0
	// Ajustar o timer chamar interrupção a cada 10us
	#define servus_TimeInit()														\
	do {																			\
		timer1_Init(1, T1TCR_ENABLE, T1MCR_MR0_INT_ENABLE|T1MCR_MR0_RESET );		\
		timer1_SetMatch0(SERVUS_TICKS_PER_10US);									\
	} while(0)

	// Anexa uma função a interrupção
	#define servus_SetIntHandler(f) timer1_SetIntHandler(TIMER1_VECTOR_INT_MATCH0, f);
	#define servus_TimeStop()		timer1_Set(T1TCR_STOP)
	#define servus_TimeEnable()		timer1_Set(T1TCR_ENABLE)
#elif (SERVUS_USE_TIMER == 2)
	#include "timer2.h"
	// Habilita timer sem prescaler e com interrupção em match0
	// Ajustar o timer chamar interrupção a cada 10us
	#define servus_TimeInit()														\
	do {																			\
		timer2_Init(1, T2TCR_ENABLE, T2MCR_MR0_INT_ENABLE|T2MCR_MR0_RESET );		\
		timer2_SetMatch0(SERVUS_TICKS_PER_10US);									\
	} while(0)

	// Anexa uma função a interrupção
	#define servus_SetIntHandler(f) 	timer2_SetIntHandler(TIMER2_VECTOR_INT_MATCH0, f);
	#define servus_TimeStop()			timer2_Set(T2TCR_STOP)
	#define servus_TimeEnable()			timer2_Set(T2TCR_ENABLE)
#endif

// ######################################################################################################
// CONTROLE DOS SERVOS

#define servo1_SetupPorts()	\
	do {							\
		SERVO1_DIR |= SERVO1;		\
		SERVO1_CLR = SERVO1;		\
	}	while(0)

#if (SERVO_N_MODULE > 1)
#define servo2_SetupPorts()	\
	do {							\
		SERVO2_DIR |= SERVO2;		\
		SERVO2_CLR = SERVO2;		\
	}	while(0)
#endif

#if (SERVO_N_MODULE > 2)
#define servo3_SetupPorts()	\
	do {							\
		SERVO3_DIR |= SERVO3;		\
		SERVO3_CLR = SERVO3;		\
	}	while(0)
#endif

#if (SERVO_N_MODULE > 3)
#define servo4_SetupPorts()	\
	do {							\
		SERVO4_DIR |= SERVO4;		\
		SERVO4_CLR = SERVO4;		\
	}	while(0)
#endif

#if (SERVO_N_MODULE > 4)
#define servo5_SetupPorts()	\
	do {							\
		SERVO5_DIR |= SERVO5;		\
		SERVO5_CLR = SERVO5;		\
	}	while(0)
#endif

#if (SERVO_N_MODULE > 5)
#define servo6_SetupPorts()	\
	do {							\
		SERVO6_DIR |= SERVO6;		\
		SERVO6_CLR = SERVO6;		\
	}	while(0)
#endif


// ######################################################################################################
// CONTROLE DO MÓDULOS ULTRASOM
#if (US04_USES == pdON)
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
#endif // #if (US04_USES == pdON)

#endif
