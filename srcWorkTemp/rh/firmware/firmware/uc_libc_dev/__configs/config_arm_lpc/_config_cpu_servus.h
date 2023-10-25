#ifndef __CONFIG_CPU_SERVUS_H
#define __CONFIG_CPU_SERVUS_H

#include "_config_cpu_.h"

#define SERVO_N_MODULE 1 		// Define quantos servos o sistema vai gerenciar (MAX 6)

// SERVO1
#define SERVO1_DIR GPIO0_DIR 	// Porta de direção dos pinos
#define SERVO1_SET GPIO0_SET	// Porta para ligar os pinos
#define SERVO1_CLR GPIO0_CLR	// Porta para desligar os pinos
	#define SERVO1	BIT_20

#define SERVO2_DIR GPIO0_DIR 	// Porta de direção dos pinos
#define SERVO2_SET GPIO0_SET	// Porta para ligar os pinos
#define SERVO2_CLR GPIO0_CLR	// Porta para desligar os pinos
	#define SERVO2	BIT_11

#define SERVO3_DIR GPIO0_DIR 	// Porta de direção dos pinos
#define SERVO3_SET GPIO0_SET	// Porta para ligar os pinos
#define SERVO3_CLR GPIO0_CLR	// Porta para desligar os pinos
	#define SERVO3	BIT_10

#define SERVO4_DIR GPIO1_DIR 	// Porta de direção dos pinos
#define SERVO4_SET GPIO1_SET	// Porta para ligar os pinos
#define SERVO4_CLR GPIO1_CLR	// Porta para desligar os pinos
	#define SERVO4	BIT_22

#define SERVO5_DIR GPIO2_DIR 	// Porta de direção dos pinos
#define SERVO5_SET GPIO2_SET	// Porta para ligar os pinos
#define SERVO5_CLR GPIO2_CLR	// Porta para desligar os pinos
	#define SERVO5	BIT_0

#define SERVO6_DIR GPIO2_DIR 	// Porta de direção dos pinos
#define SERVO6_SET GPIO2_SET	// Porta para ligar os pinos
#define SERVO6_CLR GPIO2_CLR	// Porta para desligar os pinos
	#define SERVO6	BIT_19

// ###################################################################################################################
// ULTRASOM HC-SR04
#define US04_USES pdOFF // Habilita ou não o uso do módulo de ultrasom junto com o controlador de servos

#if (US04_USES == pdON)
#define US04_N_MODULE 4 	// Define quantos módulos de ultrasom o sistema vai gerenciar (MAX 4)

// DEFINIR O TEMPO EM MILISEGUNDOS QUE O MÓDULO NECESSITA PARA FAZER AS MEDIÇÕES
#define US04_TIME_MEASURE 50 // Não pode ser inferior a 50ms


// CONFIGURAR OS PINOS TRIGGER E ECHO DE CADA MÓDULO.
	//		CADA CONJUNTO DE PINOS DE CADA MÓDULO DEVEM PERTENCER A MESMA PORTA

// MODULO 1
#define US04_MODULE1_DIR GPIO2_DIR 	// Porta de direção dos pinos
#define US04_MODULE1_SET GPIO2_SET	// Porta para ligar os pinos
#define US04_MODULE1_CLR GPIO2_CLR	// Porta para desligar os pinos
#define US04_MODULE1_PIN GPIO2_PIN	// Porta de entrada dos pinos
	#define US04_MODULE1_TRIGGER 	BIT_2
	#define US04_MODULE1_ECHO		BIT_3

// MODULO 2
#define US04_MODULE2_DIR GPIO2_DIR 	// Porta de direção dos pinos
#define US04_MODULE2_SET GPIO2_SET	// Porta para ligar os pinos
#define US04_MODULE2_CLR GPIO2_CLR	// Porta para desligar os pinos
#define US04_MODULE2_PIN GPIO2_PIN	// Porta de entrada dos pinos
	#define US04_MODULE2_TRIGGER 	BIT_4
	#define US04_MODULE2_ECHO		BIT_5

// MODULO 3
#define US04_MODULE3_DIR GPIO2_DIR 	// Porta de direção dos pinos
#define US04_MODULE3_SET GPIO2_SET	// Porta para ligar os pinos
#define US04_MODULE3_CLR GPIO2_CLR	// Porta para desligar os pinos
#define US04_MODULE3_PIN GPIO2_PIN	// Porta de entrada dos pinos
	#define US04_MODULE3_TRIGGER 	BIT_6
	#define US04_MODULE3_ECHO		BIT_7

// MODULO 4
#define US04_MODULE4_DIR GPIO2_DIR 	// Porta de direção dos pinos
#define US04_MODULE4_SET GPIO2_SET	// Porta para ligar os pinos
#define US04_MODULE4_CLR GPIO2_CLR	// Porta para desligar os pinos
#define US04_MODULE4_PIN GPIO2_PIN	// Porta de entrada dos pinos
	#define US04_MODULE4_TRIGGER 	BIT_8
	#define US04_MODULE4_ECHO		BIT_9

#if US04_USE_DEBUG == pdON
#define US04_DBGTRIGGER_DIR 	GPIO0_DIR
#define US04_DBGTRIGGER_SET 	GPIO0_SET
#define US04_DBGTRIGGER_CLR 	GPIO0_CLR
#define US04_DBGTRIGGER			BIT_22

#define US04_DBGTRIGGEROK_DIR 	GPIO0_DIR
#define US04_DBGTRIGGEROK_SET 	GPIO0_SET
#define US04_DBGTRIGGEROK_CLR 	GPIO0_CLR
#define US04_DBGTRIGGEROK		BIT_7

#define US04_DBGDETECTED_DIR 	GPIO0_DIR
#define US04_DBGDETECTED_SET 	GPIO0_SET
#define US04_DBGDETECTED_CLR 	GPIO0_CLR
#define US04_DBGDETECTED		BIT_8
#endif
#endif //#if (US04_USES == pdON)

#endif
