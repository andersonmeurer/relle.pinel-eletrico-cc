#ifndef __LPC210x_H
#define __LPC210x_H

#include "uc_libdefs.h"

// ########################################################################################################################################
// WATCHDOG
#define WDG_BASE_ADDR		0xE0000000

#define WDMOD          (*(reg32 *)(WDG_BASE_ADDR + 0x00))       // Watchdog Mode Register
#define WDTC           (*(reg32 *)(WDG_BASE_ADDR + 0x04))       // Watchdog Time Constant Register
#define WDFEED         (*(reg32 *)(WDG_BASE_ADDR + 0x08))       // Watchdog Feed Register
#define WDTV           (*(reg32 *)(WDG_BASE_ADDR + 0x0C))      	// Watchdog Time Value Register

/// ########################################################################################################################################
// REGISTRADORES DO TIMER 0

#define TMR0_BASE_ADDR		0xE0004000

#define T0IR           (*(reg32 *)(TMR0_BASE_ADDR + 0x00))	// Registrador de interrupções
	#define T0IR_MR0I    (1 << 0)        			    		// Flag de interrupção do canal de comparação 0
	#define T0IR_MR1I    (1 << 1)       			     		// Flag de interrupção do canal de comparação 1
	#define T0IR_MR2I    (1 << 2)       			     		// Flag de interrupção do canal de comparação 2
	#define T0IR_MR3I    (1 << 3)       			     		// Flag de interrupção do canal de comparação 3
	#define T0IR_CR0I    (1 << 4)        			    		// Flag de interrupção do canal de captura 0
	#define T0IR_CR1I    (1 << 5)         				   		// Flag de interrupção do canal de captura 1
	#define T0IR_CR2I    (1 << 6)        	 			   		// Flag de interrupção do canal de captura 2
	#define T0IR_CR3I    (1 << 7)        			    		// Flag de interrupção do canal de captura 3

#define T0TCR          (*(reg32 *)(TMR0_BASE_ADDR + 0x04))	// Registrador de controle de timer
	#define T0TCR_ENABLE  (1 << 0)								// Habilita o contador do timer
	#define T0TCR_STOP    (0)									// Desabilita o contador do timer
	#define T0TCR_RESET   (1 << 1)								// Zera o contador do timer

#define T0TC           (*(reg32 *)(TMR0_BASE_ADDR + 0x08))	// Contador do timer
#define T0PR           (*(reg32 *)(TMR0_BASE_ADDR + 0x0C))	// Registrador do prescaler (0 a 0xFFFFFFFF)
	#define	T0MPR_DIV1		0
	#define	T0MPR_DIV2		1
	#define	T0MPR_DIV4		3
	#define	T0MPR_DIV8		7
	#define	T0MPR_DIV16		15
	#define	T0MPR_DIV32		31
	#define	T0MPR_DIV64		63
	#define	T0MPR_DIV128	127
	#define	T0MPR_DIV256	255
	#define	T0MPR_DIV512	511
	#define	T0MPR_DIV1024	1023

#define T0PC           (*(reg32 *)(TMR0_BASE_ADDR + 0x10))	// Contador do prescaler

#define T0MCR          (*(reg32 *)(TMR0_BASE_ADDR + 0x14))	// Registrador do controle de comparações (Match Control)
	#define T0MCR_MR_NO_INT  			(0)						// Sem interrupções para match ou PWM
	#define T0_CONT_OVERFLOW  			(1 << 0)        		// Habilita a interrupção quando MR0 for igual a TC para contagem de overflow
	#define T0MCR_MR0_INT_ENABLE  		(1 << 0)        		// Habilita a interrupção quando MR0 for igual a TC
	#define T0MCR_MR0_RESET  			(1 << 1)  		      	// Reseta MR0 quando ele for igual a TC
	#define T0MCR_MR0_STOP  			(1 << 2)       		 	// Para TC e PC quando MR0 for igual a TC
	#define T0MCR_MR1_INT_ENABLE  		(1 << 3)		     	// Habilita a interrupção quando MR1 for igual a TC
	#define T0MCR_MR1_RESET 			(1 << 4)       		 	// Reseta MR1 quando ele for igual a TC
	#define T0MCR_MR1_STOP  			(1 << 5)        		// Para TC e PC quando MR1 for igual a TC
	#define T0MCR_MR2_INT_ENABLE   		(1 << 6)       		 	// Habilita a interrupção quando MR2 for igual a TC
	#define T0MCR_MR2_RESET  			(1 << 7)        		// Reseta MR2 quando ele for igual a TC
	#define T0MCR_MR2_STOP  			(1 << 8)        		// Para TC e PC quando MR2 for igual a TC
	#define T0MCR_MR3_INT_ENABLE   		(1 << 9)        		// Habilita a interrupção quando MR3 for igual a TC
	#define T0MCR_MR3_RESET  			(1 << 10)       		// Reseta MR3 quando ele for igual a TC
	#define T0MCR_MR3_STOP  			(1 << 11)       		// Para TC e PC quando MR3 for igual a TC
  	
	#define T0MCR_PWM_MR0_INT_ENABLE 	(1 << 0)        		// Habilita a interrupção quando PWMMR0 for igual a PWMTC
	#define T0MCR_PWM_MR0_RESET  		(1 << 1)        		// Reseta PWMMR0 quando ele for igual a PWMTC
	#define T0MCR_PWM_MR0_STOP  		(1 << 2)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T0MCR_PWM_MR1_INT_ENABLE 	(1 << 3)        		// Habilita a interrupção quando PWMMR1 for igual a PWMTC
	#define T0MCR_PWM_MR1_RESET  		(1 << 4)        		// Reseta PWMMR1 quando ele for igual a PWMTC
	#define T0MCR_PWM_MR1_STOP  		(1 << 5)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T0MCR_PWM_MR2_INT_ENABLE 	(1 << 6)        		// Habilita a interrupção quando PWMMR2 for igual a PWMTC
	#define T0MCR_PWM_MR2_RESET  		(1 << 7)        		// Reseta PWMMR2 quando ele for igual a PWMTC
	#define T0MCR_PWM_MR2_STOP  		(1 << 8)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T0MCR_PWM_MR3_INT_ENABLE 	(1 << 9)        		// Habilita a interrupção quando PWMMR3 for igual a PWMTC
	#define T0MCR_PWM_MR3_RESET  		(1 << 10)       		// Reseta PWMMR3 quando ele for igual a PWMTC
	#define T0MCR_PWM_MR3_STOP  		(1 << 11)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T0MCR_PWM_MR4_INT_ENABLE 	(1 << 12)       		// Habilita a interrupção quando PWMMR4 for igual a PWMTC
	#define T0MCR_PWM_MR4_RESET  		(1 << 13)       		// Reseta PWMMR4 quando ele for igual a PWMTC
	#define T0MCR_PWM_MR4_STOP  		(1 << 14)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T0MCR_PWM_MR5_INT_ENABLE 	(1 << 15)       		// Habilita a interrupção quando PWMMR5 for igual a PWMTC
	#define T0MCR_PWM_MR5_RESET  		(1 << 16)       		// Reseta PWMMR5 quando ele for igual a PWMTC
	#define T0MCR_PWM_MR5_STOP  		(1 << 17)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T0MCR_PWM_MR6_INT_ENABLE 	(1 << 18)       		// Habilita a interrupção quando PWMMR6 for igual a PWMTC
	#define T0MCR_PWM_MR6_RESET  		(1 << 19)      			// Reseta PWMMR6 quando ele for igual a PWMTC
	#define T0MCR_PWM_MR6_STOP  		(1 << 20)       		// Para PWMTC e PWMPC quando PWMMR6 for igual a TC

#define T0MR0          (*(reg32 *)(TMR0_BASE_ADDR + 0x18))	// Registrador de comparação 0
#define T0MR1          (*(reg32 *)(TMR0_BASE_ADDR + 0x1C))	// Registrador de comparação 1
#define T0MR2          (*(reg32 *)(TMR0_BASE_ADDR + 0x20))	// Registrador de comparação 2
#define T0MR3          (*(reg32 *)(TMR0_BASE_ADDR + 0x24))	// Registrador de comparação 3

#define T0CCR          (*(reg32 *)(TMR0_BASE_ADDR + 0x28))	// Registrador de controle de captura
	#define T0CCR_CR0_RISING	(1 << 0)     	  				// Habilita a captura do TC para CR0 durante a subida da borda CAPn.0
	#define T0CCR_CR0_FALLING	(1 << 1)    	    			// Habilita a captura do TC para CR0 durante a queda da borda CAPn.0
	#define T0CCR_CR0_INT 		(1 << 2)        				// Habilita a interrupção para ler o CR0
	#define T0CCR_CR1_RISING	(1 << 3)        				// Habilita a captura do TC para CR1 durante a subida da borda CAPn.1
	#define T0CCR_CR1_FALLING 	(1 << 4)        				// Habilita a captura do TC para CR1 durante a queda da borda CAPn.1
	#define T0CCR_CR1_INT 		(1 << 5)        				// Habilita a interrupção para ler o CR1
	#define T0CCR_CR2_RISING 	(1 << 6)        				// Habilita a captura do TC para CR2 durante a subida da borda CAPn.2
	#define T0CCR_CR2_FALLING 	(1 << 7)        				// Habilita a captura do TC para CR2 durante a queda da borda CAPn.2
	#define T0CCR_CR2_INT 		(1 << 8)        				// Habilita a interrupção para ler o CR2
	#define T0CCR_CR3_RISING 	(1 << 9)  		   				// Habilita a captura do TC para CR3 durante a subida da borda CAPn.3
	#define T0CCR_CR3_FALLING 	(1 << 10)       				// Habilita a captura do TC para CR3 durante a queda da borda CAPn.3
	#define T0CCR_CR3_INT 		(1 << 11)       				// Habilita a interrupção para ler o CR3

#define T0CR0          (*(reg32 *)(TMR0_BASE_ADDR + 0x2C))	// Registrador de captura 0
#define T0CR1          (*(reg32 *)(TMR0_BASE_ADDR + 0x30))	// Registrador de captura 1
#define T0CR2          (*(reg32 *)(TMR0_BASE_ADDR + 0x34))	// Registrador de captura 2
#define T0CR3          (*(reg32 *)(TMR0_BASE_ADDR + 0x38))	// Registrador de captura 3
#define T0EMR          (*(reg32 *)(TMR0_BASE_ADDR + 0x3C))	// Registrador de comparação externa

// ########################################################################################################################################
// REGISTRADORES DO TIMER 1
#define TMR1_BASE_ADDR		0xE0008000
#define T1IR           (*(reg32 *)(TMR1_BASE_ADDR + 0x00))	// Registrador de interrupções
	#define T1IR_MR0I    (1 << 0)        			    		// Flag de interrupção do canal de comparação 0
	#define T1IR_MR1I    (1 << 1)       			     		// Flag de interrupção do canal de comparação 1
	#define T1IR_MR2I    (1 << 2)       			     		// Flag de interrupção do canal de comparação 2
	#define T1IR_MR3I    (1 << 3)       			     		// Flag de interrupção do canal de comparação 3
	#define T1IR_CR0I    (1 << 4)        			    		// Flag de interrupção do canal de captura 0
	#define T1IR_CR1I    (1 << 5)         				   		// Flag de interrupção do canal de captura 1
	#define T1IR_CR2I    (1 << 6)        	 			   		// Flag de interrupção do canal de captura 2
	#define T1IR_CR3I    (1 << 7)        			    		// Flag de interrupção do canal de captura 3

#define T1TCR          (*(reg32 *)(TMR1_BASE_ADDR + 0x04))	// Registrador de controle de timer
	#define T1TCR_ENABLE  (1 << 0)								// Habilita o contador do timer
	#define T1TCR_STOP    (0)									// Desabilita o contador do timer
	#define T1TCR_RESET   (1 << 1)								// Zera o contador do timer

#define T1TC           (*(reg32 *)(TMR1_BASE_ADDR + 0x08))	// Contador do timer
#define T1PR           (*(reg32 *)(TMR1_BASE_ADDR + 0x0C))	// Registrador do prescaler (0 a 0xFFFFFFFF)
	#define	T1MPR_DIV1		0
	#define	T1MPR_DIV2		1
	#define	T1MPR_DIV4		3
	#define	T1MPR_DIV8		7
	#define	T1MPR_DIV16		15
	#define	T1MPR_DIV32		31
	#define	T1MPR_DIV64		63
	#define	T1MPR_DIV128	127
	#define	T1MPR_DIV256	255
	#define	T1MPR_DIV512	511
	#define	T1MPR_DIV1024	1023

#define T1PC           (*(reg32 *)(TMR1_BASE_ADDR + 0x10))	// Contador do prescaler
#define T1MCR          (*(reg32 *)(TMR1_BASE_ADDR + 0x14))	// Registrador do controle de comparações (Match Control)
	#define T1MCR_MR_NO_INT  			(0)						// Sem interrupções para match ou PWM
	#define T1_CONT_OVERFLOW  			(1 << 0)        		// Habilita a interrupção quando MR0 for igual a TC para contagem de overflow
	#define T1MCR_MR0_INT_ENABLE  		(1 << 0)        		// Habilita a interrupção quando MR0 for igual a TC
	#define T1MCR_MR0_RESET  			(1 << 1)  		      	// Reseta MR0 quando ele for igual a TC
	#define T1MCR_MR0_STOP  			(1 << 2)       		 	// Para TC e PC quando MR0 for igual a TC
	#define T1MCR_MR1_INT_ENABLE  		(1 << 3)		     	// Habilita a interrupção quando MR1 for igual a TC
	#define T1MCR_MR1_RESET 			(1 << 4)       		 	// Reseta MR1 quando ele for igual a TC
	#define T1MCR_MR1_STOP  			(1 << 5)        		// Para TC e PC quando MR1 for igual a TC
	#define T1MCR_MR2_INT_ENABLE   		(1 << 6)       		 	// Habilita a interrupção quando MR2 for igual a TC
	#define T1MCR_MR2_RESET  			(1 << 7)        		// Reseta MR2 quando ele for igual a TC
	#define T1MCR_MR2_STOP  			(1 << 8)        		// Para TC e PC quando MR2 for igual a TC
	#define T1MCR_MR3_INT_ENABLE   		(1 << 9)        		// Habilita a interrupção quando MR3 for igual a TC
	#define T1MCR_MR3_RESET  			(1 << 10)       		// Reseta MR3 quando ele for igual a TC
	#define T1MCR_MR3_STOP  			(1 << 11)       		// Para TC e PC quando MR3 for igual a TC
  	
	#define T1MCR_PWM_MR0_INT_ENABLE 	(1 << 0)        		// Habilita a interrupção quando PWMMR0 for igual a PWMTC
	#define T1MCR_PWM_MR0_RESET  		(1 << 1)        		// Reseta PWMMR0 quando ele for igual a PWMTC
	#define T1MCR_PWM_MR0_STOP  		(1 << 2)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T1MCR_PWM_MR1_INT_ENABLE 	(1 << 3)        		// Habilita a interrupção quando PWMMR1 for igual a PWMTC
	#define T1MCR_PWM_MR1_RESET  		(1 << 4)        		// Reseta PWMMR1 quando ele for igual a PWMTC
	#define T1MCR_PWM_MR1_STOP  		(1 << 5)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T1MCR_PWM_MR2_INT_ENABLE 	(1 << 6)        		// Habilita a interrupção quando PWMMR2 for igual a PWMTC
	#define T1MCR_PWM_MR2_RESET  		(1 << 7)        		// Reseta PWMMR2 quando ele for igual a PWMTC
	#define T1MCR_PWM_MR2_STOP  		(1 << 8)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T1MCR_PWM_MR3_INT_ENABLE 	(1 << 9)        		// Habilita a interrupção quando PWMMR3 for igual a PWMTC
	#define T1MCR_PWM_MR3_RESET  		(1 << 10)       		// Reseta PWMMR3 quando ele for igual a PWMTC
	#define T1MCR_PWM_MR3_STOP  		(1 << 11)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T1MCR_PWM_MR4_INT_ENABLE 	(1 << 12)       		// Habilita a interrupção quando PWMMR4 for igual a PWMTC
	#define T1MCR_PWM_MR4_RESET  		(1 << 13)       		// Reseta PWMMR4 quando ele for igual a PWMTC
	#define T1MCR_PWM_MR4_STOP  		(1 << 14)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T1MCR_PWM_MR5_INT_ENABLE 	(1 << 15)       		// Habilita a interrupção quando PWMMR5 for igual a PWMTC
	#define T1MCR_PWM_MR5_RESET  		(1 << 16)       		// Reseta PWMMR5 quando ele for igual a PWMTC
	#define T1MCR_PWM_MR5_STOP  		(1 << 17)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T1MCR_PWM_MR6_INT_ENABLE 	(1 << 18)       		// Habilita a interrupção quando PWMMR6 for igual a PWMTC
	#define T1MCR_PWM_MR6_RESET  		(1 << 19)      			// Reseta PWMMR6 quando ele for igual a PWMTC
	#define T1MCR_PWM_MR6_STOP  		(1 << 20)       		// Para PWMTC e PWMPC quando PWMMR6 for igual a TC

#define T1MR0          (*(reg32 *)(TMR1_BASE_ADDR + 0x18))	// Registrador de comparação 0
#define T1MR1          (*(reg32 *)(TMR1_BASE_ADDR + 0x1C))	// Registrador de comparação 1
#define T1MR2          (*(reg32 *)(TMR1_BASE_ADDR + 0x20))	// Registrador de comparação 2
#define T1MR3          (*(reg32 *)(TMR1_BASE_ADDR + 0x24))	// Registrador de comparação 3
#define T1CCR          (*(reg32 *)(TMR1_BASE_ADDR + 0x28))	// Registrador de controle de captura
	#define T1CCR_CR0_RISING	(1 << 0)     	  				// Habilita a captura do TC para CR0 durante a subida da borda CAPn.0
	#define T1CCR_CR0_FALLING	(1 << 1)    	    			// Habilita a captura do TC para CR0 durante a queda da borda CAPn.0
	#define T1CCR_CR0_INT 		(1 << 2)        				// Habilita a interrupção para ler o CR0
	#define T1CCR_CR1_RISING	(1 << 3)        				// Habilita a captura do TC para CR1 durante a subida da borda CAPn.1
	#define T1CCR_CR1_FALLING 	(1 << 4)        				// Habilita a captura do TC para CR1 durante a queda da borda CAPn.1
	#define T1CCR_CR1_INT 		(1 << 5)        				// Habilita a interrupção para ler o CR1
	#define T1CCR_CR2_RISING 	(1 << 6)        				// Habilita a captura do TC para CR2 durante a subida da borda CAPn.2
	#define T1CCR_CR2_FALLING 	(1 << 7)        				// Habilita a captura do TC para CR2 durante a queda da borda CAPn.2
	#define T1CCR_CR2_INT 		(1 << 8)        				// Habilita a interrupção para ler o CR2
	#define T1CCR_CR3_RISING 	(1 << 9)  		   				// Habilita a captura do TC para CR3 durante a subida da borda CAPn.3
	#define T1CCR_CR3_FALLING 	(1 << 10)       				// Habilita a captura do TC para CR3 durante a queda da borda CAPn.3
	#define T1CCR_CR3_INT 		(1 << 11)       				// Habilita a interrupção para ler o CR3

#define T1CR0          (*(reg32 *)(TMR1_BASE_ADDR + 0x2C))	// Registrador de captura 0
#define T1CR1          (*(reg32 *)(TMR1_BASE_ADDR + 0x30))	// Registrador de captura 1
#define T1CR2          (*(reg32 *)(TMR1_BASE_ADDR + 0x34))	// Registrador de captura 2
#define T1CR3          (*(reg32 *)(TMR1_BASE_ADDR + 0x38))	// Registrador de captura 3
#define T1EMR          (*(reg32 *)(TMR1_BASE_ADDR + 0x3C))	// Registrador de comparação externa

// ########################################################################################################################################
// PWM0 - PULSE WIDTH MODULATOR
#define PWM0_BASE_ADDR		0xE0014000

#define PWM0IR          (*(reg32 *)(PWM0_BASE_ADDR + 0x00))			//  Interrupt Register
	#define PW0MIR_MR0I  (1 << 0)            							// Flag de interrupção PWM0
	#define PW0MIR_MR1I  (1 << 1)         						 		// Flag de interrupção PWM1
	#define PW0MIR_MR2I  (1 << 2)    						       		// Flag de interrupção PWM2
	#define PW0MIR_MR3I  (1 << 3)            							// Flag de interrupção PWM3
	#define PW0MIR_MR4I  (1 << 8)            							// Flag de interrupção PWM4
	#define PW0MIR_MR5I  (1 << 9)            							// Flag de interrupção PWM5
	#define PW0MIR_MR6I  (1 << 10)           							// Flag de interrupção PWM6
	#define PW0MIR_MASK  (0x070F)

#define PWM0TCR         (*(reg32 *)(PWM0_BASE_ADDR + 0x04))        	// Timer Control Register
#define PWM0TC          (*(reg32 *)(PWM0_BASE_ADDR + 0x08))         // Timer Counter
#define PWM0PR          (*(reg32 *)(PWM0_BASE_ADDR + 0x0C))         // Prescale Register
#define PWM0PC          (*(reg32 *)(PWM0_BASE_ADDR + 0x10))         // Prescale Counter Register
#define PWM0MCR         (*(reg32 *)(PWM0_BASE_ADDR + 0x14))        	// Match Control Register
	#define PWM0MCR_ENABLE 		(1 << 0)								// Habilita o contador do timer PWM
	#define PWM0MCR_RESET  		(1 << 1)								// Zera o contador do timer PWM
	#define PWM0MCR_PWM_ENABLE  (1 << 3)

#define PWM0MR0         (*(reg32 *)(PWM0_BASE_ADDR + 0x18))			// Match Register 0
#define PWM0MR1         (*(reg32 *)(PWM0_BASE_ADDR + 0x1C))     	// Match Register 1
#define PWM0MR2         (*(reg32 *)(PWM0_BASE_ADDR + 0x20))			// Match Register 2
#define PWM0MR3         (*(reg32 *)(PWM0_BASE_ADDR + 0x24))        	// Match Register 3
#define PWM0MR4         (*(reg32 *)(PWM0_BASE_ADDR + 0x40))        	// Match Register 4
#define PWM0MR5         (*(reg32 *)(PWM0_BASE_ADDR + 0x44))        	// Match Register 5
#define PWM0MR6         (*(reg32 *)(PWM0_BASE_ADDR + 0x48))        	// Match Register 6
#define PWM0PCR         (*(reg32 *)(PWM0_BASE_ADDR + 0x4C))        	// Control Register
  	#define PWM0PCR_PWM_SEL2	(1 << 2)								// Seleciona o controle de dupla borda para o PWM2. 0 = borda simples / 1 = Dupla borda
	#define PWM0PCR_PWM_SEL3	(1 << 3)								// Seleciona o controle de dupla borda para o PWM3. 0 = borda simples / 1 = Dupla borda
	#define PWM0PCR_PWM_SEL4	(1 << 4)								// Seleciona o controle de dupla borda para o PWM4. 0 = borda simples / 1 = Dupla borda
	#define PWM0PCR_PWM_SEL5	(1 << 5)								// Seleciona o controle de dupla borda para o PWM5. 0 = borda simples / 1 = Dupla borda
	#define PWM0PCR_PWM_SEL6	(1 << 6)								// Seleciona o controle de dupla borda para o PWM6. 0 = borda simples / 1 = Dupla borda
	#define PWM0PCR_PWM_ENABLE1	(1 << 9)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
	#define PWM0PCR_PWM_ENABLE2	(1 << 10)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
	#define PWM0PCR_PWM_ENABLE3	(1 << 11)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
	#define PWM0PCR_PWM_ENABLE4	(1 << 12)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
	#define PWM0PCR_PWM_ENABLE5	(1 << 13)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
	#define PWM0PCR_PWM_ENABLE6	(1 << 14)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita

#define PWM0LER         (*(reg32 *)(PWM0_BASE_ADDR + 0x50))        	// Latch Enable Register
  	#define PWM0LER_DISABLE			0									// Desbilita todos os registradores latchs do PWM
	#define PWM0LER0_ENABLE			(1 << 0)							// Habilita o registrador latch do PWM0
	#define PWM0LER1_ENABLE			(1 << 1)							// Habilita o registrador latch do PWM1
	#define PWM0LER2_ENABLE			(1 << 2)							// Habilita o registrador latch do PWM2
	#define PWM0LER3_ENABLE			(1 << 3)							// Habilita o registrador latch do PWM3
	#define PWM0LER4_ENABLE			(1 << 4)							// Habilita o registrador latch do PWM4
	#define PWM0LER5_ENABLE			(1 << 5)							// Habilita o registrador latch do PWM5
	#define PWM0LER6_ENABLE			(1 << 6)							// Habilita o registrador latch do PWM6


// ########################################################################################################################################
// UART0 - UNIVERSAL ASYNCHRONOUS RECEIVER TRANSMITTER
#define UART0_SIZE_FIFO	16

// Modos de trabalho das UARTs
#define UART0_8N1      (u8)(U0LCR_CHAR_8 + U0LCR_PAR_NO   + U0LCR_STOP_1)
#define UART0_7N1      (u8)(U0LCR_CHAR_7 + U0LCR_PAR_NO   + U0LCR_STOP_1)
#define UART0_8N2      (u8)(U0LCR_CHAR_8 + U0LCR_PAR_NO   + U0LCR_STOP_2)
#define UART0_7N2      (u8)(U0LCR_CHAR_7 + U0LCR_PAR_NO   + U0LCR_STOP_2)
#define UART0_8E1      (u8)(U0LCR_CHAR_8 + U0LCR_PAR_EVEN + U0LCR_STOP_1)
#define UART0_7E1      (u8)(U0LCR_CHAR_7 + U0LCR_PAR_EVEN + U0LCR_STOP_1)
#define UART0_8E2      (u8)(U0LCR_CHAR_8 + U0LCR_PAR_EVEN + U0LCR_STOP_2)
#define UART0_7E2      (u8)(U0LCR_CHAR_7 + U0LCR_PAR_EVEN + U0LCR_STOP_2)
#define UART0_8O1      (u8)(U0LCR_CHAR_8 + U0LCR_PAR_ODD  + U0LCR_STOP_1)
#define UART0_7O1      (u8)(U0LCR_CHAR_7 + U0LCR_PAR_ODD  + U0LCR_STOP_1)
#define UART0_8O2      (u8)(U0LCR_CHAR_8 + U0LCR_PAR_ODD  + U0LCR_STOP_2)
#define UART0_7O2      (u8)(U0LCR_CHAR_7 + U0LCR_PAR_ODD  + U0LCR_STOP_2)


// Tipo de FIFO das UARTs
#define UART0_FIFO_OFF (0x00)
#define UART0_FIFO_RX_TRIG1   (u8)(U0FCR_FIFO_ENABLE + U0FCR_FIFO_TRIG1)
#define UART0_FIFO_RX_TRIG4   (u8)(U0FCR_FIFO_ENABLE + U0FCR_FIFO_TRIG4)
#define UART0_FIFO_RX_TRIG8   (u8)(U0FCR_FIFO_ENABLE + U0FCR_FIFO_TRIG8)
#define UART0_FIFO_RX_TRIG14  (u8)(U0FCR_FIFO_ENABLE + U0FCR_FIFO_TRIG14)

#define UART0_BASE_ADDR		0xE000C000
#define U0RBR          (*(reg32 *)(UART0_BASE_ADDR + 0x00))	// Registrador de recepção de dados
#define U0THR          (*(reg32 *)(UART0_BASE_ADDR + 0x00))	// Registrador de transmissão de dados 
#define U0DLL          (*(reg32 *)(UART0_BASE_ADDR + 0x00))	// Registrador de ajuste de baudrate (LSB)
#define U0DLM          (*(reg32 *)(UART0_BASE_ADDR + 0x04))	// Registrador de ajuste de baudrate (MSB)
#define U0IER          (*(reg32 *)(UART0_BASE_ADDR + 0x04))	// Registrador de habilitação das interrupções
	#define U0IER_ERBFI          (1 << 0)    					// Habilita a int de dado pronto recebido
	#define U0IER_ETBEI          (1 << 1)    					// Habilita a int THRE (Transmit Holding Register Empty)
	#define U0IER_ELSI           (1 << 2)    					// Enable Receive Line Status Interrupt
	#define U0IER_EDSSI          (1 << 3)    					// Enable MODEM Status Interrupt

#define U0IIR          (*(reg32 *)(UART0_BASE_ADDR + 0x08))	// Registrador de ID de interrupção. Somente para leitura
	#define U0IIR_NO_INT         (1 << 0)   				 		// 1 = Sem pendencia de interrupção / 0 = Pelo menos uma pedencia de interrupção
	#define U0IIR_MS_INT         (0 << 1)    					// MODEM Status
	#define U0IIR_THRE_INT       (1 << 1)    					// Interrupção de THRE (Transmit Holding Register Empty)(
	#define U0IIR_RDA_INT        (2 << 1)    					// Dado disponivel na recpção
	#define U0IIR_RLS_INT        (3 << 1)    					// Receive Line Status
	#define U0IIR_CTI_INT        (6 << 1)    					// Indicador de estouro de tempo para o caractere (Character Timeout Indicator)
	#define U0IIR_ID_MASK        0x0E

#define U0FCR          (*(reg32 *)(UART0_BASE_ADDR + 0x08))	// Registrador de cntrole da FIFO
	#define U0FCR_FIFO_ENABLE    (1 << 0)    					// Habilita FIFO
	#define U0FCR_RX_FIFO_RESET  (1 << 1)    					// Reseta o FIFO de recepção
	#define U0FCR_TX_FIFO_RESET  (1 << 2)   			 		// Reseta o FIFO de transmissão
	#define U0FCR_FIFO_TRIG1     (0 << 6)    					// Trigger @ 1 character in FIFO
	#define U0FCR_FIFO_TRIG4     (1 << 6)   			 		// Trigger @ 4 characters in FIFO
	#define U0FCR_FIFO_TRIG8     (2 << 6)   				 	// Trigger @ 8 characters in FIFO
	#define U0FCR_FIFO_TRIG14    (3 << 6)    					// Trigger @ 14 characters in FIFO

#define U0LCR          (*(reg32 *)(UART0_BASE_ADDR + 0x0C))	// Registrador de controle de linha
	#define U0LCR_CHAR_5         (0 << 0)   			 		// Tamanho do caractere com 5 bits
	#define U0LCR_CHAR_6         (1 << 0)    					// Tamanho do caractere com 6 bits
	#define U0LCR_CHAR_7         (2 << 0)    					// Tamanho do caractere com 7 bits
	#define U0LCR_CHAR_8         (3 << 0)    					// Tamanho do caractere com 8 bits
	#define U0LCR_STOP_1         (0 << 2)    					// 1 stop bit
	#define U0LCR_STOP_2         (1 << 2)    					// 2 stop bits ou 1.5 se ULCR[1:0] = 00
	#define U0LCR_PAR_NO         (0 << 3)    					// Sem pariedade
	#define U0LCR_PAR_ODD        (1 << 3)    					// Pariedade impar
	#define U0LCR_PAR_EVEN       (3 << 3)    					// Pariedade par
	#define U0LCR_PAR_MARK       (5 << 3)    					// Força a pariedade = 1
	#define U0LCR_PAR_SPACE      (7 << 3)   	 				// Força a pariedade = 0
	#define U0LCR_BREAK_ENABLE   (1 << 6)    					// Condições de parada a linha de saida. 1 = Habilita parada de transmissão / 0 = Desabilita parada de transmissão
	#define U0LCR_DLAB_ENABLE    (1 << 7)    					// Acessos as registradores do baudrate

#define U0LSR          (*(reg32 *)(UART0_BASE_ADDR + 0x14))	// Registrador de status de linha
	#define U0LSR_RDR            (1 << 0)    					// Receive Data Ready
	#define U0LSR_OE             (1 << 1)    					// Overrun Error
	#define U0LSR_PE             (1 << 2)   			 		// Parity Error
	#define U0LSR_FE             (1 << 3)   	 				// Framing Error
	#define U0LSR_BI             (1 << 4)    					// Break Interrupt
	#define U0LSR_THRE           (1 << 5)    					// Transmit Holding Register Empty
	#define U0LSR_TEMT           (1 << 6)    					// Transmitter Empty
	#define U0LSR_RXFE           (1 << 7)    					// Error in Receive FIFO
	#define U0LSR_ERR_MASK       0x1E

#define U0SCR          (*(reg32 *)(UART0_BASE_ADDR + 0x1C))	// Scratch Pad Register

// ########################################################################################################################################
// UART1 - UNIVERSAL ASYNCHRONOUS RECEIVER TRANSMITTER
#define UART1_SIZE_FIFO	16

// Modos de trabalho das UARTs
#define UART1_8N1      (u8)(U1LCR_CHAR_8 + U1LCR_PAR_NO   + U1LCR_STOP_1)
#define UART1_7N1      (u8)(U1LCR_CHAR_7 + U1LCR_PAR_NO   + U1LCR_STOP_1)
#define UART1_8N2      (u8)(U1LCR_CHAR_8 + U1LCR_PAR_NO   + U1LCR_STOP_2)
#define UART1_7N2      (u8)(U1LCR_CHAR_7 + U1LCR_PAR_NO   + U1LCR_STOP_2)
#define UART1_8E1      (u8)(U1LCR_CHAR_8 + U1LCR_PAR_EVEN + U1LCR_STOP_1)
#define UART1_7E1      (u8)(U1LCR_CHAR_7 + U1LCR_PAR_EVEN + U1LCR_STOP_1)
#define UART1_8E2      (u8)(U1LCR_CHAR_8 + U1LCR_PAR_EVEN + U1LCR_STOP_2)
#define UART1_7E2      (u8)(U1LCR_CHAR_7 + U1LCR_PAR_EVEN + U1LCR_STOP_2)
#define UART1_8O1      (u8)(U1LCR_CHAR_8 + U1LCR_PAR_ODD  + U1LCR_STOP_1)
#define UART1_7O1      (u8)(U1LCR_CHAR_7 + U1LCR_PAR_ODD  + U1LCR_STOP_1)
#define UART1_8O2      (u8)(U1LCR_CHAR_8 + U1LCR_PAR_ODD  + U1LCR_STOP_2)
#define UART1_7O2      (u8)(U1LCR_CHAR_7 + U1LCR_PAR_ODD  + U1LCR_STOP_2)

// Tipo de FIFO das UARTs
#define UART1_FIFO_OFF (0x00)
#define UART1_FIFO_RX_TRIG1   (u8)(U1FCR_FIFO_ENABLE + U1FCR_FIFO_TRIG1)
#define UART1_FIFO_RX_TRIG4   (u8)(U1FCR_FIFO_ENABLE + U1FCR_FIFO_TRIG4)
#define UART1_FIFO_RX_TRIG8   (u8)(U1FCR_FIFO_ENABLE + U1FCR_FIFO_TRIG8)
#define UART1_FIFO_RX_TRIG14  (u8)(U1FCR_FIFO_ENABLE + U1FCR_FIFO_TRIG14)

#define UART1_BASE_ADDR		0xE0010000
#define U1RBR          (*(reg32 *)(UART1_BASE_ADDR + 0x00))	// Registrador de recepção de dados
#define U1THR          (*(reg32 *)(UART1_BASE_ADDR + 0x00))	// Registrador de transmissão de dados 
#define U1DLL          (*(reg32 *)(UART1_BASE_ADDR + 0x00))	// Registrador de ajuste de baudrate (LSB)
#define U1DLM          (*(reg32 *)(UART1_BASE_ADDR + 0x04))	// Registrador de ajuste de baudrate (MSB)
#define U1IER          (*(reg32 *)(UART1_BASE_ADDR + 0x04))	// Registrador de habilitação das interrupções
	#define U1IER_ERBFI          (1 << 0)    					// Habilita a int de dado pronto recebido
	#define U1IER_ETBEI          (1 << 1)    					// Habilita a int THRE (Transmit Holding Register Empty)
	#define U1IER_ELSI           (1 << 2)    					// Enable Receive Line Status Interrupt
	#define U1IER_EDSSI          (1 << 3)    					// Enable MODEM Status Interrupt

#define U1IIR          (*(reg32 *)(UART1_BASE_ADDR + 0x08))	// Registrador de ID de interrupção. Somente para leitura
	#define U1IIR_NO_INT         (1 << 0)   				 	// 1 = Sem pendencia de interrupção / 0 = Pelo menos uma pedencia de interrupção
	#define U1IIR_MS_INT         (0 << 1)    					// MODEM Status
	#define U1IIR_THRE_INT       (1 << 1)    					// Interrupção de THRE (Transmit Holding Register Empty)(
	#define U1IIR_RDA_INT        (2 << 1)    					// Dado disponivel na recpção
	#define U1IIR_RLS_INT        (3 << 1)    					// Receive Line Status
	#define U1IIR_CTI_INT        (6 << 1)    					// Indicador de estouro de tempo para o caractere (Character Timeout Indicator)
	#define U1IIR_ID_MASK        0x0E

#define U1FCR          (*(reg32 *)(UART1_BASE_ADDR + 0x08))	// Registrador de controle da FIFO
	#define U1FCR_FIFO_ENABLE    (1 << 0)    					// Habilita FIFO
	#define U1FCR_RX_FIFO_RESET  (1 << 1)    					// Reseta o FIFO de recepção
	#define U1FCR_TX_FIFO_RESET  (1 << 2)   			 		// Reseta o FIFO de transmissão
	#define U1FCR_FIFO_TRIG1     (0 << 6)    					// Trigger @ 1 character in FIFO
	#define U1FCR_FIFO_TRIG4     (1 << 6)   			 		// Trigger @ 4 characters in FIFO
	#define U1FCR_FIFO_TRIG8     (2 << 6)   				 	// Trigger @ 8 characters in FIFO
	#define U1FCR_FIFO_TRIG14    (3 << 6)    					// Trigger @ 14 characters in FIFO
	
#define U1LCR          (*(reg32 *)(UART1_BASE_ADDR + 0x0C))	// Registrador de controle de linha
	#define U1LCR_CHAR_5         (0 << 0)   			 		// Tamanho do caractere com 5 bits
	#define U1LCR_CHAR_6         (1 << 0)    					// Tamanho do caractere com 6 bits
	#define U1LCR_CHAR_7         (2 << 0)    					// Tamanho do caractere com 7 bits
	#define U1LCR_CHAR_8         (3 << 0)    					// Tamanho do caractere com 8 bits
	#define U1LCR_STOP_1         (0 << 2)    					// 1 stop bit
	#define U1LCR_STOP_2         (1 << 2)    					// 2 stop bits ou 1.5 se ULCR[1:0] = 00
	#define U1LCR_PAR_NO         (0 << 3)    					// Sem pariedade
	#define U1LCR_PAR_ODD        (1 << 3)    					// Pariedade impar
	#define U1LCR_PAR_EVEN       (3 << 3)    					// Pariedade par
	#define U1LCR_PAR_MARK       (5 << 3)    					// Força a pariedade = 1
	#define U1LCR_PAR_SPACE      (7 << 3)   	 				// Força a pariedade = 0
	#define U1LCR_BREAK_ENABLE   (1 << 6)    					// Condições de parada a linha de saida. 1 = Habilita parada de transmissão / 0 = Desabilita parada de transmissão
	#define U1LCR_DLAB_ENABLE    (1 << 7)    					// Acessos as registradores do baudrate
	
#define U1MCR          (*(reg32 *)(UART1_BASE_ADDR + 0x10))
	#define U1MCR_DTR            (1 << 0)    		// Data Terminal Ready
	#define U1MCR_RTS            (1 << 1)    		// Request To Send
	#define U1MCR_LB             (1 << 4)    		// Loopback

#define U1LSR          (*(reg32 *)(UART1_BASE_ADDR + 0x14))	// Registrador de status de linha
	#define U1LSR_RDR            (1 << 0)    					// Receive Data Ready
	#define U1LSR_OE             (1 << 1)    					// Overrun Error
	#define U1LSR_PE             (1 << 2)   			 		// Parity Error
	#define U1LSR_FE             (1 << 3)   	 				// Framing Error
	#define U1LSR_BI             (1 << 4)    					// Break Interrupt
	#define U1LSR_THRE           (1 << 5)    					// Transmit Holding Register Empty
	#define U1LSR_TEMT           (1 << 6)    					// Transmitter Empty
	#define U1LSR_RXFE           (1 << 7)    					// Error in Receive FIFO
	#define U1LSR_ERR_MASK       0x1E

#define U1MSR          (*(reg32 *)(UART1_BASE_ADDR + 0x18))
	#define U1MSR_DCTS           (1 << 0)    		// Delta Clear To Send
	#define U1MSR_DDSR           (1 << 1)    		// Delta Data Set Ready
	#define U1MSR_TERI           (1 << 2)    		// Trailing Edge Ring Indicator
	#define U1MSR_DDCD           (1 << 3)    		// Delta Data Carrier Detect
	#define U1MSR_CTS            (1 << 4)    		// Clear To Send
	#define U1MSR_DSR            (1 << 5)    		// Data Set Ready
	#define U1MSR_RI             (1 << 6)  			// Ring Indicator
	#define U1MSR_DCD            (1 << 7)    		// Data Carrier Detect

#define U1SCR          (*(reg32 *)(UART1_BASE_ADDR + 0x1C)) // Scratch Pad Register

// ########################################################################################################################################
// I2C0 
#define I2C0_BASE_ADDR		0xE001C000

#define I2C0CONSET      (*(reg32 *)(I2C0_BASE_ADDR + 0x00))	// Control Set Register 
	#define I2C0_AA				(1 << 2) 						// ACK - Este bit em 1 envia um ACK em 1. bit 0 não tem efeito
  	#define I2C0_SI				(1 << 3) 						// Flag de interrupção - Este bit em 1 indica que ocorreu interrupção
  	#define I2C0_STO			(1 << 4) 						// Este bit em 1 envia stop. bit 0 não tem efeito
  	#define I2C0_STA			(1 << 5) 						// Este bit em 1 envia start. bit 0 não tem efeito
  	#define I2C0_I2EN			(1 << 6) 						// Este bit em 1 habilita bus I2C. bit 0 não tem efeito

#define I2C0STAT        (*(reg32 *)(I2C0_BASE_ADDR + 0x04))	// Status Register 
  	// MESTRE - TRANSMISSÃO
	#define I2C0_MT_SLA_ACK					0x18
	#define I2C0_MT_SLA_NACK				0x20
	#define I2C0_MT_DATA_ACK				0x28
	#define I2C0_MT_DATA_NACK				0x30
	#define I2C0_MT_ARB_LOST				0x38

	// MESTRE - RECEPÇÃO
	#define I2C0_MR_ARB_LOST				0x38
	#define I2C0_MR_SLA_ACK					0x40
	#define I2C0_MR_SLA_NACK				0x48
	#define I2C0_MR_DATA_ACK				0x50
	#define I2C0_MR_DATA_NACK				0x58

	// ESCRAVO Slave - TRANSMISSÃO
	#define I2C0_ST_SLA_ACK					0xA8
	#define I2C0_ST_ARB_LOST_SLA_ACK		0xB0
	#define I2C0_ST_DATA_ACK				0xB8
	#define I2C0_ST_DATA_NACK				0xC0
	#define I2C0_ST_LAST_DATA				0xC8

	// ESCRAVO - RECEPÇÃO
	#define I2C0_SR_SLA_ACK					0x60
	#define I2C0_SR_ARB_LOST_SLA_ACK		0x68
	#define I2C0_SR_GCALL_ACK				0x70
	#define I2C0_SR_ARB_LOST_GCALL_ACK		0x78
	#define I2C0_SR_DATA_ACK				0x80
	#define I2C0_SR_DATA_NACK				0x88
	#define I2C0_SR_GCALL_DATA_ACK			0x90
	#define I2C0_SR_GCALL_DATA_NACK			0x98
	#define I2C0_SR_STOP					0xA0

#define I2C0DAT         (*(reg32 *)(I2C0_BASE_ADDR + 0x08))	// Data Register 
#define I2C0ADR         (*(reg32 *)(I2C0_BASE_ADDR + 0x0C))	// Slave Address Register 
#define I2C0SCLH        (*(reg32 *)(I2C0_BASE_ADDR + 0x10))	// SCL Duty Cycle Register (high half word) 
#define I2C0SCLL        (*(reg32 *)(I2C0_BASE_ADDR + 0x14))	// SCL Duty Cycle Register (low half word) 
#define I2C0CONCLR     	(*(reg32 *)(I2C0_BASE_ADDR + 0x18))  // Control Clear Register 
	#define I2C0_AAC				(1 << 2) 					// ACK - Este bit em 1 envia um ACK em 0. bit 0 não tem efeito
  	#define I2C0_SIC				(1 << 3) 					// Flag de interrupção - Este bit em 1 limpa o flag de interrupção
  	#define I2C0_STAC				(1 << 5) 					// Este bit em 1 encerra o start. bit 0 não tem efeito
  	#define I2C0_I2ENC				(1 << 6) 					// Este bit em 1 desabilita bus I2C. bit 0 não tem efeito

// ########################################################################################################################################
// SPI0 - SERIAL PERIPHERAL INTERFACE 0
#define SPI0_BASE_ADDR		0xE0020000

#define SPI0_SLAVE	0
#define SPI0_MASTER	1 
	
#define S0SPCR         (*(reg32 *)(SPI0_BASE_ADDR + 0x00))	// Control Register
	#define SPI0_CPHA	(1<<3)									// Este bit em 0 configura a captura o dado na primeira borda do clock, bit 1 captura na segunda borda
	#define SPI0_CPOL	(1<<4)									// Este bit configura o padrão do clock
	#define SPI0_MSTR	(1<<5)									// Este bit em 0 ajusta o ARM como escravo, bit 1 como mestre
	#define SPI0_LSBF	(1<<6)									// Este bit em 0 o primeiro bit a TX será o MSB, o bit 1 será LSB
	#define SPI0_SPIE 	(1<<7)									// Este bit em 0 desabilita a interrupção, bit 1 habilita
	#define SPI0_STANDARD 0

#define S0SPSR         (*(reg32 *)(SPI0_BASE_ADDR + 0x04))	// Status Register
	#define SPI0_ABRT 	(1<<3)									// Este bit em 1 indica que o escravo abortou a recepção do dado
	#define SPI0_MODF 	(1<<4)									// Este bit em 1 indica que ocorreu o modo fault. Este modo ocorre quando existe uma falha no módulo SPI
	#define SPI0_ROVR 	(1<<5)									// Este bit em 1 indica que o overrun, ou seja, o dado foi recebido de maneira incorreta
	#define SPI0_WCOL 	(1<<6)									// Este bit em 1 indica que ocorre uma colisão de dados, ou seja, recebeu um novo dados sem ler o anterior
	#define SPI0_SPIF 	(1<<7)									// Este bit em 1 indica que houve uma transferencia completa dos dados

#define S0SPDR         (*(reg32 *)(SPI0_BASE_ADDR + 0x08))	// Data Register
#define S0SPCCR        (*(reg32 *)(SPI0_BASE_ADDR + 0x0C))  // Clock Counter Register
#define S0SPINT        (*(reg32 *)(SPI0_BASE_ADDR + 0x1C))	// Interrupt Flag Register

// ########################################################################################################################################
// RTC - REAL TIME CLOCK RTC
#define RTC_BASE_ADDR		0xE0024000

#define ILR         	(*(reg32 *)(RTC_BASE_ADDR + 0x00))      // Interrupt Location Register
	#define RTCCIF 		(1 << 0) 									// Quando 1 o bloco do timer RTC gera um interrupção. Escrevendo 1 limpa interrupção
	#define RTCALF		(1 << 1)									// Quando 1 o bloco do alarme gera um interrupção. Escrevendo 1 limpa interrupção

#define CTC         	(*(reg32 *)(RTC_BASE_ADDR + 0x04))      // Clock Tick Counter

// CONTROLE DO RTC
#define CCR         	(*(reg32 *)(RTC_BASE_ADDR + 0x08))      // Clock Control Register
	#define CLKEN		(1 << 0)									// habilita Clock
	#define CTCRST		(1 << 1)									// Reset 
	#define CTTEST		(3 << 2)									// Deve manter 00 na operação normal
	#define CLKSRC		(1 << 4)									// Quando 1 indica que o clock vem de fonte externa. Cristal 32Khz nos pinos RTCX1/2

#define CIIR        	(*(reg32 *)(RTC_BASE_ADDR + 0x0C))      // Counter Increment Interrupt Register
	#define IMNO		(0)			// Nenhum parametro será usado
  	#define IMSEC		(1 << 0)	// Este bit em 1, quando ocorrer o incremento no valor do segundo, gera interrupção
  	#define IMMIN		(1 << 1)	// Este bit em 1, quando ocorrer o incremento no valor do minuto, gera interrupção
  	#define IMHOUR		(1 << 2)	// Este bit em 1, quando ocorrer o incremento no valor do hora, gera interrupção
  	#define IMDOM		(1 << 3)	// Este bit em 1, quando ocorrer o incremento no valor do dia dos mês, gera interrupção
  	#define IMDOW		(1 << 4)	// Este bit em 1, quando ocorrer o incremento no valor do dia da semana, gera interrupção
  	#define IMDOY		(1 << 5)	// Este bit em 1, quando ocorrer o incremento no valor do do ano, gera interrupção
  	#define IMMON		(1 << 6)	// Este bit em 1, quando ocorrer o incremento no valor do mês, gera interrupção
  	#define IMYEAR		(1 << 7)	// Este bit em 1, quando ocorrer o incremento no valor do ano, gera interrupção

// REGISTRADOR PARA ATIVAR OS PARAMETROS DE CONTROLE DO ALARME
#define AMR         	(*(reg32 *)(RTC_BASE_ADDR + 0x10))      // Alarm Mask Register
	#define AMRNO		(0)											// Nenhum parametro será usado
	#define AMRSEC		(1 << 0)									// Este bit em 1, o valor do segundo não é usado para gerar o alarme
	#define AMRMIN		(1 << 1)									// Este bit em 1, o valor do minuto não é usado para gerar o alarme
	#define AMRHOUR		(1 << 2)									// Este bit em 1, o valor do hora não é usado para gerar o alarme
	#define AMRDOM		(1 << 3)									// Este bit em 1, o valor do dia dos mês não é usado para gerar o alarme
	#define AMRDOW		(1 << 4)									// Este bit em 1, o valor do dia da semana não é usado para gerar o alarme
	#define AMRDOY		(1 << 5)									// Este bit em 1, o valor do do ano não é usado para gerar o alarme
	#define AMRMON		(1 << 6)									// Este bit em 1, o valor do mês não é usado para gerar o alarme
	#define AMRYEAR		(1 << 7)									// Este bit em 1, o valor do ano não é usado para gerar o alarme
	#define AMRMASK		(0xFF)

// REGISTRADORES DE CONTAGEM DE TEMPO
#define CTIME0    	  	(*(reg32 *)(RTC_BASE_ADDR + 0x14))     	// Consolidated Time Register 0
#define CTIME1    	  	(*(reg32 *)(RTC_BASE_ADDR + 0x18))     	// Consolidated Time Register 1
#define CTIME2    	  	(*(reg32 *)(RTC_BASE_ADDR + 0x1C))     	// Consolidated Time Register 2

// REGISTRADORES DO TEMPO ATUAL
#define SEC         	(*(reg32 *)(RTC_BASE_ADDR + 0x20))      // Seconds Register
#define MIN         	(*(reg32 *)(RTC_BASE_ADDR + 0x24))      // Minutes Register
#define HOUR        	(*(reg32 *)(RTC_BASE_ADDR + 0x28))      // Hours Register
#define DOM         	(*(reg32 *)(RTC_BASE_ADDR + 0x2C))      // Day Of Month Register
#define DOW         	(*(reg32 *)(RTC_BASE_ADDR + 0x30))      // Day Of Week Register
#define DOY         	(*(reg32 *)(RTC_BASE_ADDR + 0x34))      // Day Of Year Register
#define MONTH  	    	(*(reg32 *)(RTC_BASE_ADDR + 0x38))      // Months Register
#define YEAR   	    	(*(reg32 *)(RTC_BASE_ADDR + 0x3C))      // Years Register

// REGISTRADORES PARA O ALARME
#define ALSEC       	(*(reg32 *)(RTC_BASE_ADDR + 0x60))      // Alarm Seconds Register
#define ALMIN       	(*(reg32 *)(RTC_BASE_ADDR + 0x64))      // Alarm Minutes Register
#define ALHOUR      	(*(reg32 *)(RTC_BASE_ADDR + 0x68))      // Alarm Hours Register
#define ALDOM       	(*(reg32 *)(RTC_BASE_ADDR + 0x6C))      // Alarm Day Of Month Register
#define ALDOW       	(*(reg32 *)(RTC_BASE_ADDR + 0x70))      // Alarm Day Of Week Register
#define ALDOY       	(*(reg32 *)(RTC_BASE_ADDR + 0x74))      // Alarm Day Of Year Register
#define ALMON       	(*(reg32 *)(RTC_BASE_ADDR + 0x78))      // Alarm Months Register
#define ALYEAR   	   	(*(reg32 *)(RTC_BASE_ADDR + 0x7C))      // Alarm Years Register

// REGISTRADORES PARA CONTROLE DO RTC ATRAVÉS DO CLOCK DA CPU
#define PREINT      	(*(reg32 *)(RTC_BASE_ADDR + 0x80))    	// Prescale Value Register (integer)
#define PREFRAC     	(*(reg32 *)(RTC_BASE_ADDR + 0x84))    	// Prescale Value Register (fraction)

// ########################################################################################################################################
// REGISTRADORES DE ENTRADA / SAIDA DE USO GERAL (GPIO - GENERAL PURPOSE INPUT/OUTPUT)
#define GPIO_BASE_ADDR		0xE0028000

#define IO0PIN         (*(reg32 *)(GPIO_BASE_ADDR + 0x00))	// P0 - Registrador de leitura da porta
#define IO0SET         (*(reg32 *)(GPIO_BASE_ADDR + 0x04))	// P0 - Registrador para acionar os pinos da porta. 1 = Aciona, 0 = Sem efeito 
#define IO0DIR         (*(reg32 *)(GPIO_BASE_ADDR + 0x08))	// P0 - Registrador para determinar o sentido dos pinos da porta. 1 = Saida  0 = Entrada
#define IO0CLR         (*(reg32 *)(GPIO_BASE_ADDR + 0x0C))	// P0 - Registrador para desligar os pinos da porta. 1 = Desliga, 0 = Sem efeito

// ########################################################################################################################################
// BLOCOS DE CONEXÃO DE PINOS IO
#define PINSEL_BASE_ADDR	0xE002C000

#define PINSEL0        (*(reg32 *)(PINSEL_BASE_ADDR + 0x00))
#define PINSEL1        (*(reg32 *)(PINSEL_BASE_ADDR + 0x04))
#define PINSEL2        (*(reg32 *)(PINSEL_BASE_ADDR + 0x14))

// ########################################################################################################################################
// BLOCO DE CONTROLE DO SISTEMA (SCB - System Control Block)
#define SCB_BASE_ADDR	0xE01FC000

// REGISTRADORES DE ACELERAÇÃO DE MEMÓRIA (MAM - Memory Accelerator Module)
#define MAMCR          (*(reg32 *)(SCB_BASE_ADDR + 0x000))		// Registrador de controle da MAM
	#define MAMCR_OFF     0											// MAM Desligada
	#define MAMCR_PART    1											// MAM funcinando parcialmente
	#define MAMCR_FULL    2											// MAM funcinando completamente

#define MAMTIM         (*(reg32 *)(SCB_BASE_ADDR + 0x004))		// Registrador de controle de tempo de acesso ao MAM referente a velocidade da CPU
	#define MAMTIM_1_CYCLES	1										// Acessa MAM com 1 clock  da CPU
	#define MAMTIM_2_CYCLES	2										// Acessa MAM com 2 clocks da CPU
	#define MAMTIM_3_CYCLES	3										// Acessa MAM com 3 clocks da CPU
	#define MAMTIM_4_CYCLES	4										// Acessa MAM com 4 clocks da CPU
	#define MAMTIM_5_CYCLES	5										// Acessa MAM com 5 clocks da CPU
	#define MAMTIM_6_CYCLES	6										// Acessa MAM com 6 clocks da CPU
	#define MAMTIM_7_CYCLES	7										// Acessa MAM com 7 clocks da CPU
	#define MAMTIM_AUTO_CYCLES 8// (((CCLK) + 19999999) / 20000000)		// Ajusta automaticamente a velocidade de acesso com a frequencia da CPU

// REGISTRADOR DE MAPEAMENTO DOS VETORES DE INTERRUPÇÕES
#define MEMMAP         (*(reg32 *)(SCB_BASE_ADDR + 0x040))		// Registrador de mapeamento dos vetores de interrupções
	#define MEMMAP_BBLK   	0                 						// Vetor de interrupção no bloco de boot (Boot Block)
	#define MEMMAP_FLASH  	1                 						// Vetor de interrupção na Flash
	#define MEMMAP_SRAM   	2                 						// Vetor de interrupção na SRAM
	#define MEMMAP_EXTERNAL 3                 						// Vetor de interrupção externamente
	
// REGISTRADORES PARA TRAVA DE PLL
#define PLLCON         (*(reg32 *)(SCB_BASE_ADDR + 0x080))		// Registrador de controle do PLL
	#define PLLCON_PLLE   (1 << 0)          						// Habilita PLL
	#define PLLCON_PLLC   (1 << 1)          						// Conecta o PLL a CPU

#define PLLCFG         (*(reg32 *)(SCB_BASE_ADDR + 0x084))		// Registrador de configuração do PLL

#define PLLSTAT        (*(reg32 *)(SCB_BASE_ADDR + 0x088))		// Registrador de Status
	#define PLLSTAT_PLLE  (1 << 8)									// Bit indicador se o PLL está ou não ativado
	#define PLLSTAT_PLLC  (1 << 9)									// Bit indicador se o PLL está ou não conectado
	#define PLLSTAT_LOCK  (1 << 10)         						// Bit de status de travamento do PLL
	
#define PLLFEED        (*(reg32 *)(SCB_BASE_ADDR + 0x08C))		// Registrador de validação de mudança do PLL

// REGISTRADOR DE CONTROLE DE CONSUMO (POWER CONTROL)
#define PCON           (*(reg32 *)(SCB_BASE_ADDR + 0x0C0))		// Registrador de controle de potencia
#define PCONP          (*(reg32 *)(SCB_BASE_ADDR + 0x0C4))		// Registrador de controle de potencia dos perifericos
	#define PCTIM0			(1 << 1)	// Timer/Counter 0 power/clock control bit. Reset = 1
	#define PCTIM1		 	(1 << 2)	// Timer/Counter 1 power/clock control bit. Reset = 1
	#define PCUART0		 	(1 << 3)	// UART0 power/clock control bit. Reset = 1
	#define PCUART1 		(1 << 4)	// UART1 power/clock control bit. Reset = 1
	#define PCPWM0 			(1 << 5)	// PWM0 power/clock control bit. Reset = 1
	#define PCI2C0 		 	(1 << 7)	// The I2C0 interface power/clock control bit. Reset = 1
	#define PCSPI 		 	(1 << 8)	// The SPI interface power/clock control bit. Reset = 1
	#define PCRTC 			(1 << 9)	// The RTC power/clock control bit. Reset = 1
	#define PCSSP1 			(1 << 10)	// The SSP1 interface power/clock control bit. Reset = 1
	#define PCEMC 		 	(1 << 11)	// External Memory Controller Reset = 1
	#define PCAD 		 	(1 << 12)	// A/D converter (ADC) power/clock control bit. Reset = 0
	#define PCCAN1 		 	(1 << 13)	// CAN Controller 1 power/clock control bit. Reset = 0
	#define PCCAN2 			(1 << 14)	// CAN Controller 2 power/clock control bit. Reset = 0

// REGISTRADOR DIVISÃO VPB
#define VPBDIV          (*(reg32 *)(SCB_BASE_ADDR + 0x100))		// Registrador da taxa de clock do barramento dos dispositivos internos do ARM em relaççao ao clock da CPU
	#define	PBSD_1_4_CCLK	4										// Pclk roda a 1/4 da velocidade do clock da CPU (Cclk)
	#define	PBSD_1_2_CCLK	2										// Pclk roda a 1/2 da velocidade do clock da CPU (Cclk)
	#define	PBSD_1_1_CCLK	1										// Pclk roda a mesma velocidade do clock da CPU (Cclk)
	
	#define VPBDIV_VALUE  (PBSD & 0x03)     					// VPBDIV value

// VALORES PADRÕES DE SEGURANÇA DE ERROS DE CONFIGURAÇÃO
#define F_CPU_MIN 			10000000 	// 10MHz
#define F_CPU_MAX 			25000000	// 25MHz
#define CCLK_MIN			10000000	// 10MHz
#define CCLK_MAX			60000000	// 60MHz
#define FCCO_MIN			150000000	// 150MHz
#define FCCO_MAX			320000000	// 320MHz
#define PLL_M_MIN		1
#define PLL_M_MAX		32


// ########################################################################################################################################
// EINT - REGISTRADORES DE INTERRUPÇÕES EXTERNAS

#define EINT0					0x1			// Interrupção externa 0
#define EINT1					0x2			// Interrupção externa 1
#define EINT2					0x4			// Interrupção externa 2
#define EINT3					0x8			// Interrupção externa 3

#define EXTINT         (*(reg32 *)(SCB_BASE_ADDR + 0x140))
   	// 	X		X		X		X 	EINT3 	EINT2 	EINT1 	EINT0
   		// Escrever 1 limpa o flag de interrupção

#define INTWAKE        (*(reg32 *)(SCB_BASE_ADDR + 0x144))
#define EXTMODE        (*(reg32 *)(SCB_BASE_ADDR + 0x148))
   	// 	X		X		X		X 	EINT3 	EINT2 	EINT1 	EINT0
	// EXTMODE - Modo de trigger de interrupção
	#define EXTMODE_LEVEL 		0 			// Sensivel ao nivel do sinal
	#define EXTMODE_EDGE 		1 			// Sensivel a borda do sinal

#define EXTPOLAR       (*(reg32 *)(SCB_BASE_ADDR + 0x14C))
    // 	X		X		X		X 	EINT3 	EINT2 	EINT1 	EINT0
	// EXTPOLAR - Polaridade da sensibilidade da interrupção
	#define EXTPOLAR_LOW 		0			// Sensivel ao nivel baixo ou na queda do sinal
	#define EXTPOLAR_HIGH 		1			// Sensivel ao nivel alto ou na subida do sinal

// ########################################################################################################################################
// VECTORED INTERRUPT CONTROLLER
#define VIC_ENABLE      (1 << 5) // Bit para habilitar int VIC em VICVectPrioX

// ANEXOS PARA CANAIS VIC
#define VIC_WDT         0 	// Watchdog
#define VIC_SW          1  	// Software interrupts
#define VIC_DEBUGRX     2  	// Embedded ICE, DbgCommRx
#define VIC_DEBUGTX     3  	// Embedded ICE, DbgCommTx
#define VIC_TIMER0      4 	// Timer 0 (Match 0-3 Capture 0-3)
#define VIC_TIMER1      5	// Timer 1 (Match 0-3 Capture 0-3)
#define VIC_UART0       6	// UART 0  (RLS, THRE, RDA, CTI)
#define VIC_UART1       7	// UART 1  (RLS, THRE, RDA, CTI, MSI
#define VIC_PWM         8	// PWM 0
#define VIC_PWM0        8	// PWM 0   (Match 0-6 Capture 0-3
#define VIC_I2C         9	// I2C 0    (SI)
#define VIC_I2C0        9	// I2C 0    (SI)
#define VIC_SPI         10	// SPI 0
#define VIC_SPI0        10	// SPI 0    	(SPIF, MODF)
#define VIC_SSP0		10
#define VIC_SPI1        11	// SPI 1 ou SSP (SPIF, MODF)
#define VIC_PLL         12	// PLL lock (PLOCK)
#define VIC_RTC         13	// RTC     (RTCCIF, RTCALF)
#define VIC_EINT0       14	// External interrupt 0 (EINT0)
#define VIC_EINT1       15	// External interrupt 1 (EINT1)
#define VIC_EINT2       16	// External interrupt 2 (EINT2)
#define VIC_EINT3       17	// External interrupt 3 (EINT3)
#define VIC_ADC         18	// A/D converter
#define VIC_ADC0        18	// A/D converter
#define VIC_I2C1		19	// I2C 1
#define VIC_BOD			20

#define VIC_SIZE		16
#define VIC_BASE_ADDR	0xFFFFF000
#define VIC_VECT_ADDR_INDEX	0x100
#define VIC_VECT_PRIO_INDEX 0x200

// REGISTRADORES DE CONTROLE DE INTERRUPÇÃO COM VETOR
#define VICIRQStatus   (*(reg32 *)(VIC_BASE_ADDR + 0x000))		// IRQ Status Register
#define VICFIQStatus   (*(reg32 *)(VIC_BASE_ADDR + 0x004))		// FIQ Status Register
#define VICRawIntr     (*(reg32 *)(VIC_BASE_ADDR + 0x008))		// Raw Interrupt Status Register
#define VICIntSelect   (*(reg32 *)(VIC_BASE_ADDR + 0x00C))		// Registrador para seleção de interupção. 0 = IRQ   1 = FIQ
#define VICIntEnable   (*(reg32 *)(VIC_BASE_ADDR + 0x010))		// Registrador para habilitar as interrupções
#define VICIntEnClr    (*(reg32 *)(VIC_BASE_ADDR + 0x014))		// Registrador para desativar as interrupções
#define VICSoftInt     (*(reg32 *)(VIC_BASE_ADDR + 0x018))		// Software Interrupt Register
#define VICSoftIntClr  (*(reg32 *)(VIC_BASE_ADDR + 0x01C))		// Software Interrupt Clear Register
#define VICProtection  (*(reg32 *)(VIC_BASE_ADDR + 0x020))		// Protection Enable Register

#define VICVectAddr    (*(reg32 *)(VIC_BASE_ADDR + 0x030))		// Vector Address Register
#define VICDefVectAddr (*(reg32 *)(VIC_BASE_ADDR + 0x034))		// Endereço padrão para o vetor de interrupção

#define VICVectAddr0   (*(reg32 *)(VIC_BASE_ADDR + 0x100))		// Registrador do endereço do IRQ 0
#define VICVectAddr1   (*(reg32 *)(VIC_BASE_ADDR + 0x104))		// Registrador do endereço do IRQ 1
#define VICVectAddr2   (*(reg32 *)(VIC_BASE_ADDR + 0x108))		// Registrador do endereço do IRQ 2
#define VICVectAddr3   (*(reg32 *)(VIC_BASE_ADDR + 0x10C))		// Registrador do endereço do IRQ 3
#define VICVectAddr4   (*(reg32 *)(VIC_BASE_ADDR + 0x110))		// Registrador do endereço do IRQ 4
#define VICVectAddr5   (*(reg32 *)(VIC_BASE_ADDR + 0x114))		// Registrador do endereço do IRQ 5
#define VICVectAddr6   (*(reg32 *)(VIC_BASE_ADDR + 0x118))		// Registrador do endereço do IRQ 6
#define VICVectAddr7   (*(reg32 *)(VIC_BASE_ADDR + 0x11C))		// Registrador do endereço do IRQ 7 
#define VICVectAddr8   (*(reg32 *)(VIC_BASE_ADDR + 0x120))		// Registrador do endereço do IRQ 8
#define VICVectAddr9   (*(reg32 *)(VIC_BASE_ADDR + 0x124))		// Registrador do endereço do IRQ 9
#define VICVectAddr10  (*(reg32 *)(VIC_BASE_ADDR + 0x128))		// Registrador do endereço do IRQ 10
#define VICVectAddr11  (*(reg32 *)(VIC_BASE_ADDR + 0x12C))		// Registrador do endereço do IRQ 11
#define VICVectAddr12  (*(reg32 *)(VIC_BASE_ADDR + 0x130))		// Registrador do endereço do IRQ 12
#define VICVectAddr13  (*(reg32 *)(VIC_BASE_ADDR + 0x134))		// Registrador do endereço do IRQ 13
#define VICVectAddr14  (*(reg32 *)(VIC_BASE_ADDR + 0x138))		// Registrador do endereço do IRQ 14
#define VICVectAddr15  (*(reg32 *)(VIC_BASE_ADDR + 0x13C))		// Registrador do endereço do IRQ 15

#define VICVectCntl0   (*(reg32 *)(VIC_BASE_ADDR + 0x200))		// Registrador de controle do IRQ 0
#define VICVectCntl1   (*(reg32 *)(VIC_BASE_ADDR + 0x204))		// Registrador de controle do IRQ 1
#define VICVectCntl2   (*(reg32 *)(VIC_BASE_ADDR + 0x208))		// Registrador de controle do IRQ 2
#define VICVectCntl3   (*(reg32 *)(VIC_BASE_ADDR + 0x20C))		// Registrador de controle do IRQ 3 
#define VICVectCntl4   (*(reg32 *)(VIC_BASE_ADDR + 0x210))		// Registrador de controle do IRQ 4
#define VICVectCntl5   (*(reg32 *)(VIC_BASE_ADDR + 0x214))		// Registrador de controle do IRQ 5
#define VICVectCntl6   (*(reg32 *)(VIC_BASE_ADDR + 0x218))		// Registrador de controle do IRQ 6
#define VICVectCntl7   (*(reg32 *)(VIC_BASE_ADDR + 0x21C))		// Registrador de controle do IRQ 7
#define VICVectCntl8   (*(reg32 *)(VIC_BASE_ADDR + 0x220))		// Registrador de controle do IRQ 8
#define VICVectCntl9   (*(reg32 *)(VIC_BASE_ADDR + 0x224))		// Registrador de controle do IRQ 9
#define VICVectCntl10  (*(reg32 *)(VIC_BASE_ADDR + 0x228))		// Registrador de controle do IRQ 10
#define VICVectCntl11  (*(reg32 *)(VIC_BASE_ADDR + 0x22C))		// Registrador de controle do IRQ 11
#define VICVectCntl12  (*(reg32 *)(VIC_BASE_ADDR + 0x230))		// Registrador de controle do IRQ 12
#define VICVectCntl13  (*(reg32 *)(VIC_BASE_ADDR + 0x234))		// Registrador de controle do IRQ 13
#define VICVectCntl14  (*(reg32 *)(VIC_BASE_ADDR + 0x238))		// Registrador de controle do IRQ 14
#define VICVectCntl15  (*(reg32 *)(VIC_BASE_ADDR + 0x23C))		// Registrador de controle do IRQ 15

#endif
