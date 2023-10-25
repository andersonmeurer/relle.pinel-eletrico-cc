
	/* 
		TESTE TRANSFERENCIA DE DADOS EM PACOTES DE 128 BYTES (SOH). 
		CONTUDO COM ALGUNS AJUSTES PODE SER TRANSMISSÕES COM PACOTES DE 1024 BYTES (STX)		
		
		UMA DAS DESVANTAGENS DO XMODEM É QUE ELE NÃO NEGOCIA O TAMANHO DO ARQUIVO A SER TRTANSMITIDOS.
		PARA QUEM RECEBE, SEMPRE ARMAZENARÁ PACOTES DE 128 OU 1024 BYTES
	*/


/*	
 * Copyright 2001-2010 Georges Menie (www.menie.org)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* this code needs standard functions memcpy() and memset()
   and input/output functions xmodem_In() and xmodem_Out().

   the prototypes of the input/output functions are:
     int _inbyte(u16 timeout); // msec timeout
     void xmodem_Out(int c);

 */

#include <string.h>
#include "xmodem.h"
#include "crc16.h"
#if (XMODEM_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif


#define SOH  0x01	// Pacotes de 128 bytes
#define STX  0x02	// Pacotes de 1024 bytes
#define EOT  0x04 	// End OF Transmission
#define ACK  0x06	// acknowledgment
#define NAK  0x15	// not acknowledgment
#define CAN  0x18	// Cancel
#define CTRLZ 0x1A

// pointers to stream I/O functions
static int (*xmodem_Out)(n16 c);
static int (*xmodem_In) (u8 *ch);
static u32 timerout_TX;
static u8 max_retrans;


//###################################################################################################################
// FUNÇÕES AUXILIARES
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Calcula o CRC ou CheckSum de um buffer
// Parametros: 	crcEn:
//					1 - Calcula CRC
//					0 - Calcula CheckSum
//				*buf: Ponteiro do buffer com dados a ser calculado o controle de erro
//				sz: Tamanho do buffer de dados
// Retorna:		pdTRUE se o caculo fora efetuado com sucesso, senão retorna pdFALSE
// -------------------------------------------------------------------------------------------------------------------
static int check(int crcEn, const u8 *buf, int sz) {
	if (crcEn) { 							// Checa se é checagem de erro via CRC
		u16 crc = crc16_ccitt(buf, sz);
		u16 tcrc = (buf[sz]<<8)+buf[sz+1];
		
		#if (XMODEM_USE_DEBUG == pdON)
		//plog("sz = %d\r\n", sz);
		//plog("buf[0] = %u - buf[1] = %u - buf[2] = %u - buf[3] = %u\r\n", buf[0], buf[1], buf[2], buf[3]);
		//plog("buf[sz] = 0x%x - buf[sz+1] = 0x%x\r\n", buf[sz], buf[sz+1]);
		//plog("check crc: %d(crc) = %d(tcrc)\r\n", crc, tcrc);
		#endif
		
		if (crc == tcrc)
			return pdTRUE;
	} else {								// A checagem de erro é via CheckSum
		int i;
		u8 cks = 0;
		for (i = 0; i < sz; ++i) {
			cks += buf[i];
		}
		if (cks == buf[sz])
			return pdTRUE;
	}

	return pdFALSE;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Checa por um determinado período de tempo se chegou algum dado da outra parte da comunicação de dados
// Parametros:	timerout: Tempo máximo para espera do dados
// Retorna:		O byte recebido, caso não chegar dado em um determinado tempo retorna errTIMEROUT, sinalizando estouro de timerout
// -------------------------------------------------------------------------------------------------------------------
static int _inbyte (u32 timerout) {
	u8 data;
	u32 t = timerout+1;
	
	while (t--) {
		if (xmodem_In(&data) == pdPASS)
			return data;
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
// Descrição: 	Retira qualquer dados recebidos, espera um tempo para que os dados atrasados chegarem
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------

static void flushinput(void) {		
	while (_inbyte((timerout_TX*3)>>1) >= 0);
}


//###################################################################################################################
// FUNÇÕES XMODEM
//###################################################################################################################
//###################################################################################################################


// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa o protocolo XMODEM
// Parametros:	sendbyte_func:	Ponteiro da função de transmissão de dados
//				getbyte_func:	Ponteiro da função de recepção de dados
//				timerout:		Tempo máximo para espera de dados da outra parte da comunicação
//				maxRetrans		Quantidade máxima de retransmissões para outra parte da comunicação
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void xmodem_Init(int (*sendbyte_func)(n16 c), int (*getbyte_func)(u8 *ch), u32 timerout, u8 maxRetrans) {
	// assign function pointers
	xmodem_Out 	= sendbyte_func;
	xmodem_In 	= getbyte_func;
	timerout_TX = timerout;
	max_retrans = maxRetrans;
}


// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Recebe um pacote de dados da outra parte da comunicação
// Parametros:  dest:	Ponteiro da buffer para recepção de dados
//				destsz:	Tamanho do buffer de recepção de dados
// Retorna:		A quantidade de dados recebidos sinalisando que a transferencia foi feita com suncesso, senão retorna o código de erro
//					errMODEM_CANCEL_REMOTE: Trasmissão cancelada remotamente
//					errMODEM_SYNC: Erro de sincronismo
//					errMODEM_MANY_RETRANSMISSION: Muitas retransmissões
// -------------------------------------------------------------------------------------------------------------------
int xmodem_Receive(u8 *dest, int destsz) {
	//u8 xbuff[1030]; // 1024 for XModem 1k + 3 head chars + 2 crc + nul
	u8 xbuff[134]; // 134 for XModem 128 + 3 head chars + 2 crc + nul
	u8 *p;
	int bufsz, crc = 0;
	u8 trychar = 'C';
	u8 packetno = 1;
	int i, c, len = 0;
	int retry, retrans = max_retrans;

	for(;;) {
		// SINCRONIZA COM O RECEPTOR
		for( retry = 0; retry < max_retrans; ++retry) { // Tenta sincronizar com receptor X vezes
			if (trychar) xmodem_Out(trychar);
	
			if ((c = _inbyte((timerout_TX)<<1)) >= 0) {		// Checa se o transmissor enviou algum dado de controle
				switch (c) {
				case SOH:									// O tramissor vai enviar pacotes de 128 bytes
					#if (XMODEM_USE_DEBUG == pdON)
					plog("Trans 128 bytes\r\n");
					#endif

					bufsz = 128;
					goto start_recv;
				case STX:									// O tramissor vai enviar pacotes de 1024 bytes
					#if (XMODEM_USE_DEBUG == pdON)
					plog("Trans 1024 bytes\r\n");
					#endif

					bufsz = 1024;
					goto start_recv;
				case EOT:									// O tramissor finalizou a comunicação
					#if (XMODEM_USE_DEBUG == pdON)
					plog("FIM transferencia. cap %u bytes\r\n", len);
					#endif

					flushinput();
					xmodem_Out(ACK);
					return len; // normal end
				case CAN:									// O tramissor cancelou a comunicação
					if ((c = _inbyte(timerout_TX)) == CAN) {
						#if (XMODEM_USE_DEBUG == pdON)
						plog("Tranferencia cancelada remotamente\r\n");
						#endif

						flushinput();
						xmodem_Out(ACK);
						return errMODEM_CANCEL_REMOTE; // canceled by remote 
					}
					break;
				default:
					break;
				}
			}
		}

		// CANCELA A COMUNCAÇÃO COM O TRANSMISSOR
		if (trychar == 'C') { trychar = NAK; continue; }

		#if (XMODEM_USE_DEBUG == pdON)
		plog("ERRO SYNC\r\n");
		#endif

		flushinput();
		xmodem_Out(CAN);
		xmodem_Out(CAN);
		xmodem_Out(CAN);
		
		return errMODEM_SYNC; // sync error 

	start_recv:
		if (trychar == 'C') crc = 1;						// Sinaliza se o controle de erro por CRC
		trychar = 0;
		p = xbuff;
		*p++ = c;
		
		#if (XMODEM_USE_DEBUG == pdON)
		plog("RX pacote %u CRC %d\r\n", packetno, crc);
		#endif

		for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
			if ((c = _inbyte(timerout_TX)) < 0) {
				#if (XMODEM_USE_DEBUG == pdON)
				plog("Reject: %d %d\r\n",  i, bufsz+(crc?1:0)+3);
				#endif

				goto reject;
			}
			*p++ = c;
		}

		if (	
			xbuff[1] == (u8)(~xbuff[2]) &&  
			(xbuff[1] == packetno || xbuff[1] == (u8)packetno-1) && 
			check(crc, &xbuff[3], bufsz)
		) {
			if (xbuff[1] == packetno)	{
				register int count = destsz - len;
				if (count > bufsz) count = bufsz;
				if (count > 0) {
					memcpy (&dest[len], &xbuff[3], count);
					
					#if (XMODEM_USE_DEBUG == pdON)
					plog("Salve pacote %u: len %u\r\n", packetno, len);
					#endif		
					
					len += count;
				}

				++packetno;
				retrans = max_retrans+1;
			}
		
			if (--retrans <= 0) {
				#if (XMODEM_USE_DEBUG == pdON)
				plog("Transferencia cancelada. MANY RETRY\r\n");
				#endif

				flushinput();
				xmodem_Out(CAN);
				xmodem_Out(CAN);
				xmodem_Out(CAN);
				return errMODEM_MANY_RETRANSMISSION; // too many retry error
			}
			xmodem_Out(ACK);
			continue;
		}
	reject:
		flushinput();
		xmodem_Out(NAK);
	}
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Transmite um pacote de dados para outra parte da comunicação
// Parametros:  dest:	Ponteiro da buffer para transmissão de dados
//				destsz:	Tamanho do buffer de transmissão de dados ou quantidade de dados a TX
// Retorna:		A quantidade de dados recebidos sinalisando que a transferencia foi feita com suncesso, senão retorna o código de erro
//					errMODEM_CANCEL_REMOTE: Trasmissão cancelada remotamente
//					errMODEM_SYNC: Erro de sincronismo
//					errMODEM_XINIT: Erro de retransmissão do pacote de dados
//					errMODEM_NACK: Não recebimento do ACK pela outra parte da comunicação 
// **********************************************************
//	PODEMOS MELHORAR COLOCANDO  bufsz = 128 ou 1024;
// **********************************************************
// -------------------------------------------------------------------------------------------------------------------
int xmodem_Transmit(u8 *src, int srcsz) {
	//u8 xbuff[1030]; // 1024 for XModem 1k + 3 head chars + 2 crc + nul
	u8 xbuff[134]; // 134 for XModem 128 + 3 head chars + 2 crc + nul
	int bufsz, crc = -1;
	u8 packetno = 1;
	int i, c, len = 0;
	int retry;

	for(;;) {
		// SINCRONIZA COM O RECEPTOR
		for( retry = 0; retry < max_retrans; ++retry) { // Tenta sincronizar com receptor X vezes
			#if (XMODEM_USE_DEBUG == pdON)
			plog("SYNC\r\n");
			#endif

			if ((c = _inbyte((timerout_TX)<<1)) >= 0) { // Checa se o receptor está pronto para receber os dados
				switch (c) {
				case 'C':								// O receptor requer controle de erro por CRC
					crc = 1;
					goto start_trans;
				case NAK:								// O receptor requer controle de erro por CheckSum
					crc = 0;
					goto start_trans;
				case CAN:								// O receptor cancelou a comunicação
					if ((c = _inbyte(timerout_TX)) == CAN) {
						xmodem_Out(ACK);
						flushinput();
						return errMODEM_CANCEL_REMOTE; // canceled by remote
					}
					break;
				default:
					break;
				}
			}
		}
		
		// CANCELA A COMUNCAÇÃO COM O RECEPTOR
		#if (XMODEM_USE_DEBUG == pdON)
		plog("CAN\r\n");
		#endif

		xmodem_Out(CAN);
		xmodem_Out(CAN);
		xmodem_Out(CAN);
		flushinput();
		return errMODEM_SYNC; // no sync

		for(;;) {
		start_trans:		
			xbuff[0] = SOH;  			// Especifica a transferencia de 128 bytes por vez
			xbuff[1] = packetno;		// Especifica a ordem do pacote, iniciando por 1
			xbuff[2] = ~packetno;
						
			bufsz = 128;				// Especifica a quantidade de dados para ser transferido
			c = srcsz - len;			// Determina o restante a ser transmitido
			if (c > bufsz) c = bufsz;	// Se o restante ainda é superior a 128 bytes, fazer o pacote com 128 bytes
			
			#if (XMODEM_USE_DEBUG == pdON)
			plog("TX pacote %u com %d bytes\r\n", packetno, c);
			#endif
				
			if (c >= 0) {
				memset (&xbuff[3], 0, bufsz); 		//Preenche o buffer com valor 0
				
				// COPIA O TRECHO DE 128 BYTES DO BUFFER PARA xbuff E FINALIZA COM CTRLZ
				if (c == 0) {
					xbuff[3] = CTRLZ;
				} else {
					memcpy (&xbuff[3], &src[len], c);
					if (c < bufsz) xbuff[3+c] = CTRLZ;
				}
				
				// ADICIONAR CRC NOS DOIS ÚLTIMOS BYTES DO xbuff OU CHECKSUM NO PELO ÚLTIMO BYTE DO xbuff
				if (crc) {
					u16 ccrc = crc16_ccitt(&xbuff[3], bufsz);
					xbuff[bufsz+3] = (ccrc>>8) & 0xFF;
					xbuff[bufsz+4] = ccrc & 0xFF;
				} else {
					u8 ccks = 0;
					for (i = 3; i < bufsz+3; ++i) {
						ccks += xbuff[i];
					}
					xbuff[bufsz+3] = ccks;
				}
				
				// TRANSMITE O PACOTE DE 128 BYTES
				for (retry = 0; retry < max_retrans; ++retry) { // Transmitir esse pacote até conseguir ou chegar ao número X de tentativas
					for (i = 0; i < bufsz+4+(crc?1:0); ++i) {	// Transmite o pacote de 128 bytes
						xmodem_Out(xbuff[i]);					// Transmite o byte do pacote
					}
					#if (XMODEM_USE_DEBUG == pdON)
					plog("TX pacote %u OK\r\n", packetno);
					#endif
					
					if ((c = _inbyte(timerout_TX)) >= 0 ) {		// Checa se o receptor enviou uma resposta
						switch (c) {
						case ACK:								// O receptor enviou um reconhecimento do comando
							#if (XMODEM_USE_DEBUG == pdON)
							plog("ACK\r\n");
							#endif
							++packetno;							// Incrementar o número de pacote para transmissão
							len += bufsz;						// Incrementar a quantidade de bytes enviados
							goto start_trans;					// Ir para transmitir o próximo pacote							
						case CAN:
							
							#if (XMODEM_USE_DEBUG == pdON)
							plog("CAN\r\n");
							#endif
							
							if ((c = _inbyte(timerout_TX)) == CAN) {	// O receptor enviou um cancelamento
								xmodem_Out(ACK);						// Enviar ao receptor um reconhecimento do comando
								flushinput();						// Limpa todos os bytes da porta de recepção
								return errMODEM_CANCEL_REMOTE; 								// Retorna avisando o cancelamento do receptor
							}
							break;
						case NAK:								// O receptor enviou um NÃO reconhecimento do comando. Vamos tentar novamente
						default:
							#if (XMODEM_USE_DEBUG == pdON)
							plog("NAK\r\n");
							#endif
							
							break;
						}
					}
				}
				
				// CANCELA A COMUNCAÇÃO COM O RECEPTOR
				#if (XMODEM_USE_DEBUG == pdON)
				plog("CAN2\r\n");
				#endif
				
				xmodem_Out(CAN);
				xmodem_Out(CAN);
				xmodem_Out(CAN);
				flushinput();
				return errMODEM_XINIT; // xmit error
			} else {
				// FINALIZA A COMUNICAÇÃO COM O RECEPTOR
				for (retry = 0; retry < 10; ++retry) {
					#if (XMODEM_USE_DEBUG == pdON)
					plog("EOT\r\n");
					#endif

					xmodem_Out(EOT); // Emite o comando de fim da transmissão
					if ((c = _inbyte((timerout_TX)<<1)) == ACK) {
						#if (XMODEM_USE_DEBUG == pdON)
						plog("ACK\r\n");
						#endif

						break; // Checa se o receptor reconheceu o comando
					}
				}

				#if (XMODEM_USE_DEBUG == pdON)
				plog("END\r\n");
				#endif

				flushinput();
				return (c == ACK)?len:errMODEM_NACK;
			}
		}
	}
}

