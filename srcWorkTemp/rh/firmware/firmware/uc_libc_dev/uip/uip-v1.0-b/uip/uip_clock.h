//######################################################################################################################
//defgroup clock Clock interface
//
//* The clock interface is the interface between the \ref timer "timer library"
// and the platform specific clock functionality. The clock
// interface must be implemented for each platform that uses the \ref
// timer "timer library".
 
//The clock interface does only one this: it measures time. The clock
//interface provides a macro, CLOCK_SECOND, which corresponds to one
// second of system time.
 
//\sa \ref timer "Timer library"
//######################################################################################################################


#ifndef __UIP_CLOCK_H
#define __UIP_CLOCK_H

#include "_config_cpu_.h"
#include "_config_lib_uip.h"
#include "uip_clock_arch.h"

/**
 * Initialize the clock library.
 *
 * This function initializes the clock library and should be called
 * from the main() function of the system.
 *
 */
#if !defined(FREE_RTOS)
	#if defined(CPU_ARM_LPC)
		int clock_init(void);
	#else
		void clock_init(void);
	#endif
#endif

/**
 * Get the current clock time.
 *
 * This function returns the current system clock time.
 *
 * \return The current clock time, measured in system ticks.
 */
clock_time_t clock_time(void);

/**
 * A second, measured in system clock time.
 *
 * \hideinitializer
 */
#ifdef CLOCK_CONF_SECOND
#define CLOCK_SECOND CLOCK_CONF_SECOND
#else
#define CLOCK_SECOND (clock_time_t)32
#endif

#endif
