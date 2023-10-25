#include "modem.h"
#include "flash_prog.h"
#include <string.h>

#define BUF_SIZE 45 // para 43 chars do arquivo HEX. mais 1 char para '\n' e mais o caractere nulo '\0'

#if (MODEM_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

typedef struct {
	int (*funcOut)(n16 c);
	int (*funcIn) (u8 *ch);
	u8 buffIdx;
	u8 buffRX[BUF_SIZE];
	u32 timerout;
} modem_t;

static modem_t modem;
static int inbyte (u8 *data, u32 timerout);

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa o protocolo MODEM
// Parametros:	sendbyte_func:	Ponteiro da função de transmissão de dados
//				flush_TX:		Ponteiro da função para limpar a fila de transmissão de dados
//				getbyte_func:	Ponteiro da função de recepção de dados
//				flush_RX:		Ponteiro da função para limpar a fila de recepção de dados
//				timerout:		Tempo máximo para espera de dados da outra parte da comunicação
//				packageSize: 	Tamanho máximo do pacote de dados a ser transferido
// 					O protocolo requer 11 bytes de controle
// 					A maioria das FTDIs tem um buffer de TX de 128 bytes, sobrando 117 bytes
// 					Se estamos usando uma UART do ARM LPC sem buffer auxiliar, teremos 16 bytes - 9 bytes para o pacote de dados
//					Se estamos usando uma UART com IRQ do ARM LPC, este usa um buffer auxiliar, teremos UARTx_RX_BUFFER_SIZE- 9 bytes para o pacote de dados
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void modem_Init(
	int (*sendbyte_func)(n16 c),
	int (*getbyte_func)(u8 *ch),
	u32 timerout
) {
	modem.funcOut 	= sendbyte_func;
	modem.funcIn 	= getbyte_func;
	modem.timerout = timerout;

	#if (MODEM_USE_DEBUG == pdON)
	plog("M: INIT" CMD_TERMINATOR);
	#endif
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Reajusta o timerout de resposta do
// Parametros:	timerout
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void modem_SetTimerOut( u32 timerout) {
	modem.timerout = timerout;
}


// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Checa por um determinado período de tempo se chegou algum dado da outra parte da comunicação de dados
// Parametros:	timerout: Tempo máximo para espera do dados
// Retorna:		pdPASS: Se recebeu o byte,
//				errTIMEROUT: Caso o dado não chegar em um determinado tempo
// -------------------------------------------------------------------------------------------------------------------
static int inbyte (u8 *data, u32 timerout) {
	u32 t = timerout+1;

	while (t--) {
		if (modem.funcIn(data) == pdPASS)
			return pdPASS;
		delay_us(1); // Usando em us para que não estoure o buffer de recepção de dados durante o delay
					// Para uma transmissão de 1M leva +- 10uS para receber um byte, logo usar um delay_us(1) é satisfatório.
		// *******************************************************************************************
		//	PORÉM O IDEAL É MEHORAR ESSA FUNÇÃO PARA FICAR MONITORANDO RX DIRETO EM UMA FAIXA DE TEMPO
		//  Seja por consulta de um tick ou uma evento que acorde esse processo por chegada de byte ou
		//	timerout
		// *******************************************************************************************
	}

	return errTIMEROUT;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Recebe bytes do transmissor
// Parametros:  dest:		Ponteiro da buffer para recepção de dados
//				sizedest:	Tamanho do buffer de recepção de dados
//				sizerx: 	quantidade de dados recebidos
// Retorna:		Código da operação
// 				PASS: Recebimento do arquivo feito com sucesso
//              errMODEM_NO_RESPONSE: Sem resposta do receptor caso não chegar um dado em um determinado tempo
//              errMODEM_USER_CANCEL: Recebimento cancelado pelo usuário
//				errMODEM_MEM_OVERFLOW

//				errINTEL_HEX_DELIMITER: Não foi encontrado o delimitador de comandos.
//				errINTEL_HEX_CHECKSUM: Erro de checsum da linha de comando Intel HEX
// 				errINTEL_HEX_NUMBER_INVALID: O caractere presente na linha de comando não é um digito
// 				errINTEL_HEX_COMMAND_INVALID

//				errIAP_PARAM_INVALID: Caso o endereço final é menor que o endereço inicial
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço da FLASH
//				errIAP_SECTOR_NOT_PREPARED: O setor a ser gravado não foi preparado para gravação
// 				errIAP_SECTOR_NOT_BLANK: Esta região já pertence ao bootloader
//				errIAP_SIZE_INVALID: O tamanho do buffer de gravação é inválido. Deve ser 256, 512, 1024 ou 4096 bytes
//				errIAP_SECTOR_INVALID: Caso não foi encontrado o setor do respectivo endereço da FLASH
//				errIAP_BUSY: Caso a memória FLASH está ocupada
//				errIAP_SECTOR_NOT_PREPARED: O setor a ser gravado não foi preparado para gravação
// 				errIAP_SRC_ADDR:
// 				errIAP_DST_ADDR:
// 				errIAP_SRC_ADDR_NOT_MAPPED:
// 				errIAP_DST_ADDR_NOT_MAPPED:
// 				errIAP_COMPARE: Os dados gravados no FLASH não foi conferiru com o buffer de dados
//				errIAP_COUNT: Caso o contador de bytes não for multiplo de 4
// -------------------------------------------------------------------------------------------------------------------
int modem_Receive(void) {
	int result;
	u8 dat;

	if ( (result = flashProg_Init()) != pdPASS) {
		// EMITE UM COMANDO DE CANCELAMENTO PARA O X-BOOT
		modem_cmdCancel();

		#if (MODEM_USE_DEBUG == pdON)
		plog("M: init flash OK"CMD_TERMINATOR);
		#endif

		return result;
	}

	memset(modem.buffRX, 0, BUF_SIZE);
	modem.buffIdx = 0;

	for (;;) {
		// ESPERAR PELO A LINHA DE COMANDO
		if (inbyte(&dat, modem.timerout) != pdPASS) return errMODEM_NO_RESPONSE;

		// CHECA SE RECEBEU UM COMANDO DE FINALIZAÇÃO DE TRANSMISSÃO
		if (dat == 4) {
			#if (MODEM_USE_DEBUG == pdON)
			plog("M: end tx"CMD_TERMINATOR);
			#endif
			break;

		// CHECA SE RECEBEU UM COMANDO DE CANCELAMENTO DE TRANSMISSÃO
		} else if (dat == 24) {
			#if (MODEM_USE_DEBUG == pdON)
			plog("M: proc cancel"CMD_TERMINATOR);
			#endif
			return errMODEM_USER_CANCEL;
		}

		// CHECA SE RECEBEU UM FINALIZAZOR DE LINHA DE COMANDO
		else if (dat == 10) {
			#if (MODEM_USE_DEBUG == pdON)
			//plog("MODEM: proc line[%s]"CMD_TERMINATOR, modem.buffRX);
			plog("M: proc l"CMD_TERMINATOR);
			#endif

			if ((result = flashProg_Record(modem.buffRX)) != pdPASS) {
				#if (MODEM_USE_DEBUG == pdON)
				plog("M: proc line erro [%d]"CMD_TERMINATOR, result);
				#endif

				// EMITE UM COMANDO DE CANCELAMENTO PARA O X-BOOT
				modem_cmdCancel();
				return result;
			}

			// ENVIAR UM ACK PARA O X-BOOT
			modem_cmdAck();

			// LIMPA BUFFER DE RECEPÇÃO
			modem.buffIdx = 0;
			memset(modem.buffRX, 0, BUF_SIZE);

			#if (MODEM_USE_DEBUG == pdON)
			plog("M: ack"CMD_TERMINATOR);
			#endif
		}

		else {
			if (sizeof(modem.buffRX) > BUF_SIZE) {
				#if (MODEM_USE_DEBUG == pdON)
				plog("M: linha de comando maior que o buffer de trabalho [%u > %u]"CMD_TERMINATOR, sizeof(modem.buffRX), BUF_SIZE);
				#endif

				// EMITE UM COMANDO DE CANCELAMENTO PARA O X-BOOT
				modem_cmdCancel();
				return errMODEM_MEM_OVERFLOW;
			}

			modem.buffRX[modem.buffIdx++] = dat;
		}
	}

	return pdPASS;
}

// Envia um comando ACK para UART
void modem_cmdAck(void) {
	modem.funcOut(10);
}

// Envia um comando de cancelamento para UART
void modem_cmdCancel(void) {
	modem.funcOut(24);
}
