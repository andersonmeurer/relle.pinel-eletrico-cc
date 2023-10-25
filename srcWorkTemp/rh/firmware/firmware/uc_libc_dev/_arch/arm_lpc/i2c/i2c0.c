// TODO: ainda não testato, caso der alguma coisa errada comparar com i2c2

#include "i2c0.h"
#if (I2C0_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

static int timeout;
static int codeError;
static tTime(*i2c_now)(void);

static int i2c0_PutChar(u8 data);
static int i2c0_GetChar(u8 mode, u8 *data);
static u8 i2c0_GetStatus(void);
static int i2c0_SendStart(void);
static n8 i2c0_SendStop(void);

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o barramento I2C
// Parametros: 	bitrate: O valor da velocidade do barramento em Khz. Exemplo: 400 = 400KHz
//				now_func: ponteiro da função now(). Essa função deve contar o tempo a cada 1ms
//				tm: Tempo em milisegundos na espera da resposta do dispositivo no barramento
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void i2c0_Init(u16 bitrate, tTime(*now_func)(void), int tm) {
	PCONP |= PCI2C0;										 // Ligar o SPI no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões
	
	#if defined(arm7tdmi)
	I2C0_PINSEL = (I2C0_PINSEL & ~I2C0_PINMASK) | I2C0_PINS; // Seleciona o pino externo para a função I2C
	#endif

	#if defined(cortexm3)
	i2c0_ConfigPort();
	#endif

	#if (I2C0_USE_DEBUG == pdON)
	plog("I2C0: Init"CMD_TERMINATOR);
	#endif

	i2c0_SetBitrate(bitrate);								 // Ajusta a velocidade do barramento
	I2C0CONCLR = 0x6C;										 // Limpa flags

	I2C0CONSET = I2C0_I2EN; 								 // Habilita o barramento I2C0
	i2c_now = now_func;
	timeout = tm;
	codeError = 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta a velocidade do barramento
// Parametros: 	o valor da velocidade do barramento em Khz. Exemplo: 400 = 400KHz
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void i2c0_SetBitrate(u16 bitrate) {
	I2C0SCLL = PCLK / (bitrate*1000*2); // Clock nível baixo
	I2C0SCLH = PCLK / (bitrate*1000*2); // Clock nível alto		
		
	#if (I2C0_USE_DEBUG == pdON)
	plog("I2C0: SET BITRATE"CMD_TERMINATOR);
	plog("I2C0: f_cpu %lu"CMD_TERMINATOR, F_CPU);
	plog("I2C0: p_clk %lu"CMD_TERMINATOR, PCLK);
	plog("I2C0: I2C0SCLL %u"CMD_TERMINATOR, I2C0SCLL);
	plog("I2C0: I2C0SCLH %u"CMD_TERMINATOR, I2C0SCLH);
	#endif	
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o endereço do ARM, caso o mesmo for configurado como escravo.
// Parametros: 	Endereço escravo I2C0 e ...
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void i2c0_SetLocalDeviceAddr(u8 addr, u8 genCallEn) {
	I2C0ADR = ((addr&0xFE) | (genCallEn?1:0)); // Ajusta o endereço local do dispositivo (usado somente no modo escravo)
}

//###################################################################################################################
// TRANSAÇÃO DE ALTO NÍVEL
//###################################################################################################################
//###################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Lê o último código de erro no barramento
//------------------------------------------------------------------------------------------------------------------
int i2c0_CodeError(void) {
	return codeError;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Transmite um buffer de dados para um determinado dispositivo - Modo sem interrupção
//			      	Note: 	After this function is run, you may need a bus free time 
//							before a new data transfer can be initiated.
// Parametros: 	Endereço do dispositivo, endereço e o tamanho do buffer
// Retorna: 	pdPASS se a transmissão ocorreu bem
//				errI2C_ERROR onde o código de erro pode ser lido com a função i2c0_CodeError()
//				errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//------------------------------------------------------------------------------------------------------------------
int i2c0_WriteBuffer( u8 addr_device, u8 *data, u16 length ) {
    #if (I2C0_USE_DEBUG == pdON)
    plog("I2C0 MASTER SEND INIT"CMD_TERMINATOR);
    plog("I2C0 send start"CMD_TERMINATOR);
    #endif

	int ret = pdPASS;
    int status;

	    // Gera a condição de START
    if (!i2c0_SendStart()) return errI2C_ERROR;

	// Escreve endereço do escravo + bit indicador de escrita
	// Faça até que consiga enviar o endereço
   	tTime tm = i2c_now();
    while (i2c0_PutChar( addr_device & 0xFE ) == errI2C_DEVICE_BUSY) {
    	if (i2c_now() >= timeout + tm) return errI2C_NO_RESPONSE;
    }

    // Leio a quantidade de bytes vindo do dispositivo
    int x; for(x=1; x<=length; x++ ) {
       	status = i2c0_GetStatus();								// Captura estado do barramento

        if (( status == 0x18 ) || ( status == 0x28 ) ) {		// Checa se o byte foi transmitido e recebido ACK
       		tm = i2c_now();
       		do {												// Faça até que consiga transmitir o byte
      			ret = i2c0_PutChar(*data);						// Escrever dados
      			if (i2c_now() >= timeout + tm) return errI2C_NO_RESPONSE;
        	} while (ret == errI2C_DEVICE_BUSY);

            data++;
		} else if ((status != 0xF8 ) || (status == errI2C_NO_RESPONSE))
           	return errI2C_ERROR;
    }

   	#if (I2C0_USE_DEBUG == pdON)
	plog("I2C0 send stop"CMD_TERMINATOR);
	#endif

	ret = 0;
	while (ret == 0) {
        status = i2c0_GetStatus();
        if( (status == 0x18) || (status == 0x28) )  {
        	ret = pdPASS;
        	codeError = 0;
        } else if ((status != 0xF8) || (status == errI2C_NO_RESPONSE)) {
            return errI2C_ERROR;
            codeError = status;
        }
    }

	return i2c0_SendStop();	// Generate Stop condition
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
//				errI2C_ERROR onde o código de erro pode ser lido com a função i2c0_CodeError()
//				errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//------------------------------------------------------------------------------------------------------------------
int i2c0_ReadBuffer(u8 addr_device, u8 *data, u16 length) {
   	#if (I2C0_USE_DEBUG == pdON)
	plog("I2C0 MASTER RECEIVE INIT"CMD_TERMINATOR);
	plog("I2C0 send start"CMD_TERMINATOR);
	#endif

    int ret = pdPASS;

    // Gera a condição de START
    if (!i2c0_SendStart()) return errI2C_ERROR;

	// Escreve endereço do escravo + bit indicador de leitura
	// Faça até que consiga enviar o endereço
    tTime tm = i2c_now();
    while( i2c0_PutChar(addr_device | 0x01) == errI2C_DEVICE_BUSY ) {
    	if (i2c_now() >= timeout + tm) return errI2C_NO_RESPONSE;
    }

    // Leio a quantidade de bytes vindo do dispositivo
    int x; for(x=1; x<=length; x++ ) {
       	int status = i2c0_GetStatus();												// Captura estado do barramento

        if (( status == 0x40 ) || ( status == 0x48 ) || ( status == 0x50 )) {	// Data received
			if (x == length ) // Checa se é o último byte a ser lido no escravo
            		ret = i2c0_GetChar(I2C0_ACK1, data ); 					// Sinaliza para o escravo para não enviar mais byte
            else    ret = i2c0_GetChar(I2C0_ACK0, data );					// Sinaliza para o escravo para enviar o próximo byte

			// fico até ler o dado no barramento
			do {
            	ret = i2c0_GetChar(I2C0_READ, data );
            } while (ret == errI2C_DEVICE_NO_DATA);

            data++;

		} else if ((status != 0xD8 ) || (status == errI2C_NO_RESPONSE))
           	return errI2C_ERROR;
    }

   	#if (I2C0_USE_DEBUG == pdON)
	plog("I2C0 send stop"CMD_TERMINATOR);
	#endif

    return i2c0_SendStop();	// Generate Stop condition
}




//###################################################################################################################
//###################################################################################################################
// FUNÇÕES DE AUXILIO
//###################################################################################################################


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Captura o estatus corrente do barramento
// Parametros: 	Nenhum
// Retorna: 	Código do estado do barramento
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
static u8 i2c0_GetStatus (void) {	
	tTime tm = i2c_now();

	// Wait for I2C Status changed, espera por um determinado tempo
	while ((I2C0CONSET&I2C0_SI) == 0) {
		if (i2c_now() >= timeout + tm) return errI2C_NO_RESPONSE;
	}

	return I2C0STAT;
}


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Emite uma condição de I2C start - modo mestre
// Parametros: 	Nenhum
// Retorna: 	pdPASS se o comando de START foi emitido com sucesso ao dispositivo
//				errI2C_ERROR onde o código de erro pode ser lido com a função i2c0_CodeError()
//------------------------------------------------------------------------------------------------------------------
static int i2c0_SendStart(void) {
	int ret = 0;
	int status;

	I2C0CONSET = I2C0_STA; 								// Emitir condição de start
    while (ret == 0) {									// Esperar até que o START seja transmitido
        status = i2c0_GetStatus();						// Captura o novo estado

        if (( status == 0x08 ) || ( status == 0x10 )) { // Checa a condição de START (0x08), ou restart (0x10), se já foi concluida
        	ret = pdPASS;								// Condição de START já foi transmitido
        	codeError = 0;
        } else if ( (status != 0xF8) || (status == errI2C_NO_RESPONSE)) {				// Houve algum erro
            ret = errI2C_ERROR;
            codeError = status;
        } else {
            I2C0CONCLR = I2C0_SIC;						// Limpa flag SI
        }
    }

	I2C0CONCLR = I2C0_STAC; 							// Limpo o flag START
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
static n8 i2c0_SendStop(void) {
	tTime tm = i2c_now();

	I2C0CONSET = I2C0_STO; 					// Emitir condição de stop
    I2C0CONCLR = I2C0_SIC; 					// Limpar flag de interrrupção
            
    while( (I2C0CONSET&I2C0_STO) == 1) { 	// Espero pelo a detecção do STOP
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
static int i2c0_PutChar (u8 data) {
    if( I2C0CONSET&I2C0_SI) {			// Checa se o o registrador de dados pode ser acessado
        #if (I2C0_USE_DEBUG == pdON)
        plog("I2C0 sendding data %X"CMD_TERMINATOR, data);
        #endif

		I2C0DAT = data;					// Atualiza o registrador com o byte a ser transmitido
		I2C0CONCLR = I2C0_SIC; 			// Limpar flag de interrrupção
		return pdPASS;
    } else return errI2C_DEVICE_BUSY;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 		Recebe um caractere no modo mestre
//			     	This function is also used to prepare if the master shall generate acknowledge or not acknowledge.
// Parametros: 	  	Mode      	= I2C0_ACK0   Set ACK=0. Slave sends next byte.
//      		            	= I2C0_ACK1   Set ACK=1. Slave sends last byte.
//                  			= I2C0_READ   Read received data from data register.
//      			pData       A pointer to where the received data shall be saved.
// Retorna: 	    pdPASS: recebimento ok
//					errI2C_DEVICE_NO_DATA: Não há dados recebidos
//------------------------------------------------------------------------------------------------------------------
static int i2c0_GetChar ( u8 mode, u8 *data ) {
    int ret = pdPASS;

    if (mode == I2C0_ACK0 ) {
        // The operation mode is changed from master transmit to master receive
        // Set ACK=0 (informs slave to send next byte)
        I2C0CONSET = I2C0_AA; 		// ACK the recevied data
        I2C0CONCLR = I2C0_SIC; 		// Limpar flag de interrrupção
    } else if (mode == I2C0_ACK1) {
        // Set ACK=1 (informs slave to send last byte)
        I2C0CONCLR = I2C0_AAC;
        I2C0CONCLR = I2C0_SIC; 		// Limpar flag de interrrupção
    } else if (mode == I2C0_READ) {
    	if (I2C0CONSET&I2C0_SI) {			// Checa se o o registrador de dados pode ser acessado
            *data = (u8) I2C0DAT;			// Leio o dado
            #if (I2C0_USE_DEBUG == pdON)
			plog("I2C0 data read %x"CMD_TERMINATOR, *data);
			#endif
        } else {
        	#if (I2C0_USE_DEBUG == pdON)
			plog("I2C0 no data"CMD_TERMINATOR);
			#endif

            ret = errI2C_DEVICE_NO_DATA;		// O dado não esta disponivel
        }
    }

    return ret;
}


/*


//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Repete uma emição de uma condição de I2C start - modo mestre
// Parametros: 	Nenhum
// Retorna: 	pdPASS se START emetido com sucesso ou retorna o código do erro
//------------------------------------------------------------------------------------------------------------------
int i2c0_RepeatStart( void ) {
    u8 status, flag = pdTRUE;
    n8 return_code;

    I2C0CONSET = I2C0_STA; 								// Emitir condição de start
    I2C0CONCLR = I2C0_SIC;									// Limpa flag SI

   	while( flag ) {										// Esperar até que o START seja transmitido {
        status = i2c0_GetStatus();						// Captura o novo estado

        if(( status == 0x08 ) || ( status == 0x10 )) { 	// Checa a condição de START (0x08), ou restart (0x10), se já foi concluida
            flag = pdFALSE;								// Condição de START já foi transmitido
            return_code = pdPASS;
        } else if( status != 0xF8 ) {					// Houve algum erro
            flag = pdFALSE;
            return_code = (n8) status;
        } else {
            I2C0CONCLR = I2C0_SIC;							// Limpa flag SI
        }
    }
    
 	I2C0CONCLR = I2C0_STAC; 								// Limpo o flag START
 	
    return return_code;
}

int i2c0_MasterReadRegister( u8 addr_device, u8 addr, u8 *data, u16 length ) {
  
    int return_code;
    u8 status, flag;
    u16 ii;

   	#if (I2C0_USE_DEBUG == pdON)
	plog("I2C0 MASTER READ REG INIT\r\n");
	plog("I2C0 send start\r\n");
	#endif

    // START
    return_code = i2c0_SendStart();										// Gera a condição de START
        
    // TRANSMITO ENDEREÇO DO DISPOSITIVO NO BUS PARA ESCRIRA
    //plog("I2C0 ADDR\r\n");
    if ( return_code == pdPASS )  {											// Transmite endereço
        do {															// Faça até que consiga TX o endereço								
        	return_code = i2c0_PutChar( addr_device & 0xFE );		// Write SLA+W
        } while( return_code == errI2C_DEVICE_BUSY );
    }
	
	// ESPERO PELO ACK DO ESCRAVO E TRANSMITO O ENDEREÇO DO REGISTRADOR
	//plog("I2C0 DAT\r\n");
    if( return_code == pdPASS ) {
        flag = pdTRUE;
        while( flag ) {												
            status = i2c0_GetStatus();									// Captura estado do barramento
				
            if( (status == 0x18) || (status == 0x28) ) {				// Checa se o byte foi transmitido e recebido ACK
            	flag = pdFALSE;
                do {													// Faça até que consiga TX o byte     				  	
      			  	return_code = i2c0_PutChar( addr );			// Escrever dados
        		} while( return_code == errI2C_DEVICE_BUSY );
      
                data++;
            } else if( status != 0xF8 ) {								// Houve algum erro
            	//plog("I2C0 ERRO1\r\n");
                flag = pdFALSE;
                return_code = errI2C_ERROR;
            } 
		}
    }
	
	// ESPERO PELO ACK DO ESCRAVO
	#if (I2C0_USE_DEBUG == pdON)
	plog("I2C0 wait ack\r\n");
	#endif

    flag = pdTRUE;
    while( flag ) {														// Espere que o byte seja transmitido
        status = i2c0_GetStatus();										// Captura estado do barramento
        if( (status == 0x18) || (status == 0x28) )  {					// Checa se o byte foi transmitido e recebido ACK
            flag = pdFALSE;
        } else if( status != 0xF8 ) {
            #if (I2C0_USE_DEBUG == pdON)
			plog("I2C0 ack error\r\n");
			#endif
            flag = pdFALSE;
            return_code = errI2C_ERROR;
        }
    }

	#if (I2C0_USE_DEBUG == pdON)
	plog("I2C0 send restart\r\n");
	#endif

	// RESTART
	return_code = i2c0_RepeatStart();									// Gera uma condição de restart
   
    // TRANSMITO ENDEREÇO DO DISPOSITIVO NO BUS PARA LEITURA
    if ( return_code == pdPASS )  {											// Transmite endereço
        do {															// Faça até que consiga TX o endereço								
        	return_code = i2c0_PutChar( addr_device | 0x01 );		// Write SLA+R
        } while( return_code == errI2C_DEVICE_BUSY );        
    }
     
    // ESPERO PELO ACK DO ESCRAVO E INICIO A LEITURA DOS DADOS MANDANDOS PELO ESCRAVO
    if( return_code == pdPASS ) {
        for( ii = 1; ii<=length; ii++ ) {												// wait until address transmitted and receive data
           	flag = pdTRUE;
            while( flag ) {																// wait until data transmitted
                status = i2c0_GetStatus();												// Get new status

                if(( status == 0x40 ) || ( status == 0x48 ) || ( status == 0x50 )) {	// Data received
                    flag = pdFALSE;

                    if( ii == length ) {
                        return_code = i2c0_GetChar( I2C0_ACK1, data ); 				// Set generate NACK
                    } else {
                        return_code = i2c0_GetChar( I2C0_ACK0, data );
                    }
                   
                    do {
                    	return_code = i2c0_GetChar( I2C0_READ, data );
                    } while( return_code == errI2C_DEVICE_NO_DATA );
                    
               		#if (I2C0_USE_DEBUG == pdON)
					plog("I2C0 read data %x\r\n", *data);
					#endif
                    data++;
                    
                } else if( status != 0xD8 ) {											// errI2C_ERROR
                    flag = pdFALSE;
                    ii = length;
                    return_code = errI2C_ERROR;
                   	#if (I2C0_USE_DEBUG == pdON)
					plog("I2C0 data error\r\n");
					#endif
                }
            }
        }
    }

	#if (I2C0_USE_DEBUG == pdON)
	plog("I2C0 send stop\r\n");
	#endif

    // STOP
    i2c0_SendStop();																		// Generate Stop condition 
    
    #if (I2C0_USE_DEBUG == pdON)
	plog("I2C0 MASTER READ REG END\r\n\r\n");
	#endif

    return return_code;
}
*/
/*


void i2cSendStart(void) {
	I2C0CONSET = I2C0_STA; 								// Emitir condição de start
	I2C0CONCLR = I2C0_SIC;							// Limpa flag SI
}

void i2cSendStop(void) {
	I2C0CONSET = I2C0_STO; 				// Emitir condição de stop
    I2C0CONCLR = I2C0_SIC; 				// Limpar flag de interrrupção
}

void i2cWaitForComplete(void) {
	// wait for a valid status code
	while(I2STAT == 0xF8) {
		plog("I2C0 I2STAT %X\r\n",I2STAT);
	delay_ms(100);
	}
}


void i2cSendByte(u8 data) {
	// save data into data register
	I2DAT = data;
	// clear SI bit to begin transfer
	I2C0CONCLR = I2C0_SIC; 				// Limpar flag de interrrupção
}

void i2cReceiveByte(u8 ackFlag) {
	// begin receive over i2c
	if( ackFlag ) {
		// ackFlag = pdTRUE: ACK the recevied data
		I2C0CONSET = I2C0_AA; 		//ACK the recevied data
		//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	}	else	{
		// ackFlag = pdFALSE: NACK the recevied data
		I2C0CONCLR = I2C0_AAC;
		//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
	}
	// clear SI bit to begin transfer
	I2C0CONCLR = I2C0_SIC; 				// Limpar flag de interrrupção
}

u8 i2cGetReceivedByte(void) {
	return I2DAT;
}

u8 i2cMasterSend(u8 deviceAddr, u8 length, u8* data) {
	u8 retval = pdPASS;

	// send start condition
	plog("I2C0 STA\r\n");
	i2cSendStart();
	
	i2cWaitForComplete();
	I2C0CONCLR = I2C0_STAC; 								// Limpo o flag START
	
	// send device address with write
	i2cSendByte( deviceAddr & 0xFE );
	plog("I2C0 ADDR\r\n");
	i2cWaitForComplete();
	plog("I2C0 DATA\r\n");

	// check if device is present and live
	if( I2STAT == I2C0_MT_SLA_ACK) {
		// send data
		while(length)	{
			plog("I2C0 data %c\r\n", *data);
			i2cSendByte( *data++ );
			i2cWaitForComplete();
			length--;
		}
	}	else	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = errI2C_ERROR;
	}

	plog("I2C0 STO\r\n");
	i2cSendStop();

	return retval;
}

u8 i2cMasterReceive(u8 deviceAddr, u8 length, u8 *data) {
	u8 retval = pdPASS;

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();
	I2C0CONCLR = I2C0_STAC; 								// Limpo o flag START

	// send device address with read
	i2cSendByte( deviceAddr | 0x01 );
	i2cWaitForComplete();

	// check if device is present and live
	if( I2STAT == I2C0_MR_SLA_ACK) 	{
		// accept receive data and ack it
		while(length > 1) {
			i2cReceiveByte(pdTRUE);
			i2cWaitForComplete();
			*data++ = i2cGetReceivedByte();
			// decrement length
			length--;
		}

		// accept receive data and nack it (last-byte signal)
		i2cReceiveByte(pdFALSE);
		i2cWaitForComplete();
		*data++ = i2cGetReceivedByte();
	}	else	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = errI2C_ERROR;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();

	return retval;
}








*/
