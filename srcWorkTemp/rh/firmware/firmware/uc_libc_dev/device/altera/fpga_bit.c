#include "fpga_bit.h"
#if defined(FREE_RTOS)
#include "systime.h"
#else
#include "delay.h"
#endif

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa as portas IO para configura��o do FPGA Cyclone. 
// Parametros: 	Nenhum 
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void fpga_Init ( void ) {
  	fpga_SetupPorts();	// Configura os pinos DCLK, DATA0, nCONFIF como sa�da e os pinos nSTATUS e CONFIG_DONE como entrada
  	 	
  	fpga_PinConfigSet();	// Pino nCONFIG=1
  	fpga_PinDclk_Clr();		// Pino DCLK=1
  	fpga_PinData0Clr();		// Pino DATA0=0
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Emite um reset para o FPGA sinalizando inicio de configura��o pelo HOST
// Parametros: 	Nenhum
// Retorna: 	pdPASS se o reset foi recebido com sucesso pelo FPGA, sen�o retorna pdFAIL
//------------------------------------------------------------------------------------------------------------------
int fpga_Reset (void ) {
	delay_us(50);			// Datasheet especifica o tempo m�nimo de 40uS entre as configura��es
	fpga_PinConfigClr();	// Pino nCONFIG=0
	delay_us(3);			// Datasheet especifica o tempo m�nimo de 2uS com o pino nCONFIG = 0
	fpga_PinConfigSet();	// Pino nCONFIG=1
	delay_us(50);			// O tempo de 26uS para que o FPGA fique preparado para configura��o

	// CHECA SE O RESET FOI EMITIDO COM SUCESSO AO FPGA
	if (fpga_Ready())  		return pdPASS;
	else					return pdFAIL;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Envia um byte para o FPGA, transmitindo o bit menos significativo primeiro
// Parametros: 	Byte a ser transmitido
// Retorna: 	pdPASS se o byte foi recebido com sucesso pelo FPGA, sen�o retorna pdFAIL
//------------------------------------------------------------------------------------------------------------------
int fpga_SendByte ( u8 data ) {
	u8 x;
	
	for (x=0; x<8; x++) {												// Fa�a para os 8 bits
		(data& (1<<x))? (fpga_PinData0Set()) : (fpga_PinData0Clr());	// Ajusta o DATA0 de acordo com o bit
		
		fpga_PinDclk_Set();												// Pino DCLK = 1
		//delay 5ns
		fpga_PinDclk_Clr();												// Pino DCLK = 0
		//delay 5ns
	}

	// CHECA SE O BYTE FOI EMITIDO COM SUCESSO AO FPGA
	if (fpga_Ready())  	return pdPASS;
	else				return pdFAIL;
}
