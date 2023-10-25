/*
 * ATENÇÃO: Recomendo a não acessar no barramento modbus não muita variáveis por vez por causa do erro do boudrate
 */

#include "app.h"
#if (LOG_MODBUS_SLAVE == pdON) || (LOG_MODBUS_MASTER == pdON)
#include "stdio_uc.h"
#endif

#include "uart1_irq.h"
#define uartM_Init			uart1_Init
#define uartM_EmptyTx 		uart1_EmptyTx
#define uartM_GetChar 		uart1_GetChar
#define uartM_BufferQtdRx 	uart1_BufferQtdRx
#define uartM_ClearBufferRx uart1_ClearBufferRx
#define uartM_WriteTx		uart1_WriteTx

#include "uart3_irq.h"
#define uartS_Init			uart3_Init
#define uartS_EmptyTx 		uart3_EmptyTx
#define uartS_GetChar 		uart3_GetChar
#define uartS_BufferQtdRx 	uart3_BufferQtdRx
#define uartS_ClearBufferRx uart3_ClearBufferRx
#define uartS_WriteTx		uart3_WriteTx

// status dos sensores
//    stsSENSOR_READING	= 0;
//    stsSENSOR_OK	    = 1;
//    stsSENSOR_ERRMIN  = 2;
//    stsSENSOR_ERRMAX  = 3;

//    tSensor = record
//        sts:integer;            // Status do sensor de temperatura
//                                //      0: Sinaliza que o multimetro está lendo pela primeira vez.
//                                //          Isto somente acontece no momento que o multimetro é ligado.
//                                //      1: O Multimetro já contém valor da amp/volts;
//                                //      2: Sinaliza um erro, indica que o valor está abaixo da escala
//                                //      3: Sinaliza um erro, indica que o valor está acima da escala
//        value:integer;      // valores em miliampers ou milivolts
//        valueMin:integer;
//        valueMax:integer;
//    end;

static int slave_SetValue(uint addr, u16 value __attribute__ ((unused)));
static int slave_PutString(u8* buffer, u16 count);
static int slave_GetValue(uint addr, u16* value);
static int slave_ReadRegisters(uint addrInit, u8* query, uint count);
static int slave_WriteRegister(uint addr, u16 value);
static int slave_WriteRegisters(uint addrInit, u8* query, uint count);

static int master_PutString(u8* buffer, u16 count);

void modbus_Init(uint id) {
	// SLAVE
	modbus_SlaveInit(id, slave_PutString, uartS_GetChar, uartS_BufferQtdRx, uartS_ClearBufferRx);
	modbus_SlaveAppendFunctions(slave_ReadRegisters, slave_WriteRegister, slave_WriteRegisters);
	rs485_ENTXS_DIR |= rs485_ENTXS;
	rs485_ENTXS_OFF = rs485_ENTXS;
	uartS_Init(BAUDRATE_SBC);

	// MASTER
	modbus_MasterInit(master_PutString, uartM_GetChar, uartM_ClearBufferRx);
	modbus_MasterTimeout(3000);
	rs485_ENTXM_DIR |= rs485_ENTXM;
	rs485_ENTXM_OFF = rs485_ENTXM;
	uartM_Init(BAUDRATE_MULTIMETER);
}

// ##################################################################################################
// SLAVE
// ##################################################################################################
static int slave_PutString (u8* buffer, u16 count) {
	rs485_ENTXS_ON = rs485_ENTXS;
	int ret = uartS_WriteTx(buffer, count);
	while (!uartS_EmptyTx()); // Vamos esperar que o serial envie a mensagem
	rs485_ENTXS_OFF = rs485_ENTXS;
	return ret;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê registradores
// Parametros:	addr: Endereço do registrador
//				value: Ponteiro para o retorno do valor de 16bits
// Retorna:		0: Se a leitura foi feita com suscesso
//				-1: O endereço do registrador não existe (modbusILLEGAL_DATA_ADDRESS (02))
// -------------------------------------------------------------------------------------------------------------------
static int slave_GetValue(uint addr, u16* value) {
	int ret = 0;
	uint addrMultimeter=0;

	if ( (addr >= 0x400) && (addr <= 0x43F) ) {
		addrMultimeter = (uint)((addr-0x400)/4); // encontrar o indice do multimetro no vetor de acordo com o addr
		addr = (uint)((addr%4)+0x400); // Conveter sempre para os endereços bases
		#if (LOG_MODBUS_SLAVE == pdON)
    	plognp("    [addrMultimeter=0x%x] [addrbase=0x%x]"CMD_TERMINATOR, addrMultimeter, addr);
    	#endif
	}

	switch (addr) {

	// ---------------------------------------------------------------------------------------------------------------------------------------
	// ID
	case 0:	*value = (MODEL[1] << 8) | MODEL[0]; break;
	case 1:	*value = (MODEL[3] << 8) | MODEL[2]; break;
	case 2:	*value = (FIRMWARE_VERSION[2] << 8) | FIRMWARE_VERSION[0]; break;

	// ---------------------------------------------------------------------------------------------------------------------------------------
	// REGISTRADORES DE TRABALHO
	case 0x10: *value = control.nMultimetersGeren; break;

	// ---------------------------------------------------------------------------------------------------------------------------------------
	// REGISTRADORES DAS SAÍDAS DIGITAIS
	case 0x200: *value = control.douts; break;

	// ---------------------------------------------------------------------------------------------------------------------------------------
	// REGISTRADORES DOS RELES
	case 0x300: *value = control.relays; break;

	// ---------------------------------------------------------------------------------------------------------------------------------------
	// REGISTRADORES DOS MULTIMETROS
	case 0x400: *value = control.multimeter[addrMultimeter].stsCom; break;
	case 0x401: *value = control.multimeter[addrMultimeter].sts; break;
	case 0x402: *value = control.multimeter[addrMultimeter].valueLSW; break;
	case 0x403: *value = control.multimeter[addrMultimeter].valueMSW; break;

	// ---------------------------------------------------------------------------------------------------------------------------------------
	// retorna sinalizando acesso em um registrador inválido
	default:
		ret = -1;
	}

	return ret;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Escrever nos registradores
// Parametros:	addr: Endereço do registrador
//				value: valor de 16bits para ser gravado no registrador
// Retorna:		0: Se a gravação foi feita com suscesso
//				-1: O endereço do registrador não existe (modbusILLEGAL_DATA_ADDRESS (2))
// 				-2: Dado a ser gravado é inválido (modbusILLEGAL_DATA_VALUE (3))
//				-3: No momento o dispositivo não pode gravar o valor, ou executar a tarefa (modbusSLAVE_DEVICE_FAILURE (4))
// -------------------------------------------------------------------------------------------------------------------
static int slave_SetValue(uint addr, u16 value __attribute__ ((unused))) {
	int ret = 0;

	switch (addr) {

	// ---------------------------------------------------------------------------------------------------------------------------------------
	// REGISTRADORES DE TRABALHO
	case 0x10:
		// if ( (value < 1) || (value > 16)) return -2; // RERTIRADO PORQUE O PESSOAL DA UFSC NÃO ESTÁ SABENDO COMO TRATAR AS EXCEÇÕES NO SBC
		if ( (value > 0) && (value < 17)) control.nMultimetersGeren = value;
		break;

	// ---------------------------------------------------------------------------------------------------------------------------------------
	// REGISTRADORES DAS SAÍDAS DIGITAIS
	case 0x200: app_SetOut(value); break;

	// ---------------------------------------------------------------------------------------------------------------------------------------
	// REGISTRADORES DOS RELES
	case 0x300: app_SetRelay(value); break;

	// ---------------------------------------------------------------------------------------------------------------------------------------
	// retorna sinalizando acesso em um registrador inválido
	default:
		ret = -1;
	}

	return ret;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Função complementar do modbus slave para ler os registradores
// Parametros:	Ver exemplos no modbus_Slave função modbus_SlaveAppendFunctions
// Retorna:		A função deve retornar
//					modbusNO_ERROR: Leitura feito com sucesso
//					modbusILLEGAL_DATA_ADDRESS:	Endereço do registrador inexistente
//					modbusILLEGAL_DATA_VALUE:	Valor inválido
//					modbusSLAVE_DEVICE_FAILURE:	Um irrecuperável erro ocorreu enquanto o dispositivo estava tentando
//													executar a ação solicitada.
// -------------------------------------------------------------------------------------------------------------------
static int slave_ReadRegisters(uint addrInit, u8* query, uint count) {
	// checa se o endereço está dentro dos limite
	if ((count == 0) || (count > 120) ) return modbusILLEGAL_DATA_ADDRESS;

	// preenche o buffer modbus com os valores dos registradores
	int ret;
	u16 value;
	uint i; for (i=0;i<count;i++) {
		ret = slave_GetValue(addrInit+i, &value);
		if (ret == -1) return modbusILLEGAL_DATA_ADDRESS;
		if (ret == -2) return modbusILLEGAL_DATA_VALUE;

		query[2*i] = (u8)(value>>8);
		query[2*i+1] = (u8)(value&0xFF);

		#if (LOG_MODBUS_SLAVE == pdON)
    	plognp("MODBUS: [i=%d %d] [a=0x%x] [msb=0x%x %c] [lsb=0x%x %c]"CMD_TERMINATOR, i, count, addrInit+i, query[2*i], query[2*i], query[2*i+1], query[2*i+1]);
    	#endif
	}

	return modbusNO_ERROR; // retorna sinalizando que a operação ocorreu bem
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Função complementar do modbus slave para escrever no registrador
// Parametros:	Ver exemplos no modbus_Slave função modbus_SlaveAppendFunctions
// Retorna:		A função deve retornar
//					modbusNO_ERROR: Escrita feita com sucesso
//					modbusILLEGAL_DATA_ADDRESS:	Endereço do registrador inexistente
//					modbusILLEGAL_DATA_VALUE:	Valor para escrita é inválido
//					modbusSLAVE_DEVICE_FAILURE:	Um irrecuperável erro ocorreu enquanto o dispositivo estava tentando
//													executar a ação solicitada.
// -------------------------------------------------------------------------------------------------------------------

static int slave_WriteRegister(uint addr, u16 value) {
	int ret;

	#if (LOG_MODBUS_SLAVE == pdON)
    plognp("MODBUS: WRITE REG [a=0x%x] [val=0x%x]"CMD_TERMINATOR, addr, value);
    #endif

	ret = slave_SetValue(addr, value);
	if (ret == -1) ret = modbusILLEGAL_DATA_ADDRESS;
	else if (ret == -2) ret = modbusILLEGAL_DATA_VALUE;
	else if (ret == -3) ret = modbusSLAVE_DEVICE_FAILURE;
	else ret = modbusNO_ERROR; // retorna sinalizando que a operação ocorreu bem

    return ret;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Função complementar do modbus slave para escrever nos registradores
// Parametros:	Ver exemplos no modbus_Slave função modbus_SlaveAppendFunctions
// Retorna:		A função deve retornar
//					modbusNO_ERROR: Escrita feita com sucesso
//					modbusILLEGAL_DATA_ADDRESS:	Endereço do registrador inexistente
//					modbusILLEGAL_DATA_VALUE:	Valor para escrita é inválido
//					modbusSLAVE_DEVICE_FAILURE:	Um irrecuperável erro ocorreu enquanto o dispositivo estava tentando
//													executar a ação solicitada.
// -------------------------------------------------------------------------------------------------------------------
static int slave_WriteRegisters(uint addrInit, u8* query, uint count) {
	// checa se o endereço está dentro dos limite
	//if ((count == 0) || (addrInit + (count-1) >= 3)) return modbusILLEGAL_DATA_ADDRESS;
	if ((count == 0) || (count > 120) ) return modbusILLEGAL_DATA_ADDRESS;

	// salva os valores recebidos nos seus respectivos registradores
	int ret;
	uint i; for (i=0;i<count;i++) {
		ret = slave_SetValue(addrInit+i, (((u8)query[2*i]<<8) | query[2*i+1]));
		if (ret == -1) return modbusILLEGAL_DATA_ADDRESS;
		else if (ret == -2) return modbusILLEGAL_DATA_VALUE;
		else if (ret == -3) return modbusSLAVE_DEVICE_FAILURE;

		#if (LOG_MODBUS_SLAVE == pdON)
    	plognp("MODBUS: [a=0x%x] [v=0x%x]"CMD_TERMINATOR, addrInit, (((u8)query[2*i]<<8) | query[2*i+1]));
    	#endif
	}

	return modbusNO_ERROR; // retorna sinalizando que a operação ocorreu bem
}


// ##################################################################################################
// MASTER
// ##################################################################################################
static int master_PutString(u8* buffer, u16 count) {
	rs485_ENTXM_ON = rs485_ENTXM;
	int ret = uartM_WriteTx(buffer, count);
	while (!uartM_EmptyTx()); // Vamos esperar que o serial envie a mensagem
	rs485_ENTXM_OFF = rs485_ENTXM;
	return ret;
}

static int waitResponse = pdFALSE;
static tCommand cmd;
static u16 regs[120]; // registrador de trabalho para troca de dados com os multimetros

// para gravação os valores dos registradores devem estar preenchidos
// após a leitura os registradores estarão preenchidos

// Envia um comando para o escravo, nesta mesma função é feito o procedimento de erro de envio
// Uma vez enviado o comando com sucesso caprturar resposta no modbus_Process
//	BUSY: Ficar na espera da resposta
//  ERROR: Notificar  o erro e tomar procedimento cabíveis
//  OK para escrita: Nada, pois os valores dos registradores foram salvos no escravo com sucesso
//	OK para Leitura: Capturar os valores dos registradores lidos do escravo
// Parametros
// slaveID. Endereço do escravo a ser acessado
// c: Tipo de comando para ser enviado ao escravo
// funcResponse: Ponteiro da função para processar a resposta da comunicação
//void modbus_SendCommand(uint slaveID, tCommand cmd, int (*funcComResponse)(void)) {
typedef enum {readREGS, writeREG, writeREGS} tcmd;
void modbus_SendCommand(uint slaveID, tCommand c) {
    if (waitResponse) return;// pdFAIL;

	cmd = c;
    // APONTA QUAIS REGISTRADORES A ACESSAR NO DISPOSITIVO
    // -----------------------------------------------------------------------------------------------------------------

    // Comando para ler os registradores: modelo, versão firmware e modo de operações dos reles
    tcmd typeCMD = readREGS;
    uint addrInit = 0;
    uint nRegs = 3;
	u16 value = 0;

    // Comando para ler os valores dos sensores
    if (cmd == cmdGET_MULTIMETER) {
        addrInit = 0x200;
        nRegs = 3;
	}

   	// ENVIA O COMANDO AO DISPOSITIVO ESCRAVO
   	// -----------------------------------------------------------------------------------------------------------------
   	int ret;
    if (typeCMD == writeREG) {
    	#if (LOG_MODBUS_MASTER == pdON)
    	plognp("modbus WriteReg [slave %d] [reg 0x%x] [value 0x%x]"CMD_TERMINATOR, slaveID, addrInit, value);
    	#endif
		ret =  modbus_MasterWriteRegister(slaveID, addrInit, value);
	} else if (typeCMD == writeREGS) {
	   	#if (LOG_MODBUS_MASTER == pdON)
    	plognp("modbus WriteRegs [slave %d] [reg 0x%x] [len %d]"CMD_TERMINATOR, slaveID, addrInit, nRegs);
    	#endif
        ret = modbus_MasterWriteRegisters(slaveID, addrInit, nRegs, regs);
    } else {
    	#if (LOG_MODBUS_MASTER == pdON)
    	plognp("modbus ReadRegs [slave %d] [reg 0x%x] [len %d]"CMD_TERMINATOR, slaveID, addrInit, nRegs);
    	#endif
		ret = modbus_MasterReadRegisters(slaveID, addrInit, nRegs, regs);
	}

	// se foi enviado com sucesso ficaremos na espera da resposta do multimetro
	if (ret == pdPASS) waitResponse = pdTRUE;
	#if (LOG_MODBUS_MASTER == pdON)
	else plognp("modbus err[%d] send querie"CMD_TERMINATOR, modbus_MasterReadStatus());
    #endif
}

static uint multimeterAccess = 1;
void modbus_Process(void) {
	modbus_SlaveProcess();
	modbus_MasterProcess();

	// se não estamos esperando a resposta do SendCommand vamos analisar o próximo comando a ser enviado
	if (!waitResponse) {
		if (multimeterAccess >= control.nMultimetersGeren) multimeterAccess = 1; else multimeterAccess++;
		modbus_SendCommand(multimeterAccess, cmdGET_MULTIMETER);
		return;
	}

	int ret = modbus_MasterReadStatus();
	//	BUSY: Ficar na espera da resposta
	//  ERROR: Notificar  o erro e tomar procedimento cabíveis
	//  OK para escrita: Nada, pois os valores dos registradores foram salvos no escravo com sucesso
	//	OK para Leitura: Capturar os valores dos registradores lidos do escravo

	// se ainda está ocupado não faz nada
	if (ret == errMODBUS_BUSY) return;
	waitResponse = pdFALSE;
	// se for algum erro
	if (ret < 0) {
    	#if (LOG_MODBUS_MASTER == pdON)
    	plognp("Multimeter %d"CMD_TERMINATOR, multimeterAccess);
    	plognp("modbus err[%d] wait response "CMD_TERMINATOR, ret);
		#endif
		if (cmd == cmdGET_MULTIMETER) {
			if (ret == errMODBUS_EXCEPTION)
		 		control.multimeter[multimeterAccess-1].stsCom = modbus_MasterReadException();
					// modbusILLEGAL_FUNCTION: O multimetro recebeu uma função que não foi implementada ou não foi habilitada.
					// modbusILLEGAL_DATA_ADDRESS: O multimetro precisou acessar um endereço inexistente.
					// modbusILLEGAL_DATA_VALUE: O valor contido no campo de dado não é permitido pelo multimetro. Isto indica uma falta de informações na estrutura do campo de dados.
					// modbusSLAVE_DEVICE_FAILURE: Um irrecuperável erro ocorreu enquanto o multimetro estava tentando executar a ação solicitada.
			else control.multimeter[multimeterAccess-1].stsCom = 0; // sinaliza que não há comunicação com o multimetro
		}
		return;
	}

    // ATUALIZA VARS COM A LEITURA
    // -----------------------------------------------------------------------------------------------------------------

	#if (LOG_MODBUS_MASTER == pdON)
   	plognp("Response multimeter %d"CMD_TERMINATOR, multimeterAccess);
   	#endif

    // Comando para ler os registradores: modelo e versão firmware
    if (cmd == cmdGET_INFO) {
    	#if (LOG_MODBUS_MASTER == pdON)
    	plognp("Multimeter %d"CMD_TERMINATOR, multimeterAccess);
    	plognp("model %c%c%c%c"CMD_TERMINATOR, (regs[0] & 0xff), (regs[0] >> 8), (regs[1] & 0xff), (regs[1] >> 8));
    	plognp("firware %c.%c"CMD_TERMINATOR, (regs[2] & 0xff), (regs[2] >> 8));
    	#endif
    	control.multimeter[multimeterAccess-1].model[0] = (regs[0] & 0xff);
    	control.multimeter[multimeterAccess-1].model[1] = (regs[0] >> 8);
    	control.multimeter[multimeterAccess-1].model[2] = (regs[1] & 0xff);
    	control.multimeter[multimeterAccess-1].model[3] = (regs[1] >> 8);
    	control.multimeter[multimeterAccess-1].model[4] = 0;
    	control.multimeter[multimeterAccess-1].firmware[0] = (regs[2] & 0xff);
    	control.multimeter[multimeterAccess-1].firmware[1] = (regs[2] >> 8);
    	control.multimeter[multimeterAccess-1].firmware[2] = 0;

	// Comando para ler o multimetro
    } else if (cmd == cmdGET_MULTIMETER) {
		control.multimeter[multimeterAccess-1].stsCom = 5; // sinaliza que a conexão fou feita com sucesso
		control.multimeter[multimeterAccess-1].sts =  regs[0] & 0x1f;
		control.multimeter[multimeterAccess-1].valueLSW = regs[1];
		control.multimeter[multimeterAccess-1].valueMSW = regs[2];
	}
}
