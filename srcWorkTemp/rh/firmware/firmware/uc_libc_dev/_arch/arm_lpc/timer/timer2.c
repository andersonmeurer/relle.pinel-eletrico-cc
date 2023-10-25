// TODO ainda n�o testado

#include "timer2.h"
#if defined(cortexm3)
#include "arm.h"
#endif

void timer2_ISR(void);

typedef void (*void_pFunc)(void);
volatile void_pFunc timer2_IntFunctions[TIMER2_NUM_INTERRUPTS];

volatile u32 Timer2CountOverflow;  // Conta a quantidade de vezes que o MATCH0 � alcan�ado

// ################################################################################################################
// TIMER 2
// ################################################################################################################
// ################################################################################################################

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa Timer 1
// Parametros:	Prescaler: valores de 1 a 32bits
//					Alguns valores padr�es T2MPR_DIVx [x = 1,2,4,8,16,32,64,128,256,512,1024]
// 				sts: T2TCR_ENABLE  	// Habilita o contador do timer
//					 T2TCR_STOP		// Desabilita o contador do timer
//				Habilitar ints para TC=reg MCRx: T2MCR_MR_NO_INT / T2_CONT_OVERFLOW ( T2MCR_MR[0..3]_INT_ENABLE | T2MCR_MR[0..3]_RESET | T2MCR_MR[0..3]_STOP
//					T2MCR_NO_INT				- Sem interrup��es para match ou PWM
//					T2MCR_OVERFLOW				- Indica para habilitar interrup��o MR0 para pelo menos fazer a contagem do overflow
//					T2MCR_MR[0..3]_INT_ENABLE 	- Indica para habilitar interrup��o de contador do timer igualar ao registrador MCR
// 					T2MCR_MR[0..3]_RESET 		- Indica quais das compara��es deve resetar o contador TC
//					T2MCR_MR[0..3]_STOP 		- Indica quais das compara��es deve parar o contador TC
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna c�digo de erro
// ----------------------------------------------------------------------------------------------------------------
int timer2_Init(u32 prescale, u8 sts, u16 int_macths) {
	u8 int_num;
	
	PCONP |= PCTIM2;														// Ligar o timer no controle de potencia. Tem que ser antes da configura��o, pois ap�s ligar o perif�ricos, seus regs assumem valores padr�es
	
	T2IR  = 0xFF;															// Limpa todas as interrup��es
	T2TCR = T2TCR_RESET;													// Reset o contador e mantem em reset
	timer2_SetPrescale(prescale);											// Ajusta o precaler do contador TC
	T2MCR = int_macths;														// Liga as interrup��es quando TC = MCRx
	T2MR0 = 0;																// zera o match0 para usarmos como contador de overflow
	
	for(int_num = 0; int_num < TIMER2_NUM_INTERRUPTS; int_num++)			// Fa�a para todas as interrup��es do timer
		timer2_ClearIntHandler(int_num);									// Retira todas as fun��es anexadas em suas respecticas interrup��es
	
	#if defined(arm7tdmi)
	if (!irq_Install(VIC_TIMER2, timer2_ISR, TIMER2_PRIO_LEVEL))			// Instala o ISR a VIC
		return errIRQ_INSTALL;												// Retonar falso se n�o conseguiu inttalar a ISR a VIC
	#endif
	
	#if defined (cortexm3)
	nvic_enableIRQ(VIC_TIMER2);
	nvic_setPriority(VIC_TIMER2, TIMER2_PRIO_LEVEL);
	#endif
		
	timer2_Set(sts);														// Ajusta a condi��o do timer. Ligado ou desligado

	return pdPASS;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Liga, desliga e re-inicia o timer
// Parametros:	STS: T2TCR_ENABLE  	// Habilita o contador do timer
//					 T2TCR_STOP		// Desabilita o contador do timer
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_Set(u8 sts) {
	T2TCR &= ~(T2TCR_ENABLE | T2TCR_RESET);
	T2TCR|=sts;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Anexa uma routina ao vetor de interrup�ao
// Parametros:	int_num. Indece do vetor de uma determinada interrup��o de timer
// 					TIMER2_VECTOR_INT_MATCH0
// 					TIMER2_VECTOR_INT_MATCH1
// 					TIMER2_VECTOR_INT_MATCH2
// 					TIMER2_VECTOR_INT_MATCH3
// 					TIMER2_VECTOR_INT_CAPTURE0
// 					TIMER2_VECTOR_INT_CAPTURE1
// 					TIMER2_VECTOR_INT_CAPTURE2
// 					TIMER2_VECTOR_INT_CAPTURE3
//				int_func
//					Ponteiro da rotina que tratar� a interrup��o do timer. 
//						Ex	timer2_SetIntHandler(TIMER2_VECTOR_INT_OVERFLOW, myOverflowFunction);
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_SetIntHandler(u8 int_num, void (*int_func)(void) ) {
	if(int_num < TIMER2_NUM_INTERRUPTS)
		timer2_IntFunctions[int_num] = int_func;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retira a routina da inetrrup�ao desejada do vetor de interrup�ao
// Parametros:	int_num. Indece do vetor de uma determinada interrup��o de timer
// 					TIMER2_VECTOR_INT_MATCH0
// 					TIMER2_VECTOR_INT_MATCH1
// 					TIMER2_VECTOR_INT_MATCH2
// 					TIMER2_VECTOR_INT_MATCH3
// 					TIMER2_VECTOR_INT_CAPTURE0
// 					TIMER2_VECTOR_INT_CAPTURE1
// 					TIMER2_VECTOR_INT_CAPTURE2
// 					TIMER2_VECTOR_INT_CAPTURE3
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_ClearIntHandler(u8 int_num) {
	if(int_num < TIMER2_NUM_INTERRUPTS)
		timer2_IntFunctions[int_num] = 0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta o prescale do timer
// Parametros:	Valor do prescale de 1 a 2^^32bits
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_SetPrescale(u32 prescale) {
	T2PR = prescale - 1;
}

// ################################################################################################################
// TIMER 2 OVERFLOW
// ################################################################################################################
// ################################################################################################################

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa os contador do timer 2
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
inline void timer2_ClearCount(void) {
	T2TCR |= T2TCR_RESET;													// Reseta contador
	T2TCR &= ~T2TCR_RESET;												// Tira do Reset
	Timer2CountOverflow = 0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador do timer 2
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer 2
// ----------------------------------------------------------------------------------------------------------------
inline u32 timer2_GetCount(void) {
	return T2TC;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa o contador de overflow
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_ClearOverflowCount(void) {
	Timer2CountOverflow = 0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador de overflow do timer 2
// Parametros:	Nenhum
// Retorna:		Valor do contador de overflow do timer 2
// ----------------------------------------------------------------------------------------------------------------
u32 timer2_GetOverflowCount(void) {
	return Timer2CountOverflow;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador do timer 2 junto com os overflows 
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer 2
// ----------------------------------------------------------------------------------------------------------------
inline u64 timer2_GetCountAll(void) {
	return (u64)T2TC + ((u64)Timer2CountOverflow * 0xFFFFFFFF);
}


// ################################################################################################################
// TIMER 2 MATCH
// ################################################################################################################
// Regitradores MRx:
//	S�o registradores para fazer compara��es com o TC e tomar alguma a��o de acordo com TxMCR

// Regitradores EMRs:
// S�o controladores dos pinos externos mediantes a compara��es MRx
//	Os bits EMx. External Match. Quando o TC=MRx, o valor do bit pode ser mudado de acordo do seu respectivo bit de configura��o EMCx
//		EMCx	a��o
//		00		N�o faz nada (padr�o)
//		01		O bit assume valor 0
//		10		O bit assume valor 1
//		11		Troca o valor do bit
//	Se os pinos de saida do ARM estiverem configurador para MATx, os valores dos bits EMx ser�o refletidos nos pinos de sa�das

#if (TIMER2_USE_MATCH0 == pdON)
// ################################################################################################################
// n�o necess�riamente precisa da interrup��o para fazer a compara��o, por exemplo
//		podemos usar a troca do pino externo sem o uso de interrup��es

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa o pino para match 0
// Parametros:	action: Define o tipo de a��o no bit EM0 quando TC2=MR0 (reg de compara��o 0)
//					T2EMR_EMC0_DONOTHING:  	Faz nada quando chegar na compara��o de MR0
//					T2EMR_EMC0_CLRPIN:		Colocar o bit em n�vel baixo
//					T2EMR_EMC0_SETPIN:		Coloca o bit em n�vel alto
//					T2EMR_EMC0_TOGGLEPIN:	Inverte o bit
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_SetPinOutputMatch0(int action) {
	#if defined(arm7tdmi)
	T2_MATCH0_PINSEL = (T2_MATCH0_PINSEL & ~T2_MATCH0_PINMASK) | T2_MATCH0_PIN; // Seleciona o pino externo para a fun��o de compara��o
	#else
	MATCH2_ConfigPort0();
	#endif
	T2EMR = (T2EMR & ~T2EMR_EMC0_MASK) | action; // ajusta o comportamento do pino e ativa o pino externo para essa fun��o
}

void timer2_SetMatch0(u32 value) {
	T2MR0 = value;
}

#endif

#if (TIMER2_USE_MATCH1 == pdON)
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa o pino para match 1
// Parametros:	action: Define o tipo de a��o no bit EM1 quando TC2=MR1 (reg de compara��o 1)
//					T2EMR_EMC1_DONOTHING:  	Faz nada quando chegar na compara��o de MR0
//					T2EMR_EMC1_CLRPIN:		Colocar o bit em n�vel baixo
//					T2EMR_EMC1_SETPIN:		Coloca o bit em n�vel alto
//					T2EMR_EMC1_TOGGLEPIN:	Inverte o bit
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_SetPinOutputMatch1(int action) {
	#if defined(arm7tdmi)
	T2_MATCH1_PINSEL = (T2_MATCH1_PINSEL & ~T2_MATCH1_PINMASK) | T2_MATCH1_PIN; // Seleciona o pino externo para a fun��o de compara��o 1
	#else
	MATCH2_ConfigPort1();
	#endif
	T2EMR = (T2EMR & ~T2EMR_EMC1_MASK) | action; // ajusta o comportamento do pino e ativa o pino externo para essa fun��o
}

void timer2_SetMatch1(u32 value) {
	T2MR1 = value;
}
#endif

#if (TIMER2_USE_MATCH2 == pdON)
void timer2_SetMatch2(u32 value) {
	T2MR2 = value;
}
#endif

#if (TIMER2_USE_MATCH3 == pdON)
void timer2_SetMatch3(u32 value) {
	T2MR3 = value;
}
#endif

// ################################################################################################################
// TIMER 2 COMPTURE
// ################################################################################################################
// ################################################################################################################
#if (TIMER2_USE_CAPTURE0 == pdON)

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa compara��o 0 do timer 2 via pino CAPn.0
// Parametros:	Definir o tipo de borda do sinal de captura e ligar ou n�o a interrup�ao
//					(T2CCR_CR0_RISING/T2CCR_CR0_FALLING) | T2CCR_CR0_INT
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_InitCapture0(u16 sts) {
	#if defined(arm7tdmi)
	T2_CAP0_PINSEL = (T2_CAP0_PINSEL & ~T2_CAP0_PINMASK) | T2_CAP0_PIN; // Seleciona o pino externo para a fun��o de captura 0
	#else
	t2cap0_ConfigPort();
	#endif
	T2CCR |= sts; 														 // Ajusta a configura��o de captura
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna com o valor capturado do contador timer 0, TC, durante o evento CAP0
// Parametros:	Nenhum
// Retorna:		Valor capturado do contador do timer0 TC armazenado no registrador CR0
// ----------------------------------------------------------------------------------------------------------------
inline u32 timer2_GetCapture0(void) {
	return T2CR0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desliga a compara��o 0 do timer 2
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_StopCapture0(void) {
	#if defined(arm7tdmi)
	T2_CAP0_PINSEL &=~T2_CAP0_PINMASK;									// Seleciona o pino CAP para GPIO
	#endif
	T2CCR &= ~(T2CCR_CR0_INT | T2CCR_CR0_RISING | T2CCR_CR0_FALLING);	// Desliga as interrrup��o e tipo de leitura
}

#endif

#if (TIMER2_USE_CAPTURE1 == pdON)
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa compara��o 1 do timer 2 via pino CAPn.1
// Parametros:	Definir o tipo de borda do sinal de captura e ligar ou n�o a interrup�ao
//					(T2CCR_CR1_RISING/T2CCR_CR1_FALLING) | T2CCR_CR1_INT
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_InitCapture1(u16 sts) {
	#if defined(arm7tdmi)
	T2_CAP1_PINSEL = (T2_CAP1_PINSEL & ~T2_CAP1_PINMASK) | T2_CAP1_PIN; // Seleciona o pino externo para a fun��o de captura 1
	#else
	t2cap1_ConfigPort();
	#endif

	T2CCR |= sts; 														// Ajusta a configura��o de captura
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna com o valor capturado do contador timer 0, TC, durante o evento CAP0
// Parametros:	Nenhum
// Retorna:		Valor capturado do contador do timer0 TC armazenado no registrador CR0
// ----------------------------------------------------------------------------------------------------------------
inline u32 timer2_GetCapture1(void) {
	return T2CR1;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desliga a compara��o 0 do timer 2
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer2_StopCapture1(void) {
	#if defined(arm7tdmi)
	T2_CAP1_PINSEL &=~T2_CAP1_PINMASK;									// Seleciona o pino CAP para GPIO
	#endif
	T2CCR &= ~(T2CCR_CR1_INT | T2CCR_CR1_RISING | T2CCR_CR1_FALLING);	// Desliga as interrrup��o e tipo de leitura
}
#endif

// ################################################################################################################
// TIMER 2 SERVI�O DE INTERRUP��O
// ################################################################################################################
// ################################################################################################################
void timer2_ISR(void) {
    ISR_ENTRY;																	// Procedimento para entrada da interrup��o
	 
	// � utilizado o mach0 para contagem de overflow
	if(T2IR & T2IR_MR0I) {												// Checa se a INT gerada � do comparador 0
		T2IR |= T2IR_MR0I;												// Limpa o flag da INT 
		
		Timer2CountOverflow++;
		
		if(timer2_IntFunctions[TIMER2_VECTOR_INT_MATCH0])				// Checa se existe uma fun��o anexada
			timer2_IntFunctions[TIMER2_VECTOR_INT_MATCH0]();				// Execute a fun��o em anexo
	}
	
	#if (TIMER2_USE_MATCH1 == pdON)
	else if(T2IR & T2IR_MR1I) {											// Checa se a INT gerada � do comparador 1
		T2IR |= T2IR_MR1I;												// Limpa o flag da INT 

		if(timer2_IntFunctions[TIMER2_VECTOR_INT_MATCH1])				// Checa se existe uma fun��o anexada
			timer2_IntFunctions[TIMER2_VECTOR_INT_MATCH1]();				// Execute a fun��o em anexo
	}
	#endif

	#if (TIMER2_USE_MATCH2 == pdON)
	else if(T2IR & T2IR_MR2I) {											// Checa se a INT gerada � do comparador 0
		T2IR |= T2IR_MR2I;												// Limpa o flag da INT 

		if(timer2_IntFunctions[TIMER2_VECTOR_INT_MATCH2])				// Checa se existe uma fun��o anexada
			timer2_IntFunctions[TIMER2_VECTOR_INT_MATCH2]();				// Execute a fun��o em anexo
	}
	#endif
	
	#if (TIMER2_USE_MATCH3 == pdON)
	else if(T2IR & T2IR_MR3I) {											// Checa se a INT gerada � do comparador 0
		T2IR |= T2IR_MR3I;												// Limpa o flag da INT 

		if(timer2_IntFunctions[TIMER2_VECTOR_INT_MATCH3])				// Checa se existe uma fun��o anexada
			timer2_IntFunctions[TIMER2_VECTOR_INT_MATCH3]();				// Execute a fun��o em anexo
	}
	#endif
	
	#if (TIMER2_USE_CAPTURE0 == pdON)
	else if(T2IR & T2IR_CR0I) {											// Checa se a INT gerada � do captura 0
		T2IR |= T2IR_CR0I;												// Limpa o flag da INT 

		if(timer2_IntFunctions[TIMER2_VECTOR_INT_CAPTURE0])				// Checa se existe uma fun��o anexada
			timer2_IntFunctions[TIMER2_VECTOR_INT_CAPTURE0]();			// Execute a fun��o em anexo
	}
	#endif
	
	#if (TIMER2_USE_CAPTURE1 == pdON)
	else if(T2IR & T2IR_CR1I) {											// Checa se a INT gerada � do captura 1
		T2IR |= T2IR_CR1I;												// Limpa o flag da INT 

		if(timer2_IntFunctions[TIMER2_VECTOR_INT_CAPTURE1])				// Checa se existe uma fun��o anexada
			timer2_IntFunctions[TIMER2_VECTOR_INT_CAPTURE1]();			// Execute a fun��o em anexo
	}
	#endif

	// Limpa interrup��o 
	#if defined(arm7tdmi)
	VICSoftIntClr = (1<<VIC_TIMER1);
	#endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_TIMER1);
	#endif

    ISR_EXIT;															// Procedimento para saida da interrup��o    
}
