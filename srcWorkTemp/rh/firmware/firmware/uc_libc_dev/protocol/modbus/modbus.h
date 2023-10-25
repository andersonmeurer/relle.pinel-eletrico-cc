#ifndef MODBUS_H
#define MODBUS_H

#include "_config_cpu_.h"
#include "crc.h"

#define modbusCMD_READ_REGISTERS	0x03
#define modbusCMD_WRITE_REGISTER	0x06
#define modbusCMD_WRITE_REGISTERS	0x10

#define modbusNO_ERROR				00
#define modbusILLEGAL_FUNCTION		01 // O servidor recebeu uma fun��o que n�o foi implementada ou n�o foi habilitada.
#define modbusILLEGAL_DATA_ADDRESS	02 // O servidor precisou acessar um endere�o inexistente.
#define modbusILLEGAL_DATA_VALUE	03 // O valor contido no campo de dado n�o � permitido pelo servidor. Isto indica uma falta de informa��es na estrutura do campo de dados.
#define modbusSLAVE_DEVICE_FAILURE	04 // Um irrecuper�vel erro ocorreu enquanto o servidor estava tentando executar a a��o solicitada.


#endif
