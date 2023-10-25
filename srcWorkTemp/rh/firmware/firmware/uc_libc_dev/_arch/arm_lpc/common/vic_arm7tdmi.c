#include "vic.h"
#if (VIC_USE_DEBUG == pdON) || (VICERR_USE_DEBUG == pdON)
#include"stdio_uc.h"
#endif



// OS SISTEMAS OPERACIONAIS TEM SEUS PRÓPRIOS CONTROLE DE INTERRUPÇÕES
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
// Descrição: 	Inicializa os canais IRQ vetorado
// Parametros:	nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void vic_Init(void) {
    int i = 0;
    u32 *vect_addr, *vect_prio;

   	#if defined(RTOS_NONE)
   	ints_Disable();
   	#endif

    VICIntEnClr = 0xFFFFFFFF; 															// Desabilita as interrupções de todos os dispositivos. Cada bit do reg corresponde a um dispositivo do LPC
    VICVectAddr = 0;																	// Limpa o registrador que guarda o endereço da interrupção atual ativa quando ocorrer uma interrupção IRQ
    VICIntSelect = 0;																	// Todos os dispositivos do LPC são atendidade pelo IRQ e não pela FIQ

    for ( i = 0; i < VIC_SIZE; i++ ) {													// Faça para todos os SLOTS de int
		vect_addr = (u32 *)(VIC_BASE_ADDR + VIC_VECT_ADDR_INDEX + i*4);					// Captura o endereço do registrador que guarda o endereço da função de tratamento de interrupção
		vect_prio = (u32 *)(VIC_BASE_ADDR + VIC_VECT_PRIO_INDEX + i*4);					// Captura o endereço do registrador de controle de interrupção
		*vect_addr = 0;																	// Limpa qualquer endereço de função de tratamento de interrupção ao registrador que aponta a SLOT(i)

		#if defined(LPC2468)
			*vect_prio = 0xF;															// Limpa controle do vetor
		#else
			*vect_prio = 0;																// Limpa controle do vetor
		#endif
    } 
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Instala um dispositivo (canal) a uma SLOT de interrupção. 
// Parametros:	Numero do canal (dispositivo o endereço da função e a prioridade
// Retorna:		Retorna pdPASS se foi instalado com sucesso senão retorna código do erro
// ----------------------------------------------------------------------------------------------------------------
int irq_Install(u32 int_number, void *handler_addr, u32 priority) {
    u32 *vect_addr;
    u32 *vect_prio;
      
    VICIntEnClr = 1 << int_number;														// Desabilita a interrupção do dispositivo selecionado. Cada bit do reg corresponde a um dispositivo do LPC
    
	if ( int_number >= VIC_SIZE )														// Checa se o dipositivo está fora, não pertence, dos slots de interrupção
    	return ( errIRQ_SLOT_OUT_OF_RANGE );																
    else   {
		#if defined(LPC2468)
			vect_addr = (u32 *)(VIC_BASE_ADDR + VIC_VECT_ADDR_INDEX + int_number*4);	// Captura o endereço do registrador que guarda o endereço da função de tratamento de interrupção
			vect_prio = (u32 *)(VIC_BASE_ADDR + VIC_VECT_PRIO_INDEX + int_number*4);	// Captura o endereço do registrador de controle de interrupção
			*vect_addr = (u32)handler_addr;												// Anexa o endereço da função de tratamento da interrupção ao registrador que aponta o SLOTx
			*vect_prio = priority;														// Determina que o prioridade o SLOTx tem
			VICIntEnable = 1 << int_number;												// Habilita interrupção do dispositivo selecionado. Cada bit do reg corresponde a um dispositivo do LPC
			return  pdPASS;
		#else
			vect_addr = (u32 *)(VIC_BASE_ADDR + VIC_VECT_ADDR_INDEX + priority*4);		// Captura o endereço do registrador que guarda o endereço da função de tratamento de interrupção
			vect_prio = (u32 *)(VIC_BASE_ADDR + VIC_VECT_PRIO_INDEX + priority*4);		// Captura o endereço do registrador de controle de interrupção
			
			if ( *vect_addr == (u32)((void *)0) ) {										// Checa se o SLOT da interrupção esta desocupado
		    	*vect_addr = (u32)handler_addr;											// Anexa o endereço da função de tratamento da interrupção ao registrador que aponta o SLOTx
	    		*vect_prio = (u32)(VIC_ENABLE | int_number);							// Habilita int do SLOT(i)
	    		VICIntEnable = 1 << int_number;											// Habilita interrupção do dispositivo selecionado. Cada bit do reg corresponde a um dispositivo do LPC
	    		return  pdPASS;
			} else																		// Retona falso, porque o SLOT já está sendo ocpupador por outro ISR
				return  errIRQ_SLOT_FULL;
		#endif	
    }
}


// OS SISTEMAS OPERACIONAIS TEM SEUS PRÓPRIOS CONTROLE DE INTERRUPÇÕES
#if defined(RTOS_NONE)
// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Retorna pdTRUE se a interrupção irq estiver habilitada, senão retorna pdFALSE
// Parametros: 	Nenhum
// Retorna:		pdTRUE ou pdFALSE
// -------------------------------------------------------------------------------------------------------------------
inline bool irq_IsEnabled(void) {
	u32 _cpsr;
	 _cpsr = __get_cpsr();
	 return (_cpsr & IRQ_MASK)? pdFALSE:pdTRUE;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Desabilita a interrupção IRQ ( bit 7 do registrador CPSR)
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
// Descrição:	Habilita a interrupção IRQ ( bit 7 do registrador CPSR)
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
// Descrição:	Restaura a condição do flag de interrupção contido no parametro
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
// Descrição:	Desabilita a interrupção FIQ ( bit 6 do registrador CPSR)
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
// Descrição:	Habilita a interrupção FIQ ( bit 6 do registrador CPSR)
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
// Descrição:	Restaura a condição do flag de interrupção contido no parametro
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
// FUNÇÕES CHAMADAS PELO vector.s quando algum interrupção é gerada

#if defined(RTOS_NONE)
// OS SISTEMAS OPERACIONAIS TEM SEUS PRÓPRIOS CONTROLE DE INTERRUPÇÕES por software
// Função que trata interrupção por software
void vectorSwiHandler(void) {
	#if VICERR_USE_DEBUG == pdON
	plognp("ERRO FATAL: Interrupção SWI não prevista "CMD_TERMINATOR);
	#endif
	while(1);
}
#endif

// Função que trata interrupção de instruções indefinidas
void vectorUndefHandler (void) {
	#if VICERR_USE_DEBUG == pdON
	plognp("ERRO FATAL: Interrupção UNDEF não prevista "CMD_TERMINATOR);
	#endif
	while(1);
}

// Função que trata interrupção de prefetch abort
void vectorPrefetchAbortHandler(void) {
	#if VICERR_USE_DEBUG == pdON
	plognp("ERRO FATAL: Interrupção Prefetch ABORT não prevista "CMD_TERMINATOR);
	#endif
	while(1);
}

// Função que trata interrupção de data abort
void vectorDataAbortHandler(void) {
	#if VICERR_USE_DEBUG == pdON
	plognp("ERRO FATAL: Interrupção DATAABORT não prevista "CMD_TERMINATOR);
	#endif
	while(1);
}


/*
 * Estou usando um arm7tdmi da NXP modelo lpc
Estou com problema em atender duas interrupções que foram geradas ao mesmo tempo em uma VIC Vertored.
Uma das ints não é atendida pelo hardware e não quero usar a interrupção NESTED (preemptiva)
Como vcs gerenciam essas interrupções?

esse é o techo de código para tratameto que uso
void vectorIrqHandler(void) {
	u32 _cpsr;
	_cpsr= irq_Disable(); // desabilitar ints. Tentativas de evitar espurias de ints. Pois o VIC do ARM7 tem bug
	static void (*isr)(void);
	isr = (void(*)(void)) VICVectAddr;

	if (isr != NULL)
		isr(); 		// Chama a função ISR

	VICVectAddr = 0;             	// Limpa o registrador de endereço do ISR, para novas interrupções

	irq_Restore(_cpsr);
}
Pergunta 2
Essa tal de espuria de interrupção do processador ARM7?
Como vc a tratam?
Li sobre o assunto e fiquei mais na dúvida ainda
*/

#if defined(SIM_PROTEUS)
// Função que trata interrupções IRQ
// TODO - Somemte este método funciona no proteus, e com isso as interrupções de mesma prioridade, ou prioridade inferior,
//	emitidas ao mesmo tempo, somente a inetrrupção de maior prioridade será atendidade e as demais serão descartadas
void vectorIrqHandler(void) {
	u32 _cpsr = irq_Disable(); 		// desabilitar ints. Tentativas de evitar espurias de ints. Pois o VIC do ARM7 tem bug
	static void (*isr)(void);
	isr = (void(*)(void)) VICVectAddr;
	isr(); 							// Chama a função ISR
	VICVectAddr = 0;             	// Limpa o registrador de endereço do ISR, para novas interrupções
	irq_Restore(_cpsr);
	//plog("0x%x 0x%x 0x%x"CMD_TERMINATOR, VICVectAddr, VICRawIntr, VICIRQStatus);
}
#else
// Função que trata interrupções IRQ
// Este procedimeto trava com o proteus
void vectorIrqHandler(void) {
	u32 _cpsr = irq_Disable(); // desabilitar ints. Tentativas de ebitar espurias de ints. Pois o VIC do ARM7 tem bug. TODO - método ISR IRQ precário e em teste
	u32 stIRQ = VICIRQStatus; // capturar status, porque esse reg é limpo na chamada da função isr()

	static void (*isr)(void);
	// isr = (void(*)(void)) VICVectAddr; // Captura o endereço da função ISR
	//isr(); 	// Chama a função ISR de maior prioridade
	//VICVectAddr = 0;             	// Limpa o registrador de endereço do ISR, para novas interrupções
	//irq_Restore(_cpsr);

	// TESTADO SOMENTE NO LPC2468
	// É uma tentativa de processar mais de uma interrupção emitida ao mesmo tempo
	// Este procedimento trata todas as ISRs com a mesma prioridade
	// TODO - método ISR IRQ precário e está em testes. Fazer mais teste com muitas ints e deviersas fontes
	// É um método bem lento de se tratar as interrupções e pode haver overrun em ISR muito longas em interrupções simultaneas
	//	Fora testado somente com ints EINT1, EINT2, UART, timer0 MACTH0 e timer1 MACTH0
	//		 EINT1, EINT2 gerado a cada 1uS ao mesmo tempo para as duas entradas,
	//			ou uma entrada era gerada de 1 a 10 nops após a outra

	int i;
	u32 *vect_addr;
	for ( i = 0; i < VIC_SIZE; i++ ) {													// Faça para todos os SLOTS de int
		if ((stIRQ&(1<<i)) == 0) continue; // Ignora se não foi emitido pedido de interrrupção desse slot
		vect_addr = (u32 *)(VIC_BASE_ADDR + VIC_VECT_ADDR_INDEX + i*4);	// Captura o endereço do registrador que guarda o endereço da função de tratamento de interrupção

		// Ignora se o endereço da ISR for a mesma que executada acima
		//if (  (*vect_addr == 0) || (*vect_addr == VICVectAddr) ) continue;
		//if (  (*vect_addr == 0) || (*vect_addr == VICVectAddr) || ((stIRQ&(1<<i)) == 0) ) continue;
		isr = (void(*)(void)) *vect_addr;
		isr(); 	// Chama a função ISR pendentes
	}

	VICVectAddr = 0;             	// Limpa o registrador de endereço do ISR, para novas interrupções
	irq_Restore(_cpsr);
}
#endif

// Função que trata interrupção FIQ
void vectorFiqHandler(void) {
	#if VICERR_USE_DEBUG == pdON
	plognp("ERRO FATAL: Interrupção FIQ não prevista"CMD_TERMINATOR);
	#endif
	while(1);
}
