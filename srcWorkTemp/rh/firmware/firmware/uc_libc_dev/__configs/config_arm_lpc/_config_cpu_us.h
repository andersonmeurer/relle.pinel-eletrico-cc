#ifndef __CONFIG_CPU_US_H
#define __CONFIG_CPU_US_H

#include "_config_cpu_.h"

// ###################################################################################################################
// ULTRASOM HC-SR04

// DEFINIR O TEMPO EM MILISEGUNDOS QUE O MÓDULO NECESSITA PARA FAZER AS MEDIÇÕES
#define US04_TIME_MEASURE 50 // Não pode ser inferior a 50ms

#define US04_N_MODULE 4 	// Define quantos módulos de ultrasom o sistema vai gerenciar (MAX 4)
	// CONFIGURAR OS PINOS TRIGGER E ECHO DE CADA MÓDULO.
	//		CADA CONJUNTO DE PINOS DE CADA MÓDULO DEVEM PERTENCER A MESMA PORTA

	// MODULO 1
	#define US04_MODULE1_DIR GPIO2_DIR 	// Porta de direção dos pinos
	#define US04_MODULE1_SET GPIO2_SET	// Porta para ligar os pinos
	#define US04_MODULE1_CLR GPIO2_CLR	// Porta para desligar os pinos
	#define US04_MODULE1_PIN GPIO2_PIN	// Porta de entrada dos pinos
		#define US04_MODULE1_TRIGGER 	BIT_4
		#define US04_MODULE1_ECHO		BIT_5

	// MODULO 2
	#define US04_MODULE2_DIR GPIO2_DIR 	// Porta de direção dos pinos
	#define US04_MODULE2_SET GPIO2_SET	// Porta para ligar os pinos
	#define US04_MODULE2_CLR GPIO2_CLR	// Porta para desligar os pinos
	#define US04_MODULE2_PIN GPIO2_PIN	// Porta de entrada dos pinos
		#define US04_MODULE2_TRIGGER 	BIT_2
		#define US04_MODULE2_ECHO		BIT_3

	// MODULO 3
	#define US04_MODULE3_DIR GPIO2_DIR 	// Porta de direção dos pinos
	#define US04_MODULE3_SET GPIO2_SET	// Porta para ligar os pinos
	#define US04_MODULE3_CLR GPIO2_CLR	// Porta para desligar os pinos
	#define US04_MODULE3_PIN GPIO2_PIN	// Porta de entrada dos pinos
		#define US04_MODULE3_TRIGGER 	BIT_0
		#define US04_MODULE3_ECHO		BIT_1

	// MODULO 4
	#define US04_MODULE4_DIR GPIO0_DIR 	// Porta de direção dos pinos
	#define US04_MODULE4_SET GPIO0_SET	// Porta para ligar os pinos
	#define US04_MODULE4_CLR GPIO0_CLR	// Porta para desligar os pinos
	#define US04_MODULE4_PIN GPIO0_PIN	// Porta de entrada dos pinos
		#define US04_MODULE4_TRIGGER 	BIT_8
		#define US04_MODULE4_ECHO		BIT_9

#if US04_USE_DEBUG == pdON
#define US04_DBGTRIGGER_DIR 	GPIO0_DIR
#define US04_DBGTRIGGER_SET 	GPIO0_SET
#define US04_DBGTRIGGER_CLR 	GPIO0_CLR
#define US04_DBGTRIGGER			BIT_22

#define US04_DBGTRIGGEROK_DIR 	GPIO2_DIR
#define US04_DBGTRIGGEROK_SET 	GPIO2_SET
#define US04_DBGTRIGGEROK_CLR 	GPIO2_CLR
#define US04_DBGTRIGGEROK		BIT_7

#define US04_DBGDETECTED_DIR 	GPIO2_DIR
#define US04_DBGDETECTED_SET 	GPIO2_SET
#define US04_DBGDETECTED_CLR 	GPIO2_CLR
#define US04_DBGDETECTED		BIT_8

#endif

#endif
