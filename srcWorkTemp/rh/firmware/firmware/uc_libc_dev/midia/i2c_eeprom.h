#ifndef __I2C_EEPROM_H
#define __I2C_EEPROM_H


#include "_config_cpu_.h"
#include "_config_cpu_i2c.h"

#if (EEPROM_USE_I2C == 0)
	#include "i2c0.h"
	#define eeprom_i2c_Send  		i2c0_WriteBuffer
	#define eeprom_i2c_Receive  	i2c0_ReadBuffer	
#elif (EEPROM_USE_I2C == 1)
	#include "i2c1.h"
	#define eeprom_i2c_Send 		i2c1_WriteBuffer
	#define eeprom_i2c_Receive 		i2c1_ReadBuffer
#elif (EEPROM_USE_I2C == 2)
	#include "i2c2.h"
	#define eeprom_i2c_Send 		i2c2_WriteBuffer
	#define eeprom_i2c_Receive 		i2c2_ReadBuffer
#else
	#error NÃO FOI DEFINIDO QUAL O BARRAMENTO I2C NO _config_cpu_i2c.h
#endif

#define I2C_EEPROM_ADDR0		0xA0
#define I2C_EEPROM_ADDR1		0xA2
#define I2C_EEPROM_ADDR2		0xA4
#define I2C_EEPROM_ADDR3		0xA6
#define I2C_EEPROM_ADDR4		0xA8
#define I2C_EEPROM_ADDR5		0xAA
#define I2C_EEPROM_ADDR6		0xAC
#define I2C_EEPROM_ADDR7		0xAE

int i2c_EEprom_ReadByte(u8 addr_device, u16 addr, u8 *data);
int i2c_EEprom_WriteByte(u8 addr_device, u16 addr, u8 data);

#endif
