#ifndef __I2C2_H
#define __I2C2_H

#include "_config_cpu_.h"

#define I2C2_ACK0     0   
#define I2C2_ACK1     1
#define I2C2_READ     2

void i2c2_Init(u16 bitrate, tTime(*now_func)(void), int tm);
void i2c2_SetBitrate(u16 bitrate);
void i2c2_SetLocalDeviceAddr(u8 addr, u8 genCallEn);
int i2c2_CodeError(void);
int i2c2_WriteBuffer( u8 addr_device, u8 *data, u16 length );
int i2c2_ReadBuffer(u8 addr_device, u8 *data, u16 length);

#endif
