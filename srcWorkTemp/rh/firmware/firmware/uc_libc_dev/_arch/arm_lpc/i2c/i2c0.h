#ifndef __I2C0_H
#define __I2C0_H

#include "_config_cpu_.h"

#define I2C0_ACK0     0   
#define I2C0_ACK1     1
#define I2C0_READ     2

void i2c0_Init(u16 bitrate, tTime(*now_func)(void), int tm);
void i2c0_SetBitrate(u16 bitrate);
void i2c0_SetLocalDeviceAddr(u8 addr, u8 genCallEn);
int i2c0_CodeError(void);
int i2c0_WriteBuffer( u8 addr_device, u8 *data, u16 length );
int i2c0_ReadBuffer(u8 addr_device, u8 *data, u16 length);

#endif
