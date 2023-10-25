//
//  $Id: uiptask.h 345 2008-11-11 03:05:06Z jcw $
//  $Revision: 345 $
//  $Author: jcw $
//  $Date: 2008-11-10 22:05:06 -0500 (Mon, 10 Nov 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/uip/uiptask.h $
//

#ifndef __UIP_TASK_H
#define __UIP_TASK_H

#include "uip.h"

typedef enum {
  	UIP_STATE_OK,
  	UIP_STATE_ALREADY_RUNNING,
  	UIPSTATE_NOT_RUNNING,
  	UIPSTATE_TASK_DIDNT_START,
  	UIP_STATE_NIC_ERR
} uip_state_t;

uip_state_t xUipStart (int doDHCP, struct uip_eth_addr mac, unsigned portSHORT stack_size, unsigned int priority_task, xTaskHandle *task_handle, u8 priority_int);
void vUipFastPoll (struct uip_conn *conn);
int xUipIsRunning (void);
uip_state_t xUipStop (void);

#endif
