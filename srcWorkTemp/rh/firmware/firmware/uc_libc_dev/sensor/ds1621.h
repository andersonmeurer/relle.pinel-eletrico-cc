#ifndef __DS1621_H
#define __DS1621_H

#include "_config_cpu_.h"
#include "_config_lib_sensor.h"

#if (DS1621_USE_I2C == 0)
	#include "i2c0.h"
	#define ds1621_i2c_Send  		i2c0_WriteBuffer
	#define ds1621_i2c_Receive  	i2c0_ReadBuffer
#elif (DS1621_USE_I2C == 1)
	#include "i2c1.h"
	#define ds1621_i2c_Send 		i2c1_WriteBuffer
	#define ds1621_i2c_Receive 		i2c1_ReadBuffer
#else
	#error NÃO FOI ESPECIFICADO O BARRAMENTO I2C NO _config_lib_sensor.h
#endif

int ds1621_WriteCommand(u8 addr, u8 data);
int ds1621_ReadCommand(u8 addr, u8 *result);
int ds1621_ReadTemp(u8 addr, u8 *temp, u8 *fraction);
int ds1621_Convert(u8 addr);
int ds1621_Write(u8 addr, u8 command, u8 data);
int ds1621_ReadInt(u8 addr, u8 command, u16 *result);
int ds1621_Read(u8 addr, u8 command, u8 *result);
int ds1621_WaitConvert(u8 addr);

#define DS1621_ADDR0 				0x90
#define DS1621_ADDR1				0x92
#define DS1621_ADDR2				0x94
#define DS1621_ADDR3				0x96
#define DS1621_ADDR4				0x98
#define DS1621_ADDR5				0x9A
#define DS1621_ADDR6				0x9C
#define DS1621_ADDR7				0x9E

#define DS1621_CMD_START_CONVERT 	0xEE
#define DS1621_CMD_STOP_CONVERT		0x22
#define DS1621_CMD_READ_TEMP		0xAA
#define DS1621_CMD_READ_ACC			0x09
#define DS1621_CMD_READ_COUNT		0xA8
#define DS1621_CMD_RW_TH			0xA1
#define DS1621_CMD_RW_TL			0xA2
#define DS1621_CMD_RW_CONFIG		0xAC

#endif
