/*
	ESTRUTURA BASICA PARA APLICA��ES COM UIP
	Mude de acordo com a sua aplica��o
*/

#ifndef UIP_APP_H
#define UIP_APP_H

#include "_config_cpu_.h"
#include "psock.h"

void net_TCP(void);
void net_UDP(void);

#define UIP_APPCALL     	net_TCP	// Nome da fun��o da aplica��o que o uIP deve chamar na resposta dos eventos TCP
#define UIP_UDP_APPCALL  	net_UDP	// Nome da fun��o da aplica��o que o uIP deve chamar na resposta dos eventos UDP

// ESTRUTURA PARA O ESTADO DA APLICA��O REFERENTE A CADA CONEX�O TCP EFETUADA. INDEPENDETE DE QUEM INICIOU A CONEX�O
typedef struct app_state {
} uip_tcp_appstate_t;



#endif
