#ifndef __ADXL345_ACCELEROMETER_H
#define __ADXL345_ACCELEROMETER_H

#include "_config_cpu_.h"
#include "_config_cpu_i2c.h"
#include "_config_lib_sensor.h"

#if (ADXL345_USE_I2C == 0)
	#include "i2c0.h"
	#define adxl345_Send  		i2c0_WriteBuffer
	#define adxl345_Receive  	i2c0_ReadBuffer
#elif (ADXL345_USE_I2C == 1)
	#include "i2c1.h"
	#define adxl345_Send 		i2c1_WriteBuffer
	#define adxl345_Receive 	i2c1_ReadBuffer
#elif (ADXL345_USE_I2C == 2)
	#include "i2c2.h"
	#define adxl345_Send 		i2c2_WriteBuffer
	#define adxl345_Receive 	i2c2_ReadBuffer
#else
	#error NÃO FOI DEFINIDO QUAL O BARRAMENTO I2C NO _config_cpu_i2c.h
#endif

// ENDEREÇO ESCRAVO DO ADX NO BARRAMENTO I2C
#define ADXL345_ADDR0		0xA6 // pino ALT ADDRESS = 0
#define ADXL345_ADDR1		0x3A // pino ALT ADDRESS = 1

#define ADXL345_DEVID			0x00 		// Endereço do registrador ID
	#define ADXL345_ID				0xE5	// Valor ID do adx
#define ADXL345_THRESH_TAP 		0x1d		// Tap Threshold
#define ADXL345_OFSX 			0x1e		// X-axis offset
#define ADXL345_OFSY 			0x1f		// Y-axis offset
#define ADXL345_OFSZ 			0x20		// Z-axis offset
#define ADXL345_DUR 			0x21		// Tap Duration
#define ADXL345_LATENT 			0x22		// Tap latency
#define ADXL345_WINDOW 			0x23		// Tap window
#define ADXL345_THRESH_ACT 		0x24		// Activity Threshold
#define ADXL345_THRESH_INACT	0x25		// Inactivity Threshold
#define ADXL345_TIME_INACT 		0x26		// Inactivity Time
#define ADXL345_ACT_INACT_CTL 	0x27		// Axis enable control for activity and inactivity detection
#define ADXL345_THRESH_FF 		0x28		// free-fall threshold
#define ADXL345_TIME_FF 		0x29		// Free-Fall Time
#define ADXL345_TAP_AXES 		0x2a		// Axis control for tap/double tap
#define ADXL345_ACT_TAP_STATUS 	0x2b		// Source of tap/double tap
#define ADXL345_BW_RATE 		0x2c		// Data rate and power mode control
#define ADXL345_POWER_CTL 		0x2d		// Power Control Register
#define ADXL345_INT_ENABLE 		0x2e		// Interrupt Enable Control
#define ADXL345_INT_MAP 		0x2f		// Interrupt Mapping Control
#define ADXL345_INT_SOURCE 		0x30		// Source of interrupts
#define ADXL345_DATA_FORMAT 	0x31		// Data format control
#define ADXL345_DATAX0 			0x32		// X-Axis Data 0
#define ADXL345_DATAX1 			0x33		// X-Axis Data 1
#define ADXL345_DATAY0 			0x34		// Y-Axis Data 0
#define ADXL345_DATAY1 			0x35		// Y-Axis Data 1
#define ADXL345_DATAZ0 			0x36		// Z-Axis Data 0
#define ADXL345_DATAZ1 			0x37		// Z-Axis Data 0
#define ADXL345_FIFO_CTL 		0x38		// FIFO control
#define ADXL345_FIFO_STATUS 	0x39		// FIFO status

int adxl345_Init(int addr);
int adxl345_Status(void);
int adxl345_ReadReg(u8 reg, u8 *data);
int adxl345_ReadBuffer(u8 reg, int len, u8* buff);
int adxl345_WriteReg(u8 reg, u8 data);
int adxl345_SetRangeSetting(int res);
	#define ADXL345_RES_2	2
	#define ADXL345_RES_4	4
	#define ADXL345_RES_8	8
	#define ADXL345_RES_16	16
int adxl345_SetFullResBit(bool fullResBit);
int adxl345_SetLowPower(bool state);
int adxl345_SetRate(int br);
	#define ADXL345_BW_1600 		0xF
	#define ADXL345_BW_800  		0xE
	#define ADXL345_BW_400  		0xD
	#define ADXL345_BW_200  		0xC
	#define ADXL345_BW_100  		0xB
	#define ADXL345_BW_50   		0xA
	#define ADXL345_BW_25   		0x9
	#define ADXL345_BW_12   		0x8
	#define ADXL345_BW_6   		 	0x7
	#define ADXL345_BW_3    		0x6
int adxl345_SetFIFO(int fifoMode);
	#define ADXL345_FIFO_MODE_BYPASS		0x00
	#define ADXL345_FIFO_MODE_FIFO			0x40
	#define ADXL345_FIFO_MODE_STREAM		0x80
	#define ADXL345_FIFO_MODE_TRIGGER		0xC0
int adxl345_SetInterrupt(u8 interruptBit, bool state);
	#define ADXL345_INT_DATA_READY 	0x07
	#define ADXL345_INT_SINGLE_TAP 	0x06
	#define ADXL345_INT_DOUBLE_TAPT 0x05
	#define ADXL345_INT_ACTIVITY   	0x04
	#define ADXL345_INT_INACTIVITY 	0x03
	#define ADXL345_INT_FREE_FALL  	0x02
	#define ADXL345_INT_WATERMARK  	0x01
	#define ADXL345_INT_OVERRUNY   	0x00

#if (ADXL345_USE_FORCE_G == pdON)
int adxl345_SetActivityThreshold(int activityThreshold);
int adxl345_SetInactivityThreshold(int inactivityThreshold);
int adxl345_SetTimeInactivity(int timeInactivity);
int adxl345_SetActivityX(bool state);
int adxl345_SetActivityY(bool state);
int adxl345_SetActivityZ(bool state);
int adxl345_SetInactivityX(bool state);
int adxl345_SetInactivityY(bool state);
int adxl345_SetInactivityZ(bool state);
int adxl345_SetTapDetectionOnX(bool state);
int adxl345_SetTapDetectionOnY(bool state);
int adxl345_SetTapDetectionOnZ(bool state);
int adxl345_SetTapThreshold(int tapThreshold);
int adxl345_SetTapDuration(int tapDuration);
int adxl345_SetDoubleTapLatency(int doubleTapLatency);
int adxl345_SetDoubleTapWindow(int doubleTapWindow);
int adxl345_SetFreeFallThreshold(int freeFallThreshold);
int adxl345_SetFreeFallDuration(int freeFallDuration);
#endif

int adxl345_PowerOn(void);

int adxl345_GetXYZ(s16* x, s16* y, s16* z);
#if (ADXL345_USE_INCLINATION == pdON)
int adxl345_GetAngle(int* angle);
int adxl345_GetRollPitch(double* roll, double *pitch);
#endif

#if (ADXL345_USE_FORCE_G == pdON)
int adxl345_GetAcceleration(double *ax, double *ay, double *az);
#endif

#if (ADXL345_USE_PROCESS == pdON)
void adxl345_Process(void);
#endif

#endif
