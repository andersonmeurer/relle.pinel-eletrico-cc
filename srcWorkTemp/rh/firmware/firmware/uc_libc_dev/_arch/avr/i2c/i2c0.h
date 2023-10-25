// Todos os bits válidos são no momento de SCL = 1
// Qualquer transação começa pela condição START SDA 1 > 0
// Qualquer transação termina pela condição STOP SDA 0 > 1
// Primerio byte é o endereço do dispositivo no barramento com o bit 0 indicando Read (bit 0 = 1) ou write (bit 0 = 0)
// Os demais bytes podem ser endereços seguidos de dados
// Quando o mestre esta escrevendo e deseja finaliza a transferencia com a condição de STOP
// Quando o mestre não que mais ler o escravo o mesmo manda um NACK antes de STOP
// Qualquer byte transmitido deve ter uma resposta da parte oposta (ACK e NACK)

#ifndef __I2C0_H
#define __I2C0_H

#include <avr/interrupt.h>
#include <util/twi.h>
#include "_config_cpu_.h"
#include "_config_cpu_i2c.h"

// defines and constants
#define I2C0_MASK_CMD   	0x0F
#define I2C0_MASK_STS  		0xF8

// Definindo tipos de estados do barramento
typedef enum	{
	I2C0_IDLE  		= 0, 
	I2C0_BUSY  		= 1,
	I2C0_MASTER_TX   = 2, 
	I2C0_MASTER_RX   = 3,
	I2C0_SLAVE_TX    = 4, 
	I2C0_SLAVE_RX    = 5
} i2c0_state_t;


void i2c0_Init(u16 Rate);
void i2c0_SetBitRate(u16 BitRate);
int i2c0_WriteBuffer(u8 AddrI2C, u8 *Buffer, u8 LenBuffer);
int i2c0_ReadBuffer(u8 AddrI2C, u8 *Buffer, u8 LenBuffer);

//void I2C0_SetLocalDeviceAddr(u8 Addr, u8 GenCallEn);
//void I2C0_SetHandlerSlaveRX(void (*I2C0_SlaveRxFunc)(u8 DataLength, u8* Data));
//void I2C0_SetHandlerSlaveTX(u8 (*I2C0_SlaveTxFunc)(u8 DataLengthMax, u8* Data));

#endif
