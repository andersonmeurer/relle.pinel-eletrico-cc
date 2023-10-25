#ifndef __SYSTIME_H
#define __SYSTIME_H

#include "_config_cpu_.h"


#if !defined(FREE_RTOS) // COM FREE RTOS
#error Esta include só é usada com freeRTOS
#else
#include "FreeRTOS.h"
#include "task.h"

#define delay_us(us)		vTaskDelay (us / portTICK_RATE_MS / 1000)
#define delay_ms(ms) 		vTaskDelay (ms / portTICK_RATE_MS)
#define systime_GetTick		xTaskGetTickCount

#define sec2tick(sec)		( ( portTickType ) ( (sec*1000) / portTICK_RATE_MS ) )
#define ms2tick(ms)			( ( portTickType ) ( ms / portTICK_RATE_MS ) )

#endif


#endif
