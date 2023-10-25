/* 
   Precise Delay Functions for LPC2100
   Martin Thomas, 11/2004
*/

#include "delay_tick.h"

void delay_ms(u32 delayval) {	
	#if defined(FREE_RTOS)
		vTaskDelay (delayval / portTICK_RATE_MS);
	#else
		u32 d = (delayval * 1e-3 * SYS_TIME_TICS_PER_SEC) + .5;
		systime_Pause(d);
	#endif	
}

// NÃO MUITO PRECISO EM CONTAGEM ABAIXO DE 30us. Tempo minimo de 10uS a CPU de 58Mhz
void delay_us(u32 delayval) {	
	#if defined(FREE_RTOS)
		vTaskDelay (delayval / portTICK_RATE_MS / 1000);
	#else
		u32 d = (delayval * 1e-6 * SYS_TIME_TICS_PER_SEC) + .5;
		systime_Pause(d);
	#endif	
}
