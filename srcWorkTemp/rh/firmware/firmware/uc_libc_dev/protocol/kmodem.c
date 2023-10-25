#include "kmodem.h"

#if (KMODEM_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

typedef enum {
	GET_SIZE_FILE = 0,
	GET_SIZE_DATA,
	GET_DATA
} stsRX_t;

typedef enum {
	SEND_SIZE_FILE = 0,
	SEND_SIZE_DATA,
	SEND_DATA
} stsTX_t;

typedef struct {
	int (*funcOut)(n16 c);
	int (*funcIn) (u8 *ch);
	void (*funcFlushTX)(void);
	void (*funcFlushRX)(void);
	stsRX_t stsRX;
	stsTX_t stsTX;
	u16 size;
	u32 filesize;
	u32 qtTX;
	u32 timerout;
	u16 packageSize;
	u8 checksum;
} kmodem_t;

static kmodem_t kmodem;

static int inbyte (u8 *data, u32 timerout);

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa o protocolo KMODEM
// Parametros:	sendbyte_func:	Ponteiro da fun��o de transmiss�o de dados
//				flush_TX:		Ponteiro da fun��o para limpar a fila de transmiss�o de dados
//				getbyte_func:	Ponteiro da fun��o de recep��o de dados
//				flush_RX:		Ponteiro da fun��o para limpar a fila de recep��o de dados
//				timerout:		Tempo m�ximo para espera de dados da outra parte da comunica��o
//				packageSize: 	Tamanho m�ximo do pacote de dados a ser transferido
// 					O protocolo requer 11 bytes de controle
// 					A maioria das FTDIs tem um buffer de TX de 128 bytes, sobrando 117 bytes
// 					Se estamos usando uma UART do ARM LPC sem buffer auxiliar, teremos 16 bytes - 9 bytes para o pacote de dados
//					Se estamos usando uma UART com IRQ do ARM LPC, este usa um buffer auxiliar, teremos UARTx_RX_BUFFER_SIZE- 9 bytes para o pacote de dados
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void kmodem_Init(
	int (*sendbyte_func)(n16 c),
	void (*flush_TX)(void),
	int (*getbyte_func)(u8 *ch),
	void (*flush_RX)(void),
	u32 timerout,
	u16 packageSize)
{
	kmodem.funcOut 	= sendbyte_func;
	kmodem.funcFlushTX = flush_TX;
	kmodem.funcIn 	= getbyte_func;
	kmodem.funcFlushRX = flush_RX;

	kmodem.size = 0;
	kmodem.stsRX = GET_SIZE_FILE;
	kmodem.stsTX = SEND_SIZE_FILE;
	kmodem.timerout = timerout;
	kmodem.packageSize = packageSize;
	kmodem.qtTX = 0;

	#if (KMODEM_USE_DEBUG == pdON)
	plog("KMODEM: INIT" CMD_TERMINATOR);
	#endif
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Reajusta o tamanho do pacote de transmiss�o
// Parametros:	packageSize: 	Tamanho m�ximo do pacote de dados a ser transferido
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void kmodem_SetPackageSize( u16 packageSize) {
	kmodem.packageSize = packageSize;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Reajusta o timerout de resposta do
// Parametros:	timerout
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void kmodem_SetTimerOut( u32 timerout) {
	kmodem.timerout = timerout;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	limpa os buffer de transmiss�o e recep��o
// Parametros:	Nenhum
// Retorna:		nada
// -------------------------------------------------------------------------------------------------------------------
static void flush (void) {
	kmodem.funcFlushRX();
	kmodem.funcFlushTX();
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Checa por um determinado per�odo de tempo se chegou algum dado da outra parte da comunica��o de dados
// Parametros:	timerout: Tempo m�ximo para espera do dados
// Retorna:		pdPASS: Se recebeu o byte,
//				errTIMEROUT: Caso o dado n�o chegar em um determinado tempo
// -------------------------------------------------------------------------------------------------------------------
static int inbyte (u8 *data, u32 timerout) {
	u32 t = timerout+1;

	while (t--) {
		if (kmodem.funcIn(data) == pdPASS)
			return pdPASS;
		delay_us(1); // Usando em us para que n�o estoure o buffer de recep��o de dados durante o delay
					// Para uma transmiss�o de 1M leva +- 10uS para receber um byte, logo usar um delay_us(1) � satisfat�rio.
		// *******************************************************************************************
		//	POR�M O IDEAL � MEHORAR ESSA FUN��O PARA FICAR MONITORANDO RX DIRETO EM UMA FAIXA DE TEMPO
		//  Seja por consulta de um tick ou uma evento que acorde esse processo por chegada de byte ou
		//	timerout
		// *******************************************************************************************
	}

	return errTIMEROUT;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Recebe bytes do transmissor
// Parametros:  dest:		Ponteiro da buffer para recep��o de dados
//				sizedest:	Tamanho do buffer de recep��o de dados
//				sizerx: 	quantidade de dados recebidos
// Retorna:		C�digo da opera��o
// 				PASS: Recebimento do arquivo feito com sucesso
//              errMODEM_NO_RESPONSE: Sem resposta do receptor caso n�o chegar um dado em um determinado tempo
//              errMODEM_USER_CANCEL: Recebimento cancelado pelo usu�rio
//              errMODEM_CHECKSUM: Houve um erro de checksum durante a transferencia
//              errMODEM_CANCEL_REMOTE: Transferencia cancelada pelo transmissor
//              errMODEM_SYNC: Houve um erro de sincronismo entre transmissor e receptor durante a transferencia
//              errMODEM_FILE_EMPTY: Se o transmissor esteja tentando enviar um arquivo vazio
// -------------------------------------------------------------------------------------------------------------------
int kmodem_Receive(u8 *dest, u32 sizedest, u32 *sizerx) {
	u8 *ptrDest;
	u8 datarx;
	u32 i;

	#if (KMODEM_USE_DEBUG == pdON)
	plog("KMODEM: Init RX packageSize (%u)"CMD_TERMINATOR, kmodem.packageSize);
	#endif

	flush();
	ptrDest = dest;
	*sizerx = 0;
	kmodem.stsRX = GET_SIZE_FILE;
	kmodem.size = 0;

	for (;;) {

		// ESPERA PELO COMANDO
		if (inbyte(&datarx, kmodem.timerout) != pdPASS) return errMODEM_NO_RESPONSE;
		if ( datarx == KMODEM_CMD_CANCEL) return errMODEM_CANCEL_REMOTE;
		if ( datarx == KMODEM_CMD_FINISH) {
			#if (KMODEM_USE_DEBUG == pdON)
			plog("KMODEM: received %u bytes"CMD_TERMINATOR, *sizerx);
			#endif

			return pdPASS;
		}

		switch (kmodem.stsRX) {
		case GET_SIZE_FILE:
			// CHECA SE O COMANDO RECEBIDO � MEMSO O DE CAPTURA DO TAMANHO
			if (datarx != KMODEM_CMD_GET_SIZE_FILE ) return errMODEM_SYNC;

			#if (KMODEM_USE_DEBUG == pdON)
			plog("KMODEM: get CMD size file (0x%x)"CMD_TERMINATOR, datarx);
			#endif

			// CAPTURA O TAMANHO DE DADOS A SER RECEBIDO
			//	O primeiro byte a ser recebido � o byte menos significativo do tamanho
			kmodem.filesize = 0;
			for (i=0;i<4; i++) {
				if (inbyte(&datarx, kmodem.timerout) != pdPASS) return errMODEM_NO_RESPONSE;
				kmodem.filesize += (datarx<<(8*i));
			}

			kmodem.stsRX = GET_SIZE_DATA;

			#if (KMODEM_USE_DEBUG == pdON)
			plog("KMODEM: get file size %u"CMD_TERMINATOR, kmodem.filesize);
			#endif

			// CHECA SE H� AINDA ESPA�O NO BUFFER DE RECEP��O
			if (sizedest < kmodem.filesize) {
				#if (KMODEM_USE_DEBUG == pdON)
				plog("KMODEM: mem overflow %u %u bytes"CMD_TERMINATOR, sizedest, *sizerx);
				#endif

				return errMODEM_MEM_OVERFLOW;
			}

			if (kmodem.filesize <=0) return errMODEM_FILE_EMPTY;

			break;

		case GET_SIZE_DATA:
			// CHECA SE O COMANDO RECEBIDO � MEMSO O DE CAPTURA DO TAMANHO
			if (datarx != KMODEM_CMD_GET_SIZE_DATA ) return errMODEM_SYNC;

			#if (KMODEM_USE_DEBUG == pdON)
			plog("KMODEM: get CMD size data (0x%x)"CMD_TERMINATOR, datarx);
			#endif

			// CAPTURA O TAMANHO DE DADOS A SER RECEBIDO
			//	O primeiro byte a ser recebido � o byte menos significativo do tamanho
			kmodem.size = 0;
			for (i=0;i<2; i++) {
				if (inbyte(&datarx, kmodem.timerout) != pdPASS) return errMODEM_NO_RESPONSE;
				kmodem.size += (datarx<<(8*i));
			}

			*sizerx+=kmodem.size;
			kmodem.stsRX = GET_DATA;

			#if (KMODEM_USE_DEBUG == pdON)
			plog("KMODEM: get size data %u"CMD_TERMINATOR, kmodem.size);
			#endif

			break;

		case GET_DATA:
            // CHECA SE O COMANDO RECEBIDO � MEMSO O DE CAPTURA DE DADOS
			if (datarx != KMODEM_CMD_GET_DATA ) return errMODEM_SYNC;
			#if (KMODEM_USE_DEBUG == pdON)
			plog("KMODEM: get data - size %d"CMD_TERMINATOR, kmodem.size);
			#endif

			kmodem.checksum = 0;
			for(i=0;i<kmodem.size;i++) {
				if (inbyte(&datarx, kmodem.timerout) != pdPASS) return errMODEM_NO_RESPONSE;
				#if (KMODEM_USE_DEBUG == pdON)
				plog("   rx 0x%x"CMD_TERMINATOR, datarx);
				#endif

				*ptrDest++ = datarx;
				kmodem.checksum += datarx;
			}

            // ESPERA O CHECKSUM DO TRANSMISSOR E FAZ A COMPARA��O
			if (inbyte(&datarx, kmodem.timerout) != pdPASS) return errMODEM_NO_RESPONSE;
			if (kmodem.checksum != datarx) {
				#if (KMODEM_USE_DEBUG == pdON)
				plog("KMODEM: checksum error %u <> %u"CMD_TERMINATOR, kmodem.checksum, datarx);
				#endif

				return errMODEM_CHECKSUM;
			}

			#if (KMODEM_USE_DEBUG == pdON)
			plog("KMODEM: checksum ok %u = %u"CMD_TERMINATOR, kmodem.checksum, datarx);
			#endif

            // ENVIA O COMANDO ACK PARA O TRANSMISSOR
			kmodem.funcOut(KMODEM_CMD_ACK);
			#if (KMODEM_USE_DEBUG == pdON)
			plog("KMODEM: send ack %u"CMD_TERMINATOR, i);
			#endif

			kmodem.stsRX = GET_SIZE_DATA;

			break;
		}
	}
}


// -------------------------------------------------------------------------------------------------------------------
// Descri��o:   Envia bytes para o receptor
// Parametro:	Nome do arquivo a ser enviado. J� deve estar checado se existe
// Retorna:		C�digo da opera��o
//              PASS: Envio foi feito com sucesso
//              errMODEM_ACK: Houve um erro de reconhecimento do receptor no envio de um pacote de dados
//              errMODEM_NO_RESPONSE: Sem resposta do receptor caso n�o chegar um dado em um determinado tempo
//              errMODEM_FILE_EMPTY: O arquivo a ser enviado est� vazio
//				errMODEM_UNKNOWN: erro desconhecido
int kmodem_Transmit(u8 *buf, u32 sizebuf) {
	u32 i;
	u8 *ptrBuf;
	u8 data;

	flush();

	kmodem.qtTX = 0;
	kmodem.stsTX = SEND_SIZE_FILE;
	ptrBuf = buf;

	#if (KMODEM_USE_DEBUG == pdON)
	plog("KMODEM: Init TX packageSize (%u) filesize (%u)"CMD_TERMINATOR, kmodem.packageSize, sizebuf);
	#endif

    if (sizebuf == 0) return errMODEM_FILE_EMPTY;

    for(;;) {
    	switch(kmodem.stsTX) {
    	case SEND_SIZE_FILE:
    		// ENVIA O COMANDO DE CAPTURA DE TAMANHO DO ARQUIVO
    		kmodem.funcOut(KMODEM_CMD_GET_SIZE_FILE);

    		#if (KMODEM_USE_DEBUG == pdON)
    		plog("KMODEM: Send CMD GET SIZE FILE(0x%x) - size file (%u)"CMD_TERMINATOR, KMODEM_CMD_GET_SIZE_FILE, sizebuf);
			#endif
    		// ENVIAR AO OUTRO DISOPSITIVO O TAMANHO DO PACOTE A SER ENVIADO
    		for (i=0; i<4; i++)
    			kmodem.funcOut(((sizebuf>>(8*i)) & 0xFF)); // Envia o tamanho do arquivo

    		kmodem.stsTX = SEND_SIZE_DATA;
    		break;
    	case SEND_SIZE_DATA:
            // ENVIA O COMANDO PARA CAPTURA DA QUANTIDADE DE DADOS A SER ENVIADO
    		kmodem.funcOut(KMODEM_CMD_GET_SIZE_DATA);

            if ((sizebuf - kmodem.qtTX) >= kmodem.packageSize)
            		kmodem.size = kmodem.packageSize;
            else 	kmodem.size = sizebuf - kmodem.qtTX;

			#if (KMODEM_USE_DEBUG == pdON)
            plog("KMODEM: Send CMD GET SIZE DATA (0x%x) - size (%u)"CMD_TERMINATOR, KMODEM_CMD_GET_SIZE_DATA, kmodem.size);
			#endif

            // ENVIA A QUANTIDADE DE DADOS A SER TRANSMITIDO
            for(i=0; i<2; i++)
            	kmodem.funcOut(((kmodem.size>>(8*i)) & 0xFF)); // Envia a quantidade de dados

            kmodem.stsTX = SEND_DATA;

    		break;
    	case SEND_DATA:
            // ENVIA O COMANDO PARA CAPTURA DE DADOS DO PACOTE
    		kmodem.funcOut(KMODEM_CMD_GET_DATA);
			#if (KMODEM_USE_DEBUG == pdON)
    		plog("KMODEM: Send CMD GET DATA (0x%x) - size %d"CMD_TERMINATOR, KMODEM_CMD_GET_SIZE_DATA, kmodem.size);
			#endif

            // ENVIA UM PACOTE DE DADOS V�LIDOS
            kmodem.checksum = 0;
            for(i=0; i<kmodem.size; i++) {
        		data= *ptrBuf++;
            	kmodem.funcOut(data);
            	kmodem.checksum += data;

                //#if (KMODEM_USE_DEBUG == pdON)
                //plog("KMODEM:   tx 0x%x (%u)"CMD_TERMINATOR, data, i);
            	//plog("%u"CMD_TERMINATOR, i);
				//#endif
            }

            kmodem.qtTX+=kmodem.size;
			#if (KMODEM_USE_DEBUG == pdON)
            plog("KMODEM: Send %u bytes de %u bytes"CMD_TERMINATOR, kmodem.qtTX, sizebuf);
			#endif

            // ENVIA O CHECKSUM
            kmodem.funcOut(kmodem.checksum);
			#if (KMODEM_USE_DEBUG == pdON)
            plog("KMODEM: Send checksum %d"CMD_TERMINATOR, kmodem.checksum);
			#endif

            // ESPERA O ACK DO RECEPTOR
            if (inbyte(&data, kmodem.timerout) != pdPASS) return errMODEM_NO_RESPONSE;
            if (data != KMODEM_CMD_ACK) {
				#if (KMODEM_USE_DEBUG == pdON)
            	plog("KMODEM: ack error(%u)"CMD_TERMINATOR, data);
				#endif

                // ENVIA O COMANDO PARA FINALIZAR A TRANSFERENCIA
            	//kmodem.funcOut(KMODEM_CMD_CANCEL);
                return errMODEM_ACK;
            }

            #if (KMODEM_USE_DEBUG == pdON)
            plog("KMODEM: rx ack(%u)"CMD_TERMINATOR, data);
			#endif

            kmodem.stsTX = SEND_SIZE_DATA;
    		break;
       	}

        // VERIFICA SE � FIM DO ARQUIVO e finaliza a transferencia
        if (kmodem.qtTX >= sizebuf) {
			#if (KMODEM_USE_DEBUG == pdON)
        	plog("KMODEM: Send CMD Finish - TX completed");
			#endif
            // ENVIA O COMANDO PARA FINALIZAR A TRANSFERENCIA COM SUCESSO
            kmodem.funcOut(KMODEM_CMD_FINISH);
            return pdPASS;
        }
    }
}
