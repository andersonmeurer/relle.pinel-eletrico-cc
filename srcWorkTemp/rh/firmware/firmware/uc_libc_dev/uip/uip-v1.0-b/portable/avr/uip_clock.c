/*
 * SPI2CF (c) 2006, Till Harbaum.
 * All rights reserved.
 *
 * This file is distributed under the GPL.
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
 * This file is part SPI2CF port of the uIP TCP/IP stack.
 */
#if !defined(FREE_RTOS)
#include <stdio.h>
#include <avr/interrupt.h>
#endif

#include "_config_cpu_.h"
#include "uip_clock.h"


// =======================================================================================================================================
// TRATAMENTO DO TICKS PELO FREERTOS
// =======================================================================================================================================
#if defined(FREE_RTOS)
#include "task.h"	

clock_time_t clock_time(void) {
  	return xTaskGetTickCount();
}

#else
// =======================================================================================================================================
// TRATAMENTO DO TICKS PELO TIMER DA CPU
// =======================================================================================================================================

#define TIMER_PRESCALE _BV(CS11)
#define TIMER_RELOAD   (n16)(((F_CPU/8) / CLOCK_SECOND)-1)
#define TIMER_REAL     (F_CPU/(TIMER_RELOAD+1)/8)

clock_time_t volatile ticks;

//SIGNAL(SIG_OUTPUT_COMPARE1A) {
TIMER1_INTERRUPT_HANDLER(TIMER1_COMPA_vect) {
  	/* clear counter */
  	TCNT1 = 0;
  	ticks++;
}

/* init timer hardware and install interrupt handler */
void clock_init(void) {
  	ticks = 0;

  	// make counter run
  	TCCR1B = TIMER_PRESCALE;

  	/* 16 bit overflow = CPU_CLK/65536, at 916 kHz hz = 13.977 Hz */
  	OCR1A = TIMER_RELOAD;

  	// make counter generate interrupts
  	TIMSK |= _BV(OCIE1A); // timer 1 compare a event
}



/*---------------------------------------------------------------------------*/
clock_time_t clock_time(void) {
  	return ticks;
}
/*---------------------------------------------------------------------------*/
#endif
