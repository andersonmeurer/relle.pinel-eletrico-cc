#include <util/delay.h>
#include "i2c0.h"

// Se esta usando o modo de interrupção
#if (I2C0_USE_INT == pdON)
	// Estado do barramento I2C
	static volatile i2c0_state_t 
		i2c0_State;

	static u8 
		i2c0_AddrDevice;

	// Buffer de transmissão no I2C
	static u8 
		i2c0_SendData[I2C0_TX_BUFFER_SIZE],
		i2c0_SendDataIndex,
		i2c0_SendDataLen;
	
	// Buffer de recepção no I2C
	static u8 
		i2c0_ReceiveData[I2C0_RX_BUFFER_SIZE],
		i2c0_ReceiveDataIndex,
		i2c0_ReceiveDataLen;
#endif

inline void i2c0_SendStart(void);
inline void i2c0_SendStop(void);
inline void i2c0_WaitForComplete(void);
inline void i2c0_SendByte(u8 Data);
inline void i2c0_ReceiveByte(u8 AckFlag);
inline u8 i2c0_GetReceivedByte(void);
inline u8 i2c0_GetStatus(void);

/*
// Ponteiro da função de Recepacao para modo escravo
static void (*I2C0_SlaveReceive)
	(u8 DataLength, u8* Data);

// Ponteiro da função de transmissão para modo escravo
static u8 (*I2C0_SlaveTransmit)
	(u8 transmitDataLengthMax, u8* transmitData);

*/




//--------------------------------------------------------------------------------------------------------------------------
// FUNCOES PARA MODO MESTRE
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// Inicializo barremento I2C. Somente nos AtMegas Rate em Khz 
//--------------------------------------------------------------------------------------------------------------------------
void i2c0_Init(u16 Rate)	{
	
	// Limpo o ponteiro de funções do modo escravo
//	I2C0_SlaveReceive 	= 0;
//	I2C0_SlaveTransmit 	= 0;
	
	//sbi(TWCR, TWINT);
	cbi(TWCR, TWSTA);
	cbi(TWCR, TWSTO);

	#if (I2C0_USE_INT == pdON)
		sbi(TWCR, TWIE);		// Habilita int para I2C
		i2c0_State = I2C0_IDLE;	// Indico que o barramento está ocioso
	#else
		cbi(TWCR, TWIE);		// Desabilita int para I2C
	#endif

	sbi(TWCR, TWEA);			// Habilita o ACK quando o ATmega for escravo da transação
	sbi(TWCR, TWEN);			// Habilita a interface I2C sobre os pinos correspodente
	
	i2c0_SetBitRate(Rate);		// Ajusta a velocidade de transmissão
}

//--------------------------------------------------------------------------------------------------------------------------
// Ajusta o clock de trabalho do barramento I2C em Khz
//--------------------------------------------------------------------------------------------------------------------------
void i2c0_SetBitRate(u16 BitRate) {
	u8 	BitRateDiv;
	
	// Ajustamos o prescaler para 1
	cbi(TWSR, TWPS0);
	cbi(TWSR, TWPS1);
		
	// Calcular a divisao para o BitRate
	BitRateDiv = ((F_CPU / 1000l) / BitRate);
	
	if(BitRateDiv >= 16)
		BitRateDiv = (BitRateDiv - 16) / 2;
	
	TWBR = BitRateDiv;
}




//--------------------------------------------------------------------------------------------------------------------------
// FUNCOES GERAIS
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// Manda a condição de inicio de transação
//--------------------------------------------------------------------------------------------------------------------------
inline void i2c0_SendStart(void)	{
	//outb(TWCR, (inb(TWCR) & I2C0_MASK_CMD) | _BV(TWINT) | _BV(TWSTA) );
	TWCR = (TWCR & I2C0_MASK_CMD) | _BV(TWINT) | _BV(TWSTA);
}

//--------------------------------------------------------------------------------------------------------------------------
// Manda a condição de parada de transação e Habilita o ACK quando o ATmega for escravo da transação
//--------------------------------------------------------------------------------------------------------------------------
inline void i2c0_SendStop(void)	{
	//outb(TWCR, (inb(TWCR) & I2C0_MASK_CMD) | BV(TWINT) | BV(TWSTO) | BV(TWEA));
	TWCR = (TWCR & I2C0_MASK_CMD) | _BV(TWINT) | _BV(TWSTO) | _BV(TWEA);
}

//--------------------------------------------------------------------------------------------------------------------------
// Espera o barramento a completar a operação
//--------------------------------------------------------------------------------------------------------------------------
inline void i2c0_WaitForComplete(void)	{
	//while( !(inb(TWCR) & BV(TWINT)) );
	while( !(TWCR & _BV(TWINT) ) );
}

//--------------------------------------------------------------------------------------------------------------------------
// Transmite o byte no barramento
//--------------------------------------------------------------------------------------------------------------------------
inline void i2c0_SendByte(u8 Data)	{
	//outb(TWDR, Data);	// Atualiza o buffer TX com o dado
	//outb(TWCR, (inb(TWCR) & I2C0_MASK_CMD) | BV(TWINT));	// tramsite o dado
	
	TWDR = Data;	// Atualiza o buffer TX com o dado
	TWCR =  (TWCR & I2C0_MASK_CMD) | _BV(TWINT);	// tramsite o dado
}

//--------------------------------------------------------------------------------------------------------------------------
// Inicia a recepção de dados sobre o barramento
//--------------------------------------------------------------------------------------------------------------------------
inline void i2c0_ReceiveByte(u8 AckFlag)	{

	if( AckFlag )		// AckFlag = pdTRUE: ACK the recevied data
		//outb(TWCR, (inb(TWCR) & I2C0_MASK_CMD) | BV(TWINT) | BV(TWEA));
		TWCR = (TWCR & I2C0_MASK_CMD) | _BV(TWINT) | _BV(TWEA);
	
	else				// AckFlag = pdFALSE: NACK the recevied data
		TWCR = (TWCR & I2C0_MASK_CMD) | _BV(TWINT);
}

//--------------------------------------------------------------------------------------------------------------------------
// Retorna com o byte recebido
//--------------------------------------------------------------------------------------------------------------------------
inline u8 i2c0_GetReceivedByte(void)	{
	//return( inb(TWDR) );
	return  TWDR;
}

//--------------------------------------------------------------------------------------------------------------------------
// Retorna com o estado atual do barramento
//--------------------------------------------------------------------------------------------------------------------------
inline u8 i2c0_GetStatus(void)	{
	//return( inb(TWSR) );
	return TWSR;
}


//###################################################################################################################
//###################################################################################################################
// ROTINAS QUANDO NAO USADO INT
//###################################################################################################################
#if (I2C0_USE_INT == pdOFF)
//--------------------------------------------------------------------------------------------------------------------------
// Manda um buffer de dados para o escravo do endereço do dispositivo no hardware
//--------------------------------------------------------------------------------------------------------------------------
int i2c0_WriteBuffer(u8 AddrI2C, u8 *Buffer, u8 LenBuffer)	{
	i2c0_SendStart();					// Inicio transação
	i2c0_WaitForComplete();				// Espero o barramento ficar pronta
	
	i2c0_SendByte( AddrI2C & 0xFE );		// Transmite o endereço do dispositivo no barramento e especifco que é para Escrita
	i2c0_WaitForComplete();				// Espero o barramento ficar pronta
	
	// Mando o buffer de dados
	while(LenBuffer)	{
		i2c0_SendByte( *Buffer++ );
		i2c0_WaitForComplete();			// Espero o barramento ficar pronta
		LenBuffer--;
	}

	i2c0_SendStop();						// Transmito o fim da transação
	
	//while( !(inb(TWCR) & _BV(TWSTO)) );	// leave with TWEA on for slave receiving
	while( ! (TWCR & _BV(TWSTO) ) );	// leave with TWEA on for slave receiving
	
	_delay_us(I2C0_DELAY_MS);
	
	return pdPASS;
}

//--------------------------------------------------------------------------------------------------------------------------
// Recebe um buffer de dados do escravo do endereço do dispositivo no hardware
//--------------------------------------------------------------------------------------------------------------------------
int  i2c0_ReadBuffer(u8 AddrI2C, u8 *Buffer, u8 LenBuffer)	{
	i2c0_SendStart();					// Inicio transmissão
	i2c0_WaitForComplete();

	i2c0_SendByte( AddrI2C | 0x01 );  	// Transmite o endereço do dispositivo no barramento e especifco que é para leitura
	i2c0_WaitForComplete();
	
	//Recebo os dados e emito ACK
	while(LenBuffer > 1)	{
		i2c0_ReceiveByte(pdTRUE);				
		i2c0_WaitForComplete();
		
		*Buffer++ = i2c0_GetReceivedByte();
		
		LenBuffer--;
	}

	// accept receive data and nack it (last-byte signal)
	i2c0_ReceiveByte(pdFALSE);
	i2c0_WaitForComplete();
	
	*Buffer++ = i2c0_GetReceivedByte();

	i2c0_SendStop();
	while( !(TWCR & _BV(TWSTO)) );
	
	_delay_us(I2C0_DELAY_MS);
	
	return pdPASS;
}
#endif


//###################################################################################################################
//###################################################################################################################
// ROTINAS QUANDO USADO INT
//###################################################################################################################
#if (I2C0_USE_INT == pdON)

//--------------------------------------------------------------------------------------------------------------------------
// Manda um buffer de dados para o escravo do endereço do dispositivo no hardware
//--------------------------------------------------------------------------------------------------------------------------
int i2c0_WriteBuffer(u8 AddrI2C, u8 *Buffer, u8 LenBuffer)	{
	u8 i;
		
	while(i2c0_State);					// Espero o barramento ficar pronto
	
	i2c0_State = I2C0_MASTER_TX;			// Endicar que o barramento está modo mestre TX
	i2c0_AddrDevice = (AddrI2C & 0xFE);	// Salvo o endereço do dispositivo no barramento e especifco que é para Escrita
	
	// Atualizo o buffer de transmissão com o buffer de dados
	for(i = 0; i < LenBuffer; i++)
		i2c0_SendData[i] = *Buffer++;
	
	i2c0_SendDataIndex = 0;
	i2c0_SendDataLen = LenBuffer;
		
	i2c0_SendStart();					// Inicia a transação
	
	_delay_us(I2C0_DELAY_MS);
	
	return pdPASS;
}

//--------------------------------------------------------------------------------------------------------------------------
// Recebe um buffer de dados do escravo do endereço do dispositivo no hardware
//--------------------------------------------------------------------------------------------------------------------------
int i2c0_ReadBuffer(u8 AddrI2C, u8 *Buffer, u8 LenBuffer)	{
	u8 	i;
	
	while(i2c0_State);					// Espero o barramento ficar pronto
	
	i2c0_State = I2C0_MASTER_RX;			// Endicar que o barramento está modo mestre RX
	i2c0_AddrDevice = (AddrI2C | 0x01);	// Salvo o endereço do dispositivo no barramento e especifco que é para Leitura
	i2c0_ReceiveDataIndex = 0;
	i2c0_ReceiveDataLen = LenBuffer;
	
	i2c0_SendStart();					// Inicia a transação
	
	while(i2c0_State);					// Quando o barramento estiver pronto a transação foi finalizada
	
	//Atualiza o buffer de retorno com o buffer de recepção
	for(i = 0; i < LenBuffer; i++)			
		*Buffer++ = i2c0_ReceiveData[i];
	
	_delay_us(I2C0_DELAY_MS);
	
	return pdPASS;
}

//Routinas de interrupcao
SIGNAL(SIG_2WIRE_SERIAL)	{
	u8 
		Status = TWSR & I2C0_MASK_STS;					// Leio os bits de status do barramento I2C

	switch(Status)	{
		// Master General
		case TW_START:												// 0x08: Condição de inicio foi transmitida
		case TW_REP_START:											// 0x10: Condição de reinicio foi transmitida
			i2c0_SendByte(i2c0_AddrDevice);							// Transmito o endereço do dispositivo no barramento
			break;
	
		// Codigos de condições de TX e RX do em modo mestre
		case TW_MT_SLA_ACK:											// 0x18: Endereco do escravo para escrita foi emitido e um reconhecimento foi recebido
		case TW_MT_DATA_ACK:										// 0x28: Dado transmitido e reconhecimento pelo escravo
			if (i2c0_SendDataIndex < i2c0_SendDataLen)
				i2c0_SendByte( i2c0_SendData[i2c0_SendDataIndex++] );	// Mando dados
			else	{
				i2c0_SendStop();										// Condições de para de transmissão, habilitando SLA ACK
				i2c0_State = I2C0_IDLE;								// Seto que o barramento esta ocioso
			}
			break;
		case TW_MR_DATA_NACK:										// 0x58: Dado recebido e também um não reconhecimento do escravo
			i2c0_ReceiveData[i2c0_ReceiveDataIndex++] = TWDR;		// Armazenando o último dado no buffer
		case TW_MR_SLA_NACK:										// 0x48: Endereco do escravo para leitura foi emitido e um não reconhecimento foi recebido
		case TW_MT_SLA_NACK:										// 0x20: Endereco do escravo para escrita foi emitido e um não reconhecimento foi recebido
		case TW_MT_DATA_NACK:										// 0x30: Dado transmitido e um não reconhecimento pelo escravo
			i2c0_SendStop();											// Condições de para de transmissão, habilitando SLA ACK
			i2c0_State = I2C0_IDLE;									// Seto que o barramento esta ocioso
			break;
		case TW_MT_ARB_LOST:										// 0x38: O perdido barramento no modo de leitura ou um NACK no momento errado
			
			//outb(TWCR, (inb(TWCR) & I2C0_MASK_CMD)|_BV(TWINT));		// Restabelece o barramento
			TWCR = (TWCR & I2C0_MASK_CMD)|_BV(TWINT);					// Restabelece o barramento
			
			i2c0_State = I2C0_IDLE;									// Seto que o barramento esta ocioso
			break;
		case TW_MR_DATA_ACK:										// 0x50: Dado recebido e também um reconhecimento do escravo
			i2c0_ReceiveData[i2c0_ReceiveDataIndex++] = TWDR;			// Armazena o dado no buffer
		case TW_MR_SLA_ACK:											// 0x40: Endereco do escravo para leitura foi emitido e um reconhecimento foi recebido
			if(i2c0_ReceiveDataIndex < (i2c0_ReceiveDataLen - 1) )
				i2c0_ReceiveByte(pdTRUE);								// Dado recebido, emitir reconecimento para mais dados
			else
				i2c0_ReceiveByte(pdFALSE);							// Dado recebido, emitir não reconecimento para finalizar TX
			break;
	}
}
#endif


/*
//--------------------------------------------------------------------------------------------------------------------------
// FUNCOES PARA MODO ESCRAVO
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// Especifica o endereço do atmega em modo esrcavo e o modo de reposta de chamada de endereço
//--------------------------------------------------------------------------------------------------------------------------
void i2c_SetLocalDeviceAddr(u8 Addr, u8 GenCallEn)	{
	// Ajusto o endereço local (usado somente no modo escravo)
	outb(TWAR, ((Addr & 0xFE) | (GenCallEn ? 1:0)) );
}

//--------------------------------------------------------------------------------------------------------------------------
//Anexo uma funcao na chegada de dados no modo escravo
//--------------------------------------------------------------------------------------------------------------------------
void i2c_SetHandlerSlaveRX(void (*i2c_SlaveRxFunc)(u8 DataLength, u8* Data))	{
	I2C_SlaveReceive = I2C_SlaveRxFunc;
}

//--------------------------------------------------------------------------------------------------------------------------
//Anexo uma funcao na transmissão de dados no modo escravo
//--------------------------------------------------------------------------------------------------------------------------
void I2C_SetHandlerSlaveTX(u8 (*I2C_SlaveTxFunc)(u8 DataLengthMax, u8* Data))	{
	I2C_SlaveTransmit = I2C_SlaveTxFunc;
}
*/
/*
	// Slave Receiver status codes
	case TW_SR_SLA_ACK:					// 0x60: own SLA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_SLA_ACK:		// 0x68: own SLA+W has been received, ACK has been returned
	case TW_SR_GCALL_ACK:				// 0x70:     GCA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_GCALL_ACK:		// 0x78:     GCA+W has been received, ACK has been returned
		// we are being addressed as slave for writing (data will be received from master)
		// set state
		I2C_State = I2C_SLAVE_RX;
		// prepare buffer
		I2C_ReceiveDataIndex = 0;
		// receive data byte and return ACK
		outb(TWCR, (inb(TWCR)&I2C_MASK_CMD)|_BV(TWINT)|_BV(TWEA));
		break;
	case TW_SR_DATA_ACK:				// 0x80: data byte has been received, ACK has been returned
	case TW_SR_GCALL_DATA_ACK:			// 0x90: data byte has been received, ACK has been returned
		// get previously received data byte
		I2C_ReceiveData[I2C_ReceiveDataIndex++] = inb(TWDR);
		// check receive buffer status
		if(I2C_ReceiveDataIndex < I2C_RX_BUFFER_SIZE)
		{
			// receive data byte and return ACK
			outb(TWCR, (inb(TWCR)&I2C_MASK_CMD)|_BV(TWINT)|_BV(TWEA));
		}
		else
		{
			// receive data byte and return NACK
			outb(TWCR, (inb(TWCR)&I2C_MASK_CMD)|_BV(TWINT));
		}
		break;
	case TW_SR_DATA_NACK:				// 0x88: data byte has been received, NACK has been returned
	case TW_SR_GCALL_DATA_NACK:			// 0x98: data byte has been received, NACK has been returned
		// receive data byte and return NACK
		outb(TWCR, (inb(TWCR)&I2C_MASK_CMD)|_BV(TWINT));
		break;
	case TW_SR_STOP:					// 0xA0: STOP or REPEATED START has been received while addressed as slave
		// switch to SR mode with SLA ACK
		outb(TWCR, (inb(TWCR)&I2C_MASK_CMD)|_BV(TWINT)|_BV(TWEA));
		// i2c receive is complete, call I2C_SlaveReceive
		if(I2C_SlaveReceive) I2C_SlaveReceive(I2C_ReceiveDataIndex, I2C_ReceiveData);
		// set state
		I2C_State = I2C_IDLE;
		break;

	// Slave Transmitter
	case TW_ST_SLA_ACK:					// 0xA8: own SLA+R has been received, ACK has been returned
	case TW_ST_ARB_LOST_SLA_ACK:		// 0xB0:     GCA+R has been received, ACK has been returned
		// we are being addressed as slave for reading (data must be transmitted back to master)
		// set state
		I2C_State = I2C_SLAVE_TX;
		// request data from application
		if(I2C_SlaveTransmit) I2C_SendDataLen = I2C_SlaveTransmit(I2C_TX_BUFFER_SIZE, I2C_SendData);
		// reset data index
		I2C_SendDataIndex = 0;
		// fall-through to transmit first data byte
	case TW_ST_DATA_ACK:				// 0xB8: data byte has been transmitted, ACK has been received
		// transmit data byte
		outb(TWDR, I2C_SendData[I2C_SendDataIndex++]);
		if(I2C_SendDataIndex < I2C_SendDataLen)
			// expect ACK to data byte
			outb(TWCR, (inb(TWCR)&I2C_MASK_CMD)|_BV(TWINT)|_BV(TWEA));
		else
			// expect NACK to data byte
			outb(TWCR, (inb(TWCR)&I2C_MASK_CMD)|_BV(TWINT));
		break;
	case TW_ST_DATA_NACK:				// 0xC0: data byte has been transmitted, NACK has been received
	case TW_ST_LAST_DATA:				// 0xC8:
		// all done
		// switch to open slave
		outb(TWCR, (inb(TWCR)&I2C_MASK_CMD)|_BV(TWINT)|_BV(TWEA));
		// set state
		I2C_State = I2C_IDLE;
		break;

	// Misc
	case TW_NO_INFO:					// 0xF8: No relevant state information
		// do nothing
		break;
	case TW_BUS_ERROR:					// 0x00: Bus error due to illegal start or stop condition
		// reset internal hardware and release bus
		outb(TWCR, (inb(TWCR)&I2C_MASK_CMD)|_BV(TWINT)|_BV(TWSTO)|_BV(TWEA));
		// set state
		I2C_State = I2C_IDLE;
		break;
*/
