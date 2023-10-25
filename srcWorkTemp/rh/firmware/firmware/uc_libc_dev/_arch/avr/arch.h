#ifndef __ARCH_H
#define __ARCH_H

#if defined(KIT_AT90S2313_GENERIC)
	#include "at90s2313.h"
#elif defined(KIT_AT90S2343_GENERIC)
	#include "at90s2343.h"
#elif defined(KIT_AT90S8515_GENERIC)
	#include "at90s8515.h"
#elif defined(KIT_ATMEGA16_GENERIC)
	#include "atmega16.h"
#elif defined(KIT_ATMEGA128_GENERIC)
	#include "atmega128.h"
#elif defined(KIT_ATMEGA8515_GENERIC)
	#include "atmega8515.h"
#elif defined(KIT_ATMEGA8535_GENERIC)
	#include "atmega8535.h"
#elif defined(KIT_ATTINY85_GENERIC)
	#include "attiny85.h"
#else
	#error ARQUITETURA DESCONHECIDA
#endif


// SYSTIME
// Valores de divisão para gerar os ticks. O systime por padrão gera um tick a cada 1ms
#define SYSTIME_TICK_1ms	1 	// Cada tick é gerado a cada 1ms (1000us)
#define SYSTIME_TICK_500us	2	// Cada tick é gerado a cada 0.5ms (500us)
#define SYSTIME_TICK_250us	4	// Cada tick é gerado a cada 0.25ms (250us)

#endif
