#ifndef __LPC1715_H
#define __LPC1715_H

#include "uc_libdefs.h"

/*
// ########################################################################################################################################
// WATCHDOG
#define WDG_BASE_ADDR		0x40000000

#define WDMOD          (*(reg32 *)(WDG_BASE_ADDR + 0x00))       // Watchdog Mode Register
#define WDTC           (*(reg32 *)(WDG_BASE_ADDR + 0x04))       // Watchdog Time Constant Register
#define WDFEED         (*(reg32 *)(WDG_BASE_ADDR + 0x08))       // Watchdog Feed Register
#define WDTV           (*(reg32 *)(WDG_BASE_ADDR + 0x0C))      	// Watchdog Time Value Register

// ########################################################################################################################################
// REGISTRADORES DO TIMER 0

#define TMR0_BASE_ADDR		0x40004000

#define T0IR           (*(reg32 *)(TMR0_BASE_ADDR + 0x00))	// Registrador de interrupções
	#define T0IR_MR0I    (1 << 0)        			    		// Flag de interrupção do canal de comparação 0
	#define T0IR_MR1I    (1 << 1)       			     		// Flag de interrupção do canal de comparação 1
	#define T0IR_MR2I    (1 << 2)       			     		// Flag de interrupção do canal de comparação 2
	#define T0IR_MR3I    (1 << 3)       			     		// Flag de interrupção do canal de comparação 3
	#define T0IR_CR0I    (1 << 4)        			    		// Flag de interrupção do canal de captura 0
	#define T0IR_CR1I    (1 << 5)         				   		// Flag de interrupção do canal de captura 1

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

#define T0CR0          (*(reg32 *)(TMR0_BASE_ADDR + 0x2C))	// Registrador de captura 0
#define T0CR1          (*(reg32 *)(TMR0_BASE_ADDR + 0x30))	// Registrador de captura 1
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
#define TMR1_BASE_ADDR		0x40008000

#define T1IR           (*(reg32 *)(TMR1_BASE_ADDR + 0x00))	// Registrador de interrupções
	#define T1IR_MR0I    (1 << 0)        			    		// Flag de interrupção do canal de comparação 0
	#define T1IR_MR1I    (1 << 1)       			     		// Flag de interrupção do canal de comparação 1
	#define T1IR_MR2I    (1 << 2)       			     		// Flag de interrupção do canal de comparação 2
	#define T1IR_MR3I    (1 << 3)       			     		// Flag de interrupção do canal de comparação 3
	#define T1IR_CR0I    (1 << 4)        			    		// Flag de interrupção do canal de captura 0
	#define T1IR_CR1I    (1 << 5)         				   		// Flag de interrupção do canal de captura 1

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

#define T1CR0          (*(reg32 *)(TMR1_BASE_ADDR + 0x2C))	// Registrador de captura 0
#define T1CR1          (*(reg32 *)(TMR1_BASE_ADDR + 0x30))	// Registrador de captura 1
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
#define TMR2_BASE_ADDR		0x40090000

#define T2IR           (*(reg32 *)(TMR2_BASE_ADDR + 0x00))	// Registrador de interrupções
	#define T2IR_MR0I    (1 << 0)        			    		// Flag de interrupção do canal de comparação 0
	#define T2IR_MR1I    (1 << 1)       			     		// Flag de interrupção do canal de comparação 1
	#define T2IR_MR2I    (1 << 2)       			     		// Flag de interrupção do canal de comparação 2
	#define T2IR_MR3I    (1 << 3)       			     		// Flag de interrupção do canal de comparação 3
	#define T2IR_CR0I    (1 << 4)        			    		// Flag de interrupção do canal de captura 0
	#define T2IR_CR1I    (1 << 5)         				   		// Flag de interrupção do canal de captura 1

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

#define T2CR0          (*(reg32 *)(TMR2_BASE_ADDR + 0x2C))	// Registrador de captura 0
#define T2CR1          (*(reg32 *)(TMR2_BASE_ADDR + 0x30))	// Registrador de captura 1
#define T2EMR          (*(reg32 *)(TMR2_BASE_ADDR + 0x3C))	// Registrador de comparação externa
#define T2CTCR         (*(reg32 *)(TMR2_BASE_ADDR + 0x70))

// ########################################################################################################################################
// REGISTRADORES DO TIMER 3
#define TMR3_BASE_ADDR		0x40094000

#define T3IR           (*(reg32 *)(TMR3_BASE_ADDR + 0x00))	// Registrador de interrupções
	#define T3IR_MR0I    (1 << 0)        			    		// Flag de interrupção do canal de comparação 0
	#define T3IR_MR1I    (1 << 1)       			     		// Flag de interrupção do canal de comparação 1
	#define T3IR_MR2I    (1 << 2)       			     		// Flag de interrupção do canal de comparação 2
	#define T3IR_MR3I    (1 << 3)       			     		// Flag de interrupção do canal de comparação 3
	#define T3IR_CR0I    (1 << 4)        			    		// Flag de interrupção do canal de captura 0
	#define T3IR_CR1I    (1 << 5)         				   		// Flag de interrupção do canal de captura 1

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

#define T3CR0          (*(reg32 *)(TMR3_BASE_ADDR + 0x2C))	// Registrador de captura 0
#define T3CR1          (*(reg32 *)(TMR3_BASE_ADDR + 0x30))	// Registrador de captura 1
#define T3EMR          (*(reg32 *)(TMR3_BASE_ADDR + 0x3C))	// Registrador de comparação externa
#define T3CTCR         (*(reg32 *)(TMR3_BASE_ADDR + 0x70))

//// ########################################################################################################################################
//// PWM0 - PULSE WIDTH MODULATOR
//#define PWM0_BASE_ADDR		0xE0014000
//
//#define PWM0IR          (*(reg32 *)(PWM0_BASE_ADDR + 0x00))			//  Interrupt Register
//	#define PW0MIR_MR0I  (1 << 0)            							// Flag de interrupção PWM0
//	#define PW0MIR_MR1I  (1 << 1)         						 		// Flag de interrupção PWM1
//	#define PW0MIR_MR2I  (1 << 2)    						       		// Flag de interrupção PWM2
//	#define PW0MIR_MR3I  (1 << 3)            							// Flag de interrupção PWM3
//	#define PW0MIR_MR4I  (1 << 8)            							// Flag de interrupção PWM4
//	#define PW0MIR_MR5I  (1 << 9)            							// Flag de interrupção PWM5
//	#define PW0MIR_MR6I  (1 << 10)           							// Flag de interrupção PWM6
//	#define PW0MIR_MASK  (0x070F)
//
//#define PWM0TCR         (*(reg32 *)(PWM0_BASE_ADDR + 0x04))        	// Timer Control Register
//#define PWM0TC          (*(reg32 *)(PWM0_BASE_ADDR + 0x08))         // Timer Counter
//#define PWM0PR          (*(reg32 *)(PWM0_BASE_ADDR + 0x0C))         // Prescale Register
//#define PWM0PC          (*(reg32 *)(PWM0_BASE_ADDR + 0x10))         // Prescale Counter Register
//#define PWM0MCR         (*(reg32 *)(PWM0_BASE_ADDR + 0x14))        	// Match Control Register
//	#define PWM0MCR_ENABLE 		(1 << 0)								// Habilita o contador do timer PWM
//	#define PWM0MCR_RESET  		(1 << 1)								// Zera o contador do timer PWM
//	#define PWM0MCR_PWM_ENABLE  (1 << 3)
//
//#define PWM0MR0         (*(reg32 *)(PWM0_BASE_ADDR + 0x18))			// Match Register 0
//#define PWM0MR1         (*(reg32 *)(PWM0_BASE_ADDR + 0x1C))     	// Match Register 1
//#define PWM0MR2         (*(reg32 *)(PWM0_BASE_ADDR + 0x20))			// Match Register 2
//#define PWM0MR3         (*(reg32 *)(PWM0_BASE_ADDR + 0x24))        	// Match Register 3
//#define PWM0CCR         (*(reg32 *)(PWM0_BASE_ADDR + 0x28))
//#define PWM0CR0         (*(reg32 *)(PWM0_BASE_ADDR + 0x2C))
//#define PWM0CR1         (*(reg32 *)(PWM0_BASE_ADDR + 0x30))
//#define PWM0CR2         (*(reg32 *)(PWM0_BASE_ADDR + 0x34))
//#define PWM0CR3         (*(reg32 *)(PWM0_BASE_ADDR + 0x38))
//#define PWM0EMR         (*(reg32 *)(PWM0_BASE_ADDR + 0x3C))
//#define PWM0MR4         (*(reg32 *)(PWM0_BASE_ADDR + 0x40))        	// Match Register 4
//#define PWM0MR5         (*(reg32 *)(PWM0_BASE_ADDR + 0x44))        	// Match Register 5
//#define PWM0MR6         (*(reg32 *)(PWM0_BASE_ADDR + 0x48))        	// Match Register 6
//#define PWM0PCR         (*(reg32 *)(PWM0_BASE_ADDR + 0x4C))        	// Control Register
//  	#define PWM0PCR_PWM_SEL2	(1 << 2)								// Seleciona o controle de dupla borda para o PWM2. 0 = borda simples / 1 = Dupla borda
//	#define PWM0PCR_PWM_SEL3	(1 << 3)								// Seleciona o controle de dupla borda para o PWM3. 0 = borda simples / 1 = Dupla borda
//	#define PWM0PCR_PWM_SEL4	(1 << 4)								// Seleciona o controle de dupla borda para o PWM4. 0 = borda simples / 1 = Dupla borda
//	#define PWM0PCR_PWM_SEL5	(1 << 5)								// Seleciona o controle de dupla borda para o PWM5. 0 = borda simples / 1 = Dupla borda
//	#define PWM0PCR_PWM_SEL6	(1 << 6)								// Seleciona o controle de dupla borda para o PWM6. 0 = borda simples / 1 = Dupla borda
//	#define PWM0PCR_PWM_ENABLE1	(1 << 9)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//	#define PWM0PCR_PWM_ENABLE2	(1 << 10)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//	#define PWM0PCR_PWM_ENABLE3	(1 << 11)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//	#define PWM0PCR_PWM_ENABLE4	(1 << 12)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//	#define PWM0PCR_PWM_ENABLE5	(1 << 13)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//	#define PWM0PCR_PWM_ENABLE6	(1 << 14)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//
//#define PWM0LER         (*(reg32 *)(PWM0_BASE_ADDR + 0x50))        	// Latch Enable Register
//  	#define PWM0LER_DISABLE			0									// Desbilita todos os registradores latchs do PWM
//	#define PWM0LER0_ENABLE			(1 << 0)							// Habilita o registrador latch do PWM0
//	#define PWM0LER1_ENABLE			(1 << 1)							// Habilita o registrador latch do PWM1
//	#define PWM0LER2_ENABLE			(1 << 2)							// Habilita o registrador latch do PWM2
//	#define PWM0LER3_ENABLE			(1 << 3)							// Habilita o registrador latch do PWM3
//	#define PWM0LER4_ENABLE			(1 << 4)							// Habilita o registrador latch do PWM4
//	#define PWM0LER5_ENABLE			(1 << 5)							// Habilita o registrador latch do PWM5
//	#define PWM0LER6_ENABLE			(1 << 6)							// Habilita o registrador latch do PWM6
//#define PWM0CTCR        (*(reg32 *)(PWM0_BASE_ADDR + 0x70))
//
//// ########################################################################################################################################
//// PWM1 - PULSE WIDTH MODULATOR
//#define PWM1_BASE_ADDR		0xE0018000
//
//#define PWM1IR          (*(reg32 *)(PWM1_BASE_ADDR + 0x00))			//  Interrupt Register
//	#define PW1MIR_MR0I  (1 << 0)            							// Flag de interrupção PWM0
//	#define PW1MIR_MR1I  (1 << 1)         						 		// Flag de interrupção PWM1
//	#define PW1MIR_MR2I  (1 << 2)    						       		// Flag de interrupção PWM2
//	#define PW1MIR_MR3I  (1 << 3)            							// Flag de interrupção PWM3
//	#define PW1MIR_MR4I  (1 << 8)            							// Flag de interrupção PWM4
//	#define PW1MIR_MR5I  (1 << 9)            							// Flag de interrupção PWM5
//	#define PW1MIR_MR6I  (1 << 10)           							// Flag de interrupção PWM6
//	#define PW1MIR_MASK  (0x070F)
//
//#define PWM1TCR         (*(reg32 *)(PWM1_BASE_ADDR + 0x04))			// Timer Control Register
//#define PWM1TC          (*(reg32 *)(PWM1_BASE_ADDR + 0x08))         // Timer Counter
//#define PWM1PR          (*(reg32 *)(PWM1_BASE_ADDR + 0x0C))         // Prescale Register
//#define PWM1PC          (*(reg32 *)(PWM1_BASE_ADDR + 0x10))         // Prescale Counter Register
//#define PWM1MCR         (*(reg32 *)(PWM1_BASE_ADDR + 0x14))        	// Match Control Register
//	#define PWM1MCR_ENABLE 		(1 << 0)								// Habilita o contador do timer PWM
//	#define PWM1MCR_RESET  		(1 << 1)								// Zera o contador do timer PWM
//	#define PWM1MCR_PWM_ENABLE  (1 << 3)
//
//#define PWM1MR0         (*(reg32 *)(PWM1_BASE_ADDR + 0x18))			// Match Register 0
//#define PWM1MR1         (*(reg32 *)(PWM1_BASE_ADDR + 0x1C))			// Match Register 1
//#define PWM1MR2         (*(reg32 *)(PWM1_BASE_ADDR + 0x20))			// Match Register 2
//#define PWM1MR3         (*(reg32 *)(PWM1_BASE_ADDR + 0x24))			// Match Register 3
//#define PWM1CCR         (*(reg32 *)(PWM1_BASE_ADDR + 0x28))
//#define PWM1CR0         (*(reg32 *)(PWM1_BASE_ADDR + 0x2C))
//#define PWM1CR1         (*(reg32 *)(PWM1_BASE_ADDR + 0x30))
//#define PWM1CR2         (*(reg32 *)(PWM1_BASE_ADDR + 0x34))
//#define PWM1CR3         (*(reg32 *)(PWM1_BASE_ADDR + 0x38))
//#define PWM1EMR         (*(reg32 *)(PWM1_BASE_ADDR + 0x3C))
//#define PWM1MR4         (*(reg32 *)(PWM1_BASE_ADDR + 0x40))			// Match Register 4
//#define PWM1MR5         (*(reg32 *)(PWM1_BASE_ADDR + 0x44))			// Match Register 5
//#define PWM1MR6         (*(reg32 *)(PWM1_BASE_ADDR + 0x48))			// Match Register 6
//#define PWM1PCR         (*(reg32 *)(PWM1_BASE_ADDR + 0x4C))			// Control Register
//  	#define PWM1PCR_PWM_SEL2	(1 << 2)								// Seleciona o controle de dupla borda para o PWM2. 0 = borda simples / 1 = Dupla borda
//	#define PWM1PCR_PWM_SEL3	(1 << 3)								// Seleciona o controle de dupla borda para o PWM3. 0 = borda simples / 1 = Dupla borda
//	#define PWM1PCR_PWM_SEL4	(1 << 4)								// Seleciona o controle de dupla borda para o PWM4. 0 = borda simples / 1 = Dupla borda
//	#define PWM1PCR_PWM_SEL5	(1 << 5)								// Seleciona o controle de dupla borda para o PWM5. 0 = borda simples / 1 = Dupla borda
//	#define PWM1PCR_PWM_SEL6	(1 << 6)								// Seleciona o controle de dupla borda para o PWM6. 0 = borda simples / 1 = Dupla borda
//	#define PWM1PCR_PWM_ENABLE1	(1 << 9)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//	#define PWM1PCR_PWM_ENABLE2	(1 << 10)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//	#define PWM1PCR_PWM_ENABLE3	(1 << 11)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//	#define PWM1PCR_PWM_ENABLE4	(1 << 12)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//	#define PWM1PCR_PWM_ENABLE5	(1 << 13)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//	#define PWM1PCR_PWM_ENABLE6	(1 << 14)								// Habilita a saida do PWM para o pino. 0 = Não habilita / 1 = Habilita
//
//#define PWM1LER         (*(reg32 *)(PWM1_BASE_ADDR + 0x50))			// Latch Enable Register
//  	#define PWM1LER_DISABLE			0									// Desbilita todos os registradores latchs do PWM
//	#define PWM1LER0_ENABLE			(1 << 0)							// Habilita o registrador latch do PWM0
//	#define PWM1LER1_ENABLE			(1 << 1)							// Habilita o registrador latch do PWM1
//	#define PWM1LER2_ENABLE			(1 << 2)							// Habilita o registrador latch do PWM2
//	#define PWM1LER3_ENABLE			(1 << 3)							// Habilita o registrador latch do PWM3
//	#define PWM1LER4_ENABLE			(1 << 4)							// Habilita o registrador latch do PWM4
//	#define PWM1LER5_ENABLE			(1 << 5)							// Habilita o registrador latch do PWM5
//	#define PWM1LER6_ENABLE			(1 << 6)							// Habilita o registrador latch do PWM6
//
//#define PWM1CTCR        (*(reg32 *)(PWM1_BASE_ADDR + 0x70))
//
//
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

#define UART0_BASE_ADDR		0x4000C000
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
//
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
//
#define U0SCR          (*(reg32 *)(UART0_BASE_ADDR + 0x1C))	// Scratch Pad Register
#define U0ACR          (*(reg32 *)(UART0_BASE_ADDR + 0x20))
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

#define UART1_BASE_ADDR		0x40010000
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

#define UART2_BASE_ADDR		0x40098000
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

#define UART3_BASE_ADDR		0x4009C000
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
#define U3FDR          (*(reg32 *)(UART3_BASE_ADDR + 0x28))
#define U3TER          (*(reg32 *)(UART3_BASE_ADDR + 0x30))
//
//// ########################################################################################################################################
//// I2C0
//#define I2C0_BASE_ADDR		0xE001C000
//
//#define I2C0CONSET      (*(reg32 *)(I2C0_BASE_ADDR + 0x00))	// Control Set Register
//	#define I2C0_AA				(1 << 2) 						// ACK - Este bit em 1 envia um ACK em 1. bit 0 não tem efeito
//  	#define I2C0_SI				(1 << 3) 						// Flag de interrupção - Este bit em 1 indica que ocorreu interrupção
//  	#define I2C0_STO			(1 << 4) 						// Este bit em 1 envia stop. bit 0 não tem efeito
//  	#define I2C0_STA			(1 << 5) 						// Este bit em 1 envia start. bit 0 não tem efeito
//  	#define I2C0_I2EN			(1 << 6) 						// Este bit em 1 habilita bus I2C. bit 0 não tem efeito
//
//#define I2C0STAT        (*(reg32 *)(I2C0_BASE_ADDR + 0x04))	// Status Register
//  	// MESTRE - TRANSMISSÃO
//	#define I2C0_MT_SLA_ACK					0x18
//	#define I2C0_MT_SLA_NACK				0x20
//	#define I2C0_MT_DATA_ACK				0x28
//	#define I2C0_MT_DATA_NACK				0x30
//	#define I2C0_MT_ARB_LOST				0x38
//
//	// MESTRE - RECEPÇÃO
//	#define I2C0_MR_ARB_LOST				0x38
//	#define I2C0_MR_SLA_ACK					0x40
//	#define I2C0_MR_SLA_NACK				0x48
//	#define I2C0_MR_DATA_ACK				0x50
//	#define I2C0_MR_DATA_NACK				0x58
//
//	// ESCRAVO Slave - TRANSMISSÃO
//	#define I2C0_ST_SLA_ACK					0xA8
//	#define I2C0_ST_ARB_LOST_SLA_ACK		0xB0
//	#define I2C0_ST_DATA_ACK				0xB8
//	#define I2C0_ST_DATA_NACK				0xC0
//	#define I2C0_ST_LAST_DATA				0xC8
//
//	// ESCRAVO - RECEPÇÃO
//	#define I2C0_SR_SLA_ACK					0x60
//	#define I2C0_SR_ARB_LOST_SLA_ACK		0x68
//	#define I2C0_SR_GCALL_ACK				0x70
//	#define I2C0_SR_ARB_LOST_GCALL_ACK		0x78
//	#define I2C0_SR_DATA_ACK				0x80
//	#define I2C0_SR_DATA_NACK				0x88
//	#define I2C0_SR_GCALL_DATA_ACK			0x90
//	#define I2C0_SR_GCALL_DATA_NACK			0x98
//	#define I2C0_SR_STOP					0xA0
//
//#define I2C0DAT         (*(reg32 *)(I2C0_BASE_ADDR + 0x08))	// Data Register
//#define I2C0ADR         (*(reg32 *)(I2C0_BASE_ADDR + 0x0C))	// Slave Address Register
//#define I2C0SCLH        (*(reg32 *)(I2C0_BASE_ADDR + 0x10))	// SCL Duty Cycle Register (high half word)
//#define I2C0SCLL        (*(reg32 *)(I2C0_BASE_ADDR + 0x14))	// SCL Duty Cycle Register (low half word)
//#define I2C0CONCLR     	(*(reg32 *)(I2C0_BASE_ADDR + 0x18))  // Control Clear Register
//	#define I2C0_AAC				(1 << 2) 					// ACK - Este bit em 1 envia um ACK em 0. bit 0 não tem efeito
//  	#define I2C0_SIC				(1 << 3) 					// Flag de interrupção - Este bit em 1 limpa o flag de interrupção
//  	#define I2C0_STAC				(1 << 5) 					// Este bit em 1 encerra o start. bit 0 não tem efeito
//  	#define I2C0_I2ENC				(1 << 6) 					// Este bit em 1 desabilita bus I2C. bit 0 não tem efeito
//
//
//// ########################################################################################################################################
//// I2C1
//#define I2C1_BASE_ADDR		0xE005C000
//#define I2C1CONSET      (*(reg32 *)(I2C1_BASE_ADDR + 0x00))
//	#define I2C1_AA				(1 << 2) 						// ACK - Este bit em 1 envia um ACK em 1. bit 0 não tem efeito
//  	#define I2C1_SI				(1 << 3) 						// Flag de interrupção - Este bit em 1 indica que ocorreu interrupção
//  	#define I2C1_STO			(1 << 4) 						// Este bit em 1 envia stop. bit 0 não tem efeito
//  	#define I2C1_STA			(1 << 5) 						// Este bit em 1 envia start. bit 0 não tem efeito
//  	#define I2C1_I2EN			(1 << 6) 						// Este bit em 1 habilita bus I2C. bit 0 não tem efeito
//#define I2C1STAT        (*(reg32 *)(I2C1_BASE_ADDR + 0x04))
//	// MESTRE - TRANSMISSÃO
//	#define I2C1_MT_SLA_ACK					0x18
//	#define I2C1_MT_SLA_NACK				0x20
//	#define I2C1_MT_DATA_ACK				0x28
//	#define I2C1_MT_DATA_NACK				0x30
//	#define I2C1_MT_ARB_LOST				0x38
//
//	// MESTRE - RECEPÇÃO
//	#define I2C1_MR_ARB_LOST				0x38
//	#define I2C1_MR_SLA_ACK					0x40
//	#define I2C1_MR_SLA_NACK				0x48
//	#define I2C1_MR_DATA_ACK				0x50
//	#define I2C1_MR_DATA_NACK				0x58
//
//	// ESCRAVO Slave - TRANSMISSÃO
//	#define I2C1_ST_SLA_ACK					0xA8
//	#define I2C1_ST_ARB_LOST_SLA_ACK		0xB0
//	#define I2C1_ST_DATA_ACK				0xB8
//	#define I2C1_ST_DATA_NACK				0xC0
//	#define I2C1_ST_LAST_DATA				0xC8
//
//	// ESCRAVO - RECEPÇÃO
//	#define I2C1_SR_SLA_ACK					0x60
//	#define I2C1_SR_ARB_LOST_SLA_ACK		0x68
//	#define I2C1_SR_GCALL_ACK				0x70
//	#define I2C1_SR_ARB_LOST_GCALL_ACK		0x78
//	#define I2C1_SR_DATA_ACK				0x80
//	#define I2C1_SR_DATA_NACK				0x88
//	#define I2C1_SR_GCALL_DATA_ACK			0x90
//	#define I2C1_SR_GCALL_DATA_NACK			0x98
//	#define I2C1_SR_STOP					0xA0
//
//#define I2C1DAT         (*(reg32 *)(I2C1_BASE_ADDR + 0x08))
//#define I2C1ADR         (*(reg32 *)(I2C1_BASE_ADDR + 0x0C))
//#define I2C1SCLH        (*(reg32 *)(I2C1_BASE_ADDR + 0x10))
//#define I2C1SCLL        (*(reg32 *)(I2C1_BASE_ADDR + 0x14))
//#define I2C1CONCLR      (*(reg32 *)(I2C1_BASE_ADDR + 0x18))
//	#define I2C1_AAC				(1 << 2) 					// ACK - Este bit em 1 envia um ACK em 0. bit 0 não tem efeito
//  	#define I2C1_SIC				(1 << 3) 					// Flag de interrupção - Este bit em 1 limpa o flag de interrupção
//  	#define I2C1_STAC				(1 << 5) 					// Este bit em 1 encerra o start. bit 0 não tem efeito
//  	#define I2C1_I2ENC				(1 << 6) 					// Este bit em 1 desabilita bus I2C. bit 0 não tem efeito
//
//
//// ########################################################################################################################################
//// I2C2
//#define I2C2_BASE_ADDR		0xE0080000
//#define I2C2CONSET      (*(reg32 *)(I2C2_BASE_ADDR + 0x00))
//	#define I2C2_AA				(1 << 2) 						// ACK - Este bit em 1 envia um ACK em 1. bit 0 não tem efeito
//  	#define I2C2_SI				(1 << 3) 						// Flag de interrupção - Este bit em 1 indica que ocorreu interrupção
//  	#define I2C2_STO			(1 << 4) 						// Este bit em 1 envia stop. bit 0 não tem efeito
//  	#define I2C2_STA			(1 << 5) 						// Este bit em 1 envia start. bit 0 não tem efeito
//  	#define I2C2_I2EN			(1 << 6) 						// Este bit em 1 habilita bus I2C. bit 0 não tem efeito
//
//#define I2C2STAT        (*(reg32 *)(I2C2_BASE_ADDR + 0x04))
//  	// MESTRE - TRANSMISSÃO
//	#define I2C2_MT_SLA_ACK					0x18
//	#define I2C2_MT_SLA_NACK				0x20
//	#define I2C2_MT_DATA_ACK				0x28
//	#define I2C2_MT_DATA_NACK				0x30
//	#define I2C2_MT_ARB_LOST				0x38
//
//	// MESTRE - RECEPÇÃO
//	#define I2C2_MR_ARB_LOST				0x38
//	#define I2C2_MR_SLA_ACK					0x40
//	#define I2C2_MR_SLA_NACK				0x48
//	#define I2C2_MR_DATA_ACK				0x50
//	#define I2C2_MR_DATA_NACK				0x58
//
//	// ESCRAVO Slave - TRANSMISSÃO
//	#define I2C2_ST_SLA_ACK					0xA8
//	#define I2C2_ST_ARB_LOST_SLA_ACK		0xB0
//	#define I2C2_ST_DATA_ACK				0xB8
//	#define I2C2_ST_DATA_NACK				0xC0
//	#define I2C2_ST_LAST_DATA				0xC8
//
//	// ESCRAVO - RECEPÇÃO
//	#define I2C2_SR_SLA_ACK					0x60
//	#define I2C2_SR_ARB_LOST_SLA_ACK		0x68
//	#define I2C2_SR_GCALL_ACK				0x70
//	#define I2C2_SR_ARB_LOST_GCALL_ACK		0x78
//	#define I2C2_SR_DATA_ACK				0x80
//	#define I2C2_SR_DATA_NACK				0x88
//	#define I2C2_SR_GCALL_DATA_ACK			0x90
//	#define I2C2_SR_GCALL_DATA_NACK			0x98
//	#define I2C2_SR_STOP					0xA0
//
//#define I2C2DAT         (*(reg32 *)(I2C2_BASE_ADDR + 0x08))
//#define I2C2ADR         (*(reg32 *)(I2C2_BASE_ADDR + 0x0C))
//#define I2C2SCLH        (*(reg32 *)(I2C2_BASE_ADDR + 0x10))
//#define I2C2SCLL        (*(reg32 *)(I2C2_BASE_ADDR + 0x14))
//#define I2C2CONCLR      (*(reg32 *)(I2C2_BASE_ADDR + 0x18))
//	#define I2C2_AAC				(1 << 2) 					// ACK - Este bit em 1 envia um ACK em 0. bit 0 não tem efeito
//  	#define I2C2_SIC				(1 << 3) 					// Flag de interrupção - Este bit em 1 limpa o flag de interrupção
//  	#define I2C2_STAC				(1 << 5) 					// Este bit em 1 encerra o start. bit 0 não tem efeito
//  	#define I2C2_I2ENC				(1 << 6) 					// Este bit em 1 desabilita bus I2C. bit 0 não tem efeito


// ########################################################################################################################################
// SSP0
#define SSP0_FIFO_DEPTH  (8)

#define SSP0_BASE_ADDR		0x40088000
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

#define SSP1_BASE_ADDR		0x40030000
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
	#define SSP1_CPOL_L  (0)	// Clock em baixa entre os frames
	#define SSP1_CPOL_H  (1<<6)	// Clock em alta entre os frames
	#define SSP1_CPHA_L  (0)	// Dados capturados a partir do primeiro clock
	#define SSP1_CPHA_H  (1<<7)	// Dados capturados a partir do segundo clock
	#define SSP1_STANDARD (0x7)

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
#define RTC_BASE_ADDR		0x40024000
//
#define ILR         	(*(reg32 *)(RTC_BASE_ADDR + 0x00))      // Interrupt Location Register
	#define RTCCIF 		(1 << 0) 									// Quando 1 o bloco do timer RTC gera uma interrupção. Escrevendo 1 limpa interrupção
	#define RTCALF		(1 << 1)									// Quando 1 o bloco do alarme gera uma interrupção. Escrevendo 1 limpa interrupção

#define CCR         	(*(reg32 *)(RTC_BASE_ADDR + 0x08))      // Clock Control Register
	#define CLKEN		(1 << 0)									// habilita Clock
	#define CTCRST		(1 << 1)									// Reset
	#define CTTEST		(3 << 2)									// Deve manter 00 na operação normal

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
//#define CISS        	(*(reg32 *)(RTC_BASE_ADDR + 0x40))

// REGISTRADORES PARA O ALARME
#define ALSEC       	(*(reg32 *)(RTC_BASE_ADDR + 0x60))      // Alarm Seconds Register
#define ALMIN       	(*(reg32 *)(RTC_BASE_ADDR + 0x64))      // Alarm Minutes Register
#define ALHOUR      	(*(reg32 *)(RTC_BASE_ADDR + 0x68))      // Alarm Hours Register
#define ALDOM       	(*(reg32 *)(RTC_BASE_ADDR + 0x6C))      // Alarm Day Of Month Register
#define ALDOW       	(*(reg32 *)(RTC_BASE_ADDR + 0x70))      // Alarm Day Of Week Register
#define ALDOY       	(*(reg32 *)(RTC_BASE_ADDR + 0x74))      // Alarm Day Of Year Register
#define ALMON       	(*(reg32 *)(RTC_BASE_ADDR + 0x78))      // Alarm Months Register
#define ALYEAR   	   	(*(reg32 *)(RTC_BASE_ADDR + 0x7C))      // Alarm Years Register

//// REGISTRADORES PARA CONTROLE DO RTC ATRAVÉS DO CLOCK DA CPU
//#define PREINT      	(*(reg32 *)(RTC_BASE_ADDR + 0x80))    	// Prescale Value Register (integer)
//#define PREFRAC     	(*(reg32 *)(RTC_BASE_ADDR + 0x84))    	// Prescale Value Register (fraction)

// ########################################################################################################################################
// BLOCOS DE CONEXÃO DE PINOS IO
// datasheet pagina 113
#define PINSEL_BASE_ADDR	0x4002C000

#define PINSEL_P0        (*(reg32 *)(PINSEL_BASE_ADDR + 0x000)) // P0[31:0]
#define PINSEL_P1        (*(reg32 *)(PINSEL_BASE_ADDR + 0x080))	// P1[31:0]
#define PINSEL_P2        (*(reg32 *)(PINSEL_BASE_ADDR + 0x100))	// P2[31:0]
#define PINSEL_P3        (*(reg32 *)(PINSEL_BASE_ADDR + 0x180))	// P3[31:0]
#define PINSEL_P4        (*(reg32 *)(PINSEL_BASE_ADDR + 0x200))	// P4[31:0]
#define PINSEL_P5        (*(reg32 *)(PINSEL_BASE_ADDR + 0x280))	// P5[31:0]
	#define PINSEL_MASK					0x7ul
	#define PINSEL_PINMODE_MASK			0x18ul
	#define PINSEL_OPENDRAIN_MASK		0x400ul

	// MODO DO PINO
	#define PINSEL_PINMODE_INACTIVE		(0)			// Sem resistor pull-up ou pull-down
	#define PINSEL_PINMODE_PULLDOWN 	(1ul<<4) 	// Internal pull-down resistor
	#define PINSEL_PINMODE_PULLUP		(2ul<<4)	// Internal pull-up resistor
	#define PINSEL_PINMODE_REPEATER 	(3ul<<4) 	// Repeater mode.


#define IOCON_P0_00 (*(reg32 *)(0x4002C000))
#define IOCON_P0_01 (*(reg32 *)(0x4002C004))
#define IOCON_P0_02 (*(reg32 *)(0x4002C008))
#define IOCON_P0_03 (*(reg32 *)(0x4002C00c))
#define IOCON_P0_04 (*(reg32 *)(0x4002C010))
#define IOCON_P0_05 (*(reg32 *)(0x4002C014))
#define IOCON_P0_06 (*(reg32 *)(0x4002C018))
#define IOCON_P0_07 (*(reg32 *)(0x4002C01c))
#define IOCON_P0_08 (*(reg32 *)(0x4002C020))
#define IOCON_P0_09 (*(reg32 *)(0x4002C024))
#define IOCON_P0_10 (*(reg32 *)(0x4002C028))
#define IOCON_P0_11 (*(reg32 *)(0x4002C02c))
#define IOCON_P0_12 (*(reg32 *)(0x4002C030))
#define IOCON_P0_13 (*(reg32 *)(0x4002C034))
#define IOCON_P0_14 (*(reg32 *)(0x4002C038))
#define IOCON_P0_15 (*(reg32 *)(0x4002C03c))
#define IOCON_P0_16 (*(reg32 *)(0x4002C040))
#define IOCON_P0_17 (*(reg32 *)(0x4002C044))
#define IOCON_P0_18 (*(reg32 *)(0x4002C048))
#define IOCON_P0_19 (*(reg32 *)(0x4002C04c))
#define IOCON_P0_20 (*(reg32 *)(0x4002C050))
#define IOCON_P0_21 (*(reg32 *)(0x4002C054))
#define IOCON_P0_22 (*(reg32 *)(0x4002C058))
#define IOCON_P0_23 (*(reg32 *)(0x4002C05c))
#define IOCON_P0_24 (*(reg32 *)(0x4002C060))
#define IOCON_P0_25 (*(reg32 *)(0x4002C064))
#define IOCON_P0_26 (*(reg32 *)(0x4002C068))
#define IOCON_P0_27 (*(reg32 *)(0x4002C06c))
#define IOCON_P0_28 (*(reg32 *)(0x4002C070))
#define IOCON_P0_29 (*(reg32 *)(0x4002C074))
#define IOCON_P0_30 (*(reg32 *)(0x4002C078))
#define IOCON_P0_31 (*(reg32 *)(0x4002C07c))

#define IOCON_P1_00 (*(reg32 *)(0x4002C080))
#define IOCON_P1_01 (*(reg32 *)(0x4002C084))
#define IOCON_P1_02 (*(reg32 *)(0x4002C088))
#define IOCON_P1_03 (*(reg32 *)(0x4002C08c))
#define IOCON_P1_04 (*(reg32 *)(0x4002C090))
#define IOCON_P1_05 (*(reg32 *)(0x4002C094))
#define IOCON_P1_06 (*(reg32 *)(0x4002C098))
#define IOCON_P1_07 (*(reg32 *)(0x4002C09c))
#define IOCON_P1_08 (*(reg32 *)(0x4002C0a0))
#define IOCON_P1_09 (*(reg32 *)(0x4002C0a4))
#define IOCON_P1_10 (*(reg32 *)(0x4002C0a8))
#define IOCON_P1_11 (*(reg32 *)(0x4002C0ac))
#define IOCON_P1_12 (*(reg32 *)(0x4002C0b0))
#define IOCON_P1_13 (*(reg32 *)(0x4002C0b4))
#define IOCON_P1_14 (*(reg32 *)(0x4002C0b8))
#define IOCON_P1_15 (*(reg32 *)(0x4002C0bc))
#define IOCON_P1_16 (*(reg32 *)(0x4002C0c0))
#define IOCON_P1_17 (*(reg32 *)(0x4002C0c4))
#define IOCON_P1_18 (*(reg32 *)(0x4002C0c8))
#define IOCON_P1_19 (*(reg32 *)(0x4002C0cc))
#define IOCON_P1_20 (*(reg32 *)(0x4002C0d0))
#define IOCON_P1_21 (*(reg32 *)(0x4002C0d4))
#define IOCON_P1_22 (*(reg32 *)(0x4002C0d8))
#define IOCON_P1_23 (*(reg32 *)(0x4002C0dc))
#define IOCON_P1_24 (*(reg32 *)(0x4002C0e0))
#define IOCON_P1_25 (*(reg32 *)(0x4002C0e4))
#define IOCON_P1_26 (*(reg32 *)(0x4002C0e8))
#define IOCON_P1_27 (*(reg32 *)(0x4002C0ec))
#define IOCON_P1_28 (*(reg32 *)(0x4002C0f0))
#define IOCON_P1_29 (*(reg32 *)(0x4002C0f4))
#define IOCON_P1_30 (*(reg32 *)(0x4002C0f8))
#define IOCON_P1_31 (*(reg32 *)(0x4002C0fc))

#define IOCON_P2_00 (*(reg32 *)(0x4002C100))
#define IOCON_P2_01 (*(reg32 *)(0x4002C104))
#define IOCON_P2_02 (*(reg32 *)(0x4002C108))
#define IOCON_P2_03 (*(reg32 *)(0x4002C10c))
#define IOCON_P2_04 (*(reg32 *)(0x4002C110))
#define IOCON_P2_05 (*(reg32 *)(0x4002C114))
#define IOCON_P2_06 (*(reg32 *)(0x4002C118))
#define IOCON_P2_07 (*(reg32 *)(0x4002C11c))
#define IOCON_P2_08 (*(reg32 *)(0x4002C120))
#define IOCON_P2_09 (*(reg32 *)(0x4002C124))
#define IOCON_P2_10 (*(reg32 *)(0x4002C128))
#define IOCON_P2_11 (*(reg32 *)(0x4002C12c))
#define IOCON_P2_12 (*(reg32 *)(0x4002C130))
#define IOCON_P2_13 (*(reg32 *)(0x4002C134))
#define IOCON_P2_14 (*(reg32 *)(0x4002C138))
#define IOCON_P2_15 (*(reg32 *)(0x4002C13c))
#define IOCON_P2_16 (*(reg32 *)(0x4002C140))
#define IOCON_P2_17 (*(reg32 *)(0x4002C144))
#define IOCON_P2_18 (*(reg32 *)(0x4002C148))
#define IOCON_P2_19 (*(reg32 *)(0x4002C14c))
#define IOCON_P2_20 (*(reg32 *)(0x4002C150))
#define IOCON_P2_21 (*(reg32 *)(0x4002C154))
#define IOCON_P2_22 (*(reg32 *)(0x4002C158))
#define IOCON_P2_23 (*(reg32 *)(0x4002C15c))
#define IOCON_P2_24 (*(reg32 *)(0x4002C160))
#define IOCON_P2_25 (*(reg32 *)(0x4002C164))
#define IOCON_P2_26 (*(reg32 *)(0x4002C168))
#define IOCON_P2_27 (*(reg32 *)(0x4002C16c))
#define IOCON_P2_28 (*(reg32 *)(0x4002C170))
#define IOCON_P2_29 (*(reg32 *)(0x4002C174))
#define IOCON_P2_30 (*(reg32 *)(0x4002C178))
#define IOCON_P2_31 (*(reg32 *)(0x4002C17c))

#define IOCON_P3_00 (*(reg32 *)(0x4002C180))
#define IOCON_P3_01 (*(reg32 *)(0x4002C184))
#define IOCON_P3_02 (*(reg32 *)(0x4002C188))
#define IOCON_P3_03 (*(reg32 *)(0x4002C18c))
#define IOCON_P3_04 (*(reg32 *)(0x4002C190))
#define IOCON_P3_05 (*(reg32 *)(0x4002C194))
#define IOCON_P3_06 (*(reg32 *)(0x4002C198))
#define IOCON_P3_07 (*(reg32 *)(0x4002C19c))
#define IOCON_P3_08 (*(reg32 *)(0x4002C1a0))
#define IOCON_P3_09 (*(reg32 *)(0x4002C1a4))
#define IOCON_P3_10 (*(reg32 *)(0x4002C1a8))
#define IOCON_P3_11 (*(reg32 *)(0x4002C1ac))
#define IOCON_P3_12 (*(reg32 *)(0x4002C1b0))
#define IOCON_P3_13 (*(reg32 *)(0x4002C1b4))
#define IOCON_P3_14 (*(reg32 *)(0x4002C1b8))
#define IOCON_P3_15 (*(reg32 *)(0x4002C1bc))
#define IOCON_P3_16 (*(reg32 *)(0x4002C1c0))
#define IOCON_P3_17 (*(reg32 *)(0x4002C1c4))
#define IOCON_P3_18 (*(reg32 *)(0x4002C1c8))
#define IOCON_P3_19 (*(reg32 *)(0x4002C1cc))
#define IOCON_P3_20 (*(reg32 *)(0x4002C1d0))
#define IOCON_P3_21 (*(reg32 *)(0x4002C1d4))
#define IOCON_P3_22 (*(reg32 *)(0x4002C1d8))
#define IOCON_P3_23 (*(reg32 *)(0x4002C1dc))
#define IOCON_P3_24 (*(reg32 *)(0x4002C1e0))
#define IOCON_P3_25 (*(reg32 *)(0x4002C1e4))
#define IOCON_P3_26 (*(reg32 *)(0x4002C1e8))
#define IOCON_P3_27 (*(reg32 *)(0x4002C1ec))
#define IOCON_P3_28 (*(reg32 *)(0x4002C1f0))
#define IOCON_P3_29 (*(reg32 *)(0x4002C1f4))
#define IOCON_P3_30 (*(reg32 *)(0x4002C1f8))
#define IOCON_P3_31 (*(reg32 *)(0x4002C1fc))

#define IOCON_P4_00 (*(reg32 *)(0x4002C200))
#define IOCON_P4_01 (*(reg32 *)(0x4002C204))
#define IOCON_P4_02 (*(reg32 *)(0x4002C208))
#define IOCON_P4_03 (*(reg32 *)(0x4002C20c))
#define IOCON_P4_04 (*(reg32 *)(0x4002C210))
#define IOCON_P4_05 (*(reg32 *)(0x4002C214))
#define IOCON_P4_06 (*(reg32 *)(0x4002C218))
#define IOCON_P4_07 (*(reg32 *)(0x4002C21c))
#define IOCON_P4_08 (*(reg32 *)(0x4002C220))
#define IOCON_P4_09 (*(reg32 *)(0x4002C224))
#define IOCON_P4_10 (*(reg32 *)(0x4002C228))
#define IOCON_P4_11 (*(reg32 *)(0x4002C22c))
#define IOCON_P4_12 (*(reg32 *)(0x4002C230))
#define IOCON_P4_13 (*(reg32 *)(0x4002C234))
#define IOCON_P4_14 (*(reg32 *)(0x4002C238))
#define IOCON_P4_15 (*(reg32 *)(0x4002C23c))
#define IOCON_P4_16 (*(reg32 *)(0x4002C240))
#define IOCON_P4_17 (*(reg32 *)(0x4002C244))
#define IOCON_P4_18 (*(reg32 *)(0x4002C248))
#define IOCON_P4_19 (*(reg32 *)(0x4002C24c))
#define IOCON_P4_20 (*(reg32 *)(0x4002C250))
#define IOCON_P4_21 (*(reg32 *)(0x4002C254))
#define IOCON_P4_22 (*(reg32 *)(0x4002C258))
#define IOCON_P4_23 (*(reg32 *)(0x4002C25c))
#define IOCON_P4_24 (*(reg32 *)(0x4002C260))
#define IOCON_P4_25 (*(reg32 *)(0x4002C264))
#define IOCON_P4_26 (*(reg32 *)(0x4002C268))
#define IOCON_P4_27 (*(reg32 *)(0x4002C26c))
#define IOCON_P4_28 (*(reg32 *)(0x4002C270))
#define IOCON_P4_29 (*(reg32 *)(0x4002C274))
#define IOCON_P4_30 (*(reg32 *)(0x4002C278))
#define IOCON_P4_31 (*(reg32 *)(0x4002C27c))

#define IOCON_P5_00 (*(reg32 *)(0x4002C280))
#define IOCON_P5_01 (*(reg32 *)(0x4002C284))
#define IOCON_P5_02 (*(reg32 *)(0x4002C288))
#define IOCON_P5_03 (*(reg32 *)(0x4002C28c))
#define IOCON_P5_04 (*(reg32 *)(0x4002C290))

// ########################################################################################################################################
// ADC0
#define AD0_BASE_ADDR		0x40034000

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

#define AD0TRM        (*(reg32 *)(AD0_BASE_ADDR + 0x34))

// ########################################################################################################################################
// DAC
#define DAC_BASE_ADDR	0x4008C000
#define DACR           	(*(reg32 *)(DAC_BASE_ADDR + 0x00))
	#define DACR_VALUE_MASK  	(0x0000ffc0)
	#define DACR_VALUE_SHIFT 	(6)
	#define DACR_BIAS       	(0x00010000)
	#define DACR_MASK        	(0x0001ffc0)
#define DACCTRL        	(*(reg32 *)(DAC_BASE_ADDR + 0x04))
#define DACCNTVAL      	(*(reg32 *)(DAC_BASE_ADDR + 0x08))


// ########################################################################################################################################
// BLOCO DE CONTROLE DO SISTEMA (SCB - System Control Block)
*/
#define SCS_BASE_ADDR	0x40048000
/*
#define VTOR *((reg32*) 0xE000ED08) // Define em que região de memória o vetor de ints ficarão
	#define vtor_SetInCode(addr) {VTOR = addr;}
	#define vtor_SetInSram(addr) {VTOR = 0x20000000 | addr;}

#define FLASHCFG       (*(reg32 *)(SCS_BASE_ADDR + 0x000))		// Flash Accelerator Configuration Register
	#define FLASHTIM_20MHZ 0		// Flash accesses use 1 CPU clock. Use for up to 20 MHz CPU clock with power boost off
	#define FLASHTIM_40MHZ (1<<12)	// Flash accesses use 2 CPU clocks. Use for up to 40 MHz CPU clock with power boost off
	#define FLASHTIM_60MHZ (2<<12) 	// Flash accesses use 3 CPU clocks. Use for up to 60 MHz CPU clock with power boost off
	#define FLASHTIM_80MHZ (3<<12)	// Flash accesses use 4 CPU clocks. Use for up to 80 MHz CPU clock with power boost off
									//	Use this setting for operation from 100 to 120 MHz operation with power boost on
	#define FLASHTIM_100MHZ (4<<12)	// Flash accesses use 5 CPU clocks. Use for up to 100 MHz CPU clock with power boost off
	#define FLASHTIM_SAFE 	(5<<12)	// Flash accesses use 6 CPU clocks. “Safe” setting for any allowed conditions
	#define FLASHTIM_AUTO 	(6<<12)	// Configura a velocidade da FLASH automaticamente


// REGISTRADORES PARA TRAVA DE PLL
// PLLCON = PLL0CON
#define PLLCON         (*(reg32 *)(SCS_BASE_ADDR + 0x080))		// Registrador de controle do PLL
	#define pll_OFF()	{ PLLCON = 0; }
	#define pll_ON()	{ PLLCON = 1; }

#define PLL1CON         (*(reg32 *)(SCS_BASE_ADDR + 0x0A0))		// Registrador de controle do PLL1
	#define PLLCON_PLLE   (1 << 0)          					// 1 Habilita PLL / 0 - desabilita PLL
	#define pll1_OFF()	{ PLL1CON = 0; }
	#define pll1_ON()	{ PLL1CON = 1; }

// PLLCFG = PLL0CFG
#define PLLCFG         (*(reg32 *)(SCS_BASE_ADDR + 0x084))		// Registrador de configuração do PLL
#define PLL1CFG         (*(reg32 *)(SCS_BASE_ADDR + 0x0A4))		// Registrador de configuração do PLL1

// PLLSTAT = PLL0STAT
#define PLLSTAT        (*(reg32 *)(SCS_BASE_ADDR + 0x088))		// Registrador de Status
#define PLL1STAT       (*(reg32 *)(SCS_BASE_ADDR + 0x0A8))		// Registrador de Status
	#define PLLSTAT_PLLE  (1 << 8)									// Bit indicador se o PLL está ou não ativado
	#define PLLSTAT_LOCK  (1 << 10)         						// Bit de status de travamento do PLL

// PLLFEED = PLL0FEED
#define PLLFEED        (*(reg32 *)(SCS_BASE_ADDR + 0x08C))		// Registrador de validação de mudança do PLL
#define PLL1FEED       (*(reg32 *)(SCS_BASE_ADDR + 0x0AC))		// Registrador de validação de mudança do PLL1

// REGISTRADOR DE CONTROLE DE CONSUMO (POWER CONTROL datasheet pg59)
#define PCON           (*(reg32 *)(SCS_BASE_ADDR + 0x0C0))		// Registrador de controle de potencia
*/
// datasheet página 20
#define PCONP          (*(reg32 *)(SCS_BASE_ADDR + 0x080))		// Registrador SYSAHBCLKCTRL de controle de potencia do sistema e dos perifericos e
	#define PCGPIO 			(1 << 6)	// Power/clock control bit for IOCON, GPIO, and GPIO interrup. Reset = 0
	#define PICON 			(1 << 16)	// IOCON
//	#define PCLCD			(1 << 0)	// LCD power/clock control bit. Reset = 0
//	#define PCTIM0			(1 << 1)	// Timer/Counter 0 power/clock control bit. Reset = 1
//	#define PCTIM1		 	(1 << 2)	// Timer/Counter 1 power/clock control bit. Reset = 1
//	#define PCUART0		 	(1 << 3)	// UART0 power/clock control bit. Reset = 1
//	#define PCUART1 		(1 << 4)	// UART1 power/clock control bit. Reset = 1
//	#define PCPWM0 			(1 << 5)	// PWM0 power/clock control bit. Reset = 0
//	#define PCPWM1 		 	(1 << 6)	// PWM1 power/clock control bit. Reset = 0
//	#define PCI2C0 		 	(1 << 7)	// The I2C0 interface power/clock control bit. Reset = 1
//	#define PCUART4		 	(1 << 8)	// UART0 power/clock control bit. Reset = 0
//	#define PCRTC 			(1 << 9)	// The RTC power/clock control bit. Reset = 1
//	#define PCSSP1 			(1 << 10)	// The SSP1 interface power/clock control bit. Reset = 0
//	#define PCEMC 		 	(1 << 11)	// External Memory Controller Reset = 0
//	#define PCAD 		 	(1 << 12)	// A/D converter (ADC) power/clock control bit. Reset = 0
//	#define PCCAN1 		 	(1 << 13)	// CAN Controller 1 power/clock control bit. Reset = 0
//	#define PCCAN2 			(1 << 14)	// CAN Controller 2 power/clock control bit. Reset = 0

//	#define PCMCPWM 		(1 << 17)	// Motor Control PWM power/clock control bit. Reset = 0
//	#define PCQEI   		(1 << 18)	// Quadrature Encoder Interface power/clock control bit. Reset = 0
//	#define PCI2C1 			(1 << 19)	// The I2C1 interface power/clock control bit. Reset = 1
//	#define PCSSP2 	 	 	(1 << 20)	// SSP2 interface power/clock control bit. Reset = 0
//	#define PCSSP0 	 	 	(1 << 21)	// SSP0 interface power/clock control bit. Reset = 0
//	#define PCTIM2 			(1 << 22)	// Timer 2 power/clock control bit. Reset = 0
//	#define PCTIM3 			(1 << 23)	// Timer 3 power/clock control bit. Reset = 0
//	#define PCUART2 		(1 << 24)	// UART 2 power/clock control bit. Reset = 0
//	#define PCUART3 		(1 << 25)	// UART 3 power/clock control bit. Reset = 0
//	#define PCI2C2 			(1 << 26)	// I2S interface 2 power/clock control bit.  Reset = 1
//	#define PCI2S 			(1 << 27)	// I2S interface power/clock control bit. Reset = 0
//	#define PCSDC 			(1 << 28)	// SD card interface power/clock control bit.  Reset = 0
//	#define PCGPDMA  		(1 << 29)	// GP DMA function power/clock control bit.  Reset = 0
//	#define PCENET 			(1 << 30)	// Ethernet block power/clock control bit.  Reset = 0
//	#define PCUSB 			(1 << 31)	// USB interface power/clock control bit.  Reset = 0
/*
// REGISTRADOR DIVISÃO DE CLOCK
#define CLKSRCSEL     	(*(reg32 *)(SCS_BASE_ADDR + 0x10C))		// Registrador para selecionar o tipo de oscilador para o PLL
	#define clksrcsel_oscRC() 		{ CLKSRCSEL = 0; }
	#define clksrcsel_oscMAIN() 	{ CLKSRCSEL = 1; }
#define CCLKSEL     	(*(reg32 *)(SCS_BASE_ADDR + 0x104))		// CPU Clock Selection register
	#define cclksel_sysClk() 	{ CCLKSEL &= ~(1UL<<8);} 				// Seleciona o oscilador RC para o divisor do CCLK, PCLK e MEMCLK
	#define cclksel_PLL() 		{ CCLKSEL |= (1UL<<8); } 	// Seleciona o oscilador MAIN para o divisor do CCLK, PCLK e MEMCLK
	#define cclksel_clkDiv(div) { CCLKSEL = (CCLKSEL & (~(0x1f))) | (div&0x1f); }
#define USBCLKSEL		(*(reg32 *)(SCS_BASE_ADDR + 0x108))		// USB Clock Selection register
	#define usbksel_sysClk() 	{ USBCLKSEL &= ~(3UL<<8)}
	#define usbksel_PLL0() 		{ USBCLKSEL = (USBCLKSEL & (~(3UL<<8))) | (1UL<<8); }
	#define usbksel_PLL1() 		{ USBCLKSEL = (USBCLKSEL & (~(3UL<<8))) | (2UL<<8); }
	#define usbksel_clkDiv(div) { USBCLKSEL = (USBCLKSEL & (~(0x1f))) | (div&0x1f); }
#define EMCCLKSEL		(*(reg32 *)(SCS_BASE_ADDR + 0x100))		// External Memory Controller Clock Selection register
	#define MCLK_FULL_CCLK 0 // Barramento de memória vai acessar na mesma velocidade do PLL
	#define MCLK_HALF_CCLK 1 // Barramento de memória vai acessar com a metade da velocidade do PLL
#define PCLKSEL			(*(reg32 *)(SCS_BASE_ADDR + 0x1A8))		// Peripheral Clock Selection register
	#define pclksel_clkDiv(div) { PCLKSEL = div;}

#define CLKOUTCFG 	(*(reg32*)(SCS_BASE_ADDR + 0x1c8))

// INDENTIFICAÇÃO DE ORIGEM DO RESET
#define RSID           (*(reg32 *)(SCS_BASE_ADDR + 0x180))

// CONTROLE E STATUS DO SISTEMA
#define SCS            (*(reg32 *)(SCS_BASE_ADDR + 0x1A0))
	#define SCS_OSCRANGE_1MHZ_20MHZ		0x0					// Define que o a frequencia de entrada do clock para o oscilador principal está na faixa de 1MHZ a 20MHZ
	#define SCS_OSCRANGE_15MHZ_25MHZ	0x10				// Define que o a frequencia de entrada do clock para o oscilador principal está na faixa de 15MHZ a 25MHZ
	#define SCS_OSCEN					(1 << 5)			// Bit para ligar o oscilador principal
	#define SCS_OSCSTAT					(1 << 6)			// Bit para verificar se o oscilador principal está pronto para uso
	#define scs_oscOFF()				{SCS &= ~(1ul<<5); }
	#define scs_oscON()					{SCS |= (1ul<<5); }


// VALORES PADRÕES DE SEGURANÇA DE ERROS DE CONFIGURAÇÃO
#define F_CPU_MIN 			1000000 	// 1MHz
#define F_CPU_MAX 			25000000	// 25MHz
#define CCLK_MIN			10000000	// 10MHz
#define CCLK_MAX			120000000	// 120MHz
#define PCLK_MIN			30000000	// 30MHz
#define PCLK_MAX			120000000	// 120MHz
#define FCCO_MIN			156000000	// 156MHz
#define FCCO_MAX			320000000	// 320MHz
#define PLL_OUT_MIN			9750000		// 9.75MHz
#define PLL_OUT_MAX			160000000	// 160MHz
#define PLL_M_MIN			0
#define PLL_M_MAX			31

// ########################################################################################################################################
// EINT - REGISTRADORES DE INTERRUPÇÕES EXTERNAS

#define EINT0					0x1			// Interrupção externa 0
#define EINT1					0x2			// Interrupção externa 1
#define EINT2					0x4			// Interrupção externa 2
#define EINT3					0x8			// Interrupção externa 3
//
#define EXTINT         (*(reg32 *)(0x400FC140))
   	// 	X		X		X		X 	EINT3 	EINT2 	EINT1 	EINT0
   		// Escrever 1 limpa o flag de interrupção

#define EXTMODE        (*(reg32 *)(0x400FC148))
   	// 	X		X		X		X 	EINT3 	EINT2 	EINT1 	EINT0
	// EXTMODE - Modo de trigger de interrupção
	#define EXTMODE_LEVEL 		0 			// Sensivel ao nivel do sinal
	#define EXTMODE_EDGE 		1 			// Sensivel a borda do sinal

#define EXTPOLAR       (*(reg32 *)(0x400FC14C))
    // 	X		X		X		X 	EINT3 	EINT2 	EINT1 	EINT0
	// EXTPOLAR - Polaridade da sensibilidade da interrupção
	#define EXTPOLAR_LOW 		0			// Sensivel ao nivel baixo ou na queda do sinal
	#define EXTPOLAR_HIGH 		1			// Sensivel ao nivel alto ou na subida do sinal

// ########################################################################################################################################
// CONTROLADOR DE ACESSO DA MEMÓRIA EXTERNA
// datasheet pg 164
// EXTERNAL MEMORY CONTROLLER (EMC)
#define EMC_BASE_ADDR		0x2009c000
#define EMC_CTRL       		(*(reg32 *)(EMC_BASE_ADDR + 0x000))
#define EMC_STAT       		(*(reg32 *)(EMC_BASE_ADDR + 0x004))
#define EMC_CONFIG     	 	(*(reg32 *)(EMC_BASE_ADDR + 0x008))

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

#define EMC_DYN_DLYCTL  	(*(reg32 *)(0x400fc1dc))
#define EMC_CAL  			(*(reg32 *)(0x400fc1e0))

// STATIC RAM ACCESS REGISTERS
#define EMC_STA_EXT_WAIT  	(*(reg32 *)(EMC_BASE_ADDR + 0x080))
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

#define VIC_BASE_ADDR		0xE000E000

// ANEXOS PARA CANAIS NVIC para core ARM
#define VIC_MEMORYMANAGEMENT          -12      //!< 4 Cortex-M3 Memory Management Interrupt
#define VIC_BUSFAULT                  -11      //!< 5 Cortex-M3 Bus Fault Interrupt        
#define VIC_USAGEFAULT                -10      //!< 6 Cortex-M3 Usage Fault Interrupt      
#define VIC_SVCALL                    -5       //!< 11 Cortex-M3 SV Call Interrupt         
#define VIC_PENDSV                    -2       //!< 14 Cortex-M3 Pend SV Interrupt         
#define VIC_SYSTICK                   -1       //!< 15 Cortex-M3 System Tick Interrupt     

#define SHPR1   (*(reg32 *)(VIC_BASE_ADDR + 0xD18))
#define SHPR2   (*(reg32 *)(VIC_BASE_ADDR + 0xD1C))
#define SHPR3   (*(reg32 *)(VIC_BASE_ADDR + 0xD20))

// ANEXOS PARA CANAIS NVIC para os periféricos
#define VIC_WDT         0 	// Interrupção watchdog
#define VIC_TIMER0      1 	// Interrupção Timer 0
#define VIC_TIMER1      2	// Interrupção Timer 1
#define VIC_TIMER2      3 	// Interrupção Timer 2
#define VIC_TIMER3      4	// Interrupção Timer 3
#define VIC_UART0       5	// Interrupção UART 0
#define VIC_UART1       6	// Interrupção UART 1
#define VIC_UART2		7	// Interrupção UART 2
#define VIC_UART3		8	// Interrupção UART 3
#define VIC_PWM1       	9	// Interrupção PWM 1
#define VIC_I2C0        10	// Interrupção I2C 0
#define VIC_I2C1        11	// Interrupção I2C 1
#define VIC_I2C2        12	// Interrupção I2C 2
#define VIC_SSP0		14	// Interrupção SSP 0
#define VIC_SSP1		15	// Interrupção SSP 1
#define VIC_PLL0		16	// Interrupção PLL 0 (MAIN)
#define VIC_RTC			17	// Interrupção RTC 0
#define VIC_EINT0		18	// Interrupção externa 0
#define VIC_EINT1		19	// Interrupção externa 1
#define VIC_EINT2		20	// Interrupção externa 2
#define VIC_EINT3		21	// Interrupção externa 3
#define VIC_ADC			22	// Interrupção ADC
#define VIC_BOD			23	// Interrupção detecção brown out
#define VIC_USB			24	// Interrupção USB
#define VIC_CAN			25	// Interrupção CAN
#define VIC_DMA			26	// Interrupção General Purpose DMA
#define VIC_I2S			27
#define VIC_ENET		28	// Interrupção Ethernet
#define VIC_SD			29
#define VIC_MCPWM		30	// Interrupção Motor Control PWM
#define VIC_QEI			31	// Interrupção Quadrature Encoder Interface
#define VIC_PLL1		32	// Interrupção PLL1 Lock (USB PLL)
#define VIC_USBACT		33	// Interrupção USB Activity interrupt
#define VIC_CANACT		34	// Interrupção CAN Activity interrupt
#define VIC_UART4		35	// Interrupção UART 4
#define VIC_SSP2		36	// Interrupção SSP 2
#define VIC_LCD			37
#define VIC_GPIO		38
#define VIC_PWM0       	39
#define VIC_FLASH     	40	// Flash e eeprom

#define VIC_SIZE			32
#define VIC_PRIO_BITS       5         //!< Number of Bits used for Priority Levels          

// REGISTRADORES PARA HABILITAR INTERRUPÇÕES
#define ISER0   (*(reg32 *)(VIC_BASE_ADDR + 0x100)) // Habilita interrupções
#define ISER1   (*(reg32 *)(VIC_BASE_ADDR + 0x104))

// REGISTRADORES PARA DESABILITAR INTERRUPÇÕES
#define ICER0   (*(reg32 *)(VIC_BASE_ADDR + 0x180))
#define ICER1   (*(reg32 *)(VIC_BASE_ADDR + 0x184))

// REGISTRADORES PARA FORÇAR A INTERRUPÇÃO PARA MODO PENDENTE
#define ISPR0   (*(reg32 *)(VIC_BASE_ADDR + 0x200))
#define ISPR1   (*(reg32 *)(VIC_BASE_ADDR + 0x204))

// REGISTRADORES PARA REMOVER AS INTERRUPÇÕES DO MODO DE PEDENCIAS
#define ICPR0   (*(reg32 *)(VIC_BASE_ADDR + 0x280))
#define ICPR1   (*(reg32 *)(VIC_BASE_ADDR + 0x284))

// REGISTRADORES PARA INIDCAR QUAL A INTERRUPÇÃO ESTÁ ATIVA - somente de leitura
#define IABR0   (*(reg32 *)(VIC_BASE_ADDR + 0x300))
#define IABR1   (*(reg32 *)(VIC_BASE_ADDR + 0x304))

// REGISTRADORES PARA AJUSTAR AS PRIORIDADES DAS INTERRUPÇÕES
#define IPR_BASE   (*(reg32 *)(VIC_BASE_ADDR + 0x400))

// REGISTRADOR PARA INTERRUPÇÃO POR SOFTWARE - somente de escrita
#define STIR   (*(reg32 *)(VIC_BASE_ADDR + 0xF00))

*/
// ########################################################################################################################################
// I/O datasheet pagina 121
// Usados quando a função do pino (IOCON) é 1 (GPIO)
#define GPIO_BASE_ADDR		0x50000000UL

// PORT 0 acesso 32 bits
#define GPIO0_DIR        	(*(reg32 *)(GPIO_BASE_ADDR + 0x2000))
#define GPIO0_MASK       	(*(reg32 *)(GPIO_BASE_ADDR + 0x2080))
#define GPIO0_PIN        	(*(reg32 *)(GPIO_BASE_ADDR + 0x2100))
#define GPIO0_SET        	(*(reg32 *)(GPIO_BASE_ADDR + 0x2200))
#define GPIO0_CLR        	(*(reg32 *)(GPIO_BASE_ADDR + 0x2280))
#define GPIO0_NOT        	(*(reg32 *)(GPIO_BASE_ADDR + 0x2300))

// PORT 0 acesso 32 bits
#define GPIO1_DIR        	(*(reg32 *)(GPIO_BASE_ADDR + 0x2004))
#define GPIO1_MASK       	(*(reg32 *)(GPIO_BASE_ADDR + 0x2084))
#define GPIO1_PIN        	(*(reg32 *)(GPIO_BASE_ADDR + 0x2104))
#define GPIO1_SET        	(*(reg32 *)(GPIO_BASE_ADDR + 0x2204))
#define GPIO1_CLR        	(*(reg32 *)(GPIO_BASE_ADDR + 0x2284))
#define GPIO1_NOT        	(*(reg32 *)(GPIO_BASE_ADDR + 0x2303))

/*
// PORT 0 acesso 16 bits
#define FIO0DIRL       (*(reg16 *)(GPIO_BASE_ADDR + 0x00))
#define FIO0DIRU       (*(reg16 *)(GPIO_BASE_ADDR + 0x02))
#define FIO0MASKL      (*(reg16 *)(GPIO_BASE_ADDR + 0x10))
#define FIO0MASKU      (*(reg16 *)(GPIO_BASE_ADDR + 0x12))
#define FIO0PINL       (*(reg16 *)(GPIO_BASE_ADDR + 0x14))
#define FIO0PINU       (*(reg16 *)(GPIO_BASE_ADDR + 0x16))
#define FIO0SETL       (*(reg16 *)(GPIO_BASE_ADDR + 0x18))
#define FIO0SETU       (*(reg16 *)(GPIO_BASE_ADDR + 0x1A))
#define FIO0CLRL       (*(reg16 *)(GPIO_BASE_ADDR + 0x1C))
#define FIO0CLRU       (*(reg16 *)(GPIO_BASE_ADDR + 0x1E))


// PORT 0 acesso 8 bits
#define FIO0DIR0       (*(reg8 *)(GPIO_BASE_ADDR + 0x00))
#define FIO0DIR1       (*(reg8 *)(GPIO_BASE_ADDR + 0x01))
#define FIO0DIR2       (*(reg8 *)(GPIO_BASE_ADDR + 0x02))
#define FIO0DIR3       (*(reg8 *)(GPIO_BASE_ADDR + 0x03))
#define FIO0MASK0      (*(reg8 *)(GPIO_BASE_ADDR + 0x10))
#define FIO0MASK1      (*(reg8 *)(GPIO_BASE_ADDR + 0x11))
#define FIO0MASK2      (*(reg8 *)(GPIO_BASE_ADDR + 0x12))
#define FIO0MASK3      (*(reg8 *)(GPIO_BASE_ADDR + 0x13))
#define FIO0PIN0       (*(reg8 *)(GPIO_BASE_ADDR + 0x14))
#define FIO0PIN1       (*(reg8 *)(GPIO_BASE_ADDR + 0x15))
#define FIO0PIN2       (*(reg8 *)(GPIO_BASE_ADDR + 0x16))
#define FIO0PIN3       (*(reg8 *)(GPIO_BASE_ADDR + 0x17))
#define FIO0SET0       (*(reg8 *)(GPIO_BASE_ADDR + 0x18))
#define FIO0SET1       (*(reg8 *)(GPIO_BASE_ADDR + 0x19))
#define FIO0SET2       (*(reg8 *)(GPIO_BASE_ADDR + 0x1A))
#define FIO0SET3       (*(reg8 *)(GPIO_BASE_ADDR + 0x1B))
#define FIO0CLR0       (*(reg8 *)(GPIO_BASE_ADDR + 0x1C))
#define FIO0CLR1       (*(reg8 *)(GPIO_BASE_ADDR + 0x1D))
#define FIO0CLR2       (*(reg8 *)(GPIO_BASE_ADDR + 0x1E))
#define FIO0CLR3       (*(reg8 *)(GPIO_BASE_ADDR + 0x1F))

//// ########################################################################################################################################
//// CAN CONTROLLERS AND ACCEPTANCE FILTER
//#define CAN_ACCEPT_BASE_ADDR		0xE003C000
//#define CAN_AFMR		(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x00))
//#define CAN_SFF_SA 		(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x04))
//#define CAN_SFF_GRP_SA 	(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x08))
//#define CAN_EFF_SA 		(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x0C))
//#define CAN_EFF_GRP_SA 	(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x10))
//#define CAN_EOT 		(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x14))
//#define CAN_LUT_ERR_ADR (*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x18))
//#define CAN_LUT_ERR 	(*(reg32 *)(CAN_ACCEPT_BASE_ADDR + 0x1C))
//
//#define CAN_CENTRAL_BASE_ADDR		0xE0040000
//#define CAN_TX_SR 	(*(reg32 *)(CAN_CENTRAL_BASE_ADDR + 0x00))
//#define CAN_RX_SR 	(*(reg32 *)(CAN_CENTRAL_BASE_ADDR + 0x04))
//#define CAN_MSR 	(*(reg32 *)(CAN_CENTRAL_BASE_ADDR + 0x08))
//
//#define CAN1_BASE_ADDR		0xE0044000
//#define CAN1MOD 	(*(reg32 *)(CAN1_BASE_ADDR + 0x00))
//#define CAN1CMR 	(*(reg32 *)(CAN1_BASE_ADDR + 0x04))
//#define CAN1GSR 	(*(reg32 *)(CAN1_BASE_ADDR + 0x08))
//#define CAN1ICR 	(*(reg32 *)(CAN1_BASE_ADDR + 0x0C))
//#define CAN1IER 	(*(reg32 *)(CAN1_BASE_ADDR + 0x10))
//#define CAN1BTR 	(*(reg32 *)(CAN1_BASE_ADDR + 0x14))
//#define CAN1EWL 	(*(reg32 *)(CAN1_BASE_ADDR + 0x18))
//#define CAN1SR 		(*(reg32 *)(CAN1_BASE_ADDR + 0x1C))
//#define CAN1RFS 	(*(reg32 *)(CAN1_BASE_ADDR + 0x20))
//#define CAN1RID 	(*(reg32 *)(CAN1_BASE_ADDR + 0x24))
//#define CAN1RDA 	(*(reg32 *)(CAN1_BASE_ADDR + 0x28))
//#define CAN1RDB 	(*(reg32 *)(CAN1_BASE_ADDR + 0x2C))
//
//#define CAN1TFI1 	(*(reg32 *)(CAN1_BASE_ADDR + 0x30))
//#define CAN1TID1 	(*(reg32 *)(CAN1_BASE_ADDR + 0x34))
//#define CAN1TDA1 	(*(reg32 *)(CAN1_BASE_ADDR + 0x38))
//#define CAN1TDB1 	(*(reg32 *)(CAN1_BASE_ADDR + 0x3C))
//#define CAN1TFI2 	(*(reg32 *)(CAN1_BASE_ADDR + 0x40))
//#define CAN1TID2 	(*(reg32 *)(CAN1_BASE_ADDR + 0x44))
//#define CAN1TDA2 	(*(reg32 *)(CAN1_BASE_ADDR + 0x48))
//#define CAN1TDB2 	(*(reg32 *)(CAN1_BASE_ADDR + 0x4C))
//#define CAN1TFI3 	(*(reg32 *)(CAN1_BASE_ADDR + 0x50))
//#define CAN1TID3 	(*(reg32 *)(CAN1_BASE_ADDR + 0x54))
//#define CAN1TDA3 	(*(reg32 *)(CAN1_BASE_ADDR + 0x58))
//#define CAN1TDB3 	(*(reg32 *)(CAN1_BASE_ADDR + 0x5C))
//
//#define CAN2_BASE_ADDR		0xE0048000
//#define CAN2MOD 	(*(reg32 *)(CAN2_BASE_ADDR + 0x00))
//#define CAN2CMR 	(*(reg32 *)(CAN2_BASE_ADDR + 0x04))
//#define CAN2GSR 	(*(reg32 *)(CAN2_BASE_ADDR + 0x08))
//#define CAN2ICR 	(*(reg32 *)(CAN2_BASE_ADDR + 0x0C))
//#define CAN2IER 	(*(reg32 *)(CAN2_BASE_ADDR + 0x10))
//#define CAN2BTR 	(*(reg32 *)(CAN2_BASE_ADDR + 0x14))
//#define CAN2EWL 	(*(reg32 *)(CAN2_BASE_ADDR + 0x18))
//#define CAN2SR 		(*(reg32 *)(CAN2_BASE_ADDR + 0x1C))
//#define CAN2RFS 	(*(reg32 *)(CAN2_BASE_ADDR + 0x20))
//#define CAN2RID 	(*(reg32 *)(CAN2_BASE_ADDR + 0x24))
//#define CAN2RDA 	(*(reg32 *)(CAN2_BASE_ADDR + 0x28))
//#define CAN2RDB 	(*(reg32 *)(CAN2_BASE_ADDR + 0x2C))
//
//#define CAN2TFI1 	(*(reg32 *)(CAN2_BASE_ADDR + 0x30))
//#define CAN2TID1 	(*(reg32 *)(CAN2_BASE_ADDR + 0x34))
//#define CAN2TDA1 	(*(reg32 *)(CAN2_BASE_ADDR + 0x38))
//#define CAN2TDB1 	(*(reg32 *)(CAN2_BASE_ADDR + 0x3C))
//#define CAN2TFI2 	(*(reg32 *)(CAN2_BASE_ADDR + 0x40))
//#define CAN2TID2 	(*(reg32 *)(CAN2_BASE_ADDR + 0x44))
//#define CAN2TDA2 	(*(reg32 *)(CAN2_BASE_ADDR + 0x48))
//#define CAN2TDB2 	(*(reg32 *)(CAN2_BASE_ADDR + 0x4C))
//#define CAN2TFI3 	(*(reg32 *)(CAN2_BASE_ADDR + 0x50))
//#define CAN2TID3 	(*(reg32 *)(CAN2_BASE_ADDR + 0x54))
//#define CAN2TDA3 	(*(reg32 *)(CAN2_BASE_ADDR + 0x58))
//#define CAN2TDB3 	(*(reg32 *)(CAN2_BASE_ADDR + 0x5C))
//
// ########################################################################################################################################
// MultiMedia Card Interface(MCI) Controller
#define MCI_BASE_ADDR		0x400C0000
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
//
//// ########################################################################################################################################
//// I2S Interface Controller (I2S)
//#define I2S_BASE_ADDR		0xE0088000
//#define I2S_DAO        (*(reg32 *)(I2S_BASE_ADDR + 0x00))
//#define I2S_DAI        (*(reg32 *)(I2S_BASE_ADDR + 0x04))
//#define I2S_TX_FIFO    (*(reg32 *)(I2S_BASE_ADDR + 0x08))
//#define I2S_RX_FIFO    (*(reg32 *)(I2S_BASE_ADDR + 0x0C))
//#define I2S_STATE      (*(reg32 *)(I2S_BASE_ADDR + 0x10))
//#define I2S_DMA1       (*(reg32 *)(I2S_BASE_ADDR + 0x14))
//#define I2S_DMA2       (*(reg32 *)(I2S_BASE_ADDR + 0x18))
//#define I2S_IRQ        (*(reg32 *)(I2S_BASE_ADDR + 0x1C))
//#define I2S_TXRATE     (*(reg32 *)(I2S_BASE_ADDR + 0x20))
//#define I2S_RXRATE     (*(reg32 *)(I2S_BASE_ADDR + 0x24))
//
//// ########################################################################################################################################
//// General-purpose DMA Controller
//#define DMA_BASE_ADDR		0xFFE04000
//#define GPDMA_INT_STAT         (*(reg32 *)(DMA_BASE_ADDR + 0x000))
//#define GPDMA_INT_TCSTAT       (*(reg32 *)(DMA_BASE_ADDR + 0x004))
//#define GPDMA_INT_TCCLR        (*(reg32 *)(DMA_BASE_ADDR + 0x008))
//#define GPDMA_INT_ERR_STAT     (*(reg32 *)(DMA_BASE_ADDR + 0x00C))
//#define GPDMA_INT_ERR_CLR      (*(reg32 *)(DMA_BASE_ADDR + 0x010))
//#define GPDMA_RAW_INT_TCSTAT   (*(reg32 *)(DMA_BASE_ADDR + 0x014))
//#define GPDMA_RAW_INT_ERR_STAT (*(reg32 *)(DMA_BASE_ADDR + 0x018))
//#define GPDMA_ENABLED_CHNS     (*(reg32 *)(DMA_BASE_ADDR + 0x01C))
//#define GPDMA_SOFT_BREQ        (*(reg32 *)(DMA_BASE_ADDR + 0x020))
//#define GPDMA_SOFT_SREQ        (*(reg32 *)(DMA_BASE_ADDR + 0x024))
//#define GPDMA_SOFT_LBREQ       (*(reg32 *)(DMA_BASE_ADDR + 0x028))
//#define GPDMA_SOFT_LSREQ       (*(reg32 *)(DMA_BASE_ADDR + 0x02C))
//#define GPDMA_CONFIG           (*(reg32 *)(DMA_BASE_ADDR + 0x030))
//#define GPDMA_SYNC             (*(reg32 *)(DMA_BASE_ADDR + 0x034))
//
//// DMA channel 0 registers
//#define GPDMA_CH0_SRC      (*(reg32 *)(DMA_BASE_ADDR + 0x100))
//#define GPDMA_CH0_DEST     (*(reg32 *)(DMA_BASE_ADDR + 0x104))
//#define GPDMA_CH0_LLI      (*(reg32 *)(DMA_BASE_ADDR + 0x108))
//#define GPDMA_CH0_CTRL     (*(reg32 *)(DMA_BASE_ADDR + 0x10C))
//#define GPDMA_CH0_CFG      (*(reg32 *)(DMA_BASE_ADDR + 0x110))
//
//// DMA channel 1 registers
//#define GPDMA_CH1_SRC      (*(reg32 *)(DMA_BASE_ADDR + 0x120))
//#define GPDMA_CH1_DEST     (*(reg32 *)(DMA_BASE_ADDR + 0x124))
//#define GPDMA_CH1_LLI      (*(reg32 *)(DMA_BASE_ADDR + 0x128))
//#define GPDMA_CH1_CTRL     (*(reg32 *)(DMA_BASE_ADDR + 0x12C))
//#define GPDMA_CH1_CFG      (*(reg32 *)(DMA_BASE_ADDR + 0x130))
//
// ########################################################################################################################################
// USB Controller
#define USB_INT_BASE_ADDR	0x400FC1C0
#define USB_BASE_ADDR		0x2008C200		// Endereço base dos registradores USB
#define USB_RAM_BASE		0x20000000		// Endereço base da SRAM interna reservado para o USB

//#define USB_INT_STAT    (*(reg32 *)(USB_INT_BASE_ADDR + 0x00))
//
//// USB Device Interrupt Registers
//#define DEV_INT_STAT    (*(reg32 *)(USB_BASE_ADDR + 0x00))
//#define DEV_INT_EN      (*(reg32 *)(USB_BASE_ADDR + 0x04))
//#define DEV_INT_CLR     (*(reg32 *)(USB_BASE_ADDR + 0x08))
//#define DEV_INT_SET     (*(reg32 *)(USB_BASE_ADDR + 0x0C))
//#define DEV_INT_PRIO    (*(reg32 *)(USB_BASE_ADDR + 0x2C))
//
//// USB Device Endpoint Interrupt Registers
//#define EP_INT_STAT     (*(reg32 *)(USB_BASE_ADDR + 0x30))
//#define EP_INT_EN       (*(reg32 *)(USB_BASE_ADDR + 0x34))
//#define EP_INT_CLR      (*(reg32 *)(USB_BASE_ADDR + 0x38))
//#define EP_INT_SET      (*(reg32 *)(USB_BASE_ADDR + 0x3C))
//#define EP_INT_PRIO     (*(reg32 *)(USB_BASE_ADDR + 0x40))
//
//// USB Device Endpoint Realization Registers
//#define REALIZE_EP      (*(reg32 *)(USB_BASE_ADDR + 0x44))
//#define EP_INDEX        (*(reg32 *)(USB_BASE_ADDR + 0x48))
//#define MAXPACKET_SIZE  (*(reg32 *)(USB_BASE_ADDR + 0x4C))
//
//// USB Device Command Reagisters
//#define CMD_CODE        (*(reg32 *)(USB_BASE_ADDR + 0x10))
//#define CMD_DATA        (*(reg32 *)(USB_BASE_ADDR + 0x14))
//
//// USB Device Data Transfer Registers
//#define RX_DATA         (*(reg32 *)(USB_BASE_ADDR + 0x18))
//#define TX_DATA         (*(reg32 *)(USB_BASE_ADDR + 0x1C))
//#define RX_PLENGTH      (*(reg32 *)(USB_BASE_ADDR + 0x20))
//#define TX_PLENGTH      (*(reg32 *)(USB_BASE_ADDR + 0x24))
//#define USB_CTRL        (*(reg32 *)(USB_BASE_ADDR + 0x28))
//
//// USB Device DMA Registers
//#define DMA_REQ_STAT        (*(reg32 *)(USB_BASE_ADDR + 0x50))
//#define DMA_REQ_CLR         (*(reg32 *)(USB_BASE_ADDR + 0x54))
//#define DMA_REQ_SET         (*(reg32 *)(USB_BASE_ADDR + 0x58))
//#define UDCA_HEAD           (*(reg32 *)(USB_BASE_ADDR + 0x80))
//#define EP_DMA_STAT         (*(reg32 *)(USB_BASE_ADDR + 0x84))
//#define EP_DMA_EN           (*(reg32 *)(USB_BASE_ADDR + 0x88))
//#define EP_DMA_DIS          (*(reg32 *)(USB_BASE_ADDR + 0x8C))
//#define DMA_INT_STAT        (*(reg32 *)(USB_BASE_ADDR + 0x90))
//#define DMA_INT_EN          (*(reg32 *)(USB_BASE_ADDR + 0x94))
//#define EOT_INT_STAT        (*(reg32 *)(USB_BASE_ADDR + 0xA0))
//#define EOT_INT_CLR         (*(reg32 *)(USB_BASE_ADDR + 0xA4))
//#define EOT_INT_SET         (*(reg32 *)(USB_BASE_ADDR + 0xA8))
//#define NDD_REQ_INT_STAT    (*(reg32 *)(USB_BASE_ADDR + 0xAC))
//#define NDD_REQ_INT_CLR     (*(reg32 *)(USB_BASE_ADDR + 0xB0))
//#define NDD_REQ_INT_SET     (*(reg32 *)(USB_BASE_ADDR + 0xB4))
//#define SYS_ERR_INT_STAT    (*(reg32 *)(USB_BASE_ADDR + 0xB8))
//#define SYS_ERR_INT_CLR     (*(reg32 *)(USB_BASE_ADDR + 0xBC))
//#define SYS_ERR_INT_SET     (*(reg32 *)(USB_BASE_ADDR + 0xC0))
//
// USB Host and OTG registers are for LPC24xx only
// USB Host Controller
#define USBHC_BASE_ADDR		0x2008C000
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

#define HcInterruptDisable      (*(reg32 *)(USBHC_BASE_ADDR + 0x14))
#define HcHCCA             		(*(reg32 *)(USBHC_BASE_ADDR + 0x18))
#define HcPeriodCurrentED   	(*(reg32 *)(USBHC_BASE_ADDR + 0x1C))
#define HcControlHeadED     	(*(reg32 *)(USBHC_BASE_ADDR + 0x20))
#define HcControlCurrentED      (*(reg32 *)(USBHC_BASE_ADDR + 0x24))
#define HcBulkHeadED 	    	(*(reg32 *)(USBHC_BASE_ADDR + 0x28))
#define HcBulkCurrentED      	(*(reg32 *)(USBHC_BASE_ADDR + 0x2C))
#define HcDoneHead        		(*(reg32 *)(USBHC_BASE_ADDR + 0x30))
#define HcFmInterval 	    	(*(reg32 *)(USBHC_BASE_ADDR + 0x34))
#define HcFmRemaining     		(*(reg32 *)(USBHC_BASE_ADDR + 0x38))
#define HcFmNumber        		(*(reg32 *)(USBHC_BASE_ADDR + 0x3C))
#define HcPeriodicStart     	(*(reg32 *)(USBHC_BASE_ADDR + 0x40))
#define HcLSThreshold        	(*(reg32 *)(USBHC_BASE_ADDR + 0x44))
#define HcRhDescriptorA         (*(reg32 *)(USBHC_BASE_ADDR + 0x48))
#define HcRhDescriptorB         (*(reg32 *)(USBHC_BASE_ADDR + 0x4C))
#define HcRhStatus          	(*(reg32 *)(USBHC_BASE_ADDR + 0x50))
	#define  HC_OR_RH_STATUS_LPSC              0x00010000
	#define  HC_OR_RH_STATUS_DRWE              0x00008000

#define HcRhPortStatus1    (*(reg32 *)(USBHC_BASE_ADDR + 0x54))
#define HcRhPortStatus2    (*(reg32 *)(USBHC_BASE_ADDR + 0x58))
	#define  HC_OR_RH_PORT_CCS                 0x00000001
	#define  HC_OR_RH_PORT_PRS                 0x00000010
	#define  HC_OR_RH_PORT_CSC                 0x00010000
	#define  HC_OR_RH_PORT_PRSC                0x00100000

// USB OTG Controller
#define USBOTG_BASE_ADDR	0x2008C100
#define OTG_INT_STAT        (*(reg32 *)(USBOTG_BASE_ADDR + 0x00))
#define OTG_INT_EN          (*(reg32 *)(USBOTG_BASE_ADDR + 0x04))
#define OTG_INT_SET         (*(reg32 *)(USBOTG_BASE_ADDR + 0x08))
#define OTG_INT_CLR         (*(reg32 *)(USBOTG_BASE_ADDR + 0x0C))
// On LPC23xx, the name is OTGStCtrl, on LPC24xx, the name is OTG_STAT_CTRL
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
#define USBOTG_CLK_BASE_ADDR	0x2008CFF0
#define OTG_CLK_CTRL        (*(reg32 *)(USBOTG_CLK_BASE_ADDR + 0x04))
#define OTG_CLK_STAT        (*(reg32 *)(USBOTG_CLK_BASE_ADDR + 0x08))

// Note: below three register name convention is for LPC23xx USB device only, match with the spec. update in USB Device Section.
#define OTGStCtrl         	(*(reg32 *)(USBOTG_BASE_ADDR + 0x10))
#define USBClkCtrl          (*(reg32 *)(USBOTG_CLK_BASE_ADDR + 0x04))
#define USBClkSt            (*(reg32 *)(USBOTG_CLK_BASE_ADDR + 0x08))
//
//// ########################################################################################################################################
//// Ethernet MAC (32 bit data bus) -- all registers are RW unless indicated in parentheses
//#define MAC_BASE_ADDR		0xFFE00000 // AHB Peripheral # 0
//#define MAC_MAC1            (*(reg32 *)(MAC_BASE_ADDR + 0x000)) // MAC config reg 1
//#define MAC_MAC2            (*(reg32 *)(MAC_BASE_ADDR + 0x004)) // MAC config reg 2
//#define MAC_IPGT            (*(reg32 *)(MAC_BASE_ADDR + 0x008)) // b2b InterPacketGap reg
//#define MAC_IPGR            (*(reg32 *)(MAC_BASE_ADDR + 0x00C)) // non b2b InterPacketGap reg
//#define MAC_CLRT            (*(reg32 *)(MAC_BASE_ADDR + 0x010)) // CoLlision window/ReTry reg
//#define MAC_MAXF            (*(reg32 *)(MAC_BASE_ADDR + 0x014)) // MAXimum Frame reg
//#define MAC_SUPP            (*(reg32 *)(MAC_BASE_ADDR + 0x018)) // PHY SUPPort reg
//#define MAC_TEST            (*(reg32 *)(MAC_BASE_ADDR + 0x01C)) // TEST reg
//#define MAC_MCFG            (*(reg32 *)(MAC_BASE_ADDR + 0x020)) // MII Mgmt ConFiG reg
//#define MAC_MCMD            (*(reg32 *)(MAC_BASE_ADDR + 0x024)) // MII Mgmt CoMmanD reg
//#define MAC_MADR            (*(reg32 *)(MAC_BASE_ADDR + 0x028)) // MII Mgmt ADdRess reg
//#define MAC_MWTD            (*(reg32 *)(MAC_BASE_ADDR + 0x02C)) // MII Mgmt WriTe Data reg (WO)
//#define MAC_MRDD            (*(reg32 *)(MAC_BASE_ADDR + 0x030)) // MII Mgmt ReaD Data reg (RO)
//#define MAC_MIND            (*(reg32 *)(MAC_BASE_ADDR + 0x034)) // MII Mgmt INDicators reg (RO)
//
//#define MAC_SA0             (*(reg32 *)(MAC_BASE_ADDR + 0x040)) // Station Address 0 reg
//#define MAC_SA1             (*(reg32 *)(MAC_BASE_ADDR + 0x044)) // Station Address 1 reg
//#define MAC_SA2             (*(reg32 *)(MAC_BASE_ADDR + 0x048)) // Station Address 2 reg
//
//#define MAC_COMMAND         (*(reg32 *)(MAC_BASE_ADDR + 0x100)) // Command reg
//#define MAC_STATUS          (*(reg32 *)(MAC_BASE_ADDR + 0x104)) // Status reg (RO)
//#define MAC_RXDESCRIPTOR    (*(reg32 *)(MAC_BASE_ADDR + 0x108)) // Rx descriptor base address reg
//#define MAC_RXSTATUS        (*(reg32 *)(MAC_BASE_ADDR + 0x10C)) // Rx status base address reg
//#define MAC_RXDESCRIPTORNUM (*(reg32 *)(MAC_BASE_ADDR + 0x110)) // Rx number of descriptors reg
//#define MAC_RXPRODUCEINDEX  (*(reg32 *)(MAC_BASE_ADDR + 0x114)) // Rx produce index reg (RO)
//#define MAC_RXCONSUMEINDEX  (*(reg32 *)(MAC_BASE_ADDR + 0x118)) // Rx consume index reg
//#define MAC_TXDESCRIPTOR    (*(reg32 *)(MAC_BASE_ADDR + 0x11C)) // Tx descriptor base address reg
//#define MAC_TXSTATUS        (*(reg32 *)(MAC_BASE_ADDR + 0x120)) // Tx status base address reg
//#define MAC_TXDESCRIPTORNUM (*(reg32 *)(MAC_BASE_ADDR + 0x124)) // Tx number of descriptors reg
//#define MAC_TXPRODUCEINDEX  (*(reg32 *)(MAC_BASE_ADDR + 0x128)) // Tx produce index reg
//#define MAC_TXCONSUMEINDEX  (*(reg32 *)(MAC_BASE_ADDR + 0x12C)) // Tx consume index reg (RO)
//
//#define MAC_TSV0            (*(reg32 *)(MAC_BASE_ADDR + 0x158)) // Tx status vector 0 reg (RO)
//#define MAC_TSV1            (*(reg32 *)(MAC_BASE_ADDR + 0x15C)) // Tx status vector 1 reg (RO)
//#define MAC_RSV             (*(reg32 *)(MAC_BASE_ADDR + 0x160)) // Rx status vector reg (RO)
//
//#define MAC_FLOWCONTROLCNT  (*(reg32 *)(MAC_BASE_ADDR + 0x170)) // Flow control counter reg
//#define MAC_FLOWCONTROLSTS  (*(reg32 *)(MAC_BASE_ADDR + 0x174)) // Flow control status reg
//
//#define MAC_RXFILTERCTRL    (*(reg32 *)(MAC_BASE_ADDR + 0x200)) // Rx filter ctrl reg
//#define MAC_RXFILTERWOLSTS  (*(reg32 *)(MAC_BASE_ADDR + 0x204)) // Rx filter WoL status reg (RO)
//#define MAC_RXFILTERWOLCLR  (*(reg32 *)(MAC_BASE_ADDR + 0x208)) // Rx filter WoL clear reg (WO)
//
//#define MAC_HASHFILTERL     (*(reg32 *)(MAC_BASE_ADDR + 0x210)) // Hash filter LSBs reg
//#define MAC_HASHFILTERH     (*(reg32 *)(MAC_BASE_ADDR + 0x214)) // Hash filter MSBs reg
//
//#define MAC_INTSTATUS       (*(reg32 *)(MAC_BASE_ADDR + 0xFE0)) // Interrupt status reg (RO)
//#define MAC_INTENABLE       (*(reg32 *)(MAC_BASE_ADDR + 0xFE4)) // Interrupt enable reg
//#define MAC_INTCLEAR        (*(reg32 *)(MAC_BASE_ADDR + 0xFE8)) // Interrupt clear reg (WO)
//#define MAC_INTSET          (*(reg32 *)(MAC_BASE_ADDR + 0xFEC)) // Interrupt set reg (WO)
//
//#define MAC_POWERDOWN       (*(reg32 *)(MAC_BASE_ADDR + 0xFF4)) // Power-down reg
//#define MAC_MODULEID        (*(reg32 *)(MAC_BASE_ADDR + 0xFFC)) // Module ID reg (RO)



// ########################################################################################################################################
// IAP/ISP
#define IAP_LOCATION            (0x1FFF1FF1) // Definindo o ponto de entrada das funções IAP
#define IAP_CMD_PREPARE         (50)
#define IAP_CMD_COPYRAMTOFLASH  (51)
#define IAP_CMD_ERASE           (52)
#define IAP_CMD_BLANKCHECK      (53)
#define IAP_CMD_READPARTID      (54)
#define IAP_CMD_READBOOTCODEVER (55)
#define IAP_CMD_COMPARE         (56)
#define IAP_CMD_REINVOKEISP     (57)


// ########################################################################################################################################
// TFT datasheet pg 288
#define LCD_BASE_ADDR	0x20088000
#define LCD_CFG        	(*(reg32 *)(0x400FC1B8))			// LCD Configuration and clocking control
#define LCD_TIMH        (*(reg32 *)(LCD_BASE_ADDR + 0x000)) // Horizontal Timing Control register
#define LCD_TIMV        (*(reg32 *)(LCD_BASE_ADDR + 0x004)) // Vertical Timing Control register
#define LCD_POL        	(*(reg32 *)(LCD_BASE_ADDR + 0x008)) // Clock and Signal Polarity Control register
#define LCD_LE        	(*(reg32 *)(LCD_BASE_ADDR + 0x00c)) // Line End Control register 0 R/W 227
#define LCD_UPBASE      (*(reg32 *)(LCD_BASE_ADDR + 0x010)) // Upper Panel Frame Base Address register
#define LCD_LPBASE      (*(reg32 *)(LCD_BASE_ADDR + 0x014)) // Lower Panel Frame Base Address register
#define LCD_CTRL        (*(reg32 *)(LCD_BASE_ADDR + 0x018)) // LCD Control register
#define LCD_INTMSK      (*(reg32 *)(LCD_BASE_ADDR + 0x01c)) // Interrupt Mask register
#define LCD_INTRAW      (*(reg32 *)(LCD_BASE_ADDR + 0x020)) // Raw Interrupt Status register
#define LCD_INTSTAT     (*(reg32 *)(LCD_BASE_ADDR + 0x024)) // Masked Interrupt Status register
#define LCD_INTCLR      (*(reg32 *)(LCD_BASE_ADDR + 0x028)) // Interrupt Clear register
#define LCD_UPCURR      (*(reg32 *)(LCD_BASE_ADDR + 0x02c)) // Upper Panel Current Address Value register
#define LCD_LPCURR      (*(reg32 *)(LCD_BASE_ADDR + 0x030)) // Lower Panel Current Address Value register

#define LCD_PAL_BASE 		(*(u8 *) (LCD_BASE_ADDR + 0x200))
#define LCD_CRSR_IMG_BASE	0x20088800

#define CRSR_CTRL      	(*(reg32 *)(LCD_BASE_ADDR + 0xc00)) // Cursor Control register
#define CRSR_CFG      	(*(reg32 *)(LCD_BASE_ADDR + 0xc04)) // Cursor Configuration register
	#define CRSR_PIX_32				0
	#define CRSR_PIX_64				1
	#define CRSR_ASYNC				0
	#define CRSR_FRAME_SYNC			2
#define CRSR_PAL0      	(*(reg32 *)(LCD_BASE_ADDR + 0xc08)) // Cursor Palette register
#define CRSR_PAL1      	(*(reg32 *)(LCD_BASE_ADDR + 0xc0c)) // Cursor Palette register
#define CRSR_XY     	(*(reg32 *)(LCD_BASE_ADDR + 0xc10)) // Cursor XY Position register
#define CRSR_CLIP      	(*(reg32 *)(LCD_BASE_ADDR + 0xc14)) // Cursor Clip Position register
#define CRSR_INTMSK     (*(reg32 *)(LCD_BASE_ADDR + 0xc20)) // Cursor Interrupt Mask register
#define CRSR_INTCLR     (*(reg32 *)(LCD_BASE_ADDR + 0xc24)) // Cursor Interrupt Clear register
#define CRSR_INTRAW     (*(reg32 *)(LCD_BASE_ADDR + 0xc28)) // Cursor Raw Interrupt Status register
#define CRSR_INTSTAT    (*(reg32 *)(LCD_BASE_ADDR + 0xc2c)) // Cursor Masked Interrupt Status register

// ########################################################################################################################################
// EEPROM
#define EE_BASE_ADDR	0x200000
#define EECMD        	(*(reg32 *)(EE_BASE_ADDR + 0x080))
#define EEADDR        	(*(reg32 *)(EE_BASE_ADDR + 0x084))
#define EEWDATA        	(*(reg32 *)(EE_BASE_ADDR + 0x088))
#define EERDATA        	(*(reg32 *)(EE_BASE_ADDR + 0x08c))
#define EEWSTATE       	(*(reg32 *)(EE_BASE_ADDR + 0x090))
#define EECLKDIV       	(*(reg32 *)(EE_BASE_ADDR + 0x094))
#define EEPWRDWN       	(*(reg32 *)(EE_BASE_ADDR + 0x098))
	#define PWRDWN	1
#define EEINTEN        	(*(reg32 *)(EE_BASE_ADDR + 0xFe4))
#define EEINTENCLR      (*(reg32 *)(EE_BASE_ADDR + 0xFd8))
#define EEINTENSET      (*(reg32 *)(EE_BASE_ADDR + 0xFdc))
#define EEINTSTAT       (*(reg32 *)(EE_BASE_ADDR + 0xFe0))
#define EEINTSTATCLR    (*(reg32 *)(EE_BASE_ADDR + 0xFe8))
#define EEINTSTATSET    (*(reg32 *)(EE_BASE_ADDR + 0xFec))
*/

#endif

