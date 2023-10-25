#include "eeprom.h"

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Grava um byte no endere�o da EEPROM interna de dados
// Parametros: 	Endere�o e o dados a ser gravado
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void eeprom_WriteByte(u16 Addr, u8 Data) {
	eeprom_busy_wait();
	_EEPUT(Addr, Data);
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	L� um byte no endere�o da EEPROM interna de dados 
// Parametros: 	Endere�o a ser lido
// Retorna: 	O dado lido
//------------------------------------------------------------------------------------------------------------------
u8 eeprom_ReadByte(u16 Addr) {
	u8 Data;
	
	eeprom_busy_wait();
	_EEGET(Data, Addr);
	
	return Data;											// Retorna com os dados lidos
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Escreve um buffer na EEPROM interna de dados a partir do endere�o inicial
// Parametros: 	Endere�o inciail a ser lido, Ponteiro do buffer e o tamanho do buffer ou qtd de bytes a ser ecrito (<=Tamanho do buffer)
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void eeprom_WriteBuffer(u16 AddrInit, const u8 *Buf, u16 Len) {
	u16 Addr;
	
	for (Addr = AddrInit; Addr<AddrInit + Len; Addr++)
		eeprom_WriteByte(Addr,*Buf++);
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	L� um buffer na EEPROM interna de dados a partir do endere�o inicial
// Parametros: 	Endere�o inciail a ser lido, Ponteiro do buffer e o tamanho do buffer ou qtd de bytes a ser lidos (<=Tamanho do buffer)
// Retorna: 	Com o buffer prenchido
//------------------------------------------------------------------------------------------------------------------
void eeprom_ReadBuffer(u16 AddrInit, u8 *Buf, u16 Len) {
	u16 Addr;
	
	for (Addr = AddrInit; Addr<AddrInit + Len; Addr++)
		*Buf++ = eeprom_ReadByte(Addr);
}
