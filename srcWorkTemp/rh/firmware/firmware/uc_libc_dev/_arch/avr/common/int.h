#ifndef __INT_H 
#define __INT_H

#include "_config_cpu_.h"

// LINHA AVR
#if defined(RTOS_NONE)
	#define int_IsEnabled()	(SREG&0x80)?pdTRUE:pdFALSE
#endif

//#define UART0_INTISENABLED	(UCSR0B&_BV(RXCIE))	?pdTRUE:pdFALSE
//#define UART1_INTISENABLED	(UCSR1B&_BV(RXCIE1))?pdTRUE:pdFALSE


#endif
