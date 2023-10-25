#include "ds1621.h"

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Escreve no registrador de comandos e estatus do dispositivo
// Parametros: 	Endere�o do dispositivo e o dados a ser gravado
// Retorna: 	Estado de execu��o. pdPASS se tudo ocorreu bem ou retorna o c�digo do erro
//------------------------------------------------------------------------------------------------------------------
int ds1621_WriteCommand(u8 addr, u8 data)	{
	return ds1621_Write(addr, DS1621_CMD_RW_CONFIG, data);
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	L� o registrador de comandos e estatus do dispositivo
// Parametros: 	Endere�o do dispositivo e o ponteiro do dado para guardar o valor do registrador lido
// Retorna: 	Estado de execu��o. pdPASS se tudo ocorreu bem ou retorna o c�digo do erro
//------------------------------------------------------------------------------------------------------------------
int ds1621_ReadCommand(u8 addr, u8 *result)	{
	return ds1621_Read(addr, DS1621_CMD_RW_CONFIG, result);
}
	
//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	L� a temperatura convertida pelo disposiivo
// Parametros: 	Endere�o do dispositivo e o ponteiros da temperatura e fra��o ( precis�o da fra��o em 0.5 graus)
// Retorna: 	Estado de execu��o. pdPASS se tudo ocorreu bem ou retorna o c�digo do erro
//------------------------------------------------------------------------------------------------------------------
int ds1621_ReadTemp(u8 addr, u8 *temp, u8 *fraction)	{
	n8 return_code;
	u16 t;

	return_code = ds1621_ReadInt(addr, DS1621_CMD_READ_TEMP, &t);
	
	if (return_code == pdPASS) {
		*fraction = t & 0xFF;
		*temp = (t >> 8);
	}
	
	return return_code;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Comando para iniciar a convers�o no dispositivo
//					Nota: Se � no modo automatico, esta rotina � necessario ser executada uma �nica vez
// Parametros: 	Endere�o do dispositivo
// Retorna: 	Estado de execu��o. pdPASS se tudo ocorreu bem ou retorna o c�digo do erro
//------------------------------------------------------------------------------------------------------------------
int ds1621_Convert(u8 addr)	{
	n8 return_code;
	u8	buffer[1];
		
	//Pulso para iniciar convers�o
	buffer[0] = DS1621_CMD_START_CONVERT;				// Comando para iniciar a convers�o	
	return_code = ds1621_i2c_Send(addr, buffer, 1);		// Mando o comando para acao
	
	if (return_code == pdPASS) {
		buffer[0] = DS1621_CMD_STOP_CONVERT;			// Comando para parar a convers�o	
		return_code = ds1621_i2c_Send(addr, buffer, 1);	// Mando o comando para acao
	}
	
	return return_code;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Escrita nos registradores do dispositivo
// Parametros: 	Endere�o do dispositivo
//				comando do dispositivo : DS1621_CMD_RW_TH /	DS1621_CMD_RW_TL / DS1621_CMD_RW_CONFIG
//				dado a ser gravado
// Retorna: 	Estado de execu��o. pdPASS se tudo ocorreu bem ou retorna o c�digo do erro
//------------------------------------------------------------------------------------------------------------------
int ds1621_Write(u8 addr, u8 command, u8 data)	{
	u8	buffer[2];
		
	buffer[0] = command;
	buffer[1] = data;
				
	return ds1621_i2c_Send(addr, buffer, 2);
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	L� os registradores de 2 bytes do dispositivo
// Parametros: 	Endere�o do dispositivo
//				Comando:  DS1621_CMD_READ_TEMP / DS1621_CMD_READ_ACC / DS1621_CMD_READ_COUNT / DS1621_CMD_RW_TH  /DS1621_CMD_RW_TL	/ DS1621_CMD_RW_CONFIG
//				Ponteiro do int para o retorno do valor lido
// Retorna: 	Estado de execu��o. pdPASS se tudo ocorreu bem ou retorna o c�digo do erro
//------------------------------------------------------------------------------------------------------------------
int ds1621_ReadInt(u8 addr, u8 command, u16 *result)	{
	n8 return_code;
	u8 buffer_rx[2];
	u8 buffer_tx[1];
		
	buffer_tx[0] = command;								// Comando
		
	return_code = ds1621_i2c_Send(addr, buffer_tx, 1);			 	// Mando o comando para leitura da temperatura
	
	if (return_code == pdPASS) {
		return_code = ds1621_i2c_Receive(addr, buffer_rx, 2);		   	// Retorna 2 bytes
		
		if (return_code == pdPASS) {
			*result = buffer_rx[0] << 8;
			*result += buffer_rx[1];
		}
	}
	
	return return_code;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	L� os registradores do dispositivo
// Parametros: 	Endere�o do dispositivo
//				Comando:  DS1621_CMD_READ_TEMP / DS1621_CMD_READ_ACC / DS1621_CMD_READ_COUNT / DS1621_CMD_RW_TH  /DS1621_CMD_RW_TL	/ DS1621_CMD_RW_CONFIG
//				Ponteiro do byte para o retorno do valor lido
// Retorna: 	Estado de execu��o. pdPASS se tudo ocorreu bem ou retorna o c�digo do erro
//------------------------------------------------------------------------------------------------------------------
int ds1621_Read(u8 addr, u8 command, u8 *result)	{
	n8 return_code;
	u8 buffer_rx[1];
	u8 buffer_tx[1];
		
	buffer_tx[0] = command;							// Comando desejado
		
	return_code = ds1621_i2c_Send(addr, buffer_tx, 1);			// Mando o comando para leitura da temperatura
	
	if (return_code == pdPASS) {
		return_code = ds1621_i2c_Receive(addr, buffer_rx, 1);		// Retorna 1 byte
		
		if (return_code == pdPASS) 
			*result  = buffer_rx[0];
	}
	
	return return_code; 
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Espera pela convers�o de temperatura
// Parametros: 	Endere�o do dispositivo
// Retorna: 	Estado de execu��o. pdPASS se tudo ocorreu bem ou retorna o c�digo do erro
//------------------------------------------------------------------------------------------------------------------
int ds1621_WaitConvert(u8 addr)	{
	n8 return_code;	
	u8 result;
		
	do {
		return_code = ds1621_ReadCommand( addr, &result );			//bi7 = 1 conversao feita e Bit 0 Convers�o em progresso
	} while ( (return_code == pdPASS) && (result < 0x80) );
	
	return return_code;
}
