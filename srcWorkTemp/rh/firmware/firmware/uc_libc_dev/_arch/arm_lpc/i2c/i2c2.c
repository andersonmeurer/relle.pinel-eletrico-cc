#include "i2c2.h"
#if (I2C2_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

static int timeout;
static int codeError;
static tTime(*i2c_now)(void);

static int i2c2_PutChar(u8 data);
static int i2c2_GetChar(u8 mode, u8 *data);
static u8 i2c2_GetStatus(void);
static int i2c2_SendStart(void);
static n8 i2c2_SendStop(void);

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o barramento I2C
// Parametros: 	bitrate: O valor da velocidade do barramento em Khz. Exemplo: 400 = 400KHz
//				now_func: ponteiro da função now(). Essa função deve contar o tempo a cada 1ms
//				tm: Tempo em milisegundos na espera da resposta do dispositivo no barramento
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void i2c2_Init(u16 bitrate, tTime(*now_func)(void), int tm) {
	PCONP |= PCI2C2;										 // Ligar o SPI no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões
	i2c2_ConfigPort();

	#if (I2C2_USE_DEBUG == pdON)
	plog("I2C2: Init"CMD_TERMINATOR);
	#endif

	i2c2_SetBitrate(bitrate);								 // Ajusta a velocidade do barramento
	I2C2CONCLR = 0x6C;										 // Limpa flags

	I2C2CONSET = I2C2_I2EN; 								 // Habilita o barramento I2C2
	i2c_now = now_func;
	timeout = tm;
	codeError = 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta a velocidade do barramento
// Parametros: 	o valor da velocidade do barramento em Khz. Exemplo: 400 = 400KHz
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void i2c2_SetBitrate(u16 bitrate) {
	I2C2SCLL = PCLK / (bitrate*1000*2); // Clock nível baixo
	I2C2SCLH = PCLK / (bitrate*1000*2); // Clock nível alto		
		
	#if (I2C2_USE_DEBUG == pdON)
	plog("I2C2: SET BITRATE"CMD_TERMINATOR);
	plog("I2C2: f_cpu %ld"CMD_TERMINATOR, F_CPU);
	plog("I2C2: p_clk %ld"CMD_TERMINATOR, PCLK);
	plog("I2C2: I2C2SCLL %u"CMD_TERMINATOR, I2C2SCLL);
	plog("I2C2: I2C2SCLH %u"CMD_TERMINATOR, I2C2SCLH);
	#endif	
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o endereço do ARM, caso o mesmo for configurado como escravo.
// Parametros: 	Endereço escravo I2C2 e ...
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void i2c2_SetLocalDeviceAddr(u8 addr, u8 genCallEn) {
	I2C2ADR = ((addr&0xFE) | (genCallEn?1:0)); // Ajusta o endereço local do dispositivo (usado somente no modo escravo)
}

//###################################################################################################################
// TRANSAÇÃO DE ALTO NÍVEL
//###################################################################################################################
//###################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê o último código de erro no barramento
//------------------------------------------------------------------------------------------------------------------
int i2c2_CodeError(void) {
	return codeError;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Transmite um buffer de dados para um determinado dispositivo - Modo sem interrupção
//			      	Note: 	After this function is run, you may need a bus free time 
//							before a new data transfer can be initiated.
// Parametros: 	Endereço do dispositivo, endereço e o tamanho do buffer
// Retorna: 	pdPASS se a transmissão ocorreu bem
//				errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//				errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//------------------------------------------------------------------------------------------------------------------
int i2c2_WriteBuffer( u8 addr_device, u8 *data, u16 length ) {
    #if (I2C2_USE_DEBUG == pdON)
    plog("I2C2 MASTER SEND INIT"CMD_TERMINATOR);
    plog("I2C2 send start"CMD_TERMINATOR);
    #endif

    int ret = pdPASS;
    int status;

    // Gera a condição de START
    if (!i2c2_SendStart()) return errI2C_ERROR;

	// Escreve endereço do escravo + bit indicador de escrita
	// Faça até que consiga enviar o endereço
   	tTime tm = i2c_now();
    while (i2c2_PutChar( addr_device & 0xFE ) == errI2C_DEVICE_BUSY) {
    	if (i2c_now() >= timeout + tm) return errI2C_NO_RESPONSE;
    }

    // Leio a quantidade de bytes vindo do dispositivo
    int x; for(x=1; x<=length; x++ ) {
       	status = i2c2_GetStatus();								// Captura estado do barramento

        if (( status == 0x18 ) || ( status == 0x28 ) ) {		// Checa se o byte foi transmitido e recebido ACK
       		tm = i2c_now();
       		do {												// Faça até que consiga transmitir o byte
      			ret = i2c2_PutChar(*data);						// Escrever dados
      			if (i2c_now() >= timeout + tm) return errI2C_NO_RESPONSE;
        	} while (ret == errI2C_DEVICE_BUSY);

            data++;
		} else if ((status != 0xF8 ) || (status == errI2C_NO_RESPONSE))
           	return errI2C_ERROR;
    }

   	#if (I2C2_USE_DEBUG == pdON)
	plog("I2C2 send stop"CMD_TERMINATOR);
	#endif

	ret = 0;
	while (ret == 0) {
        status = i2c2_GetStatus();
        if( (status == 0x18) || (status == 0x28) )  {
        	ret = pdPASS;
        	codeError = 0;
        } else if ((status != 0xF8) || (status == errI2C_NO_RESPONSE)) {
            return errI2C_ERROR;
            codeError = status;
        }
    }

	return i2c2_SendStop();	// Generate Stop condition
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna em um buffer os dados transferidos do dispositivo alvo - Modo sem interrupção
//     			Note1: 	Before using this function, the first byte in the receive
//      				buffer must be loaded with slave address and direction bit (SLA+R).
//      				The received data from the addressed slave will be saved in the 
//      				buffer starting at the second byte.
//   			Note2: After this function is run, you may need a bus free time 
//      		       before a new data transfer can be initiated.
// Parametros: 	Endereço do dispositivo, endereço e o tamanho do buffer
// Retorna: 	pdPASS se a recepção ocorreu bem
//				errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//				errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//------------------------------------------------------------------------------------------------------------------
int i2c2_ReadBuffer(u8 addr_device, u8 *data, u16 length) {
   	#if (I2C2_USE_DEBUG == pdON)
	plog("I2C2 MASTER RECEIVE INIT"CMD_TERMINATOR);
	plog("I2C2 send start"CMD_TERMINATOR);
	#endif

    int ret = pdPASS;

    // Gera a condição de START
    if (!i2c2_SendStart()) return errI2C_ERROR;

	// Escreve endereço do escravo + bit indicador de leitura
	// Faça até que consiga enviar o endereço
    tTime tm = i2c_now();
    while( i2c2_PutChar(addr_device | 0x01) == errI2C_DEVICE_BUSY ) {
    	if (i2c_now() >= timeout + tm) return errI2C_NO_RESPONSE;
    }

    // Leio a quantidade de bytes vindo do dispositivo
    int x; for(x=1; x<=length; x++ ) {
       	int status = i2c2_GetStatus();												// Captura estado do barramento

        if (( status == 0x40 ) || ( status == 0x48 ) || ( status == 0x50 )) {	// Data received
			if (x == length ) // Checa se é o último byte a ser lido no escravo
            		ret = i2c2_GetChar(I2C2_ACK1, data ); 					// Sinaliza para o escravo para não enviar mais byte
            else    ret = i2c2_GetChar(I2C2_ACK0, data );					// Sinaliza para o escravo para enviar o próximo byte

			// fico até ler o dado no barramento
			do {
            	ret = i2c2_GetChar(I2C2_READ, data );
            } while (ret == errI2C_DEVICE_NO_DATA);

            data++;

		} else if ((status != 0xD8 ) || (status == errI2C_NO_RESPONSE))
           	return errI2C_ERROR;
    }

   	#if (I2C2_USE_DEBUG == pdON)
	plog("I2C2 send stop"CMD_TERMINATOR);
	#endif

    return i2c2_SendStop();	// Generate Stop condition
}


//###################################################################################################################
//###################################################################################################################
// FUNÇÕES DE AUXILIO
//###################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Captura o estatus corrente do barramento
// Parametros: 	Nenhum
// Retorna: 	Código do estado do barramento:
//					00h Bus error
//					08h START condition transmitted
//					10h Repeated START condition transmitted
//					18h SLA + W transmitted, ACK received
//					20h SLA + W transmitted, ACK not received
//					28h Data byte transmitted, ACK received
//					30h Data byte transmitted, ACK not received
//					38h Arbitration lost
//					40h SLA + R transmitted, ACK received
//					48h SLA + R transmitted, ACK not received
//					50h Data byte received in master mode, ACK transmitted
//					58h Data byte received in master mode, ACK not transmitted
//					60h SLA + W received, ACK transmitted
//					68h Arbitration lost, SLA + W received, ACK transmitted
//					70h General call address received, ACK transmitted
//					78h Arbitration lost, general call addr received, ACK transmitted
//					80h Data byte received with own SLA, ACK transmitted
//					88h Data byte received with own SLA, ACK not transmitted
//					90h Data byte received after general call, ACK transmitted
//					98h Data byte received after general call, ACK not transmitted
//					A0h STOP or repeated START condition received in slave mode
//					A8h SLA + R received, ACK transmitted
//					B0h Arbitration lost, SLA + R received, ACK transmitted
//					B8h Data byte transmitted in slave mode, ACK received
//					C0h Data byte transmitted in slave mode, ACK not received
//					C8h Last byte transmitted in slave mode, ACK received
//					F8h No relevant status information, SI=0
//					FFh Channel error
//			errI2C_NO_RESPONSE: Ou erro de não resposta do dispositivo
//------------------------------------------------------------------------------------------------------------------
static u8 i2c2_GetStatus (void) {	
	tTime tm = i2c_now();

	// Wait for I2C Status changed, espera por um determinado tempo
	while ((I2C2CONSET&I2C2_SI) == 0) {
		if (i2c_now() >= timeout + tm) return errI2C_NO_RESPONSE;
	}

	return I2C2STAT;
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Emite uma condição de I2C start - modo mestre
// Parametros: 	Nenhum
// Retorna: 	pdPASS se o comando de START foi emitido com sucesso ao dispositivo
//				errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//------------------------------------------------------------------------------------------------------------------
static int i2c2_SendStart(void) {
	int ret = 0;
	int status;

	I2C2CONSET = I2C2_STA; 								// Emitir condição de start
    while (ret == 0) {									// Esperar até que o START seja transmitido
        status = i2c2_GetStatus();						// Captura o novo estado

        if (( status == 0x08 ) || ( status == 0x10 )) { // Checa a condição de START (0x08), ou restart (0x10), se já foi concluida
        	ret = pdPASS;								// Condição de START já foi transmitido
        	codeError = 0;
        } else if ( (status != 0xF8) || (status == errI2C_NO_RESPONSE)) {				// Houve algum erro
            ret = errI2C_ERROR;
            codeError = status;
        } else {
            I2C2CONCLR = I2C2_SIC;						// Limpa flag SI
        }
    }

	I2C2CONCLR = I2C2_STAC; 							// Limpo o flag START
    return ret;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Emite uma condição de I2C stop - modo mestre
//     		 	Generates a stop condition in master mode or recovers from an
//     		 	error condition in slave mode.
//     		 	Note: After this function is run, you may need a bus free time 
//     		 	before you can generate a new start condition.
// Parametros: 	Nenhum
// Retorna: 	pdPASS se o comando de STOP foi emitido com sucesso ao dispositivo
//				errI2C_NO_RESPONSE: Ou erro de não resposta do dispositivo
//------------------------------------------------------------------------------------------------------------------
static n8 i2c2_SendStop(void) {
	tTime tm = i2c_now();

	I2C2CONSET = I2C2_STO; 					// Emitir condição de stop
    I2C2CONCLR = I2C2_SIC; 					// Limpar flag de interrrupção
            
    while( (I2C2CONSET&I2C2_STO) == 1) { 	// Espero pelo a detecção do STOP
		if (i2c_now() >= timeout + tm) return errI2C_NO_RESPONSE;
	}
   
    return pdPASS;	
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Manda um byte no barramento I2C
// Parametros: 	Byte a ser transmitido
// Retorna: 	pdPASS se o byte foi trasnitido ou código do erro
//				errI2C_DEVICE_BUSY: Caso o barramento estiver ocupado
//------------------------------------------------------------------------------------------------------------------
static int i2c2_PutChar(u8 data) {
    if( I2C2CONSET&I2C2_SI) {			// Checa se o o registrador de dados pode ser acessado
        #if (I2C2_USE_DEBUG == pdON)
        plog("I2C2 sendding data %X"CMD_TERMINATOR, data);
        #endif

		I2C2DAT = data;					// Atualiza o registrador com o byte a ser transmitido
		I2C2CONCLR = I2C2_SIC; 			// Limpar flag de interrrupção
		return pdPASS;
    } else return errI2C_DEVICE_BUSY;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 		Recebe um caractere no modo mestre
//			     	This function is also used to prepare if the master shall generate acknowledge or not acknowledge.
// Parametros: 	  	Mode      	= I2C2_ACK0   Set ACK=0. Slave sends next byte.
//      		            	= I2C2_ACK1   Set ACK=1. Slave sends last byte.
//                  			= I2C2_READ   Read received data from data register.
//      			pData       A pointer to where the received data shall be saved.
// Retorna: 	    pdPASS: recebimento ok
//					errI2C_DEVICE_NO_DATA: Não há dados recebidos
//------------------------------------------------------------------------------------------------------------------
static int i2c2_GetChar(u8 mode, u8 *data ) {
    int ret = pdPASS;

    if (mode == I2C2_ACK0 ) {
        // The operation mode is changed from master transmit to master receive
        // Set ACK=0 (informs slave to send next byte)
        I2C2CONSET = I2C2_AA; 		// ACK the recevied data
        I2C2CONCLR = I2C2_SIC; 		// Limpar flag de interrrupção
    } else if (mode == I2C2_ACK1) {
        // Set ACK=1 (informs slave to send last byte)
        I2C2CONCLR = I2C2_AAC;
        I2C2CONCLR = I2C2_SIC; 		// Limpar flag de interrrupção
    } else if (mode == I2C2_READ) {
    	if (I2C2CONSET&I2C2_SI) {			// Checa se o o registrador de dados pode ser acessado
            *data = (u8) I2C2DAT;			// Leio o dado
            #if (I2C2_USE_DEBUG == pdON)
			plog("I2C2 data read %x"CMD_TERMINATOR, *data);
			#endif
        } else {
        	#if (I2C2_USE_DEBUG == pdON)
			plog("I2C2 no data"CMD_TERMINATOR);
			#endif

            ret = errI2C_DEVICE_NO_DATA;		// O dado não esta disponivel
        }
    }

    return ret;
}
