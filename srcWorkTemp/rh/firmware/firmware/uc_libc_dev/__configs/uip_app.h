/*
	ESTRUTURA BASICA PARA APLICAÇÕES COM UIP
	Mude de acordo com a sua aplicação
*/

#ifndef UIP_APP_H
#define UIP_APP_H

#include "_config_cpu_.h"
#include "psock.h"

void net_TCP(void);
void net_UDP(void);

#define UIP_APPCALL     	net_TCP	// Nome da função da aplicação que o uIP deve chamar na resposta dos eventos TCP
#define UIP_UDP_APPCALL  	net_UDP	// Nome da função da aplicação que o uIP deve chamar na resposta dos eventos UDP

// ESTRUTURA PARA O ESTADO DA APLICAÇÃO REFERENTE A CADA CONEXÃO TCP EFETUADA. INDEPENDETE DE QUEM INICIOU A CONEXÃO
typedef struct app_state {
} uip_tcp_appstate_t;



#endif
