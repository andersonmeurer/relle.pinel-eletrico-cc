// Todos os bits v�lidos s�o no momento de SCL = 1
// Qualquer transa��o come�a pela condi��o START SDA 1 > 0 seguido de um tempo SCL 1 > 0
// Qualquer transa��o termina pela condi��o STOP SDA 0 > 1 seguido de um tempo SCL 0 > 1
// Primerio byte � o endere�o do dispositivo no barramento com o bit 0 indicando Read (bit 0 = 1) ou write (bit 0 = 0)
// Os demais bytes podem ser endere�os seguidos de dados
// Quando o mestre esta escrevendo e deseja finaliza a transferencia com a condi��o de STOP
// Quando o mestre n�o que mais ler o escravo o mesmo manda um NACK antes de STOP
// Qualquer byte transmitido deve ter uma resposta da parte oposta (ACK() e NACK())

#ifndef __I2C_IO_H
#define __I2C_IO_H

#include <util/delay.h>
#include "_config_cpu_.h"
#include "_config_cpu_i2c.h"

void i2c_Init(void);
void i2c_SendBuffer(u8 AddrI2C, u8 *Buffer, u8 LenBuffer);
void i2c_ReceiverBuffer(u8 AddrI2C, u8 *Buffer, u8 LenBuffer);

#endif
