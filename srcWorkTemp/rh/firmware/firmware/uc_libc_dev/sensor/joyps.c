/*
	Driver para joystick de playstation ID=0x41 e 0x73
	Esta biblioteca somente l� valores digitais dos bot�es, n�o faz controle de vibra��o ou configura��es
*/

#include "joyps.h"

// #################################################################################################################
// FUN��ES GLOBAIS
// #################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descri��o: Configura o pino CHIP SELECT
//------------------------------------------------------------------------------------------------------------------

// O joystick trabalha com CPOL=1 e CPHA=1.
//	Como foge do padr�o, antes de acessar o joy configuramos o barramento e depois voltamos ao normal. EX:
//			ssp1_Init(SSP0_MASTER, SSP0_CPOL_H|SSP0_CPHA_H|SSP0_DSS_8); // inicar o barramento SPI a 400Khz
//			id = joy_ReadControls(&buttons);
//			ssp1_Init(SSP0_MASTER, SSP0_STANDARD); // inicar o barramento SPI a 400Khz
void joyps_Init(void) {
	joyps_SetupPorts();	// Configurar o pino CS
    joyps_Disable();
}

static u8 swap(u8 v) {
	#if defined(usesSSP) // bus ssp n�o tem transmiss�o LSB
	u8 aux = 0;
	int x; for (x=0; x < 8; x++) if ((v&_BV(x)) > 0) aux |= _BV(7-x);
	return aux;
	#else
	return v;
	#endif
}

// Ler a cada 16ms j� � o suficiente para uma boa resposta
int joy_ReadControls(int* ctrl ) {
	joyps_Enable();
	joyps_TransferByte(swap(0x1)); // Envio o byte para inicializar transferencia ao controle PS1
	int id = swap(joyps_TransferByte(swap(0x42))); // Envio o byte de quesito de dados do controle PS1 e recebo o ID do controle
	joyps_TransferByte(0);	// Envio qualquer valor para o controle para receber dele o indicador de inicio de transmiss�o.
	int b1 = swap(~joyps_TransferByte(0)); // Envio qualquer valor para o controle para receber dele o primeiro comando que representa os estados dos bot�es.
	int b2 = swap(~joyps_TransferByte(0)); // Envio qualquer valor para o controle para receber dele o segundo comando que representa os estados dos bot�es.
	// somente para joystick com suporte anal�gico.
	//	int a1 = swap(~joyps_TransferByte(0));
	//	int a2 = swap(~joyps_TransferByte(0));
	//	int a3 = swap(~joyps_TransferByte(0));
	//	int a4 = swap(~joyps_TransferByte(0));
	joyps_Disable();

	*ctrl = (b1 << 8) | b2;
	return id;
}

//#include "delay.h"
//// para interface humana ler a cada 16ms j� � o suficiente
//int joy_ReadControls(int* ctrl ) {
//	joyps_Enable();
//	delay_us(40);							// Espero por um tempo antes de enviar o primeiro byte
//	joyps_TransferByte(swap(0x1)); delay_us(20);	// Envio o byte para inicializar transferencia ao controle PS1
//	int id = swap(joyps_TransferByte(swap(0x42)));	delay_us(20);	// Envio o byte de quesito de dados do controle PS1 e recebo o ID do controle
//	joyps_TransferByte(0);	delay_us(20);	// Envio qualquer valor para o controle para receber dele o indicador de inicio de transmiss�o.
//	int b1 = swap(~joyps_TransferByte(0)); delay_us(20);	// Envio qualquer valor para o controle para receber dele o primeiro comando que representa os estados dos bot�es.
//	int b2 = swap(~joyps_TransferByte(0)); delay_us(20);	// Envio qualquer valor para o controle para receber dele o segundo comando que representa os estados dos bot�es.
//	// somente para joystick com suporte anal�gico.
//	//	int a1 = swap(~joyps_TransferByte(0));
//	//	int a2 = swap(~joyps_TransferByte(0));
//	//	int a3 = swap(~joyps_TransferByte(0));
//	//	int a4 = swap(~joyps_TransferByte(0));
//	joyps_Disable();
//
//	*ctrl = (b1 << 8) | b2;
//	return id;
//}
