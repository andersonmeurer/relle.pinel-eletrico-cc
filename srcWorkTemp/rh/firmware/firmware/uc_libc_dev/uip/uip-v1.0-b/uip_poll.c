#include "uip_poll.h"

#if (UIPPOLL_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

#define uipBuffer ((struct uip_eth_hdr *)&uip_buf[0])
struct timer periodic_timer, arp_timer;

void uip_PollSetTimers(clock_time_t t_poll, clock_time_t t_arp) {
  	timer_set(&periodic_timer, t_poll); 				// Tempo de controle polling, ack, retransmissão e round trip estimations
  	timer_set(&arp_timer, t_arp);

  	#if (UIPPOLL_USE_DEBUG == pdON)
	plog("UIPtimer: periodic_timer.start = %u"CMD_TERMINATOR, periodic_timer.start);
	plog("UIPtimer: periodic_timer.interval =  %u"CMD_TERMINATOR, periodic_timer.interval);
	plog("ARPtimer: arp_timer.start = %u"CMD_TERMINATOR, arp_timer.start);
	plog("ARPtimer: arp_timer.interval =  %u"CMD_TERMINATOR, arp_timer.interval);
  	#endif
}

void uip_Poll(void) {
	u32 i;
	// RECEIVE PACKET AND PUT IN UIP_BUF
	uip_len = nic_Poll();

   	if(uip_len > 0) { // Received packet
		#if (UIPPOLL_USE_DEBUG == pdON)
    	plog("UIP: RX len = %u"CMD_TERMINATOR, uip_len);
		#endif

    	if(uipBuffer->type == htons(UIP_ETHTYPE_IP)) { // IP packet
			uip_arp_ipin();
			uip_input();
			// Se a chamada da função acima resultar na produção de algum dado que deve ser enviado pela rede,
			// a variavel global uip_len deve ser ajustada para um valor acima de 0 (zero).
			if(uip_len > 0) {
	  			uip_arp_out();
	  			nic_Send();

	  			// Aqui também responde ao PING
				#if (UIPPOLL_USE_DEBUG == pdON)
	  			plog("UIP: ARP OUT"CMD_TERMINATOR); // Se o UIP responder ao ping essa mensagem será exibida
				#endif
				}
      	} else if(uipBuffer->type == htons(UIP_ETHTYPE_ARP)) { // ARP packet
			uip_arp_arpin();
			#if (UIPPOLL_USE_DEBUG == pdON)
			plog("UIP: ARP IN"CMD_TERMINATOR);
			#endif

			// Se a chamada da função acima resultar na produção de algum dado q deve ser enviado pela rede,
			// a variavel global uip_len deve ser ajustada para um valor acima de 0 (zero).
			if(uip_len > 0) {
	  			nic_Send(); // ARP ack
			}
      	}
    } else if(timer_expired(&periodic_timer)) { // no packet but periodic_timer time out
//		#if (UIPPOLL_USE_DEBUG == pdON)
//    	plog("UIP: periodic_timer expired"CMD_TERMINATOR);
//		#endif

		timer_reset(&periodic_timer);
		for(i = 0; i < UIP_CONNS; i++) { // Faça para todas as conexões
			uip_periodic(i);
			// Se a chamada da função acima resultar na produção de algum dado q deve ser enviado pela rede,
			// a variavel global uip_len deve ser ajustada para um valor acima de 0 (zero). 
			if(uip_len > 0) {
				//plog("UIP: timer_expired ARP OUT TCP"CMD_TERMINATOR);
				uip_arp_out();
				nic_Send(); 
			}
		}

		#if UIP_UDP
    	for(i = 0; i < UIP_UDP_CONNS; i++) {
			uip_udp_periodic(i);
			//plog("UIP: Ocorreu uip_udp_periodic"CMD_TERMINATOR);
			// Se a chamada da função acima resultar na produção de algum dado q deve ser enviado pela rede,
			// a variavel global uip_len deve ser ajustada para um valor acima de 0 (zero).
			if(uip_len > 0) {
				//plog("UIP: timer_expired ARP OUT UDP"CMD_TERMINATOR);
				uip_arp_out();
				nic_Send(); 
			}
    	}
		#endif // UIP_UDP
      
    	// Chama a função ARP timer a cada X segundos.
    	if(timer_expired(&arp_timer)) {
//			#if (UIPPOLL_USE_DEBUG == pdON)
//      	plog("UIP: ARP timer expired"CMD_TERMINATOR);
//			#endif
		   	timer_reset(&arp_timer);
		   	uip_arp_timer();
      	}
    }
}

#ifdef __DHCPC_H__
void dhcpc_configured(const struct dhcpc_state *s)
{
  uip_sethostaddr(s->ipaddr);
  uip_setnetmask(s->netmask);
  uip_setdraddr(s->default_router);
  //rprintf(UIP_LOGGING_USE_UART ,"\n\rDados de rede configurados via DHCP");
}
#endif // __DHCPC_H__
