#ifndef __CONFIG_CPU_I2C_H
#define __CONFIG_CPU_I2C_H

#include "_config_cpu_.h"


// ####################################################################################################################
// Acelerômetro adx345
#define ADXL345_USE_I2C	2	// Define qual barramento I2C a ser usada pelo acelerômetro. Valor 0 = I2C0, 1 = I2C1 ....

// ####################################################################################################################
// DECODER MP3 STA015
#define STA_USE_I2C		0	// Define qual o barramento I2C para acessar os registradores do STA

// ####################################################################################################################
// MEMÓRIA EEMPROM I2C
#define EEPROM_USE_I2C	0 	// Define qual barramento I2C a ser usada pela memória. Valor 0 = I2C0, 1 = I2C1 ....

// ####################################################################################################################
// DIITALIZADORA
#define SAA711x_USE_I2C 0

// ###################################################################################################################
// CONFIGURAÇÂO PARA O PLL CDCE949
#define CDCE_USE_I2C	0 		// define qual barramento I2C a ser usada pelo PLL. Valor 0 = I2C0, 1 = I2C1 ....

// ######################################################################################################
// SENSOR DE TEMPERATURA DIGITAL
#define DS1621_USE_I2C	0	// Define qual barramento I2C que será usado para o sensor de temperatura.
							// 	0=I2C0, 1=I2C1 e assim sucessivamente


// -------------------------------------------------------------------------------------------------------------------
// Linha AVR
// -------------------------------------------------------------------------------------------------------------------
// ###################################################################################################################
// I2C0
#if defined (CPU_AVR)
#define I2C0_DELAY_MS 		280		// Delay entre lituras no barramento I2C

#define I2C0_USE_IO pdOFF
	#define I2C0_PORT_DRR	DDRD
	#define I2C0_PORT_OUT	PORTD
	#define I2C0_PIN_SDA	PD1
	#define I2C0_PIN_SCL	PD0
	#define I2C0_PORT_IN	PIND

#define I2C0_USE_INT			pdON
	#define I2C0_TX_BUFFER_SIZE	10 	// Tamanho do buffer de transmissão usada na int
	#define I2C0_RX_BUFFER_SIZE	10	// Tamanho do buffer de recepção usada na int
#endif

#endif
