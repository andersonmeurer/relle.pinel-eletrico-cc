#include "_config_cpu_.h"
#include "uip_clock.h"
#include "vic.h"

//#include "stdio_uc.h"	

#if !defined (UIP_TIMER_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO DO CLOCK PARA O TIMER DO UIP NO _config_cpu_.h
#endif

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
#include "vic.h"

#if (UIP_CLOCK_USE_TIMER == 0)
	#if defined (cortexm3)
	#define  clock_ISR 				timer0_ISR
	#endif

	#define UIP_TIME_VIC 			VIC_TIMER0
	#define UIP_PCTIM				PCTIM0
	#define UIP_TCR  				T0TCR
	#define UIP_PR  				T0PR
	#define UIP_MCR 				T0MCR
	#define UIP_CCR 				T0CCR
	#define UIP_EMR 				T0EMR
	#define UIP_TC 					T0TC
	#define UIP_MR0 				T0MR0
	#define UIP_IR 					T0IR
	#define UIP_VIC_TIMER 			VIC_TIMER0
	
	#define UIP_TCR_RESET 			T0TCR_RESET
	#define UIP_MR0_INT_ENABLE 		T0MCR_MR0_INT_ENABLE
	#define UIP_TIR_MR0I 			T0IR_MR0I
	#define UIP_TMPR_DIV 			T0MPR_DIV8
	#define UIP_TCR_ENABLE 			T0TCR_ENABLE
#else
	#if defined (cortexm3)
	#define  clock_ISR 				timer1_ISR
	#endif

	#define UIP_TIME_VIC 			VIC_TIMER1
	#define UIP_PCTIM				PCTIM1
	#define UIP_TCR 				T1TCR
	#define UIP_PR 					T1PR
	#define UIP_MCR					T1MCR
	#define UIP_CCR					T1CCR
	#define UIP_EMR					T1EMR
	#define UIP_TC					T1TC
	#define UIP_MR0					T1MR0
	#define UIP_IR					T1IR
	#define UIP_VIC_TIMER			VIC_TIMER1
	#define UIP_TCR_RESET 			T1TCR_RESET
	#define UIP_MR0_INT_ENABLE		T1MCR_MR0_INT_ENABLE
	#define UIP_TIR_MR0I			T1IR_MR0I
	#define UIP_TMPR_DIV			T1MPR_DIV8
	#define UIP_TCR_ENABLE			T1TCR_ENABLE
#endif


#define SYS_TIMER_PRESCALE UIP_TMPR_DIV								// CLK/8
#define SYS_TIMER_RELOAD (u32)(((PCLK/8) / CLOCK_SECOND)-1)  	// CLK/8 / NÚMERO DE CLOCKS DO UIP GERADOS EM UM SEGUNDO - 1

volatile clock_time_t uip_tics;
void clock_ISR(void);

int clock_init(void) {
  	PCONP |= UIP_PCTIM;								// Ligar o timer no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões
  	
  	UIP_TCR = UIP_TCR_RESET;                    	// Reset o contador do timer
  	UIP_PR  = SYS_TIMER_PRESCALE;         			// Ajusta o prescaler do timer
  	UIP_MR0 = SYS_TIMER_RELOAD;						// 
  	UIP_MCR = UIP_MR0_INT_ENABLE;                	// Hablita a interrupção de comparação 0
	
	#if !defined(SIM_SKYEYE) 
  	UIP_CCR = 0;        	                    	// disable compare registers
  	UIP_EMR = 0;    	                        	// disable external match register
  	#endif
  	
  	uip_tics = 0;									// Limpa o contador de tics do sistema
  	UIP_TC = 0;

	#if defined (arm7tdmi)
  	// Instala o ISR a VIC
	if (!irq_Install(UIP_VIC_TIMER, clock_ISR, UIP_TIMER_PRIO_LEVEL))
		return errIRQ_INSTALL;
	#endif

	#if defined (cortexm3)
  	nvic_enableIRQ(UIP_TIME_VIC);
  	nvic_setPriority(UIP_TIME_VIC, SYSTIME_PRIO_LEVEL);
  	#endif

	UIP_TCR = UIP_TCR_ENABLE;                  		// Habilita timer
	return pdPASS;
}

clock_time_t clock_time(void) {
  	//plog("tic %u"CMD_TERMINATOR, uip_tics);
  	return uip_tics;
}

void clock_ISR (void) {
    ISR_ENTRY;										// Procedimento para entrada da interrupção

	UIP_IR |= UIP_TIR_MR0I;							// Limpa o flag da INT
  	
  	UIP_TC = 0;
  	uip_tics++;
  	
  	#if defined (arm7tdmi)
  	VICSoftIntClr = (1<<UIP_VIC_TIMER);				// Limpa interrupção de software
  	#endif

	#if defined (cortexm3)
	nvic_clearPendingIRQ(UIP_TIME_VIC);
	#endif

    ISR_EXIT;										// Procedimento para saida da interrupção
}
#endif
