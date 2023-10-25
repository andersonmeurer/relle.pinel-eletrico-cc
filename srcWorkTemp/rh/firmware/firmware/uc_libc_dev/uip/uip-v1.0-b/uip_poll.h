#ifndef __UIP_POLL_H
#define __UIP_POLL_H

#include "uip.h"
#include "uip_arp.h"
#include "uip_nic.h"
#include "uip_timer.h"
#include "_config_cpu_.h"

void uip_PollSetTimers(clock_time_t t_poll, clock_time_t t_arp);
void uip_Poll(void);


#endif
