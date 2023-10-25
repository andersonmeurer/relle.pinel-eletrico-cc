#ifndef __MEM_MAP_H
#define __MEM_MAP_H

#if defined(USE_EXT_SDRAM)
#include "mem_dynamic.h"
#endif

#if defined(USE_EXT_SRAM)

#if defined(LPC1788)
	#include "mem_static_lpc1788.h"
#elif defined(LPC2468)
	#include "mem_static_lpc2468.h"
#else
	#include "mem_static_lpc22xx.h"
#endif
#endif


#endif

