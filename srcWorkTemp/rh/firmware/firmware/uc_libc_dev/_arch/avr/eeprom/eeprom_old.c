#include "eeprom_old.h"

inline void eeprom_WaitBusy(void);

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Grava um byte no endereço da EEPROM interna de dados
// Parametros: 	Endereço e o dados a ser gravado
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void eeprom_WriteByte(u16 Addr, u8 Data) {
	u8 sts = SREG;
	
	//while((EECR) & (EEWE|EEMWE));							// Espera pela gravação anterior 
	eeprom_WaitBusy();

	EEARL = (u8)(Addr&0xFF);						// Atualizar endereço baixo
	EEARH = (u8)Addr>>8;							// Atualizar endereço alto
	EEDR = Data;											// Atualizar dado para gravar
	cli();													// Desligar interrupção global	
	sbi(EECR,EEMWE);										// Habilitar para gravar
	sbi(EECR,EEWE);											// Iniciar gravarção
	
	if (sts & _BV(SREG_I))									// Ligar interrupção global se o estado anterior estava ligada
		sei();													
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê um byte no endereço da EEPROM interna de dados 
// Parametros: 	Endereço a ser lido
// Retorna: 	O dado lido
//------------------------------------------------------------------------------------------------------------------
u8 eeprom_ReadByte(u16 Addr) {
	eeprom_WaitBusy();
	EEARL = (u8)(Addr&0xFF);						// Atualizar endereço baixo
	EEARH = (u8)Addr>>8;							// Atualizar endereço alto
	sbi(EECR,EERE);											// Habilito para ler

	return (EEDR);											// Retorna com os dados lidos
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escreve um buffer na EEPROM interna de dados a partir do endereço inicial
// Parametros: 	Endereço inciail a ser lido, Ponteiro do buffer e o tamanho do buffer ou qtd de bytes a ser ecrito (<=Tamanho do buffer)
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void eeprom_WriteBuffer(u16 AddrInit, const u8 *Buf, u16 Len) {
	u16 Addr;
	
	for (Addr = AddrInit; Addr<AddrInit + Len; Addr++)
		eeprom_WriteByte(Addr,*Buf++);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê um buffer na EEPROM interna de dados a partir do endereço inicial
// Parametros: 	Endereço inciail a ser lido, Ponteiro do buffer e o tamanho do buffer ou qtd de bytes a ser lidos (<=Tamanho do buffer)
// Retorna: 	Com o buffer prenchido
//------------------------------------------------------------------------------------------------------------------
void eeprom_ReadBuffer(u16 AddrInit, u8 *Buf, u16 Len) {
	u16 Addr;
	
	for (Addr = AddrInit; Addr<AddrInit + Len; Addr++)
		*Buf++ = eeprom_ReadByte(Addr);
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Espera que a CPU termine com o processo de gravação na EEprom
// Parametros: 
// Retorna: 	
//------------------------------------------------------------------------------------------------------------------
inline void eeprom_WaitBusy(void) {
	while(EECR & (EEWE|EEMWE));								// Espera pela gravação anterior 	
}
