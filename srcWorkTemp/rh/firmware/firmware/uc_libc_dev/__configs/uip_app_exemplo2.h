#ifndef UIP_APP_H
#define UIP_APP_H

#include "_config_cpu_.h"
#include "buffer_d.h"

#include "smtp.h"

void net_UDP(void);
void net_TCP(void);

#define UIP_APPCALL     	net_TCP	// Nome da fun��o da aplica��o que o uIP deve chamar na resposta dos eventos TCP
#define UIP_UDP_APPCALL  	net_UDP	// Nome da fun��o da aplica��o que o uIP deve chamar na resposta dos eventos UDP

// CASO MUDAR AQUI � PRECICSO MAUDAR EM UIP_CONNS NO uip_config.h
#define UIP_CONN_RECEIVE 	1 // PARA MAIS CONEX�ES � PRECISO TER MAIS MEM�RIA


//ESTADO DA CONEX�O DE RECEP��O
enum sts_cnx_enum {DISCONNECTED, CONNECTED_NO_LOGIN, CONNECTED_NO_USER, CONNECTED_NO_PASSWORD, CONNECTED_UPDATE, CONNECTED};
enum type_cnx_enum {NONE, RECEIVE, OPEN};

// ESTRUTURA PARA O ESTADO DA APLICA��O REFERENTE A CADA CONEX�O TCP EFETUADA. INDEPENDETE DE QUEM INICIOU A CONEX�O
typedef struct app_state {
  	u16 count;								// uso do uIP
  	enum sts_cnx_enum sts_cnx;					// Estado atual da conex�o
  	enum type_cnx_enum type_cnx;  				// Sinalize se o MSIP abriu ou recebeu a conex�o
  	u8 count_polling;						// indicador de conex�o ativa. Keep Alive
	u8 timer_out;							// Tempo para quem requisitou a conex�o para a autentica��o
  	u8 *packet_tx;  						// Buffer transmiss�o do pacote atual na rede para sua respectiva conex�o
  	u8 flag_sensor_tx;						// Sinalizo que tem pedencia na transmiss�o do estado do sensor
  	u16 sensors_read;						// Guardo o estado atual da leitura dos sensores
  	u16 sensor_last_read;					// Guardo o �ltimo estado do sensor que ficou na pendencia para transmitir
  	u8 finish;								// Indicado que � para o MSIP fechar a conex�o que abriu
  	u8 check_mod_mirror;					// Indicado para checar os sensores de forma autom�tica
  	
  	// BUFFER CIRCULAR DE TRANSMISS�O DE PACOTES - Isto para a aplica��o guardar os dados a serem transmitido para sua respectiva conex�o
	buffer_d_t buffer_tx;						// Estrutura do buffer circular de transmiss�o de pacotes
	u8 *buffer_tx_data; 					// Ponteiro para �rea de mem�ria para armazenamento os bytes do buffer

  	#if (UIPAPP_USE_DEBUG == pdON)
  	u16 count_packet_tx;				// Quantidade de pacotes transmitido nesta conex�o
  	#endif

//
//  	u8 state;
//	char *to;
//  	char *from;
//  	char *subject;
//  	char *msg;
//  	u16 msglen;
//
//  	u16 sentlen, textlen;
//  	u16 sendptr;

} uip_tcp_appstate_t; //, smtp_state;


#endif
