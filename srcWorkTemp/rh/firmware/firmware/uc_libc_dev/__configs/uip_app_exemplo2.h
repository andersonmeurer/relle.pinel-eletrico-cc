#ifndef UIP_APP_H
#define UIP_APP_H

#include "_config_cpu_.h"
#include "buffer_d.h"

#include "smtp.h"

void net_UDP(void);
void net_TCP(void);

#define UIP_APPCALL     	net_TCP	// Nome da função da aplicação que o uIP deve chamar na resposta dos eventos TCP
#define UIP_UDP_APPCALL  	net_UDP	// Nome da função da aplicação que o uIP deve chamar na resposta dos eventos UDP

// CASO MUDAR AQUI É PRECICSO MAUDAR EM UIP_CONNS NO uip_config.h
#define UIP_CONN_RECEIVE 	1 // PARA MAIS CONEXÕES É PRECISO TER MAIS MEMÓRIA


//ESTADO DA CONEXÃO DE RECEPÇÃO
enum sts_cnx_enum {DISCONNECTED, CONNECTED_NO_LOGIN, CONNECTED_NO_USER, CONNECTED_NO_PASSWORD, CONNECTED_UPDATE, CONNECTED};
enum type_cnx_enum {NONE, RECEIVE, OPEN};

// ESTRUTURA PARA O ESTADO DA APLICAÇÃO REFERENTE A CADA CONEXÃO TCP EFETUADA. INDEPENDETE DE QUEM INICIOU A CONEXÃO
typedef struct app_state {
  	u16 count;								// uso do uIP
  	enum sts_cnx_enum sts_cnx;					// Estado atual da conexão
  	enum type_cnx_enum type_cnx;  				// Sinalize se o MSIP abriu ou recebeu a conexão
  	u8 count_polling;						// indicador de conexão ativa. Keep Alive
	u8 timer_out;							// Tempo para quem requisitou a conexão para a autenticação
  	u8 *packet_tx;  						// Buffer transmissão do pacote atual na rede para sua respectiva conexão
  	u8 flag_sensor_tx;						// Sinalizo que tem pedencia na transmissão do estado do sensor
  	u16 sensors_read;						// Guardo o estado atual da leitura dos sensores
  	u16 sensor_last_read;					// Guardo o último estado do sensor que ficou na pendencia para transmitir
  	u8 finish;								// Indicado que é para o MSIP fechar a conexão que abriu
  	u8 check_mod_mirror;					// Indicado para checar os sensores de forma automática
  	
  	// BUFFER CIRCULAR DE TRANSMISSÃO DE PACOTES - Isto para a aplicação guardar os dados a serem transmitido para sua respectiva conexão
	buffer_d_t buffer_tx;						// Estrutura do buffer circular de transmissão de pacotes
	u8 *buffer_tx_data; 					// Ponteiro para área de memória para armazenamento os bytes do buffer

  	#if (UIPAPP_USE_DEBUG == pdON)
  	u16 count_packet_tx;				// Quantidade de pacotes transmitido nesta conexão
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
