#include "vic.h"
#if (VIC_USE_DEBUG == pdON) || (VICERR_USE_DEBUG == pdON)
#include"stdio_uc.h"
#endif



// OS SISTEMAS OPERACIONAIS TEM SEUS PR�PRIOS CONTROLE DE INTERRUP��ES
#if defined(RTOS_NONE)
// CAPTURA O VALOR DO REGISTRADOR CPSR
static inline u32 __get_cpsr(void) {
	u32 retval;
	asm volatile (" mrs  %0, cpsr" : "=r" (retval) : /* no inputs */  );
	return retval;
}

// GRAVA UM VALOR NO REGISTRADOR CPSR
static inline void __set_cpsr(u32 val) {
	asm volatile (" msr  cpsr, %0" : /* no outputs */ : "r" (val)  );
}
#endif


// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa os canais IRQ vetorado
// Parametros:	nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void vic_Init(void) {
    int i = 0;
    u32 *vect_addr, *vect_prio;

   	#if defined(RTOS_NONE)
   	ints_Disable();
   	#endif

    VICIntEnClr = 0xFFFFFFFF; 															// Desabilita as interrup��es de todos os dispositivos. Cada bit do reg corresponde a um dispositivo do LPC
    VICVectAddr = 0;																	// Limpa o registrador que guarda o endere�o da interrup��o atual ativa quando ocorrer uma interrup��o IRQ
    VICIntSelect = 0;																	// Todos os dispositivos do LPC s�o atendidade pelo IRQ e n�o pela FIQ

    for ( i = 0; i < VIC_SIZE; i++ ) {													// Fa�a para todos os SLOTS de int
		vect_addr = (u32 *)(VIC_BASE_ADDR + VIC_VECT_ADDR_INDEX + i*4);					// Captura o endere�o do registrador que guarda o endere�o da fun��o de tratamento de interrup��o
		vect_prio = (u32 *)(VIC_BASE_ADDR + VIC_VECT_PRIO_INDEX + i*4);					// Captura o endere�o do registrador de controle de interrup��o
		*vect_addr = 0;																	// Limpa qualquer endere�o de fun��o de tratamento de interrup��o ao registrador que aponta a SLOT(i)

		#if defined(LPC2468)
			*vect_prio = 0xF;															// Limpa controle do vetor
		#else
			*vect_prio = 0;																// Limpa controle do vetor
		#endif
    } 
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Instala um dispositivo (canal) a uma SLOT de interrup��o. 
// Parametros:	Numero do canal (dispositivo o endere�o da fun��o e a prioridade
// Retorna:		Retorna pdPASS se foi instalado com sucesso sen�o retorna c�digo do erro
// ----------------------------------------------------------------------------------------------------------------
int irq_Install(u32 int_number, void *handler_addr, u32 priority) {
    u32 *vect_addr;
    u32 *vect_prio;
      
    VICIntEnClr = 1 << int_number;														// Desabilita a interrup��o do dispositivo selecionado. Cada bit do reg corresponde a um dispositivo do LPC
    
	if ( int_number >= VIC_SIZE )														// Checa se o dipositivo est� fora, n�o pertence, dos slots de interrup��o
    	return ( errIRQ_SLOT_OUT_OF_RANGE );																
    else   {
		#if defined(LPC2468)
			vect_addr = (u32 *)(VIC_BASE_ADDR + VIC_VECT_ADDR_INDEX + int_number*4);	// Captura o endere�o do registrador que guarda o endere�o da fun��o de tratamento de interrup��o
			vect_prio = (u32 *)(VIC_BASE_ADDR + VIC_VECT_PRIO_INDEX + int_number*4);	// Captura o endere�o do registrador de controle de interrup��o
			*vect_addr = (u32)handler_addr;												// Anexa o endere�o da fun��o de tratamento da interrup��o ao registrador que aponta o SLOTx
			*vect_prio = priority;														// Determina que o prioridade o SLOTx tem
			VICIntEnable = 1 << int_number;												// Habilita interrup��o do dispositivo selecionado. Cada bit do reg corresponde a um dispositivo do LPC
			return  pdPASS;
		#else
			vect_addr = (u32 *)(VIC_BASE_ADDR + VIC_VECT_ADDR_INDEX + priority*4);		// Captura o endere�o do registrador que guarda o endere�o da fun��o de tratamento de interrup��o
			vect_prio = (u32 *)(VIC_BASE_ADDR + VIC_VECT_PRIO_INDEX + priority*4);		// Captura o endere�o do registrador de controle de interrup��o
			
			if ( *vect_addr == (u32)((void *)0) ) {										// Checa se o SLOT da interrup��o esta desocupado
		    	*vect_addr = (u32)handler_addr;											// Anexa o endere�o da fun��o de tratamento da interrup��o ao registrador que aponta o SLOTx
	    		*vect_prio = (u32)(VIC_ENABLE | int_number);							// Habilita int do SLOT(i)
	    		VICIntEnable = 1 << int_number;											// Habilita interrup��o do dispositivo selecionado. Cada bit do reg corresponde a um dispositivo do LPC
	    		return  pdPASS;
			} else																		// Retona falso, porque o SLOT j� est� sendo ocpupador por outro ISR
				return  errIRQ_SLOT_FULL;
		#endif	
    }
}


// OS SISTEMAS OPERACIONAIS TEM SEUS PR�PRIOS CONTROLE DE INTERRUP��ES
#if defined(RTOS_NONE)
// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Retorna pdTRUE se a interrup��o irq estiver habilitada, sen�o retorna pdFALSE
// Parametros: 	Nenhum
// Retorna:		pdTRUE ou pdFALSE
// -------------------------------------------------------------------------------------------------------------------
inline bool irq_IsEnabled(void) {
	u32 _cpsr;
	 _cpsr = __get_cpsr();
	 return (_cpsr & IRQ_MASK)? pdFALSE:pdTRUE;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Desabilita a interrup��o IRQ ( bit 7 do registrador CPSR)
// Parametros: 	Nenhum
// Retorna:		O valor anterior do registrador CPSR
// -------------------------------------------------------------------------------------------------------------------
inline u32 irq_Disable(void) {
	u32 _cpsr;

  	_cpsr = __get_cpsr();
  	__set_cpsr(_cpsr | IRQ_MASK);
  	return _cpsr;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Habilita a interrup��o IRQ ( bit 7 do registrador CPSR)
// Parametros: 	Nenhum
// Retorna:		O valor anterior do registrador CPSR
// -------------------------------------------------------------------------------------------------------------------
inline u32 irq_Enable(void) {
	u32 _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr(_cpsr & ~IRQ_MASK);
	return _cpsr;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Restaura a condi��o do flag de interrup��o contido no parametro
// Parametros: 	Valor do registrador cpsr lido pelo irq_Enable ou irq_Disable
// Retorna:		O valor anterior do registrador CPSR
// -------------------------------------------------------------------------------------------------------------------
inline u32 irq_Restore(u32 oldCPSR) {
	u32 _cpsr;

	_cpsr = __get_cpsr();
	//__set_cpsr((_cpsr & ~IRQ_MASK) | (oldCPSR & IRQ_MASK));
	__set_cpsr(_cpsr & (oldCPSR | ~IRQ_MASK));
	return _cpsr;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Desabilita a interrup��o FIQ ( bit 6 do registrador CPSR)
// Parametros: 	Nenhum
// Retorna:		O valor anterior do registrador CPSR
// -------------------------------------------------------------------------------------------------------------------
inline u32 fiq_Disable(void) {
	u32 _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr(_cpsr | FIQ_MASK);
	return _cpsr;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Habilita a interrup��o FIQ ( bit 6 do registrador CPSR)
// Parametros: 	Nenhum
// Retorna:		O valor anterior do registrador CPSR
// -------------------------------------------------------------------------------------------------------------------
inline u32 fiq_Enable(void) {
	u32 _cpsr;

	_cpsr = __get_cpsr();
  	__set_cpsr(_cpsr & ~FIQ_MASK);
  	return _cpsr;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Restaura a condi��o do flag de interrup��o contido no parametro
// Parametros: 	Valor do registrador cpsr lido pelo fiq_Enable ou fiq_Disable
// Retorna:		O valor anterior do registrador CPSR
// -------------------------------------------------------------------------------------------------------------------
inline u32 fiq_Restore(u32 oldCPSR) {
	u32 _cpsr;

	_cpsr = __get_cpsr();
	//__set_cpsr((_cpsr & ~FIQ_MASK) | (oldCPSR & FIQ_MASK));
	__set_cpsr(_cpsr & (oldCPSR | ~FIQ_MASK));
	return _cpsr;
}


u32 ints_Disable (void) {
	u32 _cpsr;

	_cpsr = __get_cpsr ();

	do
		__set_cpsr (_cpsr | INT_MASK);
	while ((__get_cpsr () ^ INT_MASK) & INT_MASK);

	return _cpsr;
}

u32 ints_Restore (u32 oldCPSR) {
	u32 _cpsr;

  	_cpsr = __get_cpsr();

  	do
  		__set_cpsr ((_cpsr & ~INT_MASK) | (oldCPSR & INT_MASK));
  	while ((__get_cpsr () ^ oldCPSR) & INT_MASK);

  	return _cpsr;
}
#endif


// ######################################################################################################
// FUN��ES CHAMADAS PELO vector.s quando algum interrup��o � gerada

#if defined(RTOS_NONE)
// OS SISTEMAS OPERACIONAIS TEM SEUS PR�PRIOS CONTROLE DE INTERRUP��ES por software
// Fun��o que trata interrup��o por software
void vectorSwiHandler(void) {
	#if VICERR_USE_DEBUG == pdON
	plognp("ERRO FATAL: Interrup��o SWI n�o prevista "CMD_TERMINATOR);
	#endif
	while(1);
}
#endif

// Fun��o que trata interrup��o de instru��es indefinidas
void vectorUndefHandler (void) {
	#if VICERR_USE_DEBUG == pdON
	plognp("ERRO FATAL: Interrup��o UNDEF n�o prevista "CMD_TERMINATOR);
	#endif
	while(1);
}

// Fun��o que trata interrup��o de prefetch abort
void vectorPrefetchAbortHandler(void) {
	#if VICERR_USE_DEBUG == pdON
	plognp("ERRO FATAL: Interrup��o Prefetch ABORT n�o prevista "CMD_TERMINATOR);
	#endif
	while(1);
}

// Fun��o que trata interrup��o de data abort
void vectorDataAbortHandler(void) {
	#if VICERR_USE_DEBUG == pdON
	plognp("ERRO FATAL: Interrup��o DATAABORT n�o prevista "CMD_TERMINATOR);
	#endif
	while(1);
}


/*
 * Estou usando um arm7tdmi da NXP modelo lpc
Estou com problema em atender duas interrup��es que foram geradas ao mesmo tempo em uma VIC Vertored.
Uma das ints n�o � atendida pelo hardware e n�o quero usar a interrup��o NESTED (preemptiva)
Como vcs gerenciam essas interrup��es?

esse � o techo de c�digo para tratameto que uso
void vectorIrqHandler(void) {
	u32 _cpsr;
	_cpsr= irq_Disable(); // desabilitar ints. Tentativas de evitar espurias de ints. Pois o VIC do ARM7 tem bug
	static void (*isr)(void);
	isr = (void(*)(void)) VICVectAddr;

	if (isr != NULL)
		isr(); 		// Chama a fun��o ISR

	VICVectAddr = 0;             	// Limpa o registrador de endere�o do ISR, para novas interrup��es

	irq_Restore(_cpsr);
}
Pergunta 2
Essa tal de espuria de interrup��o do processador ARM7?
Como vc a tratam?
Li sobre o assunto e fiquei mais na d�vida ainda
*/

#if defined(SIM_PROTEUS)
// Fun��o que trata interrup��es IRQ
// TODO - Somemte este m�todo funciona no proteus, e com isso as interrup��es de mesma prioridade, ou prioridade inferior,
//	emitidas ao mesmo tempo, somente a inetrrup��o de maior prioridade ser� atendidade e as demais ser�o descartadas
void vectorIrqHandler(void) {
	u32 _cpsr = irq_Disable(); 		// desabilitar ints. Tentativas de evitar espurias de ints. Pois o VIC do ARM7 tem bug
	static void (*isr)(void);
	isr = (void(*)(void)) VICVectAddr;
	isr(); 							// Chama a fun��o ISR
	VICVectAddr = 0;             	// Limpa o registrador de endere�o do ISR, para novas interrup��es
	irq_Restore(_cpsr);
	//plog("0x%x 0x%x 0x%x"CMD_TERMINATOR, VICVectAddr, VICRawIntr, VICIRQStatus);
}
#else
// Fun��o que trata interrup��es IRQ
// Este procedimeto trava com o proteus
void vectorIrqHandler(void) {
	u32 _cpsr = irq_Disable(); // desabilitar ints. Tentativas de ebitar espurias de ints. Pois o VIC do ARM7 tem bug. TODO - m�todo ISR IRQ prec�rio e em teste
	u32 stIRQ = VICIRQStatus; // capturar status, porque esse reg � limpo na chamada da fun��o isr()

	static void (*isr)(void);
	// isr = (void(*)(void)) VICVectAddr; // Captura o endere�o da fun��o ISR
	//isr(); 	// Chama a fun��o ISR de maior prioridade
	//VICVectAddr = 0;             	// Limpa o registrador de endere�o do ISR, para novas interrup��es
	//irq_Restore(_cpsr);

	// TESTADO SOMENTE NO LPC2468
	// � uma tentativa de processar mais de uma interrup��o emitida ao mesmo tempo
	// Este procedimento trata todas as ISRs com a mesma prioridade
	// TODO - m�todo ISR IRQ prec�rio e est� em testes. Fazer mais teste com muitas ints e deviersas fontes
	// � um m�todo bem lento de se tratar as interrup��es e pode haver overrun em ISR muito longas em interrup��es simultaneas
	//	Fora testado somente com ints EINT1, EINT2, UART, timer0 MACTH0 e timer1 MACTH0
	//		 EINT1, EINT2 gerado a cada 1uS ao mesmo tempo para as duas entradas,
	//			ou uma entrada era gerada de 1 a 10 nops ap�s a outra

	int i;
	u32 *vect_addr;
	for ( i = 0; i < VIC_SIZE; i++ ) {													// Fa�a para todos os SLOTS de int
		if ((stIRQ&(1<<i)) == 0) continue; // Ignora se n�o foi emitido pedido de interrrup��o desse slot
		vect_addr = (u32 *)(VIC_BASE_ADDR + VIC_VECT_ADDR_INDEX + i*4);	// Captura o endere�o do registrador que guarda o endere�o da fun��o de tratamento de interrup��o

		// Ignora se o endere�o da ISR for a mesma que executada acima
		//if (  (*vect_addr == 0) || (*vect_addr == VICVectAddr) ) continue;
		//if (  (*vect_addr == 0) || (*vect_addr == VICVectAddr) || ((stIRQ&(1<<i)) == 0) ) continue;
		isr = (void(*)(void)) *vect_addr;
		isr(); 	// Chama a fun��o ISR pendentes
	}

	VICVectAddr = 0;             	// Limpa o registrador de endere�o do ISR, para novas interrup��es
	irq_Restore(_cpsr);
}
#endif

// Fun��o que trata interrup��o FIQ
void vectorFiqHandler(void) {
	#if VICERR_USE_DEBUG == pdON
	plognp("ERRO FATAL: Interrup��o FIQ n�o prevista"CMD_TERMINATOR);
	#endif
	while(1);
}
