//
//  $Id: uiptask.c 344 2008-11-11 03:04:49Z jcw $
//  $Revision: 344 $
//  $Author: jcw $
//  $Date: 2008-11-10 22:04:49 -0500 (Mon, 10 Nov 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/uip/uiptask.c $
//

/*
 * Modified from an original work that is Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: uiptask.c 344 2008-11-11 03:04:49Z jcw $
 *
 */

//
//  Standard includes 
// 
#include <stdlib.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

//#include "../rtc/rtc.h"
//#include "../main.h"

#undef HTONS
#include "uip_task.h"
#include "uip_nic.h"
#include "uip.h"
#include "uip_arp.h" 

#include "stdio_uc.h"
#include "_config_lib_task.h"
#if (TASK_UIP_USE_DEBUG == pdON)
	#include "stdio_uc.h"
#endif

//#include "hardware/enc28j60.h"
//#include "uip/uip.h"
//#include "uip/uip_arp.h"
//#include "apps/webserver/httpd.h"
//#include "apps/telnetd/telnetd.h"
//#include "apps/dhcpc/dhcpc.h"
//#include "apps/sntp/sntp.h"


//  The start of the uIP buffer, which will contain the frame headers
#define uip_buffer ((struct uip_eth_hdr *) &uip_buf[0])

//  uIP update frequencies
#define UIP_RT_CLOCK_SECOND (configTICK_RATE_HZ)
#define UIP_ARP_FREQUENCY 	(10)
#define UIP_MAX_BLOCK_TIME 	(UIP_RT_CLOCK_SECOND / 2)

//  If CFG_UIP is not defined, we still compile the code, it's just not included.
//  Define TASKHANDLE_UIP so we compile.  Normally this is defined in main.h,
//  but it's #ifndef'ed out so that we don't allocate a task handle slot for it
//  when uIP support isn't included.

//u8 uip_buf [UIP_BUFSIZE + 8] __attribute__ ((aligned (4)));


static void vTaskUip( void *pvParameters);
	
static int useDHCP = pdTRUE;
static xQueueHandle xFastPollQueue = NULL; 
xTaskHandle *uip_task_handle;

struct uip_start_par_t {
	int 		*errFlag;
	u8 					*priority_int;
	struct uip_eth_addr *mac;	
};

// -------------------------------------------------------------------------------------------------------------------
// Descrição:
// Parametros: 	
// Retorna:		
// -------------------------------------------------------------------------------------------------------------------
uip_state_t xUipStart(int doDHCP, struct uip_eth_addr mac, unsigned portSHORT stack_size, unsigned int priority_task, xTaskHandle *task_handle, u8 priority_int) {
  	int errFlag = -1;
  	volatile portTickType xTicks;
	struct uip_start_par_t uip_start_par;
	
	uip_task_handle = task_handle;
	
	#if (TASK_UIP_USE_DEBUG == pdON)
	plog("UIP: Starting"CMD_TERMINATOR);
	#endif

  	if (xUipIsRunning ()) {
		#if (TASK_UIP_USE_DEBUG == pdON)
		plog("UIP: Task alreadu running"CMD_TERMINATOR);
		#endif

    	return UIP_STATE_ALREADY_RUNNING;
    }

  	useDHCP = doDHCP;
  		
	uip_start_par.mac = &mac;
  	uip_start_par.errFlag = &errFlag;
  	uip_start_par.priority_int = &priority_int;

	#if (TASK_UIP_USE_DEBUG == pdON)
	plog("UIP: Creating task"CMD_TERMINATOR);
	#endif
	
  	xTaskCreate (vTaskUip, (const signed portCHAR * const) "uIP", stack_size, &uip_start_par, priority_task, uip_task_handle);
  	 	
  	xTicks = xTaskGetTickCount ();

  	while ((volatile int) errFlag == -1)   {
    	vTaskDelay (100 / portTICK_RATE_MS);

    	if ((xTaskGetTickCount () - xTicks) > (5000 / portTICK_RATE_MS)) {
      		xUipStop ();
      		#if (TASK_UIP_USE_DEBUG == pdON)
			plog("UIP: Task didn´t start"CMD_TERMINATOR);
			#endif
	
      		return UIPSTATE_TASK_DIDNT_START;
    	}
  	}

  	if (errFlag == 1) {
    	xUipStop ();
    	#if (TASK_UIP_USE_DEBUG == pdON)
		plog("UIP: Nic error"CMD_TERMINATOR);
		#endif
	
    	return UIP_STATE_NIC_ERR;
  	}

  	#if (TASK_UIP_USE_DEBUG == pdON)
	plog("UIP: Init OK"CMD_TERMINATOR);
	#endif
	
  	return UIP_STATE_OK;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa 
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
static void vTaskUip( void *pvParameters) {
  	int xARPTimer;
  	struct uip_start_par_t *uip_start_par = pvParameters;
  	static volatile portTickType xStartTime, xCurrentTime;

  	if (!xFastPollQueue)
    	xFastPollQueue = xQueueCreate (1, (unsigned int) sizeof (struct uip_conn *));

  	//  Initialize the uIP TCP/IP stack
  	uip_init ();
  	uip_arp_init ();
	
	#if (TASK_UIP_USE_DEBUG == pdON)
	plog("UIP: uip and arp init OK"CMD_TERMINATOR);
	#endif
  	
  	//  Initialize the Ethernet controller hardware
  	if (nic_Init (*uip_start_par->mac, *uip_start_par->priority_int) != pdPASS) {
    	if (*uip_start_par->errFlag)
      		*uip_start_par->errFlag = 1;

    	while (1)
      		vTaskDelay (1000 / portTICK_RATE_MS);
  	}

  	if (uip_start_par->errFlag)
    	*uip_start_par->errFlag = 0;

    #if (TASK_UIP_USE_DEBUG == pdON)
	plog("UIP: NIC init OK"CMD_TERMINATOR);
	#endif
  	
    //  Either we've got an address, or we need to request one
  
  	if (useDHCP) {
		#if (TASK_UIP_USE_DHCP == pdON)
    	dhcpc_init (uip_ethaddr.addr, sizeof (uip_ethaddr.addr));
    	dhcpc_request ();
		//#else
    	//lprintf("DHCP support not enabled"CMD_TERMINATOR);
		#endif
  	} else  {
		#if (TASK_UIP_USE_HTTP == pdON)
    	httpd_init ();
		#endif
		#if (TASK_UIP_USE_TELNET == pdON)
    	telnetd_init ();
		#endif
  	}
 	
  	//  Initialise the local timers
  	xStartTime = xTaskGetTickCount ();
  	xARPTimer = 0;

	#if (TASK_UIP_USE_DEBUG == pdON)
	plog("UIP: Entering main loop"CMD_TERMINATOR);
	#endif
  	
  	//  The big loop
  	while (1)  {
    	/* nic_WaitForData() returns pdTRUE if we were woken by an 
       	interrupt, otherwise pdFALSE if the timer expired.  If pdTRUE,
       	then the ENC28J60 has data ready for us. */
    	if (nic_WaitForData (UIP_MAX_BLOCK_TIME) == pdTRUE)   {
      		/* Let the network device driver read an entire IP packet
         	into the uip_buf. If it returns > 0, there is a packet in the
         	uip_buf buffer. */
    	  	if ((uip_len = nic_Poll()) > 0) {
        		/* A packet is present in the uIP buffer. We call the
	           	appropriate ARP functions depending on what kind of packet we
    	       have received. If the packet is an IP packet, we should call
        	   uip_input() as well. */
        		if (uip_buffer->type == htons (UIP_ETHTYPE_IP)) {
          			uip_arp_ipin ();
          			uip_input ();

          			/* 	If the above function invocation resulted in data that
             			should be sent out on the network, the global variable
             			uip_len is set to a value > 0. */
          			if (uip_len > 0) {
            			uip_arp_out ();
            			nic_Send();
            			#if (TASK_UIP_USE_DEBUG == pdON)
						plog("ARP OUT"CMD_TERMINATOR);
						#endif
          			}
        		} else if (uip_buffer->type == htons (UIP_ETHTYPE_ARP)) {
          			uip_arp_arpin ();
					#if (TASK_UIP_USE_DEBUG == pdON)
					plog("ARP IN"CMD_TERMINATOR);
					#endif
          			/* If the above function invocation resulted in data that
             			should be sent out on the network, the global variable
             			uip_len is set to a value > 0. */  
          			if (uip_len > 0)
            			nic_Send();
        		}
      		}
    	} else {
      		struct uip_conn *conn;

      		//  If there's data in the short circuit queue, it means that
      		//  vUipFastPoll() was called with a connection number, and
      		//  we need to poll that connection.
      		if (xFastPollQueue && (xQueueReceive (xFastPollQueue, &conn, 0) == pdTRUE)) {
        		uip_poll_conn (conn);

        		if (uip_len > 0) {
          			uip_arp_out ();
          			nic_Send();
        		}
      		}

      		/* The poll function returned 0, so no packet was
         		received. Instead we check if it is time that we do the
         		periodic processing. */
      		xCurrentTime = xTaskGetTickCount ();

      		if ((xCurrentTime - xStartTime) >= UIP_RT_CLOCK_SECOND) {
        		int i;
				//#if (TASK_UIP_USE_DEBUG == pdON)
				//plog("timer_expired"CMD_TERMINATOR);	
				//#endif
				
        		/* Reset the timer. */
        		xStartTime = xCurrentTime;

        		/* Periodic check of all connections. */
        		for (i = 0; i < UIP_CONNS; i++) {
          			uip_periodic (i);

          			/* If the above function invocation resulted in data that
             		should be sent out on the network, the global variable
             		uip_len is set to a value > 0. */          
          			if (uip_len > 0) {
            			uip_arp_out ();
            			nic_Send();
          			}
        		}

				#if UIP_UDP
        		for (i = 0; i < UIP_UDP_CONNS; i++) {
          			uip_udp_periodic (i);

           			//#if (TASK_UIP_USE_DEBUG == pdON)
					//plog("UIP: Ocorreu uip_udp_periodic"CMD_TERMINATOR);
					//#endif

          			/* If the above function invocation resulted in data that
             		should be sent out on the network, the global variable
             		uip_len is set to a value > 0. */
          			if (uip_len > 0) {
            			uip_arp_out ();
            			nic_Send();
          			}
        		}
				#endif /* UIP_UDP */

        		/* Periodically call the ARP timer function. */
        		if (++xARPTimer == UIP_ARP_FREQUENCY) { 
          			uip_arp_timer ();
          			xARPTimer = 0;
        		}
      		}
    	}
  	}
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa 
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void vUipFastPoll (struct uip_conn *conn) {
  	if (xFastPollQueue)
    	xQueueSend (xFastPollQueue, &conn, portMAX_DELAY);
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa 
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
int xUipIsRunning(void) {
 	return *uip_task_handle ? 1 : 0;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa 
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
uip_state_t xUipStop(void) {
  	if (!xUipIsRunning ())
    	return UIPSTATE_NOT_RUNNING;
  	vTaskDelete (*uip_task_handle);
  	*uip_task_handle = NULL;
  	//enc28j60Deinit ();
  
  	return UIP_STATE_OK;
}


/*
// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa 
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void dispatch_tcp_appcall (void) {
	#if (TASK_UIP_USE_HTTP == pdON)
  		if (uip_conn->lport == HTONS (80))
    		httpd_appcall ();
	#endif
	
	#if (TASK_UIP_USE_TELNET == pdON)
  		if (uip_conn->lport == HTONS (23))
    		telnetd_appcall ();
	#endif
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa 
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
#if UIP_UDP
void dispatch_udp_appcall (void)	{
	#ifdef CFG_SNTP
	if (uip_udp_conn->rport == HTONS (123))
    		sntp_appcall ();
	else
	#endif
	#if (TASK_UIP_USE_DHCP == pdON)
  	if (uip_udp_conn->rport == HTONS (DHCPC_SERVER_PORT))
    	dhcpc_appcall ();
	#endif
}
#endif


// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa 
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
#if (TASK_UIP_USE_SNTP == pdON)
static void uipAutoSNTPTimeSynced (time_t *epochSeconds) {
  	if (*epochSeconds)	{
    	n32 to;
      
    	uip_gettimeoffset (&to);
    	*epochSeconds += to;
    	rtcSetEpochSeconds (*epochSeconds);
  	}
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa 
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
static void uipAutoSNTP (void) {
  	uip_ipaddr_t addr;

  	uip_getsntpaddr (&addr);

	if (!uip_iszeroaddr (&addr))
    	sntpSync (&addr, uipAutoSNTPTimeSynced);
}
#endif

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa 
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void dhcpc_configured (const dhcpcState_t *s) {
	#if (TASK_UIP_USE_DHCP == pdON)
  	if (!s->ipaddr [0] && !s->ipaddr [1])  {
    	lprintf("Can't get address via DHCP and no static address configured, stopping uIP task"CMD_TERMINATOR);
    	xUipStop ();
  	} else {
    	lprintf("IP address via DHCP is %d.%d.%d.%d"CMD_TERMINATOR PROMPT, uip_ipaddr1 (s->ipaddr), uip_ipaddr2 (s->ipaddr), uip_ipaddr3 (s->ipaddr), uip_ipaddr4 (s->ipaddr));
    	fflush (stdout);
    	uip_sethostaddr (s->ipaddr);
    	uip_setnetmask (s->netmask);
    	uip_setdraddr (s->default_router);
    	uip_setsntpaddr (s->sntpaddr);
    	uip_settimeoffset (&s->timeoffset);

		#if (TASK_UIP_USE_HTTP == pdON)
    	httpd_init ();
		#endif
		#if (TASK_UIP_USE_TELNET == pdON)
    	telnetd_init ();
		#endif
		#if (TASK_UIP_USE_SNTP == pdON)
    	uipAutoSNTP ();
		#endif
  	}
	#else
  	s = s;
	#endif
}

*/

