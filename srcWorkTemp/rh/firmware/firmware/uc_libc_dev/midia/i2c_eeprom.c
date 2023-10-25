#include "i2c_eeprom.h"

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê uma posição de memória I2C
// Parametros: 	Endereço do dispositivo, o endereço da posição de memória a ser lida e o ponteiro de memoria para retorno
// Retorna: 	Estado de execução. pdPASS se tudo ocorreu bem ou retorna o código do erro
//-----------------------------------------------------------------------------------------------------------------------
int i2c_EEprom_ReadByte(u8 addr_device, u16 addr, u8 *data) {
	n8 return_code;
	u8 buffer[2];

	buffer[0] = (u8) (addr >> 8) & 0xFF;
	buffer[1] = (u8) addr & 0xFF;
	
	return_code = eeprom_i2c_Send(addr_device, buffer, 2);
	
	if (return_code == pdPASS)
		return_code = eeprom_i2c_Receive(addr_device, data, 1);
		
	return return_code;
}

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê uma posição de memória I2C
// Parametros: 	Endereço do dispositivo e o endereço da posição de memória a ser lida
// Retorna: 	Estado de execução. pdPASS se tudo ocorreu bem ou retorna o código do erro
//-----------------------------------------------------------------------------------------------------------------------
int i2c_EEprom_WriteByte(u8 addr_device, u16 addr, u8 data)	{
	u8 buffer[3]; 
		
	buffer[0] = (u8) (addr >> 8) & 0xFF;
	buffer[1] = (u8) addr & 0xFF;
	buffer[2] = data;
				
	return eeprom_i2c_Send(addr_device, buffer, 3);	
}


