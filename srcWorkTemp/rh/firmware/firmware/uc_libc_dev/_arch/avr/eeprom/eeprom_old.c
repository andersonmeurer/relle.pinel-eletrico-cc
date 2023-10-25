#include "eeprom_old.h"

inline void eeprom_WaitBusy(void);

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Grava um byte no endere�o da EEPROM interna de dados
// Parametros: 	Endere�o e o dados a ser gravado
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void eeprom_WriteByte(u16 Addr, u8 Data) {
	u8 sts = SREG;
	
	//while((EECR) & (EEWE|EEMWE));							// Espera pela grava��o anterior 
	eeprom_WaitBusy();

	EEARL = (u8)(Addr&0xFF);						// Atualizar endere�o baixo
	EEARH = (u8)Addr>>8;							// Atualizar endere�o alto
	EEDR = Data;											// Atualizar dado para gravar
	cli();													// Desligar interrup��o global	
	sbi(EECR,EEMWE);										// Habilitar para gravar
	sbi(EECR,EEWE);											// Iniciar gravar��o
	
	if (sts & _BV(SREG_I))									// Ligar interrup��o global se o estado anterior estava ligada
		sei();													
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	L� um byte no endere�o da EEPROM interna de dados 
// Parametros: 	Endere�o a ser lido
// Retorna: 	O dado lido
//------------------------------------------------------------------------------------------------------------------
u8 eeprom_ReadByte(u16 Addr) {
	eeprom_WaitBusy();
	EEARL = (u8)(Addr&0xFF);						// Atualizar endere�o baixo
	EEARH = (u8)Addr>>8;							// Atualizar endere�o alto
	sbi(EECR,EERE);											// Habilito para ler

	return (EEDR);											// Retorna com os dados lidos
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

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Espera que a CPU termine com o processo de grava��o na EEprom
// Parametros: 
// Retorna: 	
//------------------------------------------------------------------------------------------------------------------
inline void eeprom_WaitBusy(void) {
	while(EECR & (EEWE|EEMWE));								// Espera pela grava��o anterior 	
}
