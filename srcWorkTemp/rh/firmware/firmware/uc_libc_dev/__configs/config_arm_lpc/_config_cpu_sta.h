#ifndef __CONFIG_CPU_STA_H
#define __CONFIG_CPU_STA_H

#include "_config_cpu_.h"
#include "_config_cpu_i2c.h"
#include "_config_cpu_spi.h"

#define STA_USE_GPIO_BITSTREAM pdON				// pdOFF utiliza o barramento SPI para mandar bistream mp3 para o sta. pdON utiliza os pinos GPIO
	// PINO DE CLOCK PARA ENVIO DO DADO MP3
	#define STA_SCKR_IODIR		GPIO1_DIR
	#define STA_SCKR_IOSET		GPIO1_SET
	#define STA_SCKR_IOCLR		GPIO1_CLR
	#define STA_SCKR			(1<<4)

	// PINO DE DADO PARA ENVIO DO DADO MP3
	#define STA_SDI_IODIR		GPIO1_DIR
	#define STA_SDI_IOSET		GPIO1_SET
	#define STA_SDI_IOCLR		GPIO1_CLR
	#define STA_SDI				(1<<1)

// PINO DE SOLICITAÇÃO DE MAIS DADOS DO STA
#define STA_DATAREQ_IODIR	GPIO1_DIR
#define STA_DATAREQ_PIN		GPIO1_PIN
#define STA_DATAREQ			(1<<0)

#endif

