#include "timer0.h"
#if defined(cortexm3)
#include "arm.h"
#endif

void timer0_ISR(void);

typedef void (*void_pFunc)(void);
volatile void_pFunc timer0_IntFunctions[TIMER0_NUM_INTERRUPTS];

volatile u32 Timer0CountOverflow;  // Conta a quantidade de vezes que o overflow é alcançado 0xFFFFFFFF para 0 (MATCH0 = 0)

// ################################################################################################################
// TIMER 0
// ################################################################################################################
// ################################################################################################################

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa Timer 0
// Parametros:	Prescaler: valores de 1 a 32bits
//					Alguns valores padrões T0MPR_DIVx [x = 1,2,4,8,16,32,64,128,256,512,1024]
// 				sts: T0TCR_ENABLE  	// Habilita o contador do timer
//					 T0TCR_STOP		// Desabilita o contador do timer
//				Habilitar ints para TC=reg MCRx: T0MCR_MR_NO_INT / T0_CONT_OVERFLOW / ( T0MCR_MR[0..3]_INT_ENABLE | T0MCR_MR[0..3]_RESET | T0MCR_MR[0..3]_STOP
//					T0MCR_NO_INT				- Sem interrupções para match ou PWM
//					T0MCR_OVERFLOW				- Indica para habilitar interrupção MR0 para pelo menos fazer a contagem do overflow
//					T0MCR_MR[0..3]_INT_ENABLE 	- Indica para habilitar interrupção de contador do timer igualar ao registrador MCR
// 					T0MCR_MR[0..3]_RESET 		- Indica quais das comparações deve resetar o contador TC
//					T0MCR_MR[0..3]_STOP 		- Indica quais das comparações deve parar o contador TC
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna código de erro
// ----------------------------------------------------------------------------------------------------------------
int timer0_Init(u32 prescale, u8 sts, u16 int_macths) {
	u8 int_num;
	
	PCONP |= PCTIM0;													// Ligar o timer no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões
	
	T0IR  = 0xFF;														// Limpa todas as interrupções
	T0TCR = T0TCR_RESET;												// Reset o contador e mantem em reset
	timer0_SetPrescale(prescale);										// Ajusta o precaler do contador TC
	T0MCR = int_macths;													// Liga as interrupções
	T0MR0 = 0;															// zera o match0 para usarmos como contador de overflow
	
	for(int_num = 0; int_num < TIMER0_NUM_INTERRUPTS; int_num++)		// Faça para todas as interrupções do timer
		timer0_ClearIntHandler(int_num);								// Retira todas as funções anexadas em suas respecticas interrupções
	
	#if defined(arm7tdmi)
	if (!irq_Install(VIC_TIMER0, timer0_ISR, TIMER0_PRIO_LEVEL))		// Instala o ISR a VIC
		return errIRQ_INSTALL;											// Retonar falso se não conseguiu instalar a ISR a VIC
	#endif
	
	#if defined (cortexm3)
	nvic_enableIRQ(VIC_TIMER0);
	nvic_setPriority(VIC_TIMER0, TIMER0_PRIO_LEVEL);
	#endif
	
	timer0_Set(sts);													// Ajusta a condição do timer. Ligado ou desligado

	return pdPASS;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Liga, desliga ou re-inicia o timer
// Parametros:	sts: 
//					 T0TCR_ENABLE  	// Habilita o contador do timer
//					 T0TCR_STOP		// Desabilita o contador do timer
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_Set(u8 sts) {
	T0TCR &= ~(T0TCR_ENABLE | T0TCR_RESET);
	T0TCR |= sts;
}

// -----------------------------------------------------------------------
// Descrição: 	Anexa uma routina ao vetor de interrupções
// Parametros:	int_num. Indece do vetor de uma determinada interrupção de timer
// 					TIMER0_VECTOR_INT_MATCH0
// 					TIMER0_VECTOR_INT_MATCH1
// 					TIMER0_VECTOR_INT_MATCH2
// 					TIMER0_VECTOR_INT_MATCH3
// 					TIMER0_VECTOR_INT_CAPTURE0
// 					TIMER0_VECTOR_INT_CAPTURE1
// 					TIMER0_VECTOR_INT_CAPTURE2
// 					TIMER0_VECTOR_INT_CAPTURE3
//				int_func
//					Ponteiro da rotina que tratará a interrupção do timer. 
//						Ex	timer0_SetIntHandler(TIMER0_VECTOR_INT_OVERFLOW, myOverflowFunction);
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_SetIntHandler(u8 int_num, void (*int_func)(void) ) {
	if(int_num < TIMER0_NUM_INTERRUPTS)
		timer0_IntFunctions[int_num] = int_func;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retira a routina da inetrrupçao desejada do vetor de interrupções
// Parametros:	int_num. Indece do vetor de uma determinada interrupção de timer
// 					TIMER0_VECTOR_INT_MATCH0
// 					TIMER0_VECTOR_INT_MATCH1
// 					TIMER0_VECTOR_INT_MATCH2
// 					TIMER0_VECTOR_INT_MATCH3
// 					TIMER0_VECTOR_INT_CAPTURE0
// 					TIMER0_VECTOR_INT_CAPTURE1
// 					TIMER0_VECTOR_INT_CAPTURE2
// 					TIMER0_VECTOR_INT_CAPTURE3
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_ClearIntHandler(u8 int_num) {
	if(int_num < TIMER0_NUM_INTERRUPTS)
		timer0_IntFunctions[int_num] = 0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta o prescale do timer
// Parametros:	Valor do prescale de 1 a 2^^32bits
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_SetPrescale(u32 prescale) {
	T0PR = prescale - 1;
}

// ################################################################################################################
// TIMER 0 OVERFLOW
// ################################################################################################################
// ################################################################################################################

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa os contador do timer 0
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
inline void timer0_ClearCount(void) {
	T0TCR |= T0TCR_RESET;													// Reseta contador
	T0TCR &= ~T0TCR_RESET;												// Tira do Reset
	Timer0CountOverflow = 0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o valor do contador do timer 0
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer 1
// ----------------------------------------------------------------------------------------------------------------
inline u32 timer0_GetCount(void) {
	return T0TC;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa o contador de overflow
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_ClearOverflowCount(void) {
	Timer0CountOverflow = 0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o valor do contador de overflow do timer 0
// Parametros:	Nenhum
// Retorna:		Valor do contador de overflow do timer 0
// ----------------------------------------------------------------------------------------------------------------
u32 timer0_GetOverflowCount(void) {
	return Timer0CountOverflow;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o valor do contador do timer 0 junto com os overflows 
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer 0
// ----------------------------------------------------------------------------------------------------------------
inline u64 timer0_GetCountAll(void)  {
	return (u64)T0TC + ((u64)Timer0CountOverflow * 0xFFFFFFFF);
}


// ################################################################################################################
// TIMER 0 MATCH
// ################################################################################################################
// Regitradores MRx:
//	São registradores para fazer comparações com o TC e tomar alguma ação de acordo com TxMCR

// Regitradores EMRs:
// São controladores dos pinos externos mediantes a comparações MRx
//	Os bits EMx. External Match. Quando o TC=MRx, o valor do bit pode ser mudado de acordo do seu respectivo bit de configuração EMCx
//		EMCx	ação
//		00		Não faz nada (padrão)
//		01		O bit assume valor 0
//		10		O bit assume valor 1
//		11		Troca o valor do bit
//	Se os pinos de saida do ARM estiverem configurador para MATx, os valores dos bits EMx serão refletidos nos pinos de saídas

#if (TIMER0_USE_MATCH0 == pdON)

// ################################################################################################################
// não necessáriamente precisa da interrupção para fazer a comparação, por exemplo
//		podemos usar a troca do pino externo sem o uso de interrupções

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa o pino para match 0
// Parametros:	action: Define o tipo de ação no bit EM0 quando TC0=MR0 (reg de comparação 0)
//					T0EMR_EMC0_DONOTHING:  	Faz nada quando chegar na comparação de MR0
//					T0EMR_EMC0_CLRPIN:		Colocar o bit em nível baixo
//					T0EMR_EMC0_SETPIN:		Coloca o bit em nível alto
//					T0EMR_EMC0_TOGGLEPIN:	Inverte o bit
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_SetPinOutputMatch0(int action) {
	#if defined(arm7tdmi)
	T0_MATCH0_PINSEL = (T0_MATCH0_PINSEL & ~T0_MATCH0_PINMASK) | T0_MATCH0_PIN; // Seleciona o pino externo para a função de comparação
	#else
	MATCH0_ConfigPort0();
	#endif

	T0EMR = (T0EMR & ~T0EMR_EMC0_MASK) | action; // ajusta o comportamento do pino e ativa o pino externo para essa função
}

void timer0_SetMatch0(u32 value) {
	T0MR0 = value;
}
#endif

#if (TIMER0_USE_MATCH1 == pdON)
// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa o pino para match 1
// Parametros:	action: Define o tipo de ação no bit EM1 quando TC0=MR1 (reg de comparação 1)
//					T0EMR_EMC1_DONOTHING:  	Faz nada quando chegar na comparação de MR0
//					T0EMR_EMC1_CLRPIN:		Colocar o bit em nível baixo
//					T0EMR_EMC1_SETPIN:		Coloca o bit em nível alto
//					T0EMR_EMC1_TOGGLEPIN:	Inverte o bit
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_SetPinOutputMatch1(int action) {
	#if defined(arm7tdmi)
	T0_MATCH1_PINSEL = (T0_MATCH1_PINSEL & ~T0_MATCH1_PINMASK) | T0_MATCH1_PIN; // Seleciona o pino externo para a função de comparação 1
	#else
	MATCH0_ConfigPort1();
	#endif

	T0EMR = (T0EMR & ~T0EMR_EMC1_MASK) | action; // ajusta o comportamento do pino e ativa o pino externo para essa função
}

void timer0_SetMatch1(u32 value) {
	T0MR1 = value;
}
#endif

#if (TIMER0_USE_MATCH2 == pdON)
void timer0_SetMatch2(u32 value) {
	T0MR2 = value;
}

#endif

#if (TIMER0_USE_MATCH3 == pdON)
void timer0_SetMatch3(u32 value) {
	T0MR3 = value;
}
#endif

// ################################################################################################################
// TIMER 0 COMPTURE
// ################################################################################################################
// ################################################################################################################
#if (TIMER0_USE_CAPTURE0 == pdON)

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa comparação 0 do timer 0 via pino CAPn.0
// Parametros:	Definir o tipo de borda do sinal de captura e ligar ou não a interrupçao
//					(T0CCR_CR0_RISING/T0CCR_CR0_FALLING) | T0CCR_CR0_INT
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_InitCapture0(u16 sts) {
	#if defined(arm7tdmi)
	T0_CAP0_PINSEL = (T0_CAP0_PINSEL & ~T0_CAP0_PINMASK) | T0_CAP0_PIN; // Seleciona o pino externo para a função de captura 0
	#else
	t0cap0_ConfigPort();
	#endif

	T0CCR |= sts; 														 // Ajusta a configuração de captura
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com o valor capturado do contador timer 0, TC, durante o evento CAP0
// Parametros:	Nenhum
// Retorna:		Valor capturado do contador do timer0 TC armazenado no registrador CR0
// ----------------------------------------------------------------------------------------------------------------
inline u32 timer0_GetCapture0(void) {
	return T0CR0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Desliga a comparação 0 do timer 0
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_StopCapture0(void) {
	#if defined(arm7tdmi)
	T0_CAP0_PINSEL &=~T0_CAP0_PINMASK;									// Seleciona o pino CAP para GPIO
	#endif
	T0CCR &= ~(T0CCR_CR0_INT | T0CCR_CR0_RISING | T0CCR_CR0_FALLING);	// Desliga as interrrupção e tipo de leitura
}

#endif

#if (TIMER0_USE_CAPTURE1 == pdON)
// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa comparação 1 do timer 0 via pino CAPn.1
// Parametros:	Definir o tipo de borda do sinal de captura e ligar ou não a interrupçao
//					(T0CCR_CR1_RISING/T0CCR_CR1_FALLING) | T0CCR_CR1_INT
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_InitCapture1(u16 sts) {
	#if defined(arm7tdmi)
	T0_CAP1_PINSEL = (T0_CAP1_PINSEL & ~T0_CAP1_PINMASK) | T0_CAP1_PIN; // Seleciona o pino externo para a função de captura 1
	#else
	t0cap1_ConfigPort();
	#endif

	T0CCR |= sts; 														// Ajusta a configuração de captura
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com o valor capturado do contador timer 0, TC, durante o evento CAP1
// Parametros:	Nenhum
// Retorna:		Valor capturado do contador do timer0 TC armazenado no registrador CR1
// ----------------------------------------------------------------------------------------------------------------
inline u32 timer0_GetCapture1(void) {
	return T0CR1;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Desliga a comparação 0 do timer 0
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer0_StopCapture1(void) {
	#if defined(arm7tdmi)
	T0_CAP1_PINSEL &=~T0_CAP1_PINMASK;									// Seleciona o pino CAP para GPIO
	#endif
	T0CCR &= ~(T0CCR_CR1_INT | T0CCR_CR1_RISING | T0CCR_CR1_FALLING);	// Desliga as interrrupção e tipo de leitura
}
#endif


// ################################################################################################################
// TIMER 0 SERVIÇO DE INTERRUPÇÃO
// ################################################################################################################
// ################################################################################################################
void timer0_ISR(void) {
    ISR_ENTRY;															// Procedimento para entrada da interrupção

    // É utilizado o mach0 para contagem de overflow
	if(T0IR & T0IR_MR0I) {												// Checa se a INT gerada é do comparador 0
		T0IR |= T0IR_MR0I;												// Limpa o flag da INT 
		
		Timer0CountOverflow++;
		
		if(timer0_IntFunctions[TIMER0_VECTOR_INT_MATCH0])				// Checa se existe uma função anexada
			timer0_IntFunctions[TIMER0_VECTOR_INT_MATCH0]();				// Execute a função em anexo
	}
	#if (TIMER0_USE_MATCH1 == pdON)
	else if(T0IR & T0IR_MR1I) {											// Checa se a INT gerada é do comparador 1
		T0IR |= T0IR_MR1I;												// Limpa o flag da INT 

		if(timer0_IntFunctions[TIMER0_VECTOR_INT_MATCH1])				// Checa se existe uma função anexada
			timer0_IntFunctions[TIMER0_VECTOR_INT_MATCH1]();			// Execute a função em anexo
	}
	#endif

	#if (TIMER0_USE_MATCH2 == pdON)
	else if(T0IR & T0IR_MR2I) {											// Checa se a INT gerada é do comparador 0
		T0IR |= T0IR_MR2I;												// Limpa o flag da INT 

		if(timer0_IntFunctions[TIMER0_VECTOR_INT_MATCH2])				// Checa se existe uma função anexada
			timer0_IntFunctions[TIMER0_VECTOR_INT_MATCH2]();				// Execute a função em anexo
	}
	#endif
	
	#if (TIMER0_USE_MATCH3 == pdON)
	else if(T0IR & T0IR_MR3I) {											// Checa se a INT gerada é do comparador 0
		T0IR |= T0IR_MR3I;												// Limpa o flag da INT 

		if(timer0_IntFunctions[TIMER0_VECTOR_INT_MATCH3])				// Checa se existe uma função anexada
			timer0_IntFunctions[TIMER0_VECTOR_INT_MATCH3]();				// Execute a função em anexo
	}
	#endif

	#if (TIMER0_USE_CAPTURE0 == pdON)
	else if(T0IR & T0IR_CR0I) {											// Checa se a INT gerada é do captura 0
		T0IR |= T0IR_CR0I;												// Limpa o flag da INT 

		if(timer0_IntFunctions[TIMER0_VECTOR_INT_CAPTURE0])				// Checa se existe uma função anexada
			timer0_IntFunctions[TIMER0_VECTOR_INT_CAPTURE0]();			// Execute a função em anexo
	}
	#endif

	#if (TIMER0_USE_CAPTURE1 == pdON)
	else if(T0IR & T0IR_CR1I) {											// Checa se a INT gerada é do captura 1
		T0IR |= T0IR_CR1I;												// Limpa o flag da INT 

		if(timer0_IntFunctions[TIMER0_VECTOR_INT_CAPTURE1])				// Checa se existe uma função anexada
			timer0_IntFunctions[TIMER0_VECTOR_INT_CAPTURE1]();			// Execute a função em anexo
	}
	#endif
	
	#if (TIMER0_USE_CAPTURE2 == pdON)
	else if(T0IR & T0IR_CR2I) {											// Checa se a INT gerada é do captura 2
		T0IR |= T0IR_CR2I;												// Limpa o flag da INT 

		if(timer0_IntFunctions[TIMER0_VECTOR_INT_CAPTURE2])				// Checa se existe uma função anexada
			timer0_IntFunctions[TIMER0_VECTOR_INT_CAPTURE2]();			// Execute a função em anexo
	}
	#endif
	
	#if (TIMER0_USE_CAPTURE3 == pdON)
	else if(T0IR & T0IR_CR3I) {											// Checa se a INT gerada é do captura 3
		T0IR |= T0IR_CR3I;												// Limpa o flag da INT 

		if(timer0_IntFunctions[TIMER0_VECTOR_INT_CAPTURE3])				// Checa se existe uma função anexada
			timer0_IntFunctions[TIMER0_VECTOR_INT_CAPTURE3]();			// Execute a função em anexo
	}
	#endif

	// Limpa interrupção 
	#if defined(arm7tdmi)
	VICSoftIntClr = (1<<VIC_TIMER0);
	#endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_TIMER0);
	#endif

    ISR_EXIT;															// Procedimento para saida da interrupção    
}
