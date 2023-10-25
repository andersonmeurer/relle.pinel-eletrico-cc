/* 
   	Precise Delay Functions for LPC2100
   	Martin Thomas, 11/2004

	REELEMBRANDO QUE EXISTEM Os delays NO SYS_TIME QUE UTILIZA TICK DE SISTEMA
*/

#include "delay.h"

void delay(u32 delayval) {
	asm volatile (
		"L_LOOPUS_%=: 		\r\n\t" \
		"subs	%0, %0, #1 	\r\n\t" \
		"bne	L_LOOPUS_%=	\r\n\t" \
		:  /* no outputs */ : "r" (delayval)
	);
}

