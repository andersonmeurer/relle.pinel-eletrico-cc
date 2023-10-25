#include "i2c_io.h"

/*
#if defined ( || )
#error Fosc out of range (10MHz-25MHz). correct and recompile
#endif
*/

// SOMENTE PARA FREQUENCIA DE BARRAMENTO 100kHZ = 10us
//	I2C_DELAY_HALF = 5us
//	I2C_DELAY_QUATER = 2us

//#define I2C_DELAY_HALF		5
//#define I2C_DELAY_QUATER 	2
#define I2C_DELAY_HALF		20
#define I2C_DELAY_QUATER 	10

//----------------------------------------------------------------------------------------------------------------------
// Inicializar barramento I2C
//----------------------------------------------------------------------------------------------------------------------
void i2c_Init(void)	{	
	sbi(I2C_PORT_DRR, I2C_PIN_SDA);				// Configurar o pino SDA como saida
	sbi(I2C_PORT_DRR, I2C_PIN_SCL);				// Configurar o pino SCL como saida
	sbi(I2C_PORT_OUT, I2C_PIN_SDA);				// Colcocar SDA = 1	
	sbi(I2C_PORT_OUT, I2C_PIN_SCL);				// Colcocar SCL = 1
}

//----------------------------------------------------------------------------------------------------------------------
// Mandar um byte no barramento I2C
//----------------------------------------------------------------------------------------------------------------------
unsigned int i2c_SendByte (u8 b)	{
	int i;

	// Fazer para todos os bits
	for (i = 7; i >= 0; i--)	{
		if ( b & (1<<i) )		
			sbi( I2C_PORT_OUT, I2C_PIN_SDA);			// Se bit ligado ativa SDA
		else					
			cbi( I2C_PORT_OUT, I2C_PIN_SDA);   			// Se bit desigado desativa SDA
			
		_delay_us(I2C_DELAY_HALF); // I2C_DELAY_HALF();
			
		// Pulso de clock SCL
		sbi(I2C_PORT_OUT, I2C_PIN_SCL); 
		_delay_us(I2C_DELAY_HALF);
		cbi(I2C_PORT_OUT, I2C_PIN_SCL);
	}

	sbi(I2C_PORT_OUT, I2C_PIN_SDA);						// Manter SDA
	cbi(I2C_PORT_DRR,  I2C_PIN_SDA);					// Configurar porta SDA para entrada
	_delay_us(I2C_DELAY_HALF);
	sbi(I2C_PORT_OUT, I2C_PIN_SCL);						// Clock alto
  	b = I2C_PORT_IN & (1 << I2C_PIN_SDA);				// Capturar ACK ou NACK da parte oposta
	_delay_us(I2C_DELAY_HALF);
	cbi(I2C_PORT_OUT, I2C_PIN_SCL);						// Clock baixo	
	sbi(I2C_PORT_DRR,  I2C_PIN_SDA);					// Configurar porta SDA para saida	
	_delay_us(I2C_DELAY_HALF);

	return (b == 0);									// Retorna resposta da parte oposta (ACK ou NACK)
}

//----------------------------------------------------------------------------------------------------------------------
// Receber um byte no barramento I2C
//----------------------------------------------------------------------------------------------------------------------
u8 i2c_GetByte(unsigned int last) {
	int i;
	u8 c, b = 0;
		
	sbi(I2C_PORT_OUT, I2C_PIN_SDA);						// Ativar pullup em SDA	
	cbi(I2C_PORT_DRR, I2C_PIN_SDA);						// Configurar porta SDA para entrada
	
	// Capturar os bits
	for(i = 7; i >= 0; i--)	{
		_delay_us(I2C_DELAY_HALF);
		sbi( I2C_PORT_OUT, I2C_PIN_SCL);				// Clock alto	  	
  		c = I2C_PORT_IN & (1 << I2C_PIN_SDA);  
	  	
		b <<= 1;		
		if(c) 
			b |= 1;
		
		_delay_us(I2C_DELAY_HALF);
    	cbi( I2C_PORT_OUT, I2C_PIN_SCL);				// Clock baixo
	}

	sbi(I2C_PORT_DRR, I2C_PIN_SDA);						// Configurar porta SDA para saida
  
	if (last)	
		sbi( I2C_PORT_OUT, I2C_PIN_SDA);				// Se é o último dado emitir NAK
	else
		cbi( I2C_PORT_OUT, I2C_PIN_SDA);				// Emitir ACK caso queira mais dados

	_delay_us(I2C_DELAY_HALF);
	
	// Um Pulso de clock
	sbi( I2C_PORT_OUT, I2C_PIN_SCL); 
	_delay_us(I2C_DELAY_HALF);
	cbi( I2C_PORT_OUT, I2C_PIN_SCL);
	
	sbi( I2C_PORT_OUT, I2C_PIN_SDA);
	
	return b;
}

//--------------------------------------------------------------------------------------------------------------------------
// Manda um buffer de dados para o escravo do endereço do dispositivo no hardware
//--------------------------------------------------------------------------------------------------------------------------
void i2c_SendBuffer(u8 AddrI2C, u8 *Buffer, u8 LenBuffer)	{
	// Emitir condição de inicio (START)
	cbi( I2C_PORT_OUT, I2C_PIN_SDA); 	
	_delay_us(I2C_DELAY_QUATER);;
	cbi( I2C_PORT_OUT, I2C_PIN_SCL);
	
	// Transmite o endereço do dispositivo no barramento e especifco que é para Escrita
	I2C_SendByte(AddrI2C & 0xFE); 			
	
	// Mando todos os bytes do buffer
	while (LenBuffer--)
		I2C_SendByte(*Buffer++);
	
	// Emitir condição de parada (STOP) 
	cbi( I2C_PORT_OUT, I2C_PIN_SDA);
	_delay_us(I2C_DELAY_HALF);
	sbi( I2C_PORT_OUT, I2C_PIN_SCL); 
	_delay_us(I2C_DELAY_QUATER);	
	sbi( I2C_PORT_OUT, I2C_PIN_SDA); 
	_delay_us(I2C_DELAY_HALF);
		
  	//Delay(cDelay_10us, cI2C_Delay);					// Necessario para dar um tempo para proxima transação
  	_delay_us(I2C_DELAY);
}

//--------------------------------------------------------------------------------------------------------------------------
// Recebe um buffer de dados do escravo do endereço do dispositivo no hardware
//--------------------------------------------------------------------------------------------------------------------------
void i2c_ReceiverBuffer(u8 AddrI2C, u8 *Buffer, u8 LenBuffer)	{
	int j = LenBuffer;
	u8 *p = Buffer;
	
	_delay_us(I2C_DELAY);
	
	// Emitir condição de inicio (START)
	cbi( I2C_PORT_OUT, I2C_PIN_SDA); 
	_delay_us(I2C_DELAY_QUATER);
	cbi( I2C_PORT_OUT, I2C_PIN_SCL);

	// Transmite o endereço do dispositivo no barramento e especifco que é para leitura
	I2C_SendByte(AddrI2C | 0x01);			

	//  Atualizar o buufer com os dados chegando no barramento
	while (j--)
		*p++ = I2C_GetByte(j == 0);

	// Emitir condição de parada (STOP) 
	cbi( I2C_PORT_OUT, I2C_PIN_SDA);					// clear data line and
	_delay_us(I2C_DELAY_HALF);
	sbi( I2C_PORT_OUT, I2C_PIN_SCL); 
	_delay_us(I2C_DELAY_QUATER);	
	sbi( I2C_PORT_OUT, I2C_PIN_SDA); 
	_delay_us(I2C_DELAY_HALF);
	
	//Delay(cDelay_10us, cI2C_Delay);					// Necessario para dar um tempo para proxima transação
	_delay_us(I2C_DELAY);
}
