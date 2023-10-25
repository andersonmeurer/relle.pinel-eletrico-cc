#ifndef __PCA9685_PWM_H
#define __PCA9685_PWM_H

#include "_config_cpu_.h"
#include "_config_cpu_i2c.h"

#if (PCA9685_USE_I2C == 0)
	#include "i2c0.h"
	#define pca9685_Send  		i2c0_WriteBuffer
	#define pca9685_Receive  	i2c0_ReadBuffer
#elif (PCA9685_USE_I2C == 1)
	#include "i2c1.h"
	#define pca9685_Send 		i2c1_WriteBuffer
	#define pca9685_Receive 	i2c1_ReadBuffer
#elif (PCA9685_USE_I2C == 2)
	#include "i2c2.h"
	#define pca9685_Send 		i2c2_WriteBuffer
	#define pca9685_Receive 	i2c2_ReadBuffer
#else
	#error NÃO FOI DEFINIDO QUAL O BARRAMENTO I2C NO _config_cpu_i2c.h
#endif

#define PCA9685_MODE1_REG 		0x00
#define PCA9685_PRESCALE_REG 	0xFE
#define PCA9685_PORT0_REG 		0x06

int pca9685_Init(int addr);
int pca9685_ReadReg(u8 reg, u8 *data);
int pca9685_WriteReg(u8 reg, u8 data);
int pca9685_Status(void);
void pca9685_PWMFrequency(float freq);
void pca9685_SetPWM(u8 ch, u16 on, u16 off);
void pca9685_Servo(u8 servo, int pulse);

#endif
