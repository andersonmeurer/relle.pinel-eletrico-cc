/* Criado em 17/03/2015
 * Atualiza��o:
 * 	12/08/2016 - Retirado controle de tempo pela fun��o now
 * 	29/04/2015
 *
 *
 *
 * Biblioteca MODBUS escravo barramento serial rs232/rs485 no modo RTU (Bin�rio)
 *
 * Este protocolo atende somente as fun��es:
 * 	 Leituta de muitos registradores, c�digo 3,
 * 	 Escrita em um simples registrador, c�digo 6
 * 	 Escrita em muitos registradores, c�digo 16
 *
 * Esta lib somente captura as mensagens quando o barramento serial fique em silencio no minimo 5ms.
 * Ent�o, o timeout do mestre na espera de uma resposta deste dispostivo deve ser superior a 5ms.
 * Geralmente usamos 3 segundos.
 * Entretanto, o mestre pode se comunicar com outros dispositivos que utilizam a t�cnica t35,
 * onde o tempo de silencio do barramento � muito menor. Neste caso, esta lib vai ignorar as mensagens
 * por considerar que as mensagens s�o destinadas para outros dispositivos.
 *
 * Os endere�os dos registradores devem ser passados pelos seus valores reais e n�o por valores enumerados,
 *  ou seja, valores de 0 a N.
 *
 * T�cnica t35 = Tempo do barramento em sil�ncio que � 3.5 vezes o tamanho de um char no barramento.
 *
 * Para detalhes do protocolo consunte o documento D:\meus_conhecimentos\_devices_misc__\modbus\resumo_modbus.docx
 *
 * */


#include "modbus_slave.h"
#if (MODBUS_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

typedef struct {
	int slaveID;						// Endere�o desse dispostivo no barramento modbus
	int (*puts)(u8 *buffer, u16 count);	// Ponteiro da fun��o para envio de bytes
	int (*getc)(u8 *ch);				// Ponteiro da fun��o para recebimento de bytes
	int (*byteAvailable)(void);			// Ponteiro da fun��o para checagem se h� bytes j� recebidos
	void(*flushRX)(void);				// Ponteiro da fun��o para limpar o buffer de recep��o
	volatile uint timeout; 				// Determina o tempo de silencio do barramento

	// vars aux para comunica��o atual
	u8 query[256];						// Buffer aux de transmiss�o e recep��o de dados
	int cmd;							// Comando (fun��o) solicitado
	int lastLenBuffer;

	// Fun��es externas para antender os comandos recebidos
	int (*read_regs)(uint addrInit,  u8* query, uint count);
	int (*write_reg)(uint addr, u16 value);
	int (*write_regs)(uint addrInit, u8* query, uint count);
} modbusSlave_t;

static modbusSlave_t modbus;

static int modbus_GetPacket(u8* query);
static void modbus_SendPacketException(int exception);
static void modbus_SendPacketRegs(u8* query, int len);

// -------------------------------------------------------------------------------------------------------------------
// FUN��O:		modbus_SlaveInit
// Descri��o: 	Inicializa o protocolo modbus no modo escravo
// Parametros:	slaveID: Endere�o do escravo
//				puts_func:	Ponteiro da fun��o de transmiss�o serial
//				getc_func:	Ponteiro da fun��o de recep��o de dados
//				byte_available_func: Ponteiro da fun��o para verificar se h� dados no buffer de recep��o serial
//				flushRX_func: Ponteiro da fun��o que limpar os buffers seriais
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------

// ATEN��O: O tamanho do buffers de recep��o e transmiss�o deve ter no m�nimo o frame do modbus, recomendo deixar no m�nimo  256 bytes
// se usar rs485 tem que ter cuidado para fazer a invers�o do barramento. exemplo
//		int uart_PutString(u8* buffer, u16 count) {
//			rs485_ENTX_ON = rs485_ENTX;
//			int ret = uart1_WriteTx(buffer, count);
//			while (!uart1_EmptyTx()); // Vamos esperar que o serial envie a mensagem
//			rs485_ENTX_OFF = rs485_ENTX;
//			return ret;
//		}

void modbus_SlaveInit(
	int slaveID,
	int(*puts_func)(u8* buffer, u16 count),
	int(*getc_func)(u8* ch),
	int(*byte_available_func)(void),
	void(*flushRX_func)(void)
) {

	modbus.slaveID = slaveID;
	modbus.puts = puts_func;
	modbus.getc = getc_func;
	modbus.byteAvailable = byte_available_func;
	modbus.flushRX = flushRX_func;
	modbus.lastLenBuffer = 0;
	modbus.timeout = 0;

	modbus.read_regs = NULL;
	modbus.write_reg = NULL;
	modbus.write_regs = NULL;
}

void modbus_SlaveSetID(int slaveID) {
	modbus.slaveID = slaveID;
}

// -------------------------------------------------------------------------------------------------------------------
// FUN��O:		modbus_AppendFunctions
// Descri��o: 	Aponta para as fun��es de controle de acesso aos registradores

// Parametros:	readregs_func: L� N valores dos registradores, ver mais detalhes abaixo
//				writereg_func:	Escreve um valor em um registrador de 16 bits
//				writeregs_func: Escreve N valores nos registradores, lembrando que o endere�amento de registradores de 16bits
// Retorna:		C�digo MODBUS, ver abaixo
// -------------------------------------------------------------------------------------------------------------------
// as fun��es devem ter a seguinte forma:
//		readregs_func: L� N valores dos registradores, lembrando que o endere�amento de registradores de 16bits
//			int nomeFunc(uint addrInit, u8* query, uint count);
//				addrInit: Endere�o do primeiro registrador de 16 bits a ser acessado
//				query: Buffer para ser armazenado os valores dos registradores para serem enviados ao mestre
//				count: Quantidade de registradores devem ser lidos
//		writereg_func: Escreve um valor em um registrador de 16 bits
//			int nomeFunc(uint addr, u16 value);
//				addr: Endere�o do registrador de 16 bits a ser gravado
//				value: Valor a ser gravado no registrador
//		writeregs_func: Escreve N valores nos registradores, lembrando que o endere�amento de registradores de 16bits
//			int nomeFunc(uint addrInit, u8* query, uint count);
//				addrInit: Endere�o do primeiro registrador de 16 bits a ser gravado
//				query: Buffer com os valores dos registradores a serem salvos
//				count: Quantidade de registradores devem ser gravados

// 	A fun��es devem retornar com os seguintes valores:
//		modbusNO_ERROR caso processador correto
//		modbusILLEGAL_DATA_ADDRESS:	Se vai acessar o endere�o inixistente
//		modbusILLEGAL_DATA_VALUE:	A quantidade de registradores a ser lido � inv�lido
//		modbusSLAVE_DEVICE_FAILURE:	Um irrecuper�vel erro ocorreu enquanto o servidor estava tentando executar a a��o solicitada.

// Exemplo para readregs_func:
//		int read_Registers(uint addrInit, u8* query, uint count) {
//			// checa se o endere�o est� dentro dos limite
//			if ((count == 0) || (addrInit + (count-1) >= 4)) return modbusILLEGAL_DATA_ADDRESS;
//
//			// preenche o buffer modbus com os valores dos registradores
//			uint i; for (i=0;i<count;i++) {
//				query[2*i] = (u8)(regs[addrInit+i]>>8);
//				query[2*i+1] = (u8)regs[addrInit+i]&0xFF;
//			}
//
//			return modbusNO_ERROR; // retorna sinalizando que a opera��o ocorreu bem
//		}

// Exemplo para writereg_func:
//		static int write_Register(uint addr, u16 value) {
//			// checa se o endere�o est� dentro dos limite
//			if (addr >= 4) return modbusILLEGAL_DATA_ADDRESS;
//			regs[addr] = value;
//			return modbusNO_ERROR; // retorna sinalizando que a opera��o ocorreu bem
//		}

// Exemplo para writeregs_func:
//		static int write_Registers(uint addrInit, u8* query, uint count) {
//			// checa se o endere�o est� dentro dos limite
//			if ((count == 0) || (addrInit + (count-1) >= 4)) return modbusILLEGAL_DATA_ADDRESS;
//
//			// preenche o buffer modbus com os valores dos registradores
//			uint i; for (i=0;i<count;i++) {
//				regs[addrInit+i] = ((u8)query[2*i]<<8) | query[2*i+1];
//			}
//
//			return modbusNO_ERROR; // retorna sinalizando que a opera��o ocorreu bem
//		}

// Exemplo de append
//		modbus_SlaveAppendFunctions(read_Registers, write_Register, NULL);
//		modbus_SlaveAppendFunctions(read_Registers, NULL, NULL);
void modbus_SlaveAppendFunctions(
	int (*readregs_func)(uint addrInit, u8* query, uint count),
	int (*writereg_func)(uint addr, u16 value),
	int (*writeregs_func)(uint addrInit, u8* query, uint count)
) {
	modbus.read_regs = readregs_func;
	modbus.write_reg = writereg_func;
	modbus.write_regs = writeregs_func;
}

// -------------------------------------------------------------------------------------------------------------------
// FUN��O:		modbus_GetPacket
// Descri��o: 	Captura o pacote de dados que est�o o buffer da serial e j� calcula o CRC
// Parametros:	query: Ponteiro do buffer para salvar o pacote
// Retorna:		retorna com a quantidade de bytes no buffer query, ou o valor do erro
// -------------------------------------------------------------------------------------------------------------------
static int modbus_GetPacket(u8* query) {
	int len = 0;
	u8 ch;

	// capturo os dados do buffer da serial e passo para o buffer modbus
	// fa�o enquanto h� dados no buffer de recep��o
	#if (MODBUS_USE_DEBUG == pdON)
	plognp("MODBUS: GET PACKET: ");
	#endif

	while (modbus.getc(&ch) == pdPASS) {
		if (len > 255) return errMODBUS_BUFRX_OVERFLOW;
		query[len++] = ch;
		#if (MODBUS_USE_DEBUG == pdON)
		plognp("0x%x ", ch);
		#endif
	}

	// o menor pacote � de 4 bytes
	if (len < 4) {
		#if (MODBUS_USE_DEBUG == pdON)
		plognp(CMD_TERMINATOR);
		plognp("ERROR: len packet "CMD_TERMINATOR);
		#endif

		return errMODBUS_BUFRX_INCOMPLETE;
	}

	// processar CRC
	len-=2; // desconsidera o CRC
	uint crc = crc16_MODBUS(query, len);
	uint crc_rx = ( query[len+1] << 8) | (query[len]);

	#if (MODBUS_USE_DEBUG == pdON)
    plognp(CMD_TERMINATOR);
    plognp("MODBUS: len %d ", len);
    plognp("query: ");
    int db_x; for(db_x=0;db_x<len;db_x++) plognp("0x%x ", query[db_x]);
    plognp(CMD_TERMINATOR);
    plognp("MODBUS: crc 0x%x == 0x%x"CMD_TERMINATOR, crc_rx, crc);
    #endif

	if (crc != crc_rx) return errMODBUS_CRC;

	return len;
}

// -------------------------------------------------------------------------------------------------------------------
// FUN��O:		modbus_SendPacketException
// Descri��o: 	Envia uma mensagem de exce��o para o mestre
// Parametros:	exception: C�digo da exce��o
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
static void modbus_SendPacketException(int exception) {
	modbus.query[0] = modbus.slaveID;
	modbus.query[1] = 0x80 + modbus.cmd;
	modbus.query[2] = exception;
	uint crc = crc16_MODBUS(modbus.query, 3);
	modbus.query[3] = (u8)(crc&0xFF);
	modbus.query[4] = (u8)(crc>>8);

	modbus.puts(modbus.query, 5);

	#if (MODBUS_USE_DEBUG == pdON)
    plognp("MODBUS: SEND EXCPETION: ");
    int db_x; for(db_x=0;db_x<5;db_x++) plognp("0x%x ", modbus.query[db_x]);
    plognp(CMD_TERMINATOR);
    plognp("MODBUS: crc 0x%x"CMD_TERMINATOR, crc);
    #endif
}

// -------------------------------------------------------------------------------------------------------------------
// FUN��O:		modbus_SendPacketException
// Descri��o: 	Envia uma mensagem com os valores lidos dos registradores
// Parametros:	query: Buffer j� contendo os valores dos registradores
//				len: Quantidade de registradores de 16 bits
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
static void modbus_SendPacketRegs(u8* query, int len) {
	query[0] = modbus.slaveID;
	query[1] = modbus.cmd;
	query[2] = (u8)(2*len);
	uint crc = crc16_MODBUS(query, (2*len)+3);
	query[(2*len)+3] = (u8)(crc&0xFF);
	query[(2*len)+4] = (u8)(crc>>8);
	modbus.puts(query, (2*len)+5);

	#if (MODBUS_USE_DEBUG == pdON)
    plognp("MODBUS: SEND PCK [%d]: ", len);
    int db_x; for(db_x=0;db_x<(2*len)+5;db_x++) plognp("0x%x ", query[db_x]);
    plognp(CMD_TERMINATOR);
    plognp("MODBUS: crc 0x%x"CMD_TERMINATOR, crc);
    #endif
}

// -------------------------------------------------------------------------------------------------------------------
// FUN��O:		modbus_SlaveProcess
// Descri��o: 	Polling dde controle do modo escravo
// -------------------------------------------------------------------------------------------------------------------
// exemplos de pacotes
// 		len	  	 0    1    2	3    4    5    6    7
// 			 	0x01 0x03 0x00 0x00 0x00 0x02 0xC4 0x0B 	lendo dois regs no device 1 addr 2
//				0x04 0x03 0x00 0x0F 0x00 0x02 0xF4 0x5D		lendo dois regs no device 4 addr 2
int modbus_SlaveProcess(void) {
   	int len = modbus.byteAvailable();

   	// se n�o h� nada no buffer n�o fazemos nada
    if ( len == 0 ) {
    	modbus.lastLenBuffer = 0;
        return pdFAIL;
    }

    // reinicia o timeout caso o buffer de recep��o vai aumentando
    // se estourar o buffer o driver serial se encarrega disso, e neste caso n�o processamos nada
    // mesmo com o buffer cheio com dados lixo os dados ser�o todos processados e o CRC n�o vai bater e tudo ser� descartado
    if (modbus.lastLenBuffer != len) {
    	modbus.lastLenBuffer = len;
        modbus.timeout = 100; // valor 10 funciona bem entre 2400 a 115200 bps. Valor 5 funcionou bem com 57600 e 115200
        									// para boudrate menores pode ser que devemos aumetar esse valor.
        									// Recomendo fazer uma macro associado ao boudrate da serial
        return pdFAIL;
    }

    // somente vamos processar quando estourar o timeout de recep��o
    if (modbus.timeout) return pdFAIL;

   	// processando o pacote
   	modbus.lastLenBuffer = 0;
    #if (MODBUS_USE_DEBUG == pdON)
    plognp("MODBUS: proc len %d"CMD_TERMINATOR, len);
    #endif

	len = modbus_GetPacket(modbus.query);

    // se houve erro de CRC ou overflow do buffer, ou o endere�o na mensagem � para este dispositivo
    // o menor pacote � 4 bytes (1 byte ID, 1 byte fun��o e 2 bytes CRC), por�m no modbus.query n�o vai conter CRC
    if ( (len < 1) || (modbus.query[0] != modbus.slaveID) ) {
	    #if (MODBUS_USE_DEBUG == pdON)
    	plognp("MODBUS: pck error"CMD_TERMINATOR);
    	#endif
		modbus.flushRX();
    	return pdFAIL;
    };

	int ret;
	uint addrInit; // N�o podemos colocar a captura do addr aqui porque pode haver comandos simples

	modbus.cmd = modbus.query[1];

	// checa se a fun��o � de leitura dos registradores, processa caso h� fun��o anexada
    if ( (modbus.cmd == modbusCMD_READ_REGISTERS) && (modbus.read_regs != NULL)) {
		addrInit = ((uint) modbus.query[2] << 8) + (uint)modbus.query[3];
		len = ((int) modbus.query[4] << 8) + (int)modbus.query[5];

		// chamar a fun��o externa para capturar os dados
		// 	passo o endere�o do buffer a partir do 4� byte, os tres primeiros s�o reservador para ID, fun��o e count para tx mais tarde
		ret = modbus.read_regs(addrInit, &modbus.query[3], len);
		if (ret == modbusNO_ERROR)
			modbus_SendPacketRegs(modbus.query, len); // Envia os valores dos registradores
		else
			modbus_SendPacketException(ret);

    // checa se a fun��o � de escrita no registrador, processa caso h� fun��o anexada
    } else if ( (modbus.cmd == modbusCMD_WRITE_REGISTER) && (modbus.write_reg != NULL)) {
		// chamar a fun��o externa gravar o registrador
		addrInit = ((uint) modbus.query[2] << 8) + (uint)modbus.query[3];
		u16 data = ((u16) modbus.query[4] << 8) + (u16)modbus.query[5];
		ret = modbus.write_reg(addrInit, data);
		if (ret == modbusNO_ERROR)
			modbus.puts(modbus.query, 8); // retorna o eco do pedido sinalizando ao mestre que tudo ocorreu bem
		else
			modbus_SendPacketException(ret);

    // checa se a fun��o � de escrita nos registradores, processa caso h� fun��o anexada
    } else if ( (modbus.cmd == modbusCMD_WRITE_REGISTERS) && (modbus.write_regs != NULL)) {
    	addrInit = ((uint) modbus.query[2] << 8) + (uint)modbus.query[3];
    	len = ((int) modbus.query[4] << 8) + (int)modbus.query[5];
		// se a quantidade de registradores n�o corresponde com a quantidade de bytes no buffer retorna erro
		if (2*len != modbus.query[6]) ret = modbusILLEGAL_FUNCTION;
		else
			// chamar a fun��o externa gravar nos registradores
			// 	passo o endere�o do buffer a partir do 7� byte onde come�a os dados
			ret = modbus.write_regs(addrInit, &modbus.query[7], len);
		if (ret == modbusNO_ERROR) { // retorna o eco do pedido sinalizando ao mestre que tudo ocorreu bem
			uint crc = crc16_MODBUS(modbus.query, 6);
			modbus.query[6] = (u8)(crc&0xFF);
			modbus.query[7] = (u8)(crc>>8);
			modbus.puts(modbus.query, 8);
		} else
			modbus_SendPacketException(ret);
	} else {
    	// envia uma mensagem de exce��o ao mestre que este dispositivo n�o suporta tal fun��o
    	modbus_SendPacketException(modbusILLEGAL_FUNCTION);
    }

	modbus.flushRX();
	return pdPASS;
}

//-----------------------------------------------------------------------------------------------------------------------
// Esta fun��o deve ser chamada a cada 1ms
//-----------------------------------------------------------------------------------------------------------------------
void modbus_SlaveTick(void) {
	if (modbus.timeout) modbus.timeout--;
}
