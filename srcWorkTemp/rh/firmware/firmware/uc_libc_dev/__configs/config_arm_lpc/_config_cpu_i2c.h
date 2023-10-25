#ifndef __CONFIG_CPU_I2C_H
#define __CONFIG_CPU_I2C_H

#include "_config_cpu_.h"


// ####################################################################################################################
// Controlador PWM pca9685
#define PCA9685_USE_I2C	2	// Define qual barramento I2C a ser usada pelo controlador de PWM. Valor 0 = I2C0, 1 = I2C1 ....

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

#endif
