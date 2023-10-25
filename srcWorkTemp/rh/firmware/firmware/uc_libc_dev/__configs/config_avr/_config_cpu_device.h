#ifndef __CONFIG_CPU_DEVICE_H
#define __CONFIG_CPU_DEVICE_H

#include "_config_cpu_.h"

// ###################################################################################################################
// CONFIGURAÇÃO BEEP
#define BUZZER_DIR			DDRE
#define BUZZER_PORT			PORTE
#define BUZZER				PE2

// ###################################################################################################################
// CONFIGURAÇÃO PARA GRAVADORA DO FPGA ALTERA CYCLONE II
// PINOS DE SAÍDA
#define FPGA_DCLK_DIR  				DDRB
#define FPGA_DCLK_PORT				PORTB
#define FPGA_DCLK					PB4

#define FPGA_DATA0_DIR  			DDRB
#define FPGA_DATA0_PORT				PORTB
#define FPGA_DATA0					PB3

#define FPGA_CONFIG_DIR  			DDRB
#define FPGA_CONFIG_PORT			PORTB
#define FPGA_CONFIG					PB2

// PINOS DE ENTRADA
#define FPGA_STATUS_DIR  			DDRB
#define FPGA_STATUS_PIN				PINB
#define FPGA_STATUS					PB1

#define FPGA_CONFIG_DONE_DIR  		DDRB
#define FPGA_CONFIG_DONE_PIN		PINB
#define FPGA_CONFIG_DONE			PB0

#endif

