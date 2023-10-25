#ifndef __LPC24xx_H
#define __LPC24xx_H

#include "uc_libdefs.h"

// ########################################################################################################################################
// ENDEREÇOS BASES DAS SRAMs E SDRAMs

#define SRAM_BANK0_BASE_ADDR		0x80000000
#define SRAM_BANK1_BASE_ADDR		0x81000000
#define SRAM_BANK2_BASE_ADDR		0x82000000
#define SRAM_BANK3_BASE_ADDR		0x83000000

#define SDRAM_BANK0_BASE_ADDR		0xa0000000
#define SDRAM_BANK1_BASE_ADDR		0xc0000000
#define SDRAM_BANK2_BASE_ADDR		0xd0000000
#define SDRAM_BANK3_BASE_ADDR		0xe0000000


// ########################################################################################################################################
// WATCHDOG
#define WDG_BASE_ADDR		0xE0000000

#define WDMOD          (*(reg32 *)(WDG_BASE_ADDR + 0x00))       // Watchdog Mode Register
#define WDTC           (*(reg32 *)(WDG_BASE_ADDR + 0x04))       // Watchdog Time Constant Register
#define WDFEED         (*(reg32 *)(WDG_BASE_ADDR + 0x08))       // Watchdog Feed Register
#define WDTV           (*(reg32 *)(WDG_BASE_ADDR + 0x0C))      	// Watchdog Time Value Register
#define WDCLKSEL       (*(reg32 *)(WDG_BASE_ADDR + 0x10))

// ########################################################################################################################################
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
	#define T0MCR_MR0_RESET  			(1 << 1)  		      	// Reseta TC quando ele for igual a MR0
	#define T0MCR_MR0_STOP  			(1 << 2)       		 	// Para TC e PC quando MR0 for igual a TC
	#define T0MCR_MR1_INT_ENABLE  		(1 << 3)		     	// Habilita a interrupção quando MR1 for igual a TC
	#define T0MCR_MR1_RESET 			(1 << 4)       		 	// Reseta TC quando ele for igual a MR1
	#define T0MCR_MR1_STOP  			(1 << 5)        		// Para TC e PC quando MR1 for igual a TC
	#define T0MCR_MR2_INT_ENABLE   		(1 << 6)       		 	// Habilita a interrupção quando MR2 for igual a TC
	#define T0MCR_MR2_RESET  			(1 << 7)        		// Reseta TC quando ele for igual a MR2
	#define T0MCR_MR2_STOP  			(1 << 8)        		// Para TC e PC quando MR2 for igual a TC
	#define T0MCR_MR3_INT_ENABLE   		(1 << 9)        		// Habilita a interrupção quando MR3 for igual a TC
	#define T0MCR_MR3_RESET  			(1 << 10)       		// Reseta TC quando ele for igual a MR3
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
	#define T0EMR_EMC0_MASK			0x30
	#define T0EMR_EMC0_DONOTHING	0x0 	// Faz nada quando chegar na comparação
	#define T0EMR_EMC0_CLRPIN		0x10 	// Clear corresponding External Match output to 0 (LOW if pinned out)
	#define T0EMR_EMC0_SETPIN		0x20 	// Set corresponding External Match output to 1(HIGH if pinned out)
	#define T0EMR_EMC0_TOGGLEPIN	0x30 	// Toggle corresponding External Match output

	#define T0EMR_EMC1_MASK			0xC0
	#define T0EMR_EMC1_DONOTHING	0x0 	// Faz nada quando chegar na comparação
	#define T0EMR_EMC1_CLRPIN		0x40 	// Clear corresponding External Match output to 0 (LOW if pinned out)
	#define T0EMR_EMC1_SETPIN		0x80 	// Set corresponding External Match output to 1(HIGH if pinned out)
	#define T0EMR_EMC1_TOGGLEPIN	0xC0 	// Toggle corresponding External Match output
#define T0CTCR         (*(reg32 *)(TMR0_BASE_ADDR + 0x70))

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
	#define T1EMR_EMC0_MASK			0x30
	#define T1EMR_EMC0_DONOTHING	0x0 	// Faz nada quando chegar na comparação
	#define T1EMR_EMC0_CLRPIN		0x10 	// Clear corresponding External Match output to 0 (LOW if pinned out)
	#define T1EMR_EMC0_SETPIN		0x20 	// Set corresponding External Match output to 1(HIGH if pinned out)
	#define T1EMR_EMC0_TOGGLEPIN	0x30 	// Toggle corresponding External Match output

	#define T1EMR_EMC1_MASK			0xC0
	#define T1EMR_EMC1_DONOTHING	0x0 	// Faz nada quando chegar na comparação
	#define T1EMR_EMC1_CLRPIN		0x40 	// Clear corresponding External Match output to 0 (LOW if pinned out)
	#define T1EMR_EMC1_SETPIN		0x80 	// Set corresponding External Match output to 1(HIGH if pinned out)
	#define T1EMR_EMC1_TOGGLEPIN	0xC0 	// Toggle corresponding External Match output

#define T1CTCR         (*(reg32 *)(TMR1_BASE_ADDR + 0x70))

// ########################################################################################################################################
// REGISTRADORES DO TIMER 2
#define TMR2_BASE_ADDR		0xE0070000
#define T2IR           (*(reg32 *)(TMR2_BASE_ADDR + 0x00))	// Registrador de interrupções
	#define T2IR_MR0I    (1 << 0)        			    		// Flag de interrupção do canal de comparação 0
	#define T2IR_MR1I    (1 << 1)       			     		// Flag de interrupção do canal de comparação 1
	#define T2IR_MR2I    (1 << 2)       			     		// Flag de interrupção do canal de comparação 2
	#define T2IR_MR3I    (1 << 3)       			     		// Flag de interrupção do canal de comparação 3
	#define T2IR_CR0I    (1 << 4)        			    		// Flag de interrupção do canal de captura 0
	#define T2IR_CR1I    (1 << 5)         				   		// Flag de interrupção do canal de captura 1
	#define T2IR_CR2I    (1 << 6)        	 			   		// Flag de interrupção do canal de captura 2
	#define T2IR_CR3I    (1 << 7)        			    		// Flag de interrupção do canal de captura 3

#define T2TCR          (*(reg32 *)(TMR2_BASE_ADDR + 0x04))	// Registrador de controle de timer
	#define T2TCR_ENABLE  (1 << 0)								// Habilita o contador do timer
	#define T2TCR_STOP    (0)									// Desabilita o contador do timer
	#define T2TCR_RESET   (1 << 1)								// Zera o contador do timer
	
#define T2TC           (*(reg32 *)(TMR2_BASE_ADDR + 0x08))	// Contador do timer
#define T2PR           (*(reg32 *)(TMR2_BASE_ADDR + 0x0C))	// Registrador do prescaler (0 a 0xFFFFFFFF)
	#define	T2MPR_DIV1		0
	#define	T2MPR_DIV2		1
	#define	T2MPR_DIV4		3
	#define	T2MPR_DIV8		7
	#define	T2MPR_DIV16		15
	#define	T2MPR_DIV32		31
	#define	T2MPR_DIV64		63
	#define	T2MPR_DIV128	127
	#define	T2MPR_DIV256	255
	#define	T2MPR_DIV512	511
	#define	T2MPR_DIV1024	1023

#define T2PC           (*(reg32 *)(TMR2_BASE_ADDR + 0x10))	// Contador do prescaler
#define T2MCR          (*(reg32 *)(TMR2_BASE_ADDR + 0x14))	// Registrador do controle de comparações (Match Control)
	#define T2MCR_NO_INT  				0						// Sem interrupções para match ou PWM
	#define T2MCR_MR0_INT_ENABLE  		(1 << 0)        		// Habilita a interrupção quando MR0 for igual a TC
	#define T2MCR_MR0_RESET  			(1 << 1)  		      	// Reseta MR0 quando ele for igual a TC
	#define T2MCR_MR0_STOP  			(1 << 2)       		 	// Para TC e PC quando MR0 for igual a TC
	#define T2MCR_MR1_INT_ENABLE  		(1 << 3)		     	// Habilita a interrupção quando MR1 for igual a TC
	#define T2MCR_MR1_RESET 			(1 << 4)       		 	// Reseta MR1 quando ele for igual a TC
	#define T2MCR_MR1_STOP  			(1 << 5)        		// Para TC e PC quando MR1 for igual a TC
	#define T2MCR_MR2_INT_ENABLE   		(1 << 6)       		 	// Habilita a interrupção quando MR2 for igual a TC
	#define T2MCR_MR2_RESET  			(1 << 7)        		// Reseta MR2 quando ele for igual a TC
	#define T2MCR_MR2_STOP  			(1 << 8)        		// Para TC e PC quando MR2 for igual a TC
	#define T2MCR_MR3_INT_ENABLE   		(1 << 9)        		// Habilita a interrupção quando MR3 for igual a TC
	#define T2MCR_MR3_RESET  			(1 << 10)       		// Reseta MR3 quando ele for igual a TC
	#define T2MCR_MR3_STOP  			(1 << 11)       		// Para TC e PC quando MR3 for igual a TC
  	
	#define T2MCR_PWM_MR0_INT_ENABLE 	(1 << 0)        		// Habilita a interrupção quando PWMMR0 for igual a PWMTC
	#define T2MCR_PWM_MR0_RESET  		(1 << 1)        		// Reseta PWMMR0 quando ele for igual a PWMTC
	#define T2MCR_PWM_MR0_STOP  		(1 << 2)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T2MCR_PWM_MR1_INT_ENABLE 	(1 << 3)        		// Habilita a interrupção quando PWMMR1 for igual a PWMTC
	#define T2MCR_PWM_MR1_RESET  		(1 << 4)        		// Reseta PWMMR1 quando ele for igual a PWMTC
	#define T2MCR_PWM_MR1_STOP  		(1 << 5)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T2MCR_PWM_MR2_INT_ENABLE 	(1 << 6)        		// Habilita a interrupção quando PWMMR2 for igual a PWMTC
	#define T2MCR_PWM_MR2_RESET  		(1 << 7)        		// Reseta PWMMR2 quando ele for igual a PWMTC
	#define T2MCR_PWM_MR2_STOP  		(1 << 8)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T2MCR_PWM_MR3_INT_ENABLE 	(1 << 9)        		// Habilita a interrupção quando PWMMR3 for igual a PWMTC
	#define T2MCR_PWM_MR3_RESET  		(1 << 10)       		// Reseta PWMMR3 quando ele for igual a PWMTC
	#define T2MCR_PWM_MR3_STOP  		(1 << 11)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T2MCR_PWM_MR4_INT_ENABLE 	(1 << 12)       		// Habilita a interrupção quando PWMMR4 for igual a PWMTC
	#define T2MCR_PWM_MR4_RESET  		(1 << 13)       		// Reseta PWMMR4 quando ele for igual a PWMTC
	#define T2MCR_PWM_MR4_STOP  		(1 << 14)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T2MCR_PWM_MR5_INT_ENABLE 	(1 << 15)       		// Habilita a interrupção quando PWMMR5 for igual a PWMTC
	#define T2MCR_PWM_MR5_RESET  		(1 << 16)       		// Reseta PWMMR5 quando ele for igual a PWMTC
	#define T2MCR_PWM_MR5_STOP  		(1 << 17)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T2MCR_PWM_MR6_INT_ENABLE 	(1 << 18)       		// Habilita a interrupção quando PWMMR6 for igual a PWMTC
	#define T2MCR_PWM_MR6_RESET  		(1 << 19)      			// Reseta PWMMR6 quando ele for igual a PWMTC
	#define T2MCR_PWM_MR6_STOP  		(1 << 20)       		// Para PWMTC e PWMPC quando PWMMR6 for igual a TC

#define T2MR0          (*(reg32 *)(TMR2_BASE_ADDR + 0x18)))	// Registrador de comparação 0
#define T2MR1          (*(reg32 *)(TMR2_BASE_ADDR + 0x1C)))	// Registrador de comparação 1
#define T2MR2          (*(reg32 *)(TMR2_BASE_ADDR + 0x20)))	// Registrador de comparação 2
#define T2MR3          (*(reg32 *)(TMR2_BASE_ADDR + 0x24)))	// Registrador de comparação 3
#define T2CCR          (*(reg32 *)(TMR2_BASE_ADDR + 0x28))	// Registrador de controle de captura
	#define T2CCR_CR0_RISING	(1 << 0)     	  				// Habilita a captura do TC para CR0 durante a subida da borda CAPn.0
	#define T2CCR_CR0_FALLING	(1 << 1)    	    			// Habilita a captura do TC para CR0 durante a queda da borda CAPn.0
	#define T2CCR_CR0_INT 		(1 << 2)        				// Habilita a interrupção para ler o CR0
	#define T2CCR_CR1_RISING	(1 << 3)        				// Habilita a captura do TC para CR1 durante a subida da borda CAPn.1
	#define T2CCR_CR1_FALLING 	(1 << 4)        				// Habilita a captura do TC para CR1 durante a queda da borda CAPn.1
	#define T2CCR_CR1_INT 		(1 << 5)        				// Habilita a interrupção para ler o CR1
	#define T2CCR_CR2_RISING 	(1 << 6)        				// Habilita a captura do TC para CR2 durante a subida da borda CAPn.2
	#define T2CCR_CR2_FALLING 	(1 << 7)        				// Habilita a captura do TC para CR2 durante a queda da borda CAPn.2
	#define T2CCR_CR2_INT 		(1 << 8)        				// Habilita a interrupção para ler o CR2
	#define T2CCR_CR3_RISING 	(1 << 9)  		   				// Habilita a captura do TC para CR3 durante a subida da borda CAPn.3
	#define T2CCR_CR3_FALLING 	(1 << 10)       				// Habilita a captura do TC para CR3 durante a queda da borda CAPn.3
	#define T2CCR_CR3_INT 		(1 << 11)       				// Habilita a interrupção para ler o CR3

#define T2CR0          (*(reg32 *)(TMR2_BASE_ADDR + 0x2C))	// Registrador de captura 0
#define T2CR1          (*(reg32 *)(TMR2_BASE_ADDR + 0x30))	// Registrador de captura 1
#define T2CR2          (*(reg32 *)(TMR2_BASE_ADDR + 0x34))	// Registrador de captura 2
#define T2CR3          (*(reg32 *)(TMR2_BASE_ADDR + 0x38))	// Registrador de captura 3
#define T2EMR          (*(reg32 *)(TMR2_BASE_ADDR + 0x3C))	// Registrador de comparação externa
#define T2CTCR         (*(reg32 *)(TMR2_BASE_ADDR + 0x70))

// ########################################################################################################################################
// REGISTRADORES DO TIMER 3
#define TMR3_BASE_ADDR		0xE0074000
#define T3IR           (*(reg32 *)(TMR3_BASE_ADDR + 0x00))	// Registrador de interrupções
	#define T3IR_MR0I    (1 << 0)        			    		// Flag de interrupção do canal de comparação 0
	#define T3IR_MR1I    (1 << 1)       			     		// Flag de interrupção do canal de comparação 1
	#define T3IR_MR2I    (1 << 2)       			     		// Flag de interrupção do canal de comparação 2
	#define T3IR_MR3I    (1 << 3)       			     		// Flag de interrupção do canal de comparação 3
	#define T3IR_CR0I    (1 << 4)        			    		// Flag de interrupção do canal de captura 0
	#define T3IR_CR1I    (1 << 5)         				   		// Flag de interrupção do canal de captura 1
	#define T3IR_CR2I    (1 << 6)        	 			   		// Flag de interrupção do canal de captura 2
	#define T3IR_CR3I    (1 << 7)        			    		// Flag de interrupção do canal de captura 3

#define T3TCR          (*(reg32 *)(TMR3_BASE_ADDR + 0x04))	// Registrador de controle de timer
	#define T3TCR_ENABLE  (1 << 0)								// Habilita o contador do timer
	#define T3TCR_STOP    (0)									// Desabilita o contador do timer
	#define T3TCR_RESET   (1 << 1)								// Zera o contador do timer

#define T3TC           (*(reg32 *)(TMR3_BASE_ADDR + 0x08))	// Contador do timer
#define T3PR           (*(reg32 *)(TMR3_BASE_ADDR + 0x0C))	// Registrador do prescaler (0 a 0xFFFFFFFF)
	#define	T3MPR_DIV1		0
	#define	T3MPR_DIV2		1
	#define	T3MPR_DIV4		3
	#define	T3MPR_DIV8		7
	#define	T3MPR_DIV16		15
	#define	T3MPR_DIV32		31
	#define	T3MPR_DIV64		63
	#define	T3MPR_DIV128	127
	#define	T3MPR_DIV256	255
	#define	T3MPR_DIV512	511
	#define	T3MPR_DIV1024	1023

#define T3PC           (*(reg32 *)(TMR3_BASE_ADDR + 0x10))	// Contador do prescaler
#define T3MCR          (*(reg32 *)(TMR3_BASE_ADDR + 0x14)))	// Registrador do controle de comparações (Match Control)
	#define T3MCR_NO_INT  				0						// Sem interrupções para match ou PWM
	#define T3MCR_MR0_INT_ENABLE  		(1 << 0)        		// Habilita a interrupção quando MR0 for igual a TC
	#define T3MCR_MR0_RESET  			(1 << 1)  		      	// Reseta MR0 quando ele for igual a TC
	#define T3MCR_MR0_STOP  			(1 << 2)       		 	// Para TC e PC quando MR0 for igual a TC
	#define T3MCR_MR1_INT_ENABLE  		(1 << 3)		     	// Habilita a interrupção quando MR1 for igual a TC
	#define T3MCR_MR1_RESET 			(1 << 4)       		 	// Reseta MR1 quando ele for igual a TC
	#define T3MCR_MR1_STOP  			(1 << 5)        		// Para TC e PC quando MR1 for igual a TC
	#define T3MCR_MR2_INT_ENABLE   		(1 << 6)       		 	// Habilita a interrupção quando MR2 for igual a TC
	#define T3MCR_MR2_RESET  			(1 << 7)        		// Reseta MR2 quando ele for igual a TC
	#define T3MCR_MR2_STOP  			(1 << 8)        		// Para TC e PC quando MR2 for igual a TC
	#define T3MCR_MR3_INT_ENABLE   		(1 << 9)        		// Habilita a interrupção quando MR3 for igual a TC
	#define T3MCR_MR3_RESET  			(1 << 10)       		// Reseta MR3 quando ele for igual a TC
	#define T3MCR_MR3_STOP  			(1 << 11)       		// Para TC e PC quando MR3 for igual a TC
  	
	#define T3MCR_PWM_MR0_INT_ENABLE 	(1 << 0)        		// Habilita a interrupção quando PWMMR0 for igual a PWMTC
	#define T3MCR_PWM_MR0_RESET  		(1 << 1)        		// Reseta PWMMR0 quando ele for igual a PWMTC
	#define T3MCR_PWM_MR0_STOP  		(1 << 2)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T3MCR_PWM_MR1_INT_ENABLE 	(1 << 3)        		// Habilita a interrupção quando PWMMR1 for igual a PWMTC
	#define T3MCR_PWM_MR1_RESET  		(1 << 4)        		// Reseta PWMMR1 quando ele for igual a PWMTC
	#define T3MCR_PWM_MR1_STOP  		(1 << 5)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T3MCR_PWM_MR2_INT_ENABLE 	(1 << 6)        		// Habilita a interrupção quando PWMMR2 for igual a PWMTC
	#define T3MCR_PWM_MR2_RESET  		(1 << 7)        		// Reseta PWMMR2 quando ele for igual a PWMTC
	#define T3MCR_PWM_MR2_STOP  		(1 << 8)        		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T3MCR_PWM_MR3_INT_ENABLE 	(1 << 9)        		// Habilita a interrupção quando PWMMR3 for igual a PWMTC
	#define T3MCR_PWM_MR3_RESET  		(1 << 10)       		// Reseta PWMMR3 quando ele for igual a PWMTC
	#define T3MCR_PWM_MR3_STOP  		(1 << 11)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T3MCR_PWM_MR4_INT_ENABLE 	(1 << 12)       		// Habilita a interrupção quando PWMMR4 for igual a PWMTC
	#define T3MCR_PWM_MR4_RESET  		(1 << 13)       		// Reseta PWMMR4 quando ele for igual a PWMTC
	#define T3MCR_PWM_MR4_STOP  		(1 << 14)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T3MCR_PWM_MR5_INT_ENABLE 	(1 << 15)       		// Habilita a interrupção quando PWMMR5 for igual a PWMTC
	#define T3MCR_PWM_MR5_RESET  		(1 << 16)       		// Reseta PWMMR5 quando ele for igual a PWMTC
	#define T3MCR_PWM_MR5_STOP  		(1 << 17)       		// Para PWMTC e PWMPC quando PWMMR0 for igual a TC
	#define T3MCR_PWM_MR6_INT_ENABLE 	(1 << 18)       		// Habilita a interrupção quando PWMMR6 for igual a PWMTC
	#define T3MCR_PWM_MR6_RESET  		(1 << 19)      			// Reseta PWMMR6 quando ele for igual a PWMTC
	#define T3MCR_PWM_MR6_STOP  		(1 << 20)       		// Para PWMTC e PWMPC quando PWMMR6 for igual a TC

#define T3MR0          (*(reg32 *)(TMR3_BASE_ADDR + 0x18)))	// Registrador de comparação 0
#define T3MR1          (*(reg32 *)(TMR3_BASE_ADDR + 0x1C)))	// Registrador de comparação 1
#define T3MR2          (*(reg32 *)(TMR3_BASE_ADDR + 0x20)))	// Registrador de comparação 2
#define T3MR3          (*(reg32 *)(TMR3_BASE_ADDR + 0x24))	// Registrador de comparação 3
#define T3CCR          (*(reg32 *)(TMR3_BASE_ADDR + 0x28))	// Registrador de controle de captura
	#define T3CCR_CR0_RISING	(1 << 0)     	  				// Habilita a captura do TC para CR0 durante a subida da borda CAPn.0
	#define T3CCR_CR0_FALLING	(1 << 1)    	    			// Habilita a captura do TC para CR0 durante a queda da borda CAPn.0
	#define T3CCR_CR0_INT 		(1 << 2)        				// Habilita a interrupção para ler o CR0
	#define T3CCR_CR1_RISING	(1 << 3)        				// Habilita a captura do TC para CR1 durante a subida da borda CAPn.1
	#define T3CCR_CR1_FALLING 	(1 << 4)        				// Habilita a captura do TC para CR1 durante a queda da borda CAPn.1
	#define T3CCR_CR1_INT 		(1 << 5)        				// Habilita a interrupção para ler o CR1
	#define T3CCR_CR2_RISING 	(1 << 6)        				// Habilita a captura do TC para CR2 durante a subida da borda CAPn.2
	#define T3CCR_CR2_FALLING 	(1 << 7)        				// Habilita a captura do TC para CR2 durante a queda da borda CAPn.2
	#define T3CCR_CR2_INT 		(1 << 8)        				// Habilita a interrupção para ler o CR2
	#define T3CCR_CR3_RISING 	(1 << 9)  		   				// Habilita a captura do TC para CR3 durante a subida da borda CAPn.3
	#define T3CCR_CR3_FALLING 	(1 << 10)       				// Habilita a captura do TC para CR3 durante a queda da borda CAPn.3
	#define T3CCR_CR3_INT 		(1 << 11)       				// Habilita a interrupção para ler o CR3

#define T3CR0          (*(reg32 *)(TMR3_BASE_ADDR + 0x2C))	// Registrador de captura 0
#define T3CR1          (*(reg32 *)(TMR3_BASE_ADDR + 0x30))	// Registrador de captura 1
#define T3CR2          (*(reg32 *)(TMR3_BASE_ADDR + 0x34))	// Registrador de captura 2
#define T3CR3          (*(reg32 *)(TMR3_BASE_ADDR + 0x38))	// Registrador de captura 3
#define T3EMR          (*(reg32 *)(TMR3_BASE_ADDR + 0x3C))	// Registrador de comparação externa
#define T3CTCR         (*(reg32 *)(TMR3_BASE_ADDR + 0x70))

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
#define PWM0CCR         (*(reg32 *)(PWM0_BASE_ADDR + 0x28))
#define PWM0CR0         (*(reg32 *)(PWM0_BASE_ADDR + 0x2C))
#define PWM0CR1         (*(reg32 *)(PWM0_BASE_ADDR + 0x30))
#define PWM0CR2         (*(reg32 *)(PWM0_BASE_ADDR + 0x34))
#define PWM0CR3         (*(reg32 *)(PWM0_BASE_ADDR + 0x38))
#define PWM0EMR         (*(reg32 *)(PWM0_BASE_ADDR + 0x3C))
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
#define PWM0CTCR        (*(reg32 *)(PWM0_BASE_ADDR + 0x70))

// ########################################################################################################################################
// PWM1 - PULSE WIDTH MODULATOR
#define PWM1_BASE_ADDR		0xE0018000

#define PWM1IR          (*(reg32 *)(PWM1_BASE_ADDR + 0x00))			//  Interrupt Register
	#define PW1MIR_MR0I  (1 << 0)            							// Flag de interrupção PWM0
	#define PW1MIR_MR1I  (1 << 1)         						 		// Flag de interrupção PWM1
	#define PW1MIR_MR2I  (1 << 2)    						       		// Flag de interrupção PWM2
	#define PW1MIR_MR3I  (1 << 3)            							// Flag de interrupção PWM3
	#define PW1MIR_MR4I  (1 << 8)            							// Flag de interrupção PWM4
	#define PW1MIR_MR5I  (1 << 9)            							// Flag de interrupção PWM5
	#define PW1MIR_MR6I  (1 << 10)           							// Flag de interrupção PWM6
	#define PW1MIR_MASK  (0x070F)

#define PWM1TCR         (*(reg32 *)(PWM1_BASE_ADDR + 0x04))			// Timer Control Register
#define PWM1TC          (*(reg32 *)(PWM1_BASE_ADDR + 0x08))         // Timer Counter
#define PWM1PR          (*(reg32 *)(PWM1_BASE_ADDR + 0x0C))         // Prescale Register
#define PWM1PC          (*(reg32 *)(PWM1_BASE_ADDR + 0x10))         // Prescale Counter Register
#define PWM1MCR         (*(reg32 *)(PWM1_BASE_ADDR + 0x14))        	// Match Control Register
	#define PWM1MCR_ENABLE 		(1 << 0)								// Habilita o contador do timer PWM
	#define PWM1MCR_RESET  		(1 << 1)								// Zera o contador do timer PWM
	#define PWM1MCR_PWM_ENABLE  (1 << 3)

#define PWM1MR0         (*(reg32 *)(PWM1_BASE_ADDR + 0x18))			// Match Register 0
#define PWM1MR1         (*(reg32 *)(PWM1_BASE_ADDR + 0x1C))			// Match Register 1
#define PWM1MR2         (*(reg32 *)(PWM1_BASE_ADDR + 0x20))			// Match Register 2
#define PWM1MR3         (*(reg32 *)(PWM1_BASE_ADDR + 0x24))			// Match Register 3
#define PWM1CCR         (*(reg32 *)(PWM1_BASE_ADDR + 0x28))
#define PWM1CR0         (*(reg32 *)(PWM1_BASE_ADDR + 0x2C))
#define PWM1CR1         (*(reg32 *)(PWM1_BASE_ADDR + 0x30))
#define PWM1CR2         (*(reg32 *)(PWM1_BASE_ADDR + 0x34))
#define PWM1CR3         (*(reg32 *)(PWM1_BASE_ADDR + 0x38))
#define PWM1EMR         (*(reg32 *)(PWM1_BASE_ADDR + 0x3C))
#define PWM1MR4         (*(reg32 *)(PWM1_BASE_ADDR + 0x40))			// Match Register 4
#define PWM1MR5         (*(reg32 *)(PWM1_BASE_ADDR + 0x44))			// Match Register 5
#define PWM1MR6         (*(reg32 *)(PWM1_BASE_ADDR + 0x48))			// Match Register 6
#define PWM1PCR         (*(reg32 *)(PWM1_BASE_ADDR + 0x4C))			// Control Register
  	#define PWM1PCR_PWM_SEL2	(1 << 2)								// Seleciona o controle de dupla borda para o PWM2. 0 = borda simples / 1 = Dupla borda
	#define PWM1PCR_PWM_SEL3	(1 << 3)								// Seleciona o controle de dupla borda para o PWM3. 0 = borda simples / 1 = Dupla borda
	#define PWM1PCR_PWM_SEL4	(1 << 4)								// Seleciona o controle de dupla borda para o PWM4. 0 = borda simples / 1 = Dupla borda
	#define PWM1PCR_PWM_SEL5	(1 << 5)								// Seleciona o controle de dupla borda para o PWM5. 0 = borda simples / 1 = Dupla borda
	#define PWM1PCR_PWM_SEL6	(1 << 6)								// Seleciona o controle de dupla borda para o PWM6. 0 = borda simples / 1 = Dupla borda
	#define PWM1PCR_PWM_ENABLE1	(1 << 9)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
	#define PWM1PCR_PWM_ENABLE2	(1 << 10)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
	#define PWM1PCR_PWM_ENABLE3	(1 << 11)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
	#define PWM1PCR_PWM_ENABLE4	(1 << 12)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
	#define PWM1PCR_PWM_ENABLE5	(1 << 13)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
	#define PWM1PCR_PWM_ENABLE6	(1 << 14)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita

#define PWM1LER         (*(reg32 *)(PWM1_BASE_ADDR + 0x50))			// Latch Enable Register
  	#define PWM1LER_DISABLE			0									// Desbilita todos os registradores latchs do PWM
	#define PWM1LER0_ENABLE			(1 << 0)							// Habilita o registrador latch do PWM0
	#define PWM1LER1_ENABLE			(1 << 1)							// Habilita o registrador latch do PWM1
	#define PWM1LER2_ENABLE			(1 << 2)							// Habilita o registrador latch do PWM2
	#define PWM1LER3_ENABLE			(1 << 3)							// Habilita o registrador latch do PWM3
	#define PWM1LER4_ENABLE			(1 << 4)							// Habilita o registrador latch do PWM4
	#define PWM1LER5_ENABLE			(1 << 5)							// Habilita o registrador latch do PWM5
	#define PWM1LER6_ENABLE			(1 << 6)							// Habilita o registrador latch do PWM6

#define PWM1CTCR        (*(reg32 *)(PWM1_BASE_ADDR + 0x70))


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


// TIPO DE FIFO DAS UARTS
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
	#define U0IIR_NO_INT         (1 << 0)   				 	// 1 = Sem pendencia de interrupção / 0 = Pelo menos uma pedencia de interrupção
	#define U0IIR_MS_INT         (0 << 1)    					// MODEM Status
	#define U0IIR_THRE_INT       (1 << 1)    					// Interrupção de THRE (Transmit Holding Register Empty)(
	#define U0IIR_RDA_INT        (2 << 1)    					// Dado disponivel na recpção
	#define U0IIR_RLS_INT        (3 << 1)    					// Receive Line Status
	#define U0IIR_CTI_INT        (6 << 1)    					// Indicador de estouro de tempo para o caractere (Character Timeout Indicator)
	#define U0IIR_ID_MASK        0x0E

#define U0FCR          (*(reg32 *)(UART0_BASE_ADDR + 0x08))	// Registrador de controle da FIFO
	#define U0FCR_FIFO_ENABLE    (1 << 0)    					// Habilita FIFO
	#define U0FCR_RX_FIFO_RESET  (1 << 1)    					// Reseta o FIFO de recepção
	#define U0FCR_TX_FIFO_RESET  (1 << 2)   			 		// Reseta o FIFO de transmissão
	#define U0FCR_FIFO_TRIG1     (0 << 6)    					// Trigger com 1 character in FIFO
	#define U0FCR_FIFO_TRIG4     (1 << 6)   			 		// Trigger com 4 characters in FIFO
	#define U0FCR_FIFO_TRIG8     (2 << 6)   				 	// Trigger com 8 characters in FIFO
	#define U0FCR_FIFO_TRIG14    (3 << 6)    					// Trigger com 14 characters in FIFO

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
#define U0ACR          (*(reg32 *)(UART0_BASE_ADDR + 0x20))
#define U0ICR          (*(reg32 *)(UART0_BASE_ADDR + 0x24))
#define U0FDR          (*(reg32 *)(UART0_BASE_ADDR + 0x28))
#define U0TER          (*(reg32 *)(UART0_BASE_ADDR + 0x30))

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
#define U1ACR          (*(reg32 *)(UART1_BASE_ADDR + 0x20))
#define U1FDR          (*(reg32 *)(UART1_BASE_ADDR + 0x28))
#define U1TER          (*(reg32 *)(UART1_BASE_ADDR + 0x30))

// ########################################################################################################################################
// UART2 - UNIVERSAL ASYNCHRONOUS RECEIVER TRANSMITTER

#define UART2_SIZE_FIFO	16

// Modos de trabalho das UARTs
#define UART2_8N1      (u8)(U2LCR_CHAR_8 + U2LCR_PAR_NO   + U2LCR_STOP_1)
#define UART2_7N1      (u8)(U2LCR_CHAR_7 + U2LCR_PAR_NO   + U2LCR_STOP_1)
#define UART2_8N2      (u8)(U2LCR_CHAR_8 + U2LCR_PAR_NO   + U2LCR_STOP_2)
#define UART2_7N2      (u8)(U2LCR_CHAR_7 + U2LCR_PAR_NO   + U2LCR_STOP_2)
#define UART2_8E1      (u8)(U2LCR_CHAR_8 + U2LCR_PAR_EVEN + U2LCR_STOP_1)
#define UART2_7E1      (u8)(U2LCR_CHAR_7 + U2LCR_PAR_EVEN + U2LCR_STOP_1)
#define UART2_8E2      (u8)(U2LCR_CHAR_8 + U2LCR_PAR_EVEN + U2LCR_STOP_2)
#define UART2_7E2      (u8)(U2LCR_CHAR_7 + U2LCR_PAR_EVEN + U2LCR_STOP_2)
#define UART2_8O1      (u8)(U2LCR_CHAR_8 + U2LCR_PAR_ODD  + U2LCR_STOP_1)
#define UART2_7O1      (u8)(U2LCR_CHAR_7 + U2LCR_PAR_ODD  + U2LCR_STOP_1)
#define UART2_8O2      (u8)(U2LCR_CHAR_8 + U2LCR_PAR_ODD  + U2LCR_STOP_2)
#define UART2_7O2      (u8)(U2LCR_CHAR_7 + U2LCR_PAR_ODD  + U2LCR_STOP_2)

// Tipo de FIFO das UARTs
#define UART2_FIFO_OFF (0x00)
#define UART2_FIFO_RX_TRIG1   (u8)(U2FCR_FIFO_ENABLE + U2FCR_FIFO_TRIG1)
#define UART2_FIFO_RX_TRIG4   (u8)(U2FCR_FIFO_ENABLE + U2FCR_FIFO_TRIG4)
#define UART2_FIFO_RX_TRIG8   (u8)(U2FCR_FIFO_ENABLE + U2FCR_FIFO_TRIG8)
#define UART2_FIFO_RX_TRIG14  (u8)(U2FCR_FIFO_ENABLE + U2FCR_FIFO_TRIG14)

#define UART2_BASE_ADDR		0xE0078000
#define U2RBR          (*(reg32 *)(UART2_BASE_ADDR + 0x00))	// Registrador de recepção de dados
#define U2THR          (*(reg32 *)(UART2_BASE_ADDR + 0x00))	// Registrador de transmissão de dados 
#define U2DLL          (*(reg32 *)(UART2_BASE_ADDR + 0x00))	// Registrador de ajuste de baudrate (LSB)
#define U2DLM          (*(reg32 *)(UART2_BASE_ADDR + 0x04))	// Registrador de ajuste de baudrate (MSB)
#define U2IER          (*(reg32 *)(UART2_BASE_ADDR + 0x04))	// Registrador de habilitação das interrupções
	#define U2IER_ERBFI          (1 << 0)    					// Habilita a int de dado pronto recebido
	#define U2IER_ETBEI          (1 << 1)    					// Habilita a int THRE (Transmit Holding Register Empty)
	#define U2IER_ELSI           (1 << 2)    					// Enable Receive Line Status Interrupt
	#define U2IER_EDSSI          (1 << 3)    					// Enable MODEM Status Interrupt

#define U2IIR          (*(reg32 *)(UART2_BASE_ADDR + 0x08))	// Registrador de ID de interrupção. Somente para leitura
	#define U2IIR_NO_INT         (1 << 0)   				 	// 1 = Sem pendencia de interrupção / 0 = Pelo menos uma pedencia de interrupção
	#define U2IIR_MS_INT         (0 << 1)    					// MODEM Status
	#define U2IIR_THRE_INT       (1 << 1)    					// Interrupção de THRE (Transmit Holding Register Empty)(
	#define U2IIR_RDA_INT        (2 << 1)    					// Dado disponivel na recpção
	#define U2IIR_RLS_INT        (3 << 1)    					// Receive Line Status
	#define U2IIR_CTI_INT        (6 << 1)    					// Indicador de estouro de tempo para o caractere (Character Timeout Indicator)
	#define U2IIR_ID_MASK        0x0E

#define U2FCR          (*(reg32 *)(UART2_BASE_ADDR + 0x08))		// Registrador de controle da FIFO
	#define U2FCR_FIFO_ENABLE    (1 << 0)    					// Habilita FIFO
	#define U2FCR_RX_FIFO_RESET  (1 << 1)    					// Reseta o FIFO de recepção
	#define U2FCR_TX_FIFO_RESET  (1 << 2)   			 		// Reseta o FIFO de transmissão
	#define U2FCR_FIFO_TRIG1     (0 << 6)    					// Trigger @ 1 character in FIFO
	#define U2FCR_FIFO_TRIG4     (1 << 6)   			 		// Trigger @ 4 characters in FIFO
	#define U2FCR_FIFO_TRIG8     (2 << 6)   				 	// Trigger @ 8 characters in FIFO
	#define U2FCR_FIFO_TRIG14    (3 << 6)    					// Trigger @ 14 characters in FIFO

#define U2LCR          (*(reg32 *)(UART2_BASE_ADDR + 0x0C))	// Registrador de controle de linha
	#define U2LCR_CHAR_5         (0 << 0)   			 		// Tamanho do caractere com 5 bits
	#define U2LCR_CHAR_6         (1 << 0)    					// Tamanho do caractere com 6 bits
	#define U2LCR_CHAR_7         (2 << 0)    					// Tamanho do caractere com 7 bits
	#define U2LCR_CHAR_8         (3 << 0)    					// Tamanho do caractere com 8 bits
	#define U2LCR_STOP_1         (0 << 2)    					// 1 stop bit
	#define U2LCR_STOP_2         (1 << 2)    					// 2 stop bits ou 1.5 se ULCR[1:0] = 00
	#define U2LCR_PAR_NO         (0 << 3)    					// Sem pariedade
	#define U2LCR_PAR_ODD        (1 << 3)    					// Pariedade impar
	#define U2LCR_PAR_EVEN       (3 << 3)    					// Pariedade par
	#define U2LCR_PAR_MARK       (5 << 3)    					// Força a pariedade = 1
	#define U2LCR_PAR_SPACE      (7 << 3)   	 				// Força a pariedade = 0
	#define U2LCR_BREAK_ENABLE   (1 << 6)    					// Condições de parada a linha de saida. 1 = Habilita parada de transmissão / 0 = Desabilita parada de transmissão
	#define U2LCR_DLAB_ENABLE    (1 << 7)    					// Acessos as registradores do baudrate
	
#define U2LSR          (*(reg32 *)(UART2_BASE_ADDR + 0x14))	// Registrador de status de linha
	#define U2LSR_RDR            (1 << 0)    					// Receive Data Ready
	#define U2LSR_OE             (1 << 1)    					// Overrun Error
	#define U2LSR_PE             (1 << 2)   			 		// Parity Error
	#define U2LSR_FE             (1 << 3)   	 				// Framing Error
	#define U2LSR_BI             (1 << 4)    					// Break Interrupt
	#define U2LSR_THRE           (1 << 5)    					// Transmit Holding Register Empty
	#define U2LSR_TEMT           (1 << 6)    					// Transmitter Empty
	#define U2LSR_RXFE           (1 << 7)    					// Error in Receive FIFO
	#define U2LSR_ERR_MASK       0x1E

#define U2SCR          (*(reg32 *)(UART2_BASE_ADDR + 0x1C))
#define U2ACR          (*(reg32 *)(UART2_BASE_ADDR + 0x20))
#define U2ICR          (*(reg32 *)(UART2_BASE_ADDR + 0x24))
#define U2FDR          (*(reg32 *)(UART2_BASE_ADDR + 0x28))
#define U2TER          (*(reg32 *)(UART2_BASE_ADDR + 0x30))

// ########################################################################################################################################
// UART3 - UNIVERSAL ASYNCHRONOUS RECEIVER TRANSMITTER

#define UART3_SIZE_FIFO	16

// Modos de trabalho das UARTs
#define UART3_8N1      (u8)(U3LCR_CHAR_8 + U3LCR_PAR_NO   + U3LCR_STOP_1)
#define UART3_7N1      (u8)(U3LCR_CHAR_7 + U3LCR_PAR_NO   + U3LCR_STOP_1)
#define UART3_8N2      (u8)(U3LCR_CHAR_8 + U3LCR_PAR_NO   + U3LCR_STOP_2)
#define UART3_7N2      (u8)(U3LCR_CHAR_7 + U3LCR_PAR_NO   + U3LCR_STOP_2)
#define UART3_8E1      (u8)(U3LCR_CHAR_8 + U3LCR_PAR_EVEN + U3LCR_STOP_1)
#define UART3_7E1      (u8)(U3LCR_CHAR_7 + U3LCR_PAR_EVEN + U3LCR_STOP_1)
#define UART3_8E2      (u8)(U3LCR_CHAR_8 + U3LCR_PAR_EVEN + U3LCR_STOP_2)
#define UART3_7E2      (u8)(U3LCR_CHAR_7 + U3LCR_PAR_EVEN + U3LCR_STOP_2)
#define UART3_8O1      (u8)(U3LCR_CHAR_8 + U3LCR_PAR_ODD  + U3LCR_STOP_1)
#define UART3_7O1      (u8)(U3LCR_CHAR_7 + U3LCR_PAR_ODD  + U3LCR_STOP_1)
#define UART3_8O2      (u8)(U3LCR_CHAR_8 + U3LCR_PAR_ODD  + U3LCR_STOP_2)
#define UART3_7O2      (u8)(U3LCR_CHAR_7 + U3LCR_PAR_ODD  + U3LCR_STOP_2)

// Tipo de FIFO das UARTs
#define UART3_FIFO_OFF (0x00)
#define UART3_FIFO_RX_TRIG1   (u8)(U3FCR_FIFO_ENABLE + U3FCR_FIFO_TRIG1)
#define UART3_FIFO_RX_TRIG4   (u8)(U3FCR_FIFO_ENABLE + U3FCR_FIFO_TRIG4)
#define UART3_FIFO_RX_TRIG8   (u8)(U3FCR_FIFO_ENABLE + U3FCR_FIFO_TRIG8)
#define UART3_FIFO_RX_TRIG14  (u8)(U3FCR_FIFO_ENABLE + U3FCR_FIFO_TRIG14)

#define UART3_BASE_ADDR		0xE007C000
#define U3RBR          (*(reg32 *)(UART3_BASE_ADDR + 0x00))	// Registrador de recepção de dados
#define U3THR          (*(reg32 *)(UART3_BASE_ADDR + 0x00))	// Registrador de transmissão de dados 
#define U3DLL          (*(reg32 *)(UART3_BASE_ADDR + 0x00))	// Registrador de ajuste de baudrate (LSB)
#define U3DLM          (*(reg32 *)(UART3_BASE_ADDR + 0x04))	// Registrador de ajuste de baudrate (MSB)
#define U3IER          (*(reg32 *)(UART3_BASE_ADDR + 0x04))	// Registrador de habilitação das interrupções
	#define U3IER_ERBFI          (1 << 0)    					// Habilita a int de dado pronto recebido
	#define U3IER_ETBEI          (1 << 1)    					// Habilita a int THRE (Transmit Holding Register Empty)
	#define U3IER_ELSI           (1 << 2)    					// Enable Receive Line Status Interrupt
	#define U3IER_EDSSI          (1 << 3)    					// Enable MODEM Status Interrupt

#define U3IIR          (*(reg32 *)(UART3_BASE_ADDR + 0x08))	// Registrador de ID de interrupção. Somente para leitura
	#define U3IIR_NO_INT         (1 << 0)   				 	// 1 = Sem pendencia de interrupção / 0 = Pelo menos uma pedencia de interrupção
	#define U3IIR_MS_INT         (0 << 1)    					// MODEM Status
	#define U3IIR_THRE_INT       (1 << 1)    					// Interrupção de THRE (Transmit Holding Register Empty)(
	#define U3IIR_RDA_INT        (2 << 1)    					// Dado disponivel na recpção
	#define U3IIR_RLS_INT        (3 << 1)    					// Receive Line Status
	#define U3IIR_CTI_INT        (6 << 1)    					// Indicador de estouro de tempo para o caractere (Character Timeout Indicator)
	#define U3IIR_ID_MASK        0x0E

#define U3FCR          (*(reg32 *)(UART3_BASE_ADDR + 0x08))	// Registrador de controle da FIFO
	#define U3FCR_FIFO_ENABLE    (1 << 0)    					// Habilita FIFO
	#define U3FCR_RX_FIFO_RESET  (1 << 1)    					// Reseta o FIFO de recepção
	#define U3FCR_TX_FIFO_RESET  (1 << 2)   			 		// Reseta o FIFO de transmissão
	#define U3FCR_FIFO_TRIG1     (0 << 6)    					// Trigger @ 1 character in FIFO
	#define U3FCR_FIFO_TRIG4     (1 << 6)   			 		// Trigger @ 4 characters in FIFO
	#define U3FCR_FIFO_TRIG8     (2 << 6)   				 	// Trigger @ 8 characters in FIFO
	#define U3FCR_FIFO_TRIG14    (3 << 6)    					// Trigger @ 14 characters in FIFO

#define U3LCR          (*(reg32 *)(UART3_BASE_ADDR + 0x0C))	// Registrador de controle de linha
	#define U3LCR_CHAR_5         (0 << 0)   			 		// Tamanho do caractere com 5 bits
	#define U3LCR_CHAR_6         (1 << 0)    					// Tamanho do caractere com 6 bits
	#define U3LCR_CHAR_7         (2 << 0)    					// Tamanho do caractere com 7 bits
	#define U3LCR_CHAR_8         (3 << 0)    					// Tamanho do caractere com 8 bits
	#define U3LCR_STOP_1         (0 << 2)    					// 1 stop bit
	#define U3LCR_STOP_2         (1 << 2)    					// 2 stop bits ou 1.5 se ULCR[1:0] = 00
	#define U3LCR_PAR_NO         (0 << 3)    					// Sem pariedade
	#define U3LCR_PAR_ODD        (1 << 3)    					// Pariedade impar
	#define U3LCR_PAR_EVEN       (3 << 3)    					// Pariedade par
	#define U3LCR_PAR_MARK       (5 << 3)    					// Força a pariedade = 1
	#define U3LCR_PAR_SPACE      (7 << 3)   	 				// Força a pariedade = 0
	#define U3LCR_BREAK_ENABLE   (1 << 6)    					// Condições de parada a linha de saida. 1 = Habilita parada de transmissão / 0 = Desabilita parada de transmissão
	#define U3LCR_DLAB_ENABLE    (1 << 7)    					// Acessos as registradores do baudrate

#define U3LSR          (*(reg32 *)(UART3_BASE_ADDR + 0x14))	// Registrador de status de linha
	#define U3LSR_RDR            (1 << 0)    					// Receive Data Ready
	#define U3LSR_OE             (1 << 1)    					// Overrun Error
	#define U3LSR_PE             (1 << 2)   			 		// Parity Error
	#define U3LSR_FE             (1 << 3)   	 				// Framing Error
	#define U3LSR_BI             (1 << 4)    					// Break Interrupt
	#define U3LSR_THRE           (1 << 5)    					// Transmit Holding Register Empty
	#define U3LSR_TEMT           (1 << 6)    					// Transmitter Empty
	#define U3LSR_RXFE           (1 << 7)    					// Error in Receive FIFO
	#define U3LSR_ERR_MASK       0x1E

#define U3SCR          (*(reg32 *)(UART3_BASE_ADDR + 0x1C))
#define U3ACR          (*(reg32 *)(UART3_BASE_ADDR + 0x20))
#define U3ICR          (*(reg32 *)(UART3_BASE_ADDR + 0x24))
#define U3FDR          (*(reg32 *)(UART3_BASE_ADDR + 0x28))
#define U3TER          (*(reg32 *)(UART3_BASE_ADDR + 0x30))

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
// I2C1
#define I2C1_BASE_ADDR		0xE005C000
#define I2C1CONSET      (*(reg32 *)(I2C1_BASE_ADDR + 0x00))
	#define I2C1_AA				(1 << 2) 						// ACK - Este bit em 1 envia um ACK em 1. bit 0 não tem efeito
  	#define I2C1_SI				(1 << 3) 						// Flag de interrupção - Este bit em 1 indica que ocorreu interrupção
  	#define I2C1_STO			(1 << 4) 						// Este bit em 1 envia stop. bit 0 não tem efeito
  	#define I2C1_STA			(1 << 5) 						// Este bit em 1 envia start. bit 0 não tem efeito
  	#define I2C1_I2EN			(1 << 6) 						// Este bit em 1 habilita bus I2C. bit 0 não tem efeito
#define I2C1STAT        (*(reg32 *)(I2C1_BASE_ADDR + 0x04))
	// MESTRE - TRANSMISSÃO
	#define I2C1_MT_SLA_ACK					0x18
	#define I2C1_MT_SLA_NACK				0x20
	#define I2C1_MT_DATA_ACK				0x28
	#define I2C1_MT_DATA_NACK				0x30
	#define I2C1_MT_ARB_LOST				0x38

	// MESTRE - RECEPÇÃO
	#define I2C1_MR_ARB_LOST				0x38
	#define I2C1_MR_SLA_ACK					0x40
	#define I2C1_MR_SLA_NACK				0x48
	#define I2C1_MR_DATA_ACK				0x50
	#define I2C1_MR_DATA_NACK				0x58

	// ESCRAVO Slave - TRANSMISSÃO
	#define I2C1_ST_SLA_ACK					0xA8
	#define I2C1_ST_ARB_LOST_SLA_ACK		0xB0
	#define I2C1_ST_DATA_ACK				0xB8
	#define I2C1_ST_DATA_NACK				0xC0
	#define I2C1_ST_LAST_DATA				0xC8

	// ESCRAVO - RECEPÇÃO
	#define I2C1_SR_SLA_ACK					0x60
	#define I2C1_SR_ARB_LOST_SLA_ACK		0x68
	#define I2C1_SR_GCALL_ACK				0x70
	#define I2C1_SR_ARB_LOST_GCALL_ACK		0x78
	#define I2C1_SR_DATA_ACK				0x80
	#define I2C1_SR_DATA_NACK				0x88
	#define I2C1_SR_GCALL_DATA_ACK			0x90
	#define I2C1_SR_GCALL_DATA_NACK			0x98
	#define I2C1_SR_STOP					0xA0

#define I2C1DAT         (*(reg32 *)(I2C1_BASE_ADDR + 0x08))
#define I2C1ADR         (*(reg32 *)(I2C1_BASE_ADDR + 0x0C))
#define I2C1SCLH        (*(reg32 *)(I2C1_BASE_ADDR + 0x10))
#define I2C1SCLL        (*(reg32 *)(I2C1_BASE_ADDR + 0x14))
#define I2C1CONCLR      (*(reg32 *)(I2C1_BASE_ADDR + 0x18))
	#define I2C1_AAC				(1 << 2) 					// ACK - Este bit em 1 envia um ACK em 0. bit 0 não tem efeito
  	#define I2C1_SIC				(1 << 3) 					// Flag de interrupção - Este bit em 1 limpa o flag de interrupção
  	#define I2C1_STAC				(1 << 5) 					// Este bit em 1 encerra o start. bit 0 não tem efeito
  	#define I2C1_I2ENC				(1 << 6) 					// Este bit em 1 desabilita bus I2C. bit 0 não tem efeito


// ########################################################################################################################################
// I2C2
#define I2C2_BASE_ADDR		0xE0080000
#define I2C2CONSET      (*(reg32 *)(I2C2_BASE_ADDR + 0x00))
	#define I2C2_AA				(1 << 2) 						// ACK - Este bit em 1 envia um ACK em 1. bit 0 não tem efeito
  	#define I2C2_SI				(1 << 3) 						// Flag de interrupção - Este bit em 1 indica que ocorreu interrupção
  	#define I2C2_STO			(1 << 4) 						// Este bit em 1 envia stop. bit 0 não tem efeito
  	#define I2C2_STA			(1 << 5) 						// Este bit em 1 envia start. bit 0 não tem efeito
  	#define I2C2_I2EN			(1 << 6) 						// Este bit em 1 habilita bus I2C. bit 0 não tem efeito

#define I2C2STAT        (*(reg32 *)(I2C2_BASE_ADDR + 0x04))
  	// MESTRE - TRANSMISSÃO
	#define I2C2_MT_SLA_ACK					0x18
	#define I2C2_MT_SLA_NACK				0x20
	#define I2C2_MT_DATA_ACK				0x28
	#define I2C2_MT_DATA_NACK				0x30
	#define I2C2_MT_ARB_LOST				0x38

	// MESTRE - RECEPÇÃO
	#define I2C2_MR_ARB_LOST				0x38
	#define I2C2_MR_SLA_ACK					0x40
	#define I2C2_MR_SLA_NACK				0x48
	#define I2C2_MR_DATA_ACK				0x50
	#define I2C2_MR_DATA_NACK				0x58

	// ESCRAVO Slave - TRANSMISSÃO
	#define I2C2_ST_SLA_ACK					0xA8
	#define I2C2_ST_ARB_LOST_SLA_ACK		0xB0
	#define I2C2_ST_DATA_ACK				0xB8
	#define I2C2_ST_DATA_NACK				0xC0
	#define I2C2_ST_LAST_DATA				0xC8

	// ESCRAVO - RECEPÇÃO
	#define I2C2_SR_SLA_ACK					0x60
	#define I2C2_SR_ARB_LOST_SLA_ACK		0x68
	#define I2C2_SR_GCALL_ACK				0x70
	#define I2C2_SR_ARB_LOST_GCALL_ACK		0x78
	#define I2C2_SR_DATA_ACK				0x80
	#define I2C2_SR_DATA_NACK				0x88
	#define I2C2_SR_GCALL_DATA_ACK			0x90
	#define I2C2_SR_GCALL_DATA_NACK			0x98
	#define I2C2_SR_STOP					0xA0

#define I2C2DAT         (*(reg32 *)(I2C2_BASE_ADDR + 0x08))
#define I2C2ADR         (*(reg32 *)(I2C2_BASE_ADDR + 0x0C))
#define I2C2SCLH        (*(reg32 *)(I2C2_BASE_ADDR + 0x10))
#define I2C2SCLL        (*(reg32 *)(I2C2_BASE_ADDR + 0x14))
#define I2C2CONCLR      (*(reg32 *)(I2C2_BASE_ADDR + 0x18))
	#define I2C2_AAC				(1 << 2) 					// ACK - Este bit em 1 envia um ACK em 0. bit 0 não tem efeito
  	#define I2C2_SIC				(1 << 3) 					// Flag de interrupção - Este bit em 1 limpa o flag de interrupção
  	#define I2C2_STAC				(1 << 5) 					// Este bit em 1 encerra o start. bit 0 não tem efeito
  	#define I2C2_I2ENC				(1 << 6) 					// Este bit em 1 desabilita bus I2C. bit 0 não tem efeito


// ########################################################################################################################################
// SPI0 - SERIAL PERIPHERAL INTERFACE 0
#define SPI0_BASE_ADDR		0xE0020000

#define SPI0_SLAVE	0
#define SPI0_MASTER	1 
	
#define S0SPCR         (*(reg32 *)(SPI0_BASE_ADDR + 0x00))	// Control Register
	#define SPI0_CPHA		(1<<3)								// Este bit em 0 configura a captura o dado na primeira borda do clock, bit 1 captura na segunda borda
	#define SPI0_CPOL		(1<<4)								// Este bit configura o padrão do clock
	#define SPI0_MSTR		(1<<5)								// Este bit em 0 ajusta o ARM como escravo, bit 1 como mestre
	#define SPI0_LSBF		(1<<6)								// Este bit em 0 o primeiro bit a TX será o MSB, o bit 1 será LSB
	#define SPI0_SPIE 		(1<<7)								// Este bit em 0 desabilita a interrupção, bit 1 habilita
	#define SPI0_BITS_8  	(0x804)								// 8 bits de transferencia
	#define SPI0_BITS_9  	(0x904)								// 9 bits de transferencia
	#define SPI0_BITS_10 	(0xA04)								// 10 bits de transferencia
	#define SPI0_BITS_11 	(0xB04)								// 11 bits de transferencia
	#define SPI0_BITS_12 	(0xC04)								// 12 bits de transferencia
	#define SPI0_BITS_13 	(0xD04)								// 13 bits de transferencia
	#define SPI0_BITS_14 	(0xE04)								// 14 bits de transferencia
	#define SPI0_BITS_15 	(0xF04)								// 15 bits de transferencia
	#define SPI0_BITS_16 	(0x004)								// 16 bits de transferencia
	#define SPI0_BITS_MASK 	(0xF04)
	#define SPI0_STANDARD 	0

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
// SSP0
#define SSP0_FIFO_DEPTH  (8)

#define SSP0_BASE_ADDR		0xE0068000
#define SSP0CR0        (*(reg32 *)(SSP0_BASE_ADDR + 0x00))
	#define SSP0_DSS_4   (0x3)	// 4 bits de transferencia
	#define SSP0_DSS_5   (0x4)	// 5 bits de transferencia
	#define SSP0_DSS_6   (0x5)	// 6 bits de transferencia
	#define SSP0_DSS_7   (0x6)	// 7 bits de transferencia
	#define SSP0_DSS_8   (0x7)	// 8 bits de transferencia
	#define SSP0_DSS_9   (0x8)	// 9 bits de transferencia
	#define SSP0_DSS_10  (0x9)	// 10 bits de transferencia
	#define SSP0_DSS_11  (0xa)	// 11 bits de transferencia
	#define SSP0_DSS_12  (0xb)	// 12 bits de transferencia
	#define SSP0_DSS_13  (0xc)	// 13 bits de transferencia
	#define SSP0_DSS_14  (0xd)	// 14 bits de transferencia
	#define SSP0_DSS_15  (0xe)	// 15 bits de transferencia
	#define SSP0_DSS_16  (0xf)	// 16 bits de transferencia
		#define SSP0_DSS_MASK (0xf)
	#define SSP0_FRF_SPI (0x0)	// Formato frame = SPI
	#define SSP0_FRF_SSI (0x10)	// Formato frame = TI
	#define SSP0_FRF_MW  (0x20)	// Formato frame = Microwire
	#define SSP0_STANDARD (0x7)
#define SSP0CR1        (*(reg32 *)(SSP0_BASE_ADDR + 0x04))
	#define SSP0_LBM_NORMAL    	(0x0)	// Loop back mode normal operation
	#define SSP0_LBM_SOUT    	(0x1)	// Loop back mode pego da saida MISO ou MOSI
	#define SSP0_SSE_OFF   		(0x0)	// SSP enable. O controlador SSP é desabilitado
	#define SSP0_SSE_ON    		(0x2)	// SSP enable. O controlador SSP interage com outros dispositivos seriais
	#define SSP0_MASTER    		(0x0) 	// SSP como mestre
	#define SSP0_SLAVE     		(0x4)	// SSP como escravo
	#define SSP0_SOD     		(0x8)	// Somente usados para modo escravo. 
#define SSP0DR         (*(reg32 *)(SSP0_BASE_ADDR + 0x08)) // Regitrador de leitura e escrita dados transferidos no barramento SPP
#define SSP0SR         (*(reg32 *)(SSP0_BASE_ADDR + 0x0C)) // Registrador de estatus
	#define SSP0_TFE      (0x00000001)	// 1 = FIFO TX = vazio
	#define SSP0_TNF      (0x00000002)	// 1 = FIFO TX não está cheio. 0 = FIFO TX está cheio
	#define SSP0_RNE      (0x00000004)	// 0 = FIFO RX está vazio
	#define SSP0_RFF      (0x00000008)	// 1 = FIFO RX não está cheio
	#define SSP0_BSY      (0x00000010)	// 0 = BUS oscioso. 1 = BUS TX ou RX
#define SSP0CPSR       (*(reg32 *)(SSP0_BASE_ADDR + 0x10))
#define SSP0IMSC       (*(reg32 *)(SSP0_BASE_ADDR + 0x14))
	#define SSP0_RORIM  (0x1)
	#define SSP0_RTIM   (0x2)
	#define SSP0_RXIM   (0x4)
	#define SSP0_TXIM   (0x8)
#define SSP0RIS        (*(reg32 *)(SSP0_BASE_ADDR + 0x18))
	#define SSP0_RORRIS  (0x00000001)
	#define SSP0_RTRIS   (0x00000002)
	#define SSP0_RXRIS   (0x00000004)
	#define SSP0_TXRIS   (0x00000008)
#define SSP0MIS        (*(reg32 *)(SSP0_BASE_ADDR + 0x1C))
	#define SSP0_RORMIS  (0x00000001)
	#define SSP0_RTMIS   (0x00000002)
	#define SSP0_RXMIS   (0x00000004)
	#define SSP0_TXMIS   (0x00000008)
#define SSP0ICR        (*(reg32 *)(SSP0_BASE_ADDR + 0x20))
	#define SSP0_RORIC   (0x00000001)
	#define SSP0_RTIC    (0x00000002)
#define SSP0DMACR      (*(reg32 *)(SSP0_BASE_ADDR + 0x24))

// ########################################################################################################################################
// SSP1 Controller
#define SSP1_FIFO_DEPTH  (8)

#define SSP1_BASE_ADDR		0xE0030000
#define SSP1CR0        (*(reg32 *)(SSP1_BASE_ADDR + 0x00))
	#define SSP1_DSS_4   (0x3)	// 4 bits de transferencia
	#define SSP1_DSS_5   (0x4)	// 5 bits de transferencia
	#define SSP1_DSS_6   (0x5)	// 6 bits de transferencia
	#define SSP1_DSS_7   (0x6)	// 7 bits de transferencia
	#define SSP1_DSS_8   (0x7)	// 8 bits de transferencia
	#define SSP1_DSS_9   (0x8)	// 9 bits de transferencia
	#define SSP1_DSS_10  (0x9)	// 10 bits de transferencia
	#define SSP1_DSS_11  (0xa)	// 11 bits de transferencia
	#define SSP1_DSS_12  (0xb)	// 12 bits de transferencia
	#define SSP1_DSS_13  (0xc)	// 13 bits de transferencia
	#define SSP1_DSS_14  (0xd)	// 14 bits de transferencia
	#define SSP1_DSS_15  (0xe)	// 15 bits de transferencia
	#define SSP1_DSS_16  (0xf)	// 16 bits de transferencia
		#define SSP1_DSS_MASK (0xf)
	#define SSP1_FRF_SPI (0x0)	// Formato frame = SPI
	#define SSP1_FRF_SSI (0x10)	// Formato frame = TI
	#define SSP1_FRF_MW  (0x20)	// Formato frame = Microwire

#define SSP1CR1        (*(reg32 *)(SSP1_BASE_ADDR + 0x04))
	#define SSP1_LBM_NORMAL    	(0x0)	// Loop back mode normal operation
	#define SSP1_LBM_SOUT    	(0x1)	// Loop back mode pego da saida MISO ou MOSI
	#define SSP1_SSE_OFF   		(0x0)	// SSP enable. O controlador SSP é desabilitado
	#define SSP1_SSE_ON    		(0x2)	// SSP enable. O controlador SSP interage com outros dispositivos seriais
	#define SSP1_MASTER    		(0x0) 	// SSP como mestre
	#define SSP1_SLAVE     		(0x4)	// SSP como escravo
	#define SSP1_SOD     		(0x8)	// Somente usados para modo escravo. 

#define SSP1DR         (*(reg32 *)(SSP1_BASE_ADDR + 0x08))
#define SSP1SR         (*(reg32 *)(SSP1_BASE_ADDR + 0x0C))
	#define SSP1_TFE      (0x00000001)	// 1 = FIFO TX = vazio
	#define SSP1_TNF      (0x00000002)	// 1 = FIFO TX não está cheio. 0 = FIFO TX está cheio
	#define SSP1_RNE      (0x00000004)	// 0 = FIFO RX está vazio
	#define SSP1_RFF      (0x00000008)	// 1 = FIFO RX não está cheio
	#define SSP1_BSY      (0x00000010)	// 0 = BUS oscioso. 1 = BUS TX ou RX

#define SSP1CPSR       (*(reg32 *)(SSP1_BASE_ADDR + 0x10))
#define SSP1IMSC       (*(reg32 *)(SSP1_BASE_ADDR + 0x14))
	#define SSP1_RORIM  (0x1)
	#define SSP1_RTIM   (0x2)
	#define SSP1_RXIM   (0x4)
	#define SSP1_TXIM   (0x8)

#define SSP1RIS        (*(reg32 *)(SSP1_BASE_ADDR + 0x18))
	#define SSP1_RORRIS  (0x00000001)
	#define SSP1_RTRIS   (0x00000002)
	#define SSP1_RXRIS   (0x00000004)
	#define SSP1_TXRIS   (0x00000008)

#define SSP1MIS        (*(reg32 *)(SSP1_BASE_ADDR + 0x1C))
	#define SSP1_RORMIS  (0x00000001)
	#define SSP1_RTMIS   (0x00000002)
	#define SSP1_RXMIS   (0x00000004)
	#define SSP1_TXMIS   (0x00000008)

#define SSP1ICR        (*(reg32 *)(SSP1_BASE_ADDR + 0x20))
	#define SSP1_RORIC   (0x00000001)
	#define SSP1_RTIC    (0x00000002)

#define SSP1DMACR      (*(reg32 *)(SSP1_BASE_ADDR + 0x24))

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
#define CISS        	(*(reg32 *)(RTC_BASE_ADDR + 0x40))

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

#define IO1PIN         (*(reg32 *)(GPIO_BASE_ADDR + 0x10))	// P1 - Registrador de leitura da porta
#define IO1SET         (*(reg32 *)(GPIO_BASE_ADDR + 0x14))	// P1 - Registrador para acionar os pinos da porta. 1 = Aciona, 0 = Sem efeito 
#define IO1DIR         (*(reg32 *)(GPIO_BASE_ADDR + 0x18))	// P1 - Registrador para determinar o sentido dos pinos da porta. 1 = Saida  0 = Entrada
#define IO1CLR         (*(reg32 *)(GPIO_BASE_ADDR + 0x1C))	// P1 - Registrador para desligar os pinos da porta. 1 = Desliga, 0 = Sem efeito

// GPIO Interrupt Registers
#define IO0_INT_EN_R    (*(reg32 *)(GPIO_BASE_ADDR + 0x90)) 
#define IO0_INT_EN_F    (*(reg32 *)(GPIO_BASE_ADDR + 0x94))
#define IO0_INT_STAT_R  (*(reg32 *)(GPIO_BASE_ADDR + 0x84))
#define IO0_INT_STAT_F  (*(reg32 *)(GPIO_BASE_ADDR + 0x88))
#define IO0_INT_CLR     (*(reg32 *)(GPIO_BASE_ADDR + 0x8C))

#define IO2_INT_EN_R    (*(reg32 *)(GPIO_BASE_ADDR + 0xB0)) 
#define IO2_INT_EN_F    (*(reg32 *)(GPIO_BASE_ADDR + 0xB4))
#define IO2_INT_STAT_R  (*(reg32 *)(GPIO_BASE_ADDR + 0xA4))
#define IO2_INT_STAT_F  (*(reg32 *)(GPIO_BASE_ADDR + 0xA8))
#define IO2_INT_CLR     (*(reg32 *)(GPIO_BASE_ADDR + 0xAC))

#define IO_INT_STAT     (*(reg32 *)(GPIO_BASE_ADDR + 0x80))

#define PARTCFG_BASE_ADDR		0x3FFF8000
#define PARTCFG        (*(reg32 *)(PARTCFG_BASE_ADDR + 0x00)) 

// ########################################################################################################################################
// BLOCOS DE CONEXÃO DE PINOS IO
#define PINSEL_BASE_ADDR	0xE002C000

#define PINSEL0        (*(reg32 *)(PINSEL_BASE_ADDR + 0x00))
#define PINSEL1        (*(reg32 *)(PINSEL_BASE_ADDR + 0x04))
#define PINSEL2        (*(reg32 *)(PINSEL_BASE_ADDR + 0x08))
#define PINSEL3        (*(reg32 *)(PINSEL_BASE_ADDR + 0x0C))
#define PINSEL4        (*(reg32 *)(PINSEL_BASE_ADDR + 0x10))
#define PINSEL5        (*(reg32 *)(PINSEL_BASE_ADDR + 0x14))
#define PINSEL6        (*(reg32 *)(PINSEL_BASE_ADDR + 0x18))
#define PINSEL7        (*(reg32 *)(PINSEL_BASE_ADDR + 0x1C))
#define PINSEL8        (*(reg32 *)(PINSEL_BASE_ADDR + 0x20))
#define PINSEL9        (*(reg32 *)(PINSEL_BASE_ADDR + 0x24))
#define PINSEL10       (*(reg32 *)(PINSEL_BASE_ADDR + 0x28))


#define PINMODE0        (*(reg32 *)(PINSEL_BASE_ADDR + 0x40))  	// Configuração de pullup (00 padrão)/repetidor(01)/nem pullup e nem oulldown (10)/pulldown(11)
#define PINMODE1        (*(reg32 *)(PINSEL_BASE_ADDR + 0x44))	// Idem acima
#define PINMODE2        (*(reg32 *)(PINSEL_BASE_ADDR + 0x48))
#define PINMODE3        (*(reg32 *)(PINSEL_BASE_ADDR + 0x4C))
#define PINMODE4        (*(reg32 *)(PINSEL_BASE_ADDR + 0x50))
#define PINMODE5        (*(reg32 *)(PINSEL_BASE_ADDR + 0x54))
#define PINMODE6        (*(reg32 *)(PINSEL_BASE_ADDR + 0x58))
#define PINMODE7        (*(reg32 *)(PINSEL_BASE_ADDR + 0x5C))
#define PINMODE8        (*(reg32 *)(PINSEL_BASE_ADDR + 0x60))
#define PINMODE9        (*(reg32 *)(PINSEL_BASE_ADDR + 0x64))

// ########################################################################################################################################
// ADC0
#define AD0_BASE_ADDR		0xE0034000

#define AD0CR          (*(reg32 *)(AD0_BASE_ADDR + 0x00))
  	#define AD0_CHANNEL0		0
	#define AD0_CHANNEL1		1
	#define AD0_CHANNEL2		2
	#define AD0_CHANNEL3		3
	#define AD0_CHANNEL4		4
	#define AD0_CHANNEL5		5
	#define AD0_CHANNEL6		6
	#define AD0_CHANNEL7		7
	#define AD0_CHANNEL_MASK	(0xFF)
	
  	#define AD0_STOPPED 		( 0 << 24 )						// Somente no modo não BUSRT. Conversor parado
  	#define AD0_START_NOW		( 1 << 24 )						// Somente no modo não BUSRT. Incializa conversçao agora
  	#define AD0_START_MAT0_0 	( 3 << 24 )						// Somente no modo não BUSRT. Incializa conversçao pela mudança da borda do pino MAT0.0
  	#define AD0_START_MAT0_1 	( 4 << 24 )						// Somente no modo não BUSRT. Incializa conversçao pela mudança da borda do pino MAT0.1
  	#define AD0_START_MAT0_2 	( 2 << 24 )						// Somente no modo não BUSRT. Incializa conversçao pela mudança da borda do pino MAT0.2
  	#define AD0_START_MAT0_3 	( 5 << 24 )						// Somente no modo não BUSRT. Incializa conversçao pela mudança da borda do pino MAT0.3
  	#define AD0_START_MAT1_0 	( 6 << 24 )						// Somente no modo não BUSRT. Incializa conversçao pela mudança da borda do pino MAT1.0
  	#define AD0_START_MAT1_1 	( 7 << 24 )						// Somente no modo não BUSRT. Incializa conversçao pela mudança da borda do pino MAT1.1
  	#define AD0_START_MASK		( 7 << 24 )
  	
  	#define AD0_EDGE_FALLING 	( 0 << 27 )						// Começa a captura na borda de descrida do gatilho
  	#define AD0_EDGE_RISING		( 1 << 27 )						// Começa a captura na borda de subida do gatilho
  	#define AD0_EDGE_MASK		( 1 << 27 )
  	
  	#define AD0_CLKS_10BIT		( 0 << 17)
	#define AD0_CLKS_9BIT		( 1 << 17)
	#define AD0_CLKS_8BIT		( 2 << 17)
	#define AD0_CLKS_7BIT		( 3 << 17)
	#define AD0_CLKS_6BIT		( 4 << 17)
	#define AD0_CLKS_5BIT		( 5 << 17)
	#define AD0_CLKS_4BIT		( 6 << 17)
	#define AD0_CLKS_3BIT		( 7 << 17)
	#define AD0_CLKS_MASK		( 7 << 17)

  	#define AD0_BUSRT			( 1 << 16)

#define AD0GDR         (*(reg32 *)(AD0_BASE_ADDR + 0x04))
#define AD0INTEN       (*(reg32 *)(AD0_BASE_ADDR + 0x0C))

#define AD0DR0         (*(reg32 *)(AD0_BASE_ADDR + 0x10))
#define AD0DR1         (*(reg32 *)(AD0_BASE_ADDR + 0x14))
#define AD0DR2         (*(reg32 *)(AD0_BASE_ADDR + 0x18))
#define AD0DR3         (*(reg32 *)(AD0_BASE_ADDR + 0x1C))
#define AD0DR4         (*(reg32 *)(AD0_BASE_ADDR + 0x20))
#define AD0DR5         (*(reg32 *)(AD0_BASE_ADDR + 0x24))
#define AD0DR6         (*(reg32 *)(AD0_BASE_ADDR + 0x28))
#define AD0DR7         (*(reg32 *)(AD0_BASE_ADDR + 0x2C))
  	#define AD0_DONE		( 1 << 31 ) 				// Sinaliza quando a conversão ADC esteja completa
  	#define AD0_OVERUN		( 1 << 30 )					// Sinaliza quando os dados de uma ou mais conversções foram perdidos no modo busrt

#define AD0STAT        (*(reg32 *)(AD0_BASE_ADDR + 0x30))
  	// AD0STAT[7:0] Sinaliza o canal que a conversão foi concluida
  	#define AD0_CHANNEL0_DONE		(1 << 0)
	#define AD0_CHANNEL1_DONE		(1 << 1)
	#define AD0_CHANNEL2_DONE		(1 << 2)
	#define AD0_CHANNEL3_DONE		(1 << 3)
	#define AD0_CHANNEL4_DONE		(1 << 4)
	#define AD0_CHANNEL5_DONE		(1 << 5)
	#define AD0_CHANNEL6_DONE		(1 << 6)
	#define AD0_CHANNEL7_DONE		(1 << 7)
	#define AD0_CHANNEL_DONE_MASK	0xFF

  	// AD0STAT[15:8] Sinaliza o canal que a houve overrum no modo burst
  	#define AD0_CHANNEL0_OVERUN	(1 << 8)
	#define AD0_CHANNEL1_OVERUN		(1 << 9)
	#define AD0_CHANNEL2_OVERUN		(1 << 10)
	#define AD0_CHANNEL3_OVERUN		(1 << 11)
	#define AD0_CHANNEL4_OVERUN		(1 << 12)
	#define AD0_CHANNEL5_OVERUN		(1 << 13)
	#define AD0_CHANNEL6_OVERUN		(1 << 14)
	#define AD0_CHANNEL7_OVERUN		(1 << 15)
	#define AD0_CHANNEL_OVERUN_MASK	0xFF00
	
	



// ########################################################################################################################################
// DAC
#define DAC_BASE_ADDR	0xE006C000
#define DACR           	(*(reg32 *)(DAC_BASE_ADDR + 0x00))
	#define DACR_VALUE_MASK  	(0x0000ffc0)
	#define DACR_VALUE_SHIFT 	(6)
	#define DACR_BIAS       	(0x00010000)
	#define DACR_MASK        	(0x0001ffc0)


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
	#define MEMMAP_RAMEXT  	3                 						// Vetor de interrupção na RAM externo no banco 0
	
// REGISTRADORES PARA TRAVA DE PLL
#define PLLCON         (*(reg32 *)(SCB_BASE_ADDR + 0x080))		// Registrador de controle do PLL
	#define PLLCON_PLLE   (1 << 0)          						// Habilita PLL
	#define PLLCON_PLLC   (1 << 1)          						// Conecta o PLL a CPU

#define PLLCFG         (*(reg32 *)(SCB_BASE_ADDR + 0x084))		// Registrador de configuração do PLL

#define PLLSTAT        (*(reg32 *)(SCB_BASE_ADDR + 0x088))		// Registrador de Status
	#define PLLSTAT_PLLE  (1 << 24)									// Bit indicador se o PLL está ou não ativado
	#define PLLSTAT_PLLC  (1 << 25)									// Bit indicador se o PLL está ou não conectado
	#define PLLSTAT_LOCK  (1 << 26)         						// Bit de status de travamento do PLL
	
#define PLLFEED        (*(reg32 *)(SCB_BASE_ADDR + 0x08C))		// Registrador de validação de mudança do PLL

// REGISTRADOR DE CONTROLE DE CONSUMO (POWER CONTROL)
#define PCON           (*(reg32 *)(SCB_BASE_ADDR + 0x0C0))		// Registrador de controle de potencia
#define PCONP          (*(reg32 *)(SCB_BASE_ADDR + 0x0C4))		// Registrador de controle de potencia dos perifericos
	#define PCTIM0			(1 << 1)	// Timer/Counter 0 power/clock control bit. Reset = 1
	#define PCTIM1		 	(1 << 2)	// Timer/Counter 1 power/clock control bit. Reset = 1
	#define PCUART0		 	(1 << 3)	// UART0 power/clock control bit. Reset = 1
	#define PCUART1 		(1 << 4)	// UART1 power/clock control bit. Reset = 1
	#define PCPWM0 			(1 << 5)	// PWM0 power/clock control bit. Reset = 1
	#define PCPWM1 		 	(1 << 6)	// PWM1 power/clock control bit. Reset = 1
	#define PCI2C0 		 	(1 << 7)	// The I2C0 interface power/clock control bit. Reset = 1
	#define PCSPI 		 	(1 << 8)	// The SPI interface power/clock control bit. Reset = 1
	#define PCRTC 			(1 << 9)	// The RTC power/clock control bit. Reset = 1
	#define PCSSP1 			(1 << 10)	// The SSP1 interface power/clock control bit. Reset = 1
	#define PCEMC 		 	(1 << 11)	// External Memory Controller Reset = 1
	#define PCAD 		 	(1 << 12)	// A/D converter (ADC) power/clock control bit. Reset = 0
	#define PCCAN1 		 	(1 << 13)	// CAN Controller 1 power/clock control bit. Reset = 0
	#define PCCAN2 			(1 << 14)	// CAN Controller 2 power/clock control bit. Reset = 0
	#define PCI2C1 			(1 << 19)	// The I2C1 interface power/clock control bit. Reset = 1
	#define PCLCD	 	 	(1 << 20)	// LCD controller power control bit. Reset = 0
	#define PCSSP0 			(1 << 21)	// The SSP0 interface power/clock control bit. Reset = 1
	#define PCTIM2 			(1 << 22)	// Timer 2 power/clock control bit. Reset = 0
	#define PCTIM3 			(1 << 23)	// Timer 3 power/clock control bit. Reset = 0
	#define PCUART2 		(1 << 24)	// UART 2 power/clock control bit. Reset = 0
	#define PCUART3 		(1 << 25)	// UART 3 power/clock control bit. Reset = 0
	#define PCI2C2 			(1 << 26)	// I2S interface 2 power/clock control bit.  Reset = 1
	#define PCI2S 			(1 << 27)	// I2S interface power/clock control bit. Reset = 0
	#define PCSDC 			(1 << 28)	// SD card interface power/clock control bit.  Reset = 0
	#define PCGPDMA  		(1 << 29)	// GP DMA function power/clock control bit.  Reset = 0
	#define PCENET 			(1 << 30)	// Ethernet block power/clock control bit.  Reset = 0
	#define PCUSB 			(1 << 31)	// USB interface power/clock control bit.  Reset = 0

// REGISTRADOR DIVISÃO DE CLOCK
#define CCLKCFG        (*(reg32 *)(SCB_BASE_ADDR + 0x104))		// Registrador para selecionar o divisor do Fcco para gerar o clock da CPU
#define USBCLKCFG      (*(reg32 *)(SCB_BASE_ADDR + 0x108))		// Registrador para selecionar o divisor do Fcco para gerar o clock do USB
#define PCLKSRCSEL     (*(reg32 *)(SCB_BASE_ADDR + 0x10C))		// Registrador para selecionar o tipo de oscilador para o PLL

#define PCLKSEL0       (*(reg32 *)(SCB_BASE_ADDR + 0x1A8))		// Registrador para selecionar o divisor do Fcco para gerar o clock dos dispositivos internos 1
#define PCLKSEL1       (*(reg32 *)(SCB_BASE_ADDR + 0x1AC))		// Registrador para selecionar o divisor do Fcco para gerar o clock dos dispositivos internos 2
	#define	PBSD_1_4_CCLK	4										// Pclk roda a 1/4 da velocidade do clock da CPU (Cclk)
	#define	PBSD_1_2_CCLK	2										// Pclk roda a 1/2 da velocidade do clock da CPU (Cclk)
	#define	PBSD_1_1_CCLK	1										// Pclk roda a mesma velocidade do clock da CPU (Cclk)

// INDENTIFICAÇÃO DE ORIGEM DO RESET
#define RSIR           (*(reg32 *)(SCB_BASE_ADDR + 0x180))

// PROTEÇÃO DE SEGURANÇA DO CÓDIGO
#define CSPR           (*(reg32 *)(SCB_BASE_ADDR + 0x184))

// AHB configuration
//#define AHBCFG1        (*(reg32 *)(SCB_BASE_ADDR + 0x188))
//#define AHBCFG2        (*(reg32 *)(SCB_BASE_ADDR + 0x18C))

// CONTROLE E STATUS DO SISTEMA
#define SCS            (*(reg32 *)(SCB_BASE_ADDR + 0x1A0))
	#define SCS_OSCRANGE_1MHZ_20MHZ		0x0					// Define que o a frequencia de entrada do clock para o oscilador principal está na faixa de 1MHZ a 20MHZ
	#define SCS_OSCRANGE_15MHZ_24MHZ	0x10				// Define que o a frequencia de entrada do clock para o oscilador principal está na faixa de 15MHZ a 24MHZ
	#define SCS_OSCEN					(1 << 5)			// Bit para ligar o oscilador principal
	#define SCS_OSCSTAT					(1 << 6)			// Bit para verificar se o oscilador principal está pronto para uso


// VALORES PADRÕES DE SEGURANÇA DE ERROS DE CONFIGURAÇÃO
#define F_CPU_MIN 			1000000 	// 1MHz
#define F_CPU_MAX 			24000000	// 24MHz
#define CCLK_MIN			10000000	// 10MHz
#define CCLK_MAX			72000000	// 72MHz
#define FCCO_MIN			275000000	// 275MHz
#define FCCO_MAX			550000000	// 550MHz
#define PLL_M_MIN		6
#define PLL_M_MAX		512
#define PLL_N_MIN		1
#define PLL_N_MAX		32


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
// CONTROLADOR DE ACESSO DA MEMÓRIA EXTERNA
#define STATIC_MEM0_BASE		0x80000000
#define STATIC_MEM1_BASE		0x81000000
#define STATIC_MEM2_BASE		0x82000000
#define STATIC_MEM3_BASE		0x83000000

#define DYNAMIC_MEM0_BASE		0xA0000000
#define DYNAMIC_MEM1_BASE		0xB0000000
#define DYNAMIC_MEM2_BASE		0xC0000000
#define DYNAMIC_MEM3_BASE		0xD0000000

// EXTERNAL MEMORY CONTROLLER (EMC)
#define EMC_BASE_ADDR		0xFFE08000
#define EMC_CTRL       		(*(reg32 *)(EMC_BASE_ADDR + 0x000))
#define EMC_STAT       		(*(reg32 *)(EMC_BASE_ADDR + 0x004))
#define EMC_CONFIG     	 	(*(reg32 *)(EMC_BASE_ADDR + 0x008))
#define EMC_STA_EXT_WAIT  	(*(reg32 *)(EMC_BASE_ADDR + 0x080))

// DYNAMIC RAM ACCESS REGISTERS
#define EMC_DYN_CTRL     	(*(reg32 *)(EMC_BASE_ADDR + 0x020))
#define EMC_DYN_RFSH     	(*(reg32 *)(EMC_BASE_ADDR + 0x024))
#define EMC_DYN_RD_CFG   	(*(reg32 *)(EMC_BASE_ADDR + 0x028))
#define EMC_DYN_RP      	(*(reg32 *)(EMC_BASE_ADDR + 0x030))
#define EMC_DYN_RAS      	(*(reg32 *)(EMC_BASE_ADDR + 0x034))
#define EMC_DYN_SREX     	(*(reg32 *)(EMC_BASE_ADDR + 0x038))
#define EMC_DYN_APR      	(*(reg32 *)(EMC_BASE_ADDR + 0x03C))
#define EMC_DYN_DAL      	(*(reg32 *)(EMC_BASE_ADDR + 0x040))
#define EMC_DYN_WR       	(*(reg32 *)(EMC_BASE_ADDR + 0x044))
#define EMC_DYN_RC       	(*(reg32 *)(EMC_BASE_ADDR + 0x048))
#define EMC_DYN_RFC      	(*(reg32 *)(EMC_BASE_ADDR + 0x04C))
#define EMC_DYN_XSR      	(*(reg32 *)(EMC_BASE_ADDR + 0x050))
#define EMC_DYN_RRD      	(*(reg32 *)(EMC_BASE_ADDR + 0x054))
#define EMC_DYN_MRD      	(*(reg32 *)(EMC_BASE_ADDR + 0x058))

#define EMC_DYN_CFG0     	(*(reg32 *)(EMC_BASE_ADDR + 0x100))
#define EMC_DYN_RASCAS0  	(*(reg32 *)(EMC_BASE_ADDR + 0x104))
#define EMC_DYN_CFG1     	(*(reg32 *)(EMC_BASE_ADDR + 0x120))
#define EMC_DYN_RASCAS1  	(*(reg32 *)(EMC_BASE_ADDR + 0x124))
#define EMC_DYN_CFG2     	(*(reg32 *)(EMC_BASE_ADDR + 0x140))
#define EMC_DYN_RASCAS2  	(*(reg32 *)(EMC_BASE_ADDR + 0x144))
#define EMC_DYN_CFG3     	(*(reg32 *)(EMC_BASE_ADDR + 0x160))
#define EMC_DYN_RASCAS3  	(*(reg32 *)(EMC_BASE_ADDR + 0x164))

// STATIC RAM ACCESS REGISTERS
#define EMC_STA_CFG0      	(*(reg32 *)(EMC_BASE_ADDR + 0x200))
#define EMC_STA_WAITWEN0  	(*(reg32 *)(EMC_BASE_ADDR + 0x204))
#define EMC_STA_WAITOEN0  	(*(reg32 *)(EMC_BASE_ADDR + 0x208))
#define EMC_STA_WAITRD0   	(*(reg32 *)(EMC_BASE_ADDR + 0x20C))
#define EMC_STA_WAITPAGE0 	(*(reg32 *)(EMC_BASE_ADDR + 0x210))
#define EMC_STA_WAITWR0   	(*(reg32 *)(EMC_BASE_ADDR + 0x214))
#define EMC_STA_WAITTURN0 	(*(reg32 *)(EMC_BASE_ADDR + 0x218))

#define EMC_STA_CFG1      	(*(reg32 *)(EMC_BASE_ADDR + 0x220))
#define EMC_STA_WAITWEN1  	(*(reg32 *)(EMC_BASE_ADDR + 0x224))
#define EMC_STA_WAITOEN1  	(*(reg32 *)(EMC_BASE_ADDR + 0x228))
#define EMC_STA_WAITRD1   	(*(reg32 *)(EMC_BASE_ADDR + 0x22C))
#define EMC_STA_WAITPAGE1 	(*(reg32 *)(EMC_BASE_ADDR + 0x230))
#define EMC_STA_WAITWR1   	(*(reg32 *)(EMC_BASE_ADDR + 0x234))
#define EMC_STA_WAITTURN1 	(*(reg32 *)(EMC_BASE_ADDR + 0x238))

#define EMC_STA_CFG2      	(*(reg32 *)(EMC_BASE_ADDR + 0x240))
#define EMC_STA_WAITWEN2  	(*(reg32 *)(EMC_BASE_ADDR + 0x244))
#define EMC_STA_WAITOEN2  	(*(reg32 *)(EMC_BASE_ADDR + 0x248))
#define EMC_STA_WAITRD2   	(*(reg32 *)(EMC_BASE_ADDR + 0x24C))
#define EMC_STA_WAITPAGE2 	(*(reg32 *)(EMC_BASE_ADDR + 0x250))
#define EMC_STA_WAITWR2   	(*(reg32 *)(EMC_BASE_ADDR + 0x254))
#define EMC_STA_WAITTURN2 	(*(reg32 *)(EMC_BASE_ADDR + 0x258))

#define EMC_STA_CFG3      	(*(reg32 *)(EMC_BASE_ADDR + 0x260))
#define EMC_STA_WAITWEN3  	(*(reg32 *)(EMC_BASE_ADDR + 0x264))
#define EMC_STA_WAITOEN3  	(*(reg32 *)(EMC_BASE_ADDR + 0x268))
#define EMC_STA_WAITRD3   	(*(reg32 *)(EMC_BASE_ADDR + 0x26C))
#define EMC_STA_WAITPAGE3 	(*(reg32 *)(EMC_BASE_ADDR + 0x270))
#define EMC_STA_WAITWR3   	(*(reg32 *)(EMC_BASE_ADDR + 0x274))
#define EMC_STA_WAITTURN3 	(*(reg32 *)(EMC_BASE_ADDR + 0x278))

// ########################################################################################################################################
// VECTORED INTERRUPT CONTROLLER
#define VIC_ENABLE      (1 << 5) 	// Bit para habilitar int VIC em VICVectPrioX

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
#define VIC_EMAC        21	
#define VIC_USB			22
#define VIC_CAN			23
#define VIC_MCI			24
#define VIC_GPDMA		25
#define VIC_TIMER2		26
#define VIC_TIMER3		27
#define VIC_UART2		28
#define VIC_UART3		29
#define VIC_I2C2		30
#define VIC_I2S			31

#define VIC_SIZE			32
#define VIC_BASE_ADDR		0xFFFFF000
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
#define VICSWPrioMask  (*(reg32 *)(VIC_BASE_ADDR + 0x024))

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
#define VICVectAddr16  (*(reg32 *)(VIC_BASE_ADDR + 0x140))		// Registrador do endereço do IRQ 16
#define VICVectAddr17  (*(reg32 *)(VIC_BASE_ADDR + 0x144))		// Registrador do endereço do IRQ 17
#define VICVectAddr18  (*(reg32 *)(VIC_BASE_ADDR + 0x148))		// Registrador do endereço do IRQ 18
#define VICVectAddr19  (*(reg32 *)(VIC_BASE_ADDR + 0x14C))		// Registrador do endereço do IRQ 19
#define VICVectAddr20  (*(reg32 *)(VIC_BASE_ADDR + 0x150))		// Registrador do endereço do IRQ 20
#define VICVectAddr21  (*(reg32 *)(VIC_BASE_ADDR + 0x154))		// Registrador do endereço do IRQ 21
#define VICVectAddr22  (*(reg32 *)(VIC_BASE_ADDR + 0x158))		// Registrador do endereço do IRQ 22
#define VICVectAddr23  (*(reg32 *)(VIC_BASE_ADDR + 0x15C))		// Registrador do endereço do IRQ 23
#define VICVectAddr24  (*(reg32 *)(VIC_BASE_ADDR + 0x160))		// Registrador do endereço do IRQ 24
#define VICVectAddr25  (*(reg32 *)(VIC_BASE_ADDR + 0x164))		// Registrador do endereço do IRQ 25
#define VICVectAddr26  (*(reg32 *)(VIC_BASE_ADDR + 0x168))		// Registrador do endereço do IRQ 26
#define VICVectAddr27  (*(reg32 *)(VIC_BASE_ADDR + 0x16C))		// Registrador do endereço do IRQ 27
#define VICVectAddr28  (*(reg32 *)(VIC_BASE_ADDR + 0x170))		// Registrador do endereço do IRQ 28
#define VICVectAddr29  (*(reg32 *)(VIC_BASE_ADDR + 0x174))		// Registrador do endereço do IRQ 29
#define VICVectAddr30  (*(reg32 *)(VIC_BASE_ADDR + 0x178))		// Registrador do endereço do IRQ 30
#define VICVectAddr31  (*(reg32 *)(VIC_BASE_ADDR + 0x17C))		// Registrador do endereço do IRQ 31

// PARA O LPC23xx E LPC24xx ESTES REFISTRADORES SÃO CHAMADOS DE VICVectPriority
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
#define VICVectCntl16  (*(reg32 *)(VIC_BASE_ADDR + 0x240))		// Registrador de controle do IRQ 16
#define VICVectCntl17  (*(reg32 *)(VIC_BASE_ADDR + 0x244))		// Registrador de controle do IRQ 17
#define VICVectCntl18  (*(reg32 *)(VIC_BASE_ADDR + 0x248))		// Registrador de controle do IRQ 18
#define VICVectCntl19  (*(reg32 *)(VIC_BASE_ADDR + 0x24C))		// Registrador de controle do IRQ 19
#define VICVectCntl20  (*(reg32 *)(VIC_BASE_ADDR + 0x250))		// Registrador de controle do IRQ 20
#define VICVectCntl21  (*(reg32 *)(VIC_BASE_ADDR + 0x254))		// Registrador de controle do IRQ 21
#define VICVectCntl22  (*(reg32 *)(VIC_BASE_ADDR + 0x258))		// Registrador de controle do IRQ 22
#define VICVectCntl23  (*(reg32 *)(VIC_BASE_ADDR + 0x25C))		// Registrador de controle do IRQ 23
#define VICVectCntl24  (*(reg32 *)(VIC_BASE_ADDR + 0x260))		// Registrador de controle do IRQ 24
#define VICVectCntl25  (*(reg32 *)(VIC_BASE_ADDR + 0x264))		// Registrador de controle do IRQ 25
#define VICVectCntl26  (*(reg32 *)(VIC_BASE_ADDR + 0x268))		// Registrador de controle do IRQ 26
#define VICVectCntl27  (*(reg32 *)(VIC_BASE_ADDR + 0x26C))		// Registrador de controle do IRQ 27
#define VICVectCntl28  (*(reg32 *)(VIC_BASE_ADDR + 0x270))		// Registrador de controle do IRQ 28
#define VICVectCntl29  (*(reg32 *)(VIC_BASE_ADDR + 0x274))		// Registrador de controle do IRQ 29
#define VICVectCntl30  (*(reg32 *)(VIC_BASE_ADDR + 0x278))		// Registrador de controle do IRQ 30
#define VICVectCntl31  (*(reg32 *)(VIC_BASE_ADDR + 0x27C))		// Registrador de controle do IRQ 31

// LPC23xx E LPC24xx
#define VICVectPriority0   (*(reg32 *)(VIC_BASE_ADDR + 0x200))
#define VICVectPriority1   (*(reg32 *)(VIC_BASE_ADDR + 0x204))
#define VICVectPriority2   (*(reg32 *)(VIC_BASE_ADDR + 0x208))
#define VICVectPriority3   (*(reg32 *)(VIC_BASE_ADDR + 0x20C))
#define VICVectPriority4   (*(reg32 *)(VIC_BASE_ADDR + 0x210))
#define VICVectPriority5   (*(reg32 *)(VIC_BASE_ADDR + 0x214))
#define VICVectPriority6   (*(reg32 *)(VIC_BASE_ADDR + 0x218))
#define VICVectPriority7   (*(reg32 *)(VIC_BASE_ADDR + 0x21C))
#define VICVectPriority8   (*(reg32 *)(VIC_BASE_ADDR + 0x220))
#define VICVectPriority9   (*(reg32 *)(VIC_BASE_ADDR + 0x224))
#define VICVectPriority10  (*(reg32 *)(VIC_BASE_ADDR + 0x228))
#define VICVectPriority11  (*(reg32 *)(VIC_BASE_ADDR + 0x22C))
#define VICVectPriority12  (*(reg32 *)(VIC_BASE_ADDR + 0x230))
#define VICVectPriority13  (*(reg32 *)(VIC_BASE_ADDR + 0x234))
#define VICVectPriority14  (*(reg32 *)(VIC_BASE_ADDR + 0x238))
#define VICVectPriority15  (*(reg32 *)(VIC_BASE_ADDR + 0x23C))
#define VICVectPriority16  (*(reg32 *)(VIC_BASE_ADDR + 0x240))
#define VICVectPriority17  (*(reg32 *)(VIC_BASE_ADDR + 0x244))
#define VICVectPriority18  (*(reg32 *)(VIC_BASE_ADDR + 0x248))
#define VICVectPriority19  (*(reg32 *)(VIC_BASE_ADDR + 0x24C))
#define VICVectPriority20  (*(reg32 *)(VIC_BASE_ADDR + 0x250))
#define VICVectPriority21  (*(reg32 *)(VIC_BASE_ADDR + 0x254))
#define VICVectPriority22  (*(reg32 *)(VIC_BASE_ADDR + 0x258))
#define VICVectPriority23  (*(reg32 *)(VIC_BASE_ADDR + 0x25C))
#define VICVectPriority24  (*(reg32 *)(VIC_BASE_ADDR + 0x260))
#define VICVectPriority25  (*(reg32 *)(VIC_BASE_ADDR + 0x264))
#define VICVectPriority26  (*(reg32 *)(VIC_BASE_ADDR + 0x268))
#define VICVectPriority27  (*(reg32 *)(VIC_BASE_ADDR + 0x26C))
#define VICVectPriority28  (*(reg32 *)(VIC_BASE_ADDR + 0x270))
#define VICVectPriority29  (*(reg32 *)(VIC_BASE_ADDR + 0x274))
#define VICVectPriority30  (*(reg32 *)(VIC_BASE_ADDR + 0x278))
#define VICVectPriority31  (*(reg32 *)(VIC_BASE_ADDR + 0x27C))

#define VICVectAddr    	   (*(reg32 *)(VIC_BASE_ADDR + 0xF00))


// ########################################################################################################################################
// SETUP FAST I/O 

#define FIO_BASE_ADDR		0x3FFFC000

#define FIO0DIR        (*(reg32 *)(FIO_BASE_ADDR + 0x00)) 
#define FIO0MASK       (*(reg32 *)(FIO_BASE_ADDR + 0x10))
#define FIO0PIN        (*(reg32 *)(FIO_BASE_ADDR + 0x14))
#define FIO0SET        (*(reg32 *)(FIO_BASE_ADDR + 0x18))
#define FIO0CLR        (*(reg32 *)(FIO_BASE_ADDR + 0x1C))

#define FIO1DIR        (*(reg32 *)(FIO_BASE_ADDR + 0x20)) 
#define FIO1MASK       (*(reg32 *)(FIO_BASE_ADDR + 0x30))
#define FIO1PIN        (*(reg32 *)(FIO_BASE_ADDR + 0x34))
#define FIO1SET        (*(reg32 *)(FIO_BASE_ADDR + 0x38))
#define FIO1CLR        (*(reg32 *)(FIO_BASE_ADDR + 0x3C))

#define FIO2DIR        (*(reg32 *)(FIO_BASE_ADDR + 0x40)) 
#define FIO2MASK       (*(reg32 *)(FIO_BASE_ADDR + 0x50))
#define FIO2PIN        (*(reg32 *)(FIO_BASE_ADDR + 0x54))
#define FIO2SET        (*(reg32 *)(FIO_BASE_ADDR + 0x58))
#define FIO2CLR        (*(reg32 *)(FIO_BASE_ADDR + 0x5C))

#define FIO3DIR        (*(reg32 *)(FIO_BASE_ADDR + 0x60)) 
#define FIO3MASK       (*(reg32 *)(FIO_BASE_ADDR + 0x70))
#define FIO3PIN        (*(reg32 *)(FIO_BASE_ADDR + 0x74))
#define FIO3SET        (*(reg32 *)(FIO_BASE_ADDR + 0x78))
#define FIO3CLR        (*(reg32 *)(FIO_BASE_ADDR + 0x7C))

#define FIO4DIR        (*(reg32 *)(FIO_BASE_ADDR + 0x80)) 
#define FIO4MASK       (*(reg32 *)(FIO_BASE_ADDR + 0x90))
#define FIO4PIN        (*(reg32 *)(FIO_BASE_ADDR + 0x94))
#define FIO4SET        (*(reg32 *)(FIO_BASE_ADDR + 0x98))
#define FIO4CLR        (*(reg32 *)(FIO_BASE_ADDR + 0x9C))

// FIOs can be accessed through WORD, HALF-WORD or BYTE.
#define FIO0DIR0       (*(reg8 *)(FIO_BASE_ADDR + 0x00)) 
#define FIO1DIR0       (*(reg8 *)(FIO_BASE_ADDR + 0x20)) 
#define FIO2DIR0       (*(reg8 *)(FIO_BASE_ADDR + 0x40)) 
#define FIO3DIR0       (*(reg8 *)(FIO_BASE_ADDR + 0x60)) 
#define FIO4DIR0       (*(reg8 *)(FIO_BASE_ADDR + 0x80)) 

#define FIO0DIR1       (*(reg8 *)(FIO_BASE_ADDR + 0x01)) 
#define FIO1DIR1       (*(reg8 *)(FIO_BASE_ADDR + 0x21)) 
#define FIO2DIR1       (*(reg8 *)(FIO_BASE_ADDR + 0x41)) 
#define FIO3DIR1       (*(reg8 *)(FIO_BASE_ADDR + 0x61)) 
#define FIO4DIR1       (*(reg8 *)(FIO_BASE_ADDR + 0x81)) 

#define FIO0DIR2       (*(reg8 *)(FIO_BASE_ADDR + 0x02)) 
#define FIO1DIR2       (*(reg8 *)(FIO_BASE_ADDR + 0x22)) 
#define FIO2DIR2       (*(reg8 *)(FIO_BASE_ADDR + 0x42)) 
#define FIO3DIR2       (*(reg8 *)(FIO_BASE_ADDR + 0x62)) 
#define FIO4DIR2       (*(reg8 *)(FIO_BASE_ADDR + 0x82)) 

#define FIO0DIR3       (*(reg8 *)(FIO_BASE_ADDR + 0x03)) 
#define FIO1DIR3       (*(reg8 *)(FIO_BASE_ADDR + 0x23)) 
#define FIO2DIR3       (*(reg8 *)(FIO_BASE_ADDR + 0x43)) 
#define FIO3DIR3       (*(reg8 *)(FIO_BASE_ADDR + 0x63)) 
#define FIO4DIR3       (*(reg8 *)(FIO_BASE_ADDR + 0x83)) 

#define FIO0DIRL       (*(reg16 *)(FIO_BASE_ADDR + 0x00)) 
#define FIO1DIRL       (*(reg16 *)(FIO_BASE_ADDR + 0x20)) 
#define FIO2DIRL       (*(reg16 *)(FIO_BASE_ADDR + 0x40)) 
#define FIO3DIRL       (*(reg16 *)(FIO_BASE_ADDR + 0x60)) 
#define FIO4DIRL       (*(reg16 *)(FIO_BASE_ADDR + 0x80)) 

#define FIO0DIRU       (*(reg16 *)(FIO_BASE_ADDR + 0x02)) 
#define FIO1DIRU       (*(reg16 *)(FIO_BASE_ADDR + 0x22)) 
#define FIO2DIRU       (*(reg16 *)(FIO_BASE_ADDR + 0x42)) 
#define FIO3DIRU       (*(reg16 *)(FIO_BASE_ADDR + 0x62)) 
#define FIO4DIRU       (*(reg16 *)(FIO_BASE_ADDR + 0x82)) 

#define FIO0MASK0      (*(reg8 *)(FIO_BASE_ADDR + 0x10)) 
#define FIO1MASK0      (*(reg8 *)(FIO_BASE_ADDR + 0x30)) 
#define FIO2MASK0      (*(reg8 *)(FIO_BASE_ADDR + 0x50)) 
#define FIO3MASK0      (*(reg8 *)(FIO_BASE_ADDR + 0x70)) 
#define FIO4MASK0      (*(reg8 *)(FIO_BASE_ADDR + 0x90)) 

#define FIO0MASK1      (*(reg8 *)(FIO_BASE_ADDR + 0x11)) 
#define FIO1MASK1      (*(reg8 *)(FIO_BASE_ADDR + 0x21)) 
#define FIO2MASK1      (*(reg8 *)(FIO_BASE_ADDR + 0x51)) 
#define FIO3MASK1      (*(reg8 *)(FIO_BASE_ADDR + 0x71)) 
#define FIO4MASK1      (*(reg8 *)(FIO_BASE_ADDR + 0x91)) 

#define FIO0MASK2      (*(reg8 *)(FIO_BASE_ADDR + 0x12)) 
#define FIO1MASK2      (*(reg8 *)(FIO_BASE_ADDR + 0x32)) 
#define FIO2MASK2      (*(reg8 *)(FIO_BASE_ADDR + 0x52)) 
#define FIO3MASK2      (*(reg8 *)(FIO_BASE_ADDR + 0x72)) 
#define FIO4MASK2      (*(reg8 *)(FIO_BASE_ADDR + 0x92)) 

#define FIO0MASK3      (*(reg8 *)(FIO_BASE_ADDR + 0x13)) 
#define FIO1MASK3      (*(reg8 *)(FIO_BASE_ADDR + 0x33)) 
#define FIO2MASK3      (*(reg8 *)(FIO_BASE_ADDR + 0x53)) 
#define FIO3MASK3      (*(reg8 *)(FIO_BASE_ADDR + 0x73)) 
#define FIO4MASK3      (*(reg8 *)(FIO_BASE_ADDR + 0x93)) 

#define FIO0MASKL      (*(reg16 *)(FIO_BASE_ADDR + 0x10)) 
#define FIO1MASKL      (*(reg16 *)(FIO_BASE_ADDR + 0x30)) 
#define FIO2MASKL      (*(reg16 *)(FIO_BASE_ADDR + 0x50)) 
#define FIO3MASKL      (*(reg16 *)(FIO_BASE_ADDR + 0x70)) 
#define FIO4MASKL      (*(reg16 *)(FIO_BASE_ADDR + 0x90)) 

#define FIO0MASKU      (*(reg16 *)(FIO_BASE_ADDR + 0x12)) 
#define FIO1MASKU      (*(reg16 *)(FIO_BASE_ADDR + 0x32)) 
#define FIO2MASKU      (*(reg16 *)(FIO_BASE_ADDR + 0x52)) 
#define FIO3MASKU      (*(reg16 *)(FIO_BASE_ADDR + 0x72)) 
#define FIO4MASKU      (*(reg16 *)(FIO_BASE_ADDR + 0x92)) 

#define FIO0PIN0       (*(reg8 *)(FIO_BASE_ADDR + 0x14)) 
#define FIO1PIN0       (*(reg8 *)(FIO_BASE_ADDR + 0x34)) 
#define FIO2PIN0       (*(reg8 *)(FIO_BASE_ADDR + 0x54)) 
#define FIO3PIN0       (*(reg8 *)(FIO_BASE_ADDR + 0x74)) 
#define FIO4PIN0       (*(reg8 *)(FIO_BASE_ADDR + 0x94)) 

#define FIO0PIN1       (*(reg8 *)(FIO_BASE_ADDR + 0x15)) 
#define FIO1PIN1       (*(reg8 *)(FIO_BASE_ADDR + 0x35)) 
#define FIO2PIN1       (*(reg8 *)(FIO_BASE_ADDR + 0x55)) 
#define FIO3PIN1       (*(reg8 *)(FIO_BASE_ADDR + 0x75)) 
#define FIO4PIN1       (*(reg8 *)(FIO_BASE_ADDR + 0x95)) 

#define FIO0PIN2       (*(reg8 *)(FIO_BASE_ADDR + 0x16)) 
#define FIO1PIN2       (*(reg8 *)(FIO_BASE_ADDR + 0x36)) 
#define FIO2PIN2       (*(reg8 *)(FIO_BASE_ADDR + 0x56)) 
#define FIO3PIN2       (*(reg8 *)(FIO_BASE_ADDR + 0x76)) 
#define FIO4PIN2       (*(reg8 *)(FIO_BASE_ADDR + 0x96)) 

#define FIO0PIN3       (*(reg8 *)(FIO_BASE_ADDR + 0x17)) 
#define FIO1PIN3       (*(reg8 *)(FIO_BASE_ADDR + 0x37)) 
#define FIO2PIN3       (*(reg8 *)(FIO_BASE_ADDR + 0x57)) 
#define FIO3PIN3       (*(reg8 *)(FIO_BASE_ADDR + 0x77)) 
#define FIO4PIN3       (*(reg8 *)(FIO_BASE_ADDR + 0x97)) 

#define FIO0PINL       (*(reg16 *)(FIO_BASE_ADDR + 0x14)) 
#define FIO1PINL       (*(reg16 *)(FIO_BASE_ADDR + 0x34)) 
#define FIO2PINL       (*(reg16 *)(FIO_BASE_ADDR + 0x54)) 
#define FIO3PINL       (*(reg16 *)(FIO_BASE_ADDR + 0x74)) 
#define FIO4PINL       (*(reg16 *)(FIO_BASE_ADDR + 0x94)) 

#define FIO0PINU       (*(reg16 *)(FIO_BASE_ADDR + 0x16)) 
#define FIO1PINU       (*(reg16 *)(FIO_BASE_ADDR + 0x36)) 
#define FIO2PINU       (*(reg16 *)(FIO_BASE_ADDR + 0x56)) 
#define FIO3PINU       (*(reg16 *)(FIO_BASE_ADDR + 0x76)) 
#define FIO4PINU       (*(reg16 *)(FIO_BASE_ADDR + 0x96)) 

#define FIO0SET0       (*(reg8 *)(FIO_BASE_ADDR + 0x18)) 
#define FIO1SET0       (*(reg8 *)(FIO_BASE_ADDR + 0x38)) 
#define FIO2SET0       (*(reg8 *)(FIO_BASE_ADDR + 0x58)) 
#define FIO3SET0       (*(reg8 *)(FIO_BASE_ADDR + 0x78)) 
#define FIO4SET0       (*(reg8 *)(FIO_BASE_ADDR + 0x98)) 

#define FIO0SET1       (*(reg8 *)(FIO_BASE_ADDR + 0x19)) 
#define FIO1SET1       (*(reg8 *)(FIO_BASE_ADDR + 0x29)) 
#define FIO2SET1       (*(reg8 *)(FIO_BASE_ADDR + 0x59)) 
#define FIO3SET1       (*(reg8 *)(FIO_BASE_ADDR + 0x79)) 
#define FIO4SET1       (*(reg8 *)(FIO_BASE_ADDR + 0x99)) 

#define FIO0SET2       (*(reg8 *)(FIO_BASE_ADDR + 0x1A)) 
#define FIO1SET2       (*(reg8 *)(FIO_BASE_ADDR + 0x3A)) 
#define FIO2SET2       (*(reg8 *)(FIO_BASE_ADDR + 0x5A)) 
#define FIO3SET2       (*(reg8 *)(FIO_BASE_ADDR + 0x7A)) 
#define FIO4SET2       (*(reg8 *)(FIO_BASE_ADDR + 0x9A)) 

#define FIO0SET3       (*(reg8 *)(FIO_BASE_ADDR + 0x1B)) 
#define FIO1SET3       (*(reg8 *)(FIO_BASE_ADDR + 0x3B)) 
#define FIO2SET3       (*(reg8 *)(FIO_BASE_ADDR + 0x5B)) 
#define FIO3SET3       (*(reg8 *)(FIO_BASE_ADDR + 0x7B)) 
#define FIO4SET3       (*(reg8 *)(FIO_BASE_ADDR + 0x9B)) 

#define FIO0SETL       (*(reg16 *)(FIO_BASE_ADDR + 0x18)) 
#define FIO1SETL       (*(reg16 *)(FIO_BASE_ADDR + 0x38)) 
#define FIO2SETL       (*(reg16 *)(FIO_BASE_ADDR + 0x58)) 
#define FIO3SETL       (*(reg16 *)(FIO_BASE_ADDR + 0x78)) 
#define FIO4SETL       (*(reg16 *)(FIO_BASE_ADDR + 0x98)) 

#define FIO0SETU       (*(reg16 *)(FIO_BASE_ADDR + 0x1A)) 
#define FIO1SETU       (*(reg16 *)(FIO_BASE_ADDR + 0x3A)) 
#define FIO2SETU       (*(reg16 *)(FIO_BASE_ADDR + 0x5A)) 
#define FIO3SETU       (*(reg16 *)(FIO_BASE_ADDR + 0x7A)) 
#define FIO4SETU       (*(reg16 *)(FIO_BASE_ADDR + 0x9A)) 

#define FIO0CLR0       (*(reg8 *)(FIO_BASE_ADDR + 0x1C)) 
#define FIO1CLR0       (*(reg8 *)(FIO_BASE_ADDR + 0x3C)) 
#define FIO2CLR0       (*(reg8 *)(FIO_BASE_ADDR + 0x5C)) 
#define FIO3CLR0       (*(reg8 *)(FIO_BASE_ADDR + 0x7C)) 
#define FIO4CLR0       (*(reg8 *)(FIO_BASE_ADDR + 0x9C)) 

#define FIO0CLR1       (*(reg8 *)(FIO_BASE_ADDR + 0x1D)) 
#define FIO1CLR1       (*(reg8 *)(FIO_BASE_ADDR + 0x2D)) 
#define FIO2CLR1       (*(reg8 *)(FIO_BASE_ADDR + 0x5D)) 
#define FIO3CLR1       (*(reg8 *)(FIO_BASE_ADDR + 0x7D)) 
#define FIO4CLR1       (*(reg8 *)(FIO_BASE_ADDR + 0x9D)) 

#define FIO0CLR2       (*(reg8 *)(FIO_BASE_ADDR + 0x1E)) 
#define FIO1CLR2       (*(reg8 *)(FIO_BASE_ADDR + 0x3E)) 
#define FIO2CLR2       (*(reg8 *)(FIO_BASE_ADDR + 0x5E)) 
#define FIO3CLR2       (*(reg8 *)(FIO_BASE_ADDR + 0x7E)) 
#define FIO4CLR2       (*(reg8 *)(FIO_BASE_ADDR + 0x9E)) 

#define FIO0CLR3       (*(reg8 *)(FIO_BASE_ADDR + 0x1F)) 
#define FIO1CLR3       (*(reg8 *)(FIO_BASE_ADDR + 0x3F)) 
#define FIO2CLR3       (*(reg8 *)(FIO_BASE_ADDR + 0x5F)) 
#define FIO3CLR3       (*(reg8 *)(FIO_BASE_ADDR + 0x7F)) 
#define FIO4CLR3       (*(reg8 *)(FIO_BASE_ADDR + 0x9F)) 

#define FIO0CLRL       (*(reg16 *)(FIO_BASE_ADDR + 0x1C)) 
#define FIO1CLRL       (*(reg16 *)(FIO_BASE_ADDR + 0x3C)) 
#define FIO2CLRL       (*(reg16 *)(FIO_BASE_ADDR + 0x5C)) 
#define FIO3CLRL       (*(reg16 *)(FIO_BASE_ADDR + 0x7C)) 
#define FIO4CLRL       (*(reg16 *)(FIO_BASE_ADDR + 0x9C)) 

#define FIO0CLRU       (*(reg16 *)(FIO_BASE_ADDR + 0x1E)) 
#define FIO1CLRU       (*(reg16 *)(FIO_BASE_ADDR + 0x3E)) 
#define FIO2CLRU       (*(reg16 *)(FIO_BASE_ADDR + 0x5E)) 
#define FIO3CLRU       (*(reg16 *)(FIO_BASE_ADDR + 0x7E)) 
#define FIO4CLRU       (*(reg16 *)(FIO_BASE_ADDR + 0x9E)) 

// ########################################################################################################################################
// CAN CONTROLLERS AND ACCEPTANCE FILTER 
#define CAN_ACCEPT_BASE_ADDR		0xE003C000
#define CAN_AFMR		(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x00))  	
#define CAN_SFF_SA 		(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x04))  	
#define CAN_SFF_GRP_SA 	(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x08))
#define CAN_EFF_SA 		(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x0C))
#define CAN_EFF_GRP_SA 	(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x10))  	
#define CAN_EOT 		(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x14))
#define CAN_LUT_ERR_ADR (*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x18))  	
#define CAN_LUT_ERR 	(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x1C))

#define CAN_CENTRAL_BASE_ADDR		0xE0040000  	
#define CAN_TX_SR 	(*(reg32 *)(CAN_CENTRAL_BASE_ADDR + 0x00))  	
#define CAN_RX_SR 	(*(reg32 *)(CAN_CENTRAL_BASE_ADDR + 0x04))  	
#define CAN_MSR 	(*(reg32 *)(CAN_CENTRAL_BASE_ADDR + 0x08))

#define CAN1_BASE_ADDR		0xE0044000
#define CAN1MOD 	(*(reg32 *)(CAN1_BASE_ADDR + 0x00))  	
#define CAN1CMR 	(*(reg32 *)(CAN1_BASE_ADDR + 0x04))  	
#define CAN1GSR 	(*(reg32 *)(CAN1_BASE_ADDR + 0x08))  	
#define CAN1ICR 	(*(reg32 *)(CAN1_BASE_ADDR + 0x0C))  	
#define CAN1IER 	(*(reg32 *)(CAN1_BASE_ADDR + 0x10))
#define CAN1BTR 	(*(reg32 *)(CAN1_BASE_ADDR + 0x14))  	
#define CAN1EWL 	(*(reg32 *)(CAN1_BASE_ADDR + 0x18))  	
#define CAN1SR 		(*(reg32 *)(CAN1_BASE_ADDR + 0x1C))  	
#define CAN1RFS 	(*(reg32 *)(CAN1_BASE_ADDR + 0x20))  	
#define CAN1RID 	(*(reg32 *)(CAN1_BASE_ADDR + 0x24))
#define CAN1RDA 	(*(reg32 *)(CAN1_BASE_ADDR + 0x28))  	
#define CAN1RDB 	(*(reg32 *)(CAN1_BASE_ADDR + 0x2C))
  	
#define CAN1TFI1 	(*(reg32 *)(CAN1_BASE_ADDR + 0x30))  	
#define CAN1TID1 	(*(reg32 *)(CAN1_BASE_ADDR + 0x34))  	
#define CAN1TDA1 	(*(reg32 *)(CAN1_BASE_ADDR + 0x38))
#define CAN1TDB1 	(*(reg32 *)(CAN1_BASE_ADDR + 0x3C))  	
#define CAN1TFI2 	(*(reg32 *)(CAN1_BASE_ADDR + 0x40))  	
#define CAN1TID2 	(*(reg32 *)(CAN1_BASE_ADDR + 0x44))  	
#define CAN1TDA2 	(*(reg32 *)(CAN1_BASE_ADDR + 0x48))  	
#define CAN1TDB2 	(*(reg32 *)(CAN1_BASE_ADDR + 0x4C))
#define CAN1TFI3 	(*(reg32 *)(CAN1_BASE_ADDR + 0x50))  	
#define CAN1TID3 	(*(reg32 *)(CAN1_BASE_ADDR + 0x54))  	
#define CAN1TDA3 	(*(reg32 *)(CAN1_BASE_ADDR + 0x58))  	
#define CAN1TDB3 	(*(reg32 *)(CAN1_BASE_ADDR + 0x5C))

#define CAN2_BASE_ADDR		0xE0048000
#define CAN2MOD 	(*(reg32 *)(CAN2_BASE_ADDR + 0x00))  	
#define CAN2CMR 	(*(reg32 *)(CAN2_BASE_ADDR + 0x04))  	
#define CAN2GSR 	(*(reg32 *)(CAN2_BASE_ADDR + 0x08))  	
#define CAN2ICR 	(*(reg32 *)(CAN2_BASE_ADDR + 0x0C))  	
#define CAN2IER 	(*(reg32 *)(CAN2_BASE_ADDR + 0x10))
#define CAN2BTR 	(*(reg32 *)(CAN2_BASE_ADDR + 0x14))  	
#define CAN2EWL 	(*(reg32 *)(CAN2_BASE_ADDR + 0x18))  	
#define CAN2SR 		(*(reg32 *)(CAN2_BASE_ADDR + 0x1C))  	
#define CAN2RFS 	(*(reg32 *)(CAN2_BASE_ADDR + 0x20))  	
#define CAN2RID 	(*(reg32 *)(CAN2_BASE_ADDR + 0x24))
#define CAN2RDA 	(*(reg32 *)(CAN2_BASE_ADDR + 0x28))  	
#define CAN2RDB 	(*(reg32 *)(CAN2_BASE_ADDR + 0x2C))
  	
#define CAN2TFI1 	(*(reg32 *)(CAN2_BASE_ADDR + 0x30))  	
#define CAN2TID1 	(*(reg32 *)(CAN2_BASE_ADDR + 0x34))  	
#define CAN2TDA1 	(*(reg32 *)(CAN2_BASE_ADDR + 0x38))
#define CAN2TDB1 	(*(reg32 *)(CAN2_BASE_ADDR + 0x3C))  	
#define CAN2TFI2 	(*(reg32 *)(CAN2_BASE_ADDR + 0x40))  	
#define CAN2TID2 	(*(reg32 *)(CAN2_BASE_ADDR + 0x44))  	
#define CAN2TDA2 	(*(reg32 *)(CAN2_BASE_ADDR + 0x48))  	
#define CAN2TDB2 	(*(reg32 *)(CAN2_BASE_ADDR + 0x4C))
#define CAN2TFI3 	(*(reg32 *)(CAN2_BASE_ADDR + 0x50))  	
#define CAN2TID3 	(*(reg32 *)(CAN2_BASE_ADDR + 0x54))  	
#define CAN2TDA3 	(*(reg32 *)(CAN2_BASE_ADDR + 0x58))  	
#define CAN2TDB3 	(*(reg32 *)(CAN2_BASE_ADDR + 0x5C))

// ########################################################################################################################################
// MultiMedia Card Interface(MCI) Controller 
#define MCI_BASE_ADDR		0xE008C000
#define MCI_POWER      (*(reg32 *)(MCI_BASE_ADDR + 0x00))
#define MCI_CLOCK      (*(reg32 *)(MCI_BASE_ADDR + 0x04))
#define MCI_ARGUMENT   (*(reg32 *)(MCI_BASE_ADDR + 0x08))
#define MCI_COMMAND    (*(reg32 *)(MCI_BASE_ADDR + 0x0C))
#define MCI_RESP_CMD   (*(reg32 *)(MCI_BASE_ADDR + 0x10))
#define MCI_RESP0      (*(reg32 *)(MCI_BASE_ADDR + 0x14))
#define MCI_RESP1      (*(reg32 *)(MCI_BASE_ADDR + 0x18))
#define MCI_RESP2      (*(reg32 *)(MCI_BASE_ADDR + 0x1C))
#define MCI_RESP3      (*(reg32 *)(MCI_BASE_ADDR + 0x20))
#define MCI_DATA_TMR   (*(reg32 *)(MCI_BASE_ADDR + 0x24))
#define MCI_DATA_LEN   (*(reg32 *)(MCI_BASE_ADDR + 0x28))
#define MCI_DATA_CTRL  (*(reg32 *)(MCI_BASE_ADDR + 0x2C))
#define MCI_DATA_CNT   (*(reg32 *)(MCI_BASE_ADDR + 0x30))
#define MCI_STATUS     (*(reg32 *)(MCI_BASE_ADDR + 0x34))
#define MCI_CLEAR      (*(reg32 *)(MCI_BASE_ADDR + 0x38))
#define MCI_MASK0      (*(reg32 *)(MCI_BASE_ADDR + 0x3C))
#define MCI_FIFO_CNT   (*(reg32 *)(MCI_BASE_ADDR + 0x48))
#define MCI_FIFO       (*(reg32 *)(MCI_BASE_ADDR + 0x80))

// ########################################################################################################################################
// I2S Interface Controller (I2S) 
#define I2S_BASE_ADDR		0xE0088000
#define I2S_DAO        (*(reg32 *)(I2S_BASE_ADDR + 0x00))
#define I2S_DAI        (*(reg32 *)(I2S_BASE_ADDR + 0x04))
#define I2S_TX_FIFO    (*(reg32 *)(I2S_BASE_ADDR + 0x08))
#define I2S_RX_FIFO    (*(reg32 *)(I2S_BASE_ADDR + 0x0C))
#define I2S_STATE      (*(reg32 *)(I2S_BASE_ADDR + 0x10))
#define I2S_DMA1       (*(reg32 *)(I2S_BASE_ADDR + 0x14))
#define I2S_DMA2       (*(reg32 *)(I2S_BASE_ADDR + 0x18))
#define I2S_IRQ        (*(reg32 *)(I2S_BASE_ADDR + 0x1C))
#define I2S_TXRATE     (*(reg32 *)(I2S_BASE_ADDR + 0x20))
#define I2S_RXRATE     (*(reg32 *)(I2S_BASE_ADDR + 0x24))

// ########################################################################################################################################
// General-purpose DMA Controller 
#define DMA_BASE_ADDR		0xFFE04000
#define GPDMA_INT_STAT         (*(reg32 *)(DMA_BASE_ADDR + 0x000))
#define GPDMA_INT_TCSTAT       (*(reg32 *)(DMA_BASE_ADDR + 0x004))
#define GPDMA_INT_TCCLR        (*(reg32 *)(DMA_BASE_ADDR + 0x008))
#define GPDMA_INT_ERR_STAT     (*(reg32 *)(DMA_BASE_ADDR + 0x00C))
#define GPDMA_INT_ERR_CLR      (*(reg32 *)(DMA_BASE_ADDR + 0x010))
#define GPDMA_RAW_INT_TCSTAT   (*(reg32 *)(DMA_BASE_ADDR + 0x014))
#define GPDMA_RAW_INT_ERR_STAT (*(reg32 *)(DMA_BASE_ADDR + 0x018))
#define GPDMA_ENABLED_CHNS     (*(reg32 *)(DMA_BASE_ADDR + 0x01C))
#define GPDMA_SOFT_BREQ        (*(reg32 *)(DMA_BASE_ADDR + 0x020))
#define GPDMA_SOFT_SREQ        (*(reg32 *)(DMA_BASE_ADDR + 0x024))
#define GPDMA_SOFT_LBREQ       (*(reg32 *)(DMA_BASE_ADDR + 0x028))
#define GPDMA_SOFT_LSREQ       (*(reg32 *)(DMA_BASE_ADDR + 0x02C))
#define GPDMA_CONFIG           (*(reg32 *)(DMA_BASE_ADDR + 0x030))
#define GPDMA_SYNC             (*(reg32 *)(DMA_BASE_ADDR + 0x034))

// DMA channel 0 registers 
#define GPDMA_CH0_SRC      (*(reg32 *)(DMA_BASE_ADDR + 0x100))
#define GPDMA_CH0_DEST     (*(reg32 *)(DMA_BASE_ADDR + 0x104))
#define GPDMA_CH0_LLI      (*(reg32 *)(DMA_BASE_ADDR + 0x108))
#define GPDMA_CH0_CTRL     (*(reg32 *)(DMA_BASE_ADDR + 0x10C))
#define GPDMA_CH0_CFG      (*(reg32 *)(DMA_BASE_ADDR + 0x110))

// DMA channel 1 registers 
#define GPDMA_CH1_SRC      (*(reg32 *)(DMA_BASE_ADDR + 0x120))
#define GPDMA_CH1_DEST     (*(reg32 *)(DMA_BASE_ADDR + 0x124))
#define GPDMA_CH1_LLI      (*(reg32 *)(DMA_BASE_ADDR + 0x128))
#define GPDMA_CH1_CTRL     (*(reg32 *)(DMA_BASE_ADDR + 0x12C))
#define GPDMA_CH1_CFG      (*(reg32 *)(DMA_BASE_ADDR + 0x130))

// ########################################################################################################################################
// USB Controller 
#define USB_INT_BASE_ADDR	0xE01FC1C0
#define USB_BASE_ADDR		0xFFE0C200		// Endereço base dos registradores USB
#define USB_RAM_BASE		0x7FD00000		// Endereço base da SRAM interna reservado para o USB

#define USB_INT_STAT    (*(reg32 *)(USB_INT_BASE_ADDR + 0x00))

// USB Device Interrupt Registers 
#define DEV_INT_STAT    (*(reg32 *)(USB_BASE_ADDR + 0x00))
#define DEV_INT_EN      (*(reg32 *)(USB_BASE_ADDR + 0x04))
#define DEV_INT_CLR     (*(reg32 *)(USB_BASE_ADDR + 0x08))
#define DEV_INT_SET     (*(reg32 *)(USB_BASE_ADDR + 0x0C))
#define DEV_INT_PRIO    (*(reg32 *)(USB_BASE_ADDR + 0x2C))

// USB Device Endpoint Interrupt Registers 
#define EP_INT_STAT     (*(reg32 *)(USB_BASE_ADDR + 0x30))
#define EP_INT_EN       (*(reg32 *)(USB_BASE_ADDR + 0x34))
#define EP_INT_CLR      (*(reg32 *)(USB_BASE_ADDR + 0x38))
#define EP_INT_SET      (*(reg32 *)(USB_BASE_ADDR + 0x3C))
#define EP_INT_PRIO     (*(reg32 *)(USB_BASE_ADDR + 0x40))

// USB Device Endpoint Realization Registers 
#define REALIZE_EP      (*(reg32 *)(USB_BASE_ADDR + 0x44))
#define EP_INDEX        (*(reg32 *)(USB_BASE_ADDR + 0x48))
#define MAXPACKET_SIZE  (*(reg32 *)(USB_BASE_ADDR + 0x4C))

// USB Device Command Reagisters 
#define CMD_CODE        (*(reg32 *)(USB_BASE_ADDR + 0x10))
#define CMD_DATA        (*(reg32 *)(USB_BASE_ADDR + 0x14))

// USB Device Data Transfer Registers 
#define RX_DATA         (*(reg32 *)(USB_BASE_ADDR + 0x18))
#define TX_DATA         (*(reg32 *)(USB_BASE_ADDR + 0x1C))
#define RX_PLENGTH      (*(reg32 *)(USB_BASE_ADDR + 0x20))
#define TX_PLENGTH      (*(reg32 *)(USB_BASE_ADDR + 0x24))
#define USB_CTRL        (*(reg32 *)(USB_BASE_ADDR + 0x28))

// USB Device DMA Registers 
#define DMA_REQ_STAT        (*(reg32 *)(USB_BASE_ADDR + 0x50))
#define DMA_REQ_CLR         (*(reg32 *)(USB_BASE_ADDR + 0x54))
#define DMA_REQ_SET         (*(reg32 *)(USB_BASE_ADDR + 0x58))
#define UDCA_HEAD           (*(reg32 *)(USB_BASE_ADDR + 0x80))
#define EP_DMA_STAT         (*(reg32 *)(USB_BASE_ADDR + 0x84))
#define EP_DMA_EN           (*(reg32 *)(USB_BASE_ADDR + 0x88))
#define EP_DMA_DIS          (*(reg32 *)(USB_BASE_ADDR + 0x8C))
#define DMA_INT_STAT        (*(reg32 *)(USB_BASE_ADDR + 0x90))
#define DMA_INT_EN          (*(reg32 *)(USB_BASE_ADDR + 0x94))
#define EOT_INT_STAT        (*(reg32 *)(USB_BASE_ADDR + 0xA0))
#define EOT_INT_CLR         (*(reg32 *)(USB_BASE_ADDR + 0xA4))
#define EOT_INT_SET         (*(reg32 *)(USB_BASE_ADDR + 0xA8))
#define NDD_REQ_INT_STAT    (*(reg32 *)(USB_BASE_ADDR + 0xAC))
#define NDD_REQ_INT_CLR     (*(reg32 *)(USB_BASE_ADDR + 0xB0))
#define NDD_REQ_INT_SET     (*(reg32 *)(USB_BASE_ADDR + 0xB4))
#define SYS_ERR_INT_STAT    (*(reg32 *)(USB_BASE_ADDR + 0xB8))
#define SYS_ERR_INT_CLR     (*(reg32 *)(USB_BASE_ADDR + 0xBC))
#define SYS_ERR_INT_SET     (*(reg32 *)(USB_BASE_ADDR + 0xC0))

// USB Host and OTG registers are for LPC24xx only 
// USB Host Controller 
#define USBHC_BASE_ADDR		0xFFE0C000
#define HcRevision         (*(reg32 *)(USBHC_BASE_ADDR + 0x00))
#define HcControl          	(*(reg32 *)(USBHC_BASE_ADDR + 0x04))
	#define  HC_OR_CONTROL_CLE                 0x00000010
	#define  HC_OR_CONTROL_BLE                 0x00000020
	#define  HC_OR_CONTROL_HCFS                0x000000C0
	#define  HC_OR_CONTROL_HC_OPER             0x00000080
#define HcCommandStatus     (*(reg32 *)(USBHC_BASE_ADDR + 0x08))
	#define  HC_OR_CMD_STATUS_HCR   0x00000001
	#define  HC_OR_CMD_STATUS_CLF   0x00000002
	#define  HC_OR_CMD_STATUS_BLF   0x00000004

#define HcInterruptStatus         (*(reg32 *)(USBHC_BASE_ADDR + 0x0C))
	#define  HC_OR_INTR_STATUS_WDH             0x00000002
	#define  HC_OR_INTR_STATUS_RHSC            0x00000040

#define HcInterruptEnable           (*(reg32 *)(USBHC_BASE_ADDR + 0x10))
	#define  HC_OR_INTR_ENABLE_WDH             0x00000002
	#define  HC_OR_INTR_ENABLE_RHSC            0x00000040
	#define  HC_OR_INTR_ENABLE_MIE             0x80000000

#define HcInterruptDisable          (*(reg32 *)(USBHC_BASE_ADDR + 0x14))
#define HcHCCA             	(*(reg32 *)(USBHC_BASE_ADDR + 0x18))
#define HcPeriodCurrentED   (*(reg32 *)(USBHC_BASE_ADDR + 0x1C))
#define HcControlHeadED     (*(reg32 *)(USBHC_BASE_ADDR + 0x20))
#define HcControlCurrentED  (*(reg32 *)(USBHC_BASE_ADDR + 0x24))
#define HcBulkHeadED	    (*(reg32 *)(USBHC_BASE_ADDR + 0x28))
#define HcBulkCurrentED     (*(reg32 *)(USBHC_BASE_ADDR + 0x2C))
#define HcDoneHead        	(*(reg32 *)(USBHC_BASE_ADDR + 0x30))
#define HcFmInterval      	(*(reg32 *)(USBHC_BASE_ADDR + 0x34))
#define HcFmRemaining     	(*(reg32 *)(USBHC_BASE_ADDR + 0x38))
#define HcFmNumber        	(*(reg32 *)(USBHC_BASE_ADDR + 0x3C))
#define HcPeriodicStart     (*(reg32 *)(USBHC_BASE_ADDR + 0x40))
#define HcLSThreshold       (*(reg32 *)(USBHC_BASE_ADDR + 0x44))
#define HcRhDescriptorA     (*(reg32 *)(USBHC_BASE_ADDR + 0x48))
#define HcRhDescriptorB     (*(reg32 *)(USBHC_BASE_ADDR + 0x4C))
#define HcRhStatus          (*(reg32 *)(USBHC_BASE_ADDR + 0x50))
	#define  HC_OR_RH_STATUS_LPSC              0x00010000
	#define  HC_OR_RH_STATUS_DRWE              0x00008000

#define HcRhPortStatus1    	(*(reg32 *)(USBHC_BASE_ADDR + 0x54))
#define HcRhPortStatus2    	(*(reg32 *)(USBHC_BASE_ADDR + 0x58))
	#define  HC_OR_RH_PORT_CCS                 0x00000001
	#define  HC_OR_RH_PORT_PRS                 0x00000010
	#define  HC_OR_RH_PORT_CSC                 0x00010000
	#define  HC_OR_RH_PORT_PRSC                0x00100000

// USB OTG Controller 
#define USBOTG_BASE_ADDR	0xFFE0C100
#define OTG_INT_STAT        (*(reg32 *)(USBOTG_BASE_ADDR + 0x00))
#define OTG_INT_EN          (*(reg32 *)(USBOTG_BASE_ADDR + 0x04))
#define OTG_INT_SET         (*(reg32 *)(USBOTG_BASE_ADDR + 0x08))
#define OTG_INT_CLR         (*(reg32 *)(USBOTG_BASE_ADDR + 0x0C))
// On LPC23xx, the name is USBPortSel, on LPC24xx, the name is OTG_STAT_CTRL  
#define OTG_STAT_CTRL       (*(reg32 *)(USBOTG_BASE_ADDR + 0x10))
#define OTG_TIMER           (*(reg32 *)(USBOTG_BASE_ADDR + 0x14))

#define USBOTG_I2C_BASE_ADDR	0xFFE0C300
#define OTG_I2C_RX          (*(reg32 *)(USBOTG_I2C_BASE_ADDR + 0x00))
#define OTG_I2C_TX          (*(reg32 *)(USBOTG_I2C_BASE_ADDR + 0x00))
#define OTG_I2C_STS         (*(reg32 *)(USBOTG_I2C_BASE_ADDR + 0x04))
#define OTG_I2C_CTL         (*(reg32 *)(USBOTG_I2C_BASE_ADDR + 0x08))
#define OTG_I2C_CLKHI       (*(reg32 *)(USBOTG_I2C_BASE_ADDR + 0x0C))
#define OTG_I2C_CLKLO       (*(reg32 *)(USBOTG_I2C_BASE_ADDR + 0x10))

// On LPC23xx, the names are USBClkCtrl and USBClkSt; on LPC24xx, the names are OTG_CLK_CTRL and OTG_CLK_STAT respectively. 
#define USBOTG_CLK_BASE_ADDR	0xFFE0CFF0
#define OTG_CLK_CTRL        (*(reg32 *)(USBOTG_CLK_BASE_ADDR + 0x04))
#define OTG_CLK_STAT        (*(reg32 *)(USBOTG_CLK_BASE_ADDR + 0x08))

// Note: below three register name convention is for LPC23xx USB device only, match with the spec. update in USB Device Section.  
#define OTGStCtrl          	(*(reg32 *)(USBOTG_BASE_ADDR + 0x10))
#define USBClkCtrl          (*(reg32 *)(USBOTG_CLK_BASE_ADDR + 0x04))
#define USBClkSt            (*(reg32 *)(USBOTG_CLK_BASE_ADDR + 0x08))

// ########################################################################################################################################
// Ethernet MAC (32 bit data bus) -- all registers are RW unless indicated in parentheses 
#define MAC_BASE_ADDR		0xFFE00000 // AHB Peripheral # 0 
#define MAC_MAC1            (*(reg32 *)(MAC_BASE_ADDR + 0x000)) // MAC config reg 1 
#define MAC_MAC2            (*(reg32 *)(MAC_BASE_ADDR + 0x004)) // MAC config reg 2
#define MAC_IPGT            (*(reg32 *)(MAC_BASE_ADDR + 0x008)) // b2b InterPacketGap reg
#define MAC_IPGR            (*(reg32 *)(MAC_BASE_ADDR + 0x00C)) // non b2b InterPacketGap reg
#define MAC_CLRT            (*(reg32 *)(MAC_BASE_ADDR + 0x010)) // CoLlision window/ReTry reg
#define MAC_MAXF            (*(reg32 *)(MAC_BASE_ADDR + 0x014)) // MAXimum Frame reg
#define MAC_SUPP            (*(reg32 *)(MAC_BASE_ADDR + 0x018)) // PHY SUPPort reg
#define MAC_TEST            (*(reg32 *)(MAC_BASE_ADDR + 0x01C)) // TEST reg
#define MAC_MCFG            (*(reg32 *)(MAC_BASE_ADDR + 0x020)) // MII Mgmt ConFiG reg
#define MAC_MCMD            (*(reg32 *)(MAC_BASE_ADDR + 0x024)) // MII Mgmt CoMmanD reg
#define MAC_MADR            (*(reg32 *)(MAC_BASE_ADDR + 0x028)) // MII Mgmt ADdRess reg 
#define MAC_MWTD            (*(reg32 *)(MAC_BASE_ADDR + 0x02C)) // MII Mgmt WriTe Data reg (WO) 
#define MAC_MRDD            (*(reg32 *)(MAC_BASE_ADDR + 0x030)) // MII Mgmt ReaD Data reg (RO) 
#define MAC_MIND            (*(reg32 *)(MAC_BASE_ADDR + 0x034)) // MII Mgmt INDicators reg (RO) 

#define MAC_SA0             (*(reg32 *)(MAC_BASE_ADDR + 0x040)) // Station Address 0 reg 
#define MAC_SA1             (*(reg32 *)(MAC_BASE_ADDR + 0x044)) // Station Address 1 reg 
#define MAC_SA2             (*(reg32 *)(MAC_BASE_ADDR + 0x048)) // Station Address 2 reg 

#define MAC_COMMAND         (*(reg32 *)(MAC_BASE_ADDR + 0x100)) // Command reg 
#define MAC_STATUS          (*(reg32 *)(MAC_BASE_ADDR + 0x104)) // Status reg (RO) 
#define MAC_RXDESCRIPTOR    (*(reg32 *)(MAC_BASE_ADDR + 0x108)) // Rx descriptor base address reg 
#define MAC_RXSTATUS        (*(reg32 *)(MAC_BASE_ADDR + 0x10C)) // Rx status base address reg 
#define MAC_RXDESCRIPTORNUM (*(reg32 *)(MAC_BASE_ADDR + 0x110)) // Rx number of descriptors reg 
#define MAC_RXPRODUCEINDEX  (*(reg32 *)(MAC_BASE_ADDR + 0x114)) // Rx produce index reg (RO) 
#define MAC_RXCONSUMEINDEX  (*(reg32 *)(MAC_BASE_ADDR + 0x118)) // Rx consume index reg 
#define MAC_TXDESCRIPTOR    (*(reg32 *)(MAC_BASE_ADDR + 0x11C)) // Tx descriptor base address reg 
#define MAC_TXSTATUS        (*(reg32 *)(MAC_BASE_ADDR + 0x120)) // Tx status base address reg 
#define MAC_TXDESCRIPTORNUM (*(reg32 *)(MAC_BASE_ADDR + 0x124)) // Tx number of descriptors reg 
#define MAC_TXPRODUCEINDEX  (*(reg32 *)(MAC_BASE_ADDR + 0x128)) // Tx produce index reg 
#define MAC_TXCONSUMEINDEX  (*(reg32 *)(MAC_BASE_ADDR + 0x12C)) // Tx consume index reg (RO) 

#define MAC_TSV0            (*(reg32 *)(MAC_BASE_ADDR + 0x158)) // Tx status vector 0 reg (RO) 
#define MAC_TSV1            (*(reg32 *)(MAC_BASE_ADDR + 0x15C)) // Tx status vector 1 reg (RO) 
#define MAC_RSV             (*(reg32 *)(MAC_BASE_ADDR + 0x160)) // Rx status vector reg (RO) 

#define MAC_FLOWCONTROLCNT  (*(reg32 *)(MAC_BASE_ADDR + 0x170)) // Flow control counter reg 
#define MAC_FLOWCONTROLSTS  (*(reg32 *)(MAC_BASE_ADDR + 0x174)) // Flow control status reg 

#define MAC_RXFILTERCTRL    (*(reg32 *)(MAC_BASE_ADDR + 0x200)) // Rx filter ctrl reg 
#define MAC_RXFILTERWOLSTS  (*(reg32 *)(MAC_BASE_ADDR + 0x204)) // Rx filter WoL status reg (RO) 
#define MAC_RXFILTERWOLCLR  (*(reg32 *)(MAC_BASE_ADDR + 0x208)) // Rx filter WoL clear reg (WO) 

#define MAC_HASHFILTERL     (*(reg32 *)(MAC_BASE_ADDR + 0x210)) // Hash filter LSBs reg 
#define MAC_HASHFILTERH     (*(reg32 *)(MAC_BASE_ADDR + 0x214)) // Hash filter MSBs reg 

#define MAC_INTSTATUS       (*(reg32 *)(MAC_BASE_ADDR + 0xFE0)) // Interrupt status reg (RO) 
#define MAC_INTENABLE       (*(reg32 *)(MAC_BASE_ADDR + 0xFE4)) // Interrupt enable reg  
#define MAC_INTCLEAR        (*(reg32 *)(MAC_BASE_ADDR + 0xFE8)) // Interrupt clear reg (WO) 
#define MAC_INTSET          (*(reg32 *)(MAC_BASE_ADDR + 0xFEC)) // Interrupt set reg (WO) 

#define MAC_POWERDOWN       (*(reg32 *)(MAC_BASE_ADDR + 0xFF4)) // Power-down reg 
#define MAC_MODULEID        (*(reg32 *)(MAC_BASE_ADDR + 0xFFC)) // Module ID reg (RO) 



// ########################################################################################################################################
// IAP/ISP
#define IAP_LOCATION            (0x7ffffff1) // Definindo o ponto de entrada das funções IAP no modo de instruções ARM
#define IAP_CMD_PREPARE         (50)
#define IAP_CMD_COPYRAMTOFLASH  (51)
#define IAP_CMD_ERASE           (52)
#define IAP_CMD_BLANKCHECK      (53)
#define IAP_CMD_READPARTID      (54)
#define IAP_CMD_READBOOTCODEVER (55)
#define IAP_CMD_COMPARE         (56)
#define IAP_CMD_REINVOKEISP     (57)


#endif
