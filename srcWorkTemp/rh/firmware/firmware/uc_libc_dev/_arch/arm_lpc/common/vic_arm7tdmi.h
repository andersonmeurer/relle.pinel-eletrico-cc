#ifndef __VIC_ARM7TDMI_H 
#define __VIC_ARM7TDMI_H

#include "_config_cpu_.h"

#if defined(FREE_RTOS) // USANDO FREE RTOS
	#include "FreeRTOS.h"
	#include "task.h"

	// Utilize este atributo para indicar que a fun��o especifica � um gerenciador de interrup��o para GCC.
	//	O compilador gerar� fun��es de entrada e saida ajustada para uso
	#define __irq __attribute__ ((naked))		// O FREE RTOS tem o seu pr�prio tratamento na entrada e saida do atendiento da rotina de interrup��o. N�o precisando do prolog e epilog do compilador
	#define __fiq __attribute__ ((interrupt("FIQ")))
	#define __abort __attribute__ ((interrupt("ABORT")))
	#define __undef __attribute__ ((interrupt("UNDEF")))

	#define ISR_EXIT  	portRESTORE_CONTEXT()
	#define ISR_ENTRY 	portSAVE_CONTEXT()

	#define irq_Disable taskDISABLE_INTERRUPTS
	#define irq_Enable 	taskENABLE_INTERRUPTS

	void vic_Init(void);
	int irq_Install(u32 int_number, void *handler_addr, u32 priority);

	// OS SISTEMAS OPERACIONAIS TEM SEUS PR�PRIOS CONTROLE DE INTERRUP��ES por software por isso n�o vai _swi
	void vectorUndefHandler(void) __undef;
	void vectorPrefetchAbortHandler(void) __abort;
	void vectorDataAbortHandler(void) __abort;
	void vectorIrqHandler(void) __irq;
	void vectorFiqHandler(void) __fiq;

#else // SEM USO DE SISTEMA OPERACIONAL
	#define IRQ_MASK 0x80
	#define FIQ_MASK 0x40
	#define INT_MASK (IRQ_MASK | FIQ_MASK)

	// Utilize este atributo para indicar que a fun��o especifica � um gerenciador de interrup��o para GCC.
	//	O compilador gerar� fun��es de entrada e saida ajustada para uso
	#define __irq __attribute__ ((interrupt ("IRQ")))
	#define __fiq __attribute__ ((interrupt("FIQ")))
	#define __swi __attribute__ ((interrupt("SWI")))
	#define __abort __attribute__ ((interrupt("ABORT")))
	#define __undef __attribute__ ((interrupt("UNDEF")))

	#if (VIC_USE == VIC_USE_NORMAL)
		// ##############################################################################################################################
		// TRATAMENTO DE INTERRUP��ES PELO PROCEDIMENTO PADR�O
		//		Atende uma interrup��o por vez, ou seja, este tipo de tratamento somente atende as outras interrup��es
		//		em pedendencia s� quando finalizar o tratamento da interrup��o atual. Salva somente os reg necess�rios

	
		// N�O � PRECISO ADICIONAR OS PROCEDIMENTO DE ENTRADA E SAIDA DO ISR,
		//	Devido que o compilador adiciona os procedimentos automaticamente pela macro __irq
		#define ISR_EXIT
		#define ISR_ENTRY

	#elif (VIC_USE == VIC_USE_NESTED)
		// ##############################################################################################################################
		// TRATAMENTO DE INTERRUP��ES ANINHADAS (NESTED)
		//		Este tipo de tratamento permite que outras interrup��es de maior prioridade sejam atendidadas antes de finalizar
		//			o tratamento da interrup��o atual. Salva somente os reg necess�rios

		// Entrada (prolog) da interrup��o NESTED
			// Copia SPSR_irq para LR
			// Salva SPSR_irq na pilha
	 		// Habilita IRQ (modo sys)
			// Salava LR na pilha
		#define ISR_ENTRY asm volatile(		\
			"MRS     LR, SPSR \r\n\t"  		\
			"STMFD   SP!, {LR} \r\n\t" 		\
	 		"MSR     CPSR_c, #0x1F \r\n\t" 	\
			"STMFD   SP!, {LR} " 			\
		)

		// Sa�da (epilog) da interrup��o NESTED
			// Restaura LR da pilha
 			// Desabilita IRQ (modo IRQ)
			// Restaura SPSR_irq da pilha
			// Copia LR para SPSR_irq
		#define ISR_EXIT asm volatile( 	\
			"LDMFD   SP!, {LR} \n" 		\
 			"MSR     CPSR_c, #0x92 \n" 	\
			"LDMFD   SP!, {LR} \n" 		\
			"MSR     SPSR_cxsf, LR \n" 	\
		)

	#else
		#error N�O FOI ESPECIFICADO NO _config_cpu_.h O TIPO DE TRATAMENTO DE INTERRUP��O
	#endif

	void vic_Init(void);
	int irq_Install(u32 int_number, void *handler_addr, u32 priority);

	inline bool irq_IsEnabled(void);
	inline u32 irq_Disable(void);
	inline u32 irq_Enable(void);
	inline u32 irq_Restore(u32 oldCPSR);
	inline u32 fiq_Disable(void);
	inline u32 fiq_Enable(void);
	inline u32 fiq_Restore(u32 oldCPSR);
	u32 ints_Disable (void);
	u32 ints_Restore (u32 oldCPSR);

	void vectorSwiHandler(void) __swi;
	void vectorUndefHandler(void) __undef;
	void vectorPrefetchAbortHandler(void) __abort;
	void vectorDataAbortHandler(void) __abort;
	void vectorIrqHandler(void) __irq;
	void vectorFiqHandler(void) __fiq;

#endif // FREE_RTOS


#endif

