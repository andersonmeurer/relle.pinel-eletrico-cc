#include "timer1.h"
#if defined(cortexm3)
#include "arm.h"
#endif

void timer1_ISR(void);

typedef void (*void_pFunc)(void);
volatile void_pFunc timer1_IntFunctions[TIMER1_NUM_INTERRUPTS];

volatile u32 Timer1CountOverflow;  // Conta a quantidade de vezes que o MATCH0 � alcan�ado

// ################################################################################################################
// TIMER 1
// ################################################################################################################
// ################################################################################################################

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa Timer 1
// Parametros:	Prescaler: valores de 1 a 32bits
//					Alguns valores padr�es T1MPR_DIVx [x = 1,2,4,8,16,32,64,128,256,512,1024]
// 				sts: T1TCR_ENABLE  	// Habilita o contador do timer
//					 T1TCR_STOP		// Desabilita o contador do timer
//				Habilitar ints para TC=reg MCRx: T1MCR_MR_NO_INT / T1_CONT_OVERFLOW ( T1MCR_MR[0..3]_INT_ENABLE | T1MCR_MR[0..3]_RESET | T1MCR_MR[0..3]_STOP
//					T1MCR_NO_INT				- Sem interrup��es para match ou PWM
//					T1MCR_OVERFLOW				- Indica para habilitar interrup��o MR0 para pelo menos fazer a contagem do overflow
//					T1MCR_MR[0..3]_INT_ENABLE 	- Indica para habilitar interrup��o de contador do timer igualar ao registrador MCR
// 					T1MCR_MR[0..3]_RESET 		- Indica quais das compara��es deve resetar o contador TC
//					T1MCR_MR[0..3]_STOP 		- Indica quais das compara��es deve parar o contador TC
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna c�digo de erro
// ----------------------------------------------------------------------------------------------------------------
int timer1_Init(u32 prescale, u8 sts, u16 int_macths) {
	u8 int_num;
	
	PCONP |= PCTIM1;														// Ligar o timer no controle de potencia. Tem que ser antes da configura��o, pois ap�s ligar o perif�ricos, seus regs assumem valores padr�es
	
	T1IR  = 0xFF;															// Limpa todas as interrup��es
	T1TCR = T1TCR_RESET;													// Reset o contador e mantem em reset
	timer1_SetPrescale(prescale);											// Ajusta o precaler do contador TC
	T1MCR = int_macths;														// Liga as interrup��es quando TC = MCRx
	T1MR0 = 0;																// zera o match0 para usarmos como contador de overflow
	
	for(int_num = 0; int_num < TIMER1_NUM_INTERRUPTS; int_num++)			// Fa�a para todas as interrup��es do timer
		timer1_ClearIntHandler(int_num);									// Retira todas as fun��es anexadas em suas respecticas interrup��es
	
	#if defined(arm7tdmi)
	if (!irq_Install(VIC_TIMER1, timer1_ISR, TIMER1_PRIO_LEVEL))			// Instala o ISR a VIC
		return errIRQ_INSTALL;												// Retonar falso se n�o conseguiu inttalar a ISR a VIC
	#endif
	
	#if defined (cortexm3)
	nvic_enableIRQ(VIC_TIMER1);
	nvic_setPriority(VIC_TIMER1, TIMER1_PRIO_LEVEL);
	#endif
		
	timer1_Set(sts);														// Ajusta a condi��o do timer. Ligado ou desligado

	return pdPASS;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Liga, desliga e re-inicia o timer
// Parametros:	STS: T1TCR_ENABLE  	// Habilita o contador do timer
//					 T1TCR_STOP		// Desabilita o contador do timer
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_Set(u8 sts) {
	T1TCR &= ~(T1TCR_ENABLE | T1TCR_RESET);
	T1TCR|=sts;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Anexa uma routina ao vetor de interrup�ao
// Parametros:	int_num. Indece do vetor de uma determinada interrup��o de timer
// 					TIMER1_VECTOR_INT_MATCH0
// 					TIMER1_VECTOR_INT_MATCH1
// 					TIMER1_VECTOR_INT_MATCH2
// 					TIMER1_VECTOR_INT_MATCH3
// 					TIMER1_VECTOR_INT_CAPTURE0
// 					TIMER1_VECTOR_INT_CAPTURE1
// 					TIMER1_VECTOR_INT_CAPTURE2
// 					TIMER1_VECTOR_INT_CAPTURE3
//				int_func
//					Ponteiro da rotina que tratar� a interrup��o do timer. 
//						Ex	timer1_SetIntHandler(TIMER1_VECTOR_INT_OVERFLOW, myOverflowFunction);
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetIntHandler(u8 int_num, void (*int_func)(void) ) {
	if(int_num < TIMER1_NUM_INTERRUPTS)
		timer1_IntFunctions[int_num] = int_func;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retira a routina da inetrrup�ao desejada do vetor de interrup�ao
// Parametros:	int_num. Indece do vetor de uma determinada interrup��o de timer
// 					TIMER1_VECTOR_INT_MATCH0
// 					TIMER1_VECTOR_INT_MATCH1
// 					TIMER1_VECTOR_INT_MATCH2
// 					TIMER1_VECTOR_INT_MATCH3
// 					TIMER1_VECTOR_INT_CAPTURE0
// 					TIMER1_VECTOR_INT_CAPTURE1
// 					TIMER1_VECTOR_INT_CAPTURE2
// 					TIMER1_VECTOR_INT_CAPTURE3
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_ClearIntHandler(u8 int_num) {
	if(int_num < TIMER1_NUM_INTERRUPTS)
		timer1_IntFunctions[int_num] = 0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta o prescale do timer
// Parametros:	Valor do prescale de 1 a 2^^32bits
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetPrescale(u32 prescale) {
	T1PR = prescale - 1;
}

// ################################################################################################################
// TIMER 1 OVERFLOW
// ################################################################################################################
// ################################################################################################################

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa os contador do timer 1
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
inline void timer1_ClearCount(void) {
	T1TCR |= T1TCR_RESET;													// Reseta contador
	T1TCR &= ~T1TCR_RESET;												// Tira do Reset
	Timer1CountOverflow = 0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador do timer 1
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer 1
// ----------------------------------------------------------------------------------------------------------------
inline u32 timer1_GetCount(void) {
	return T1TC;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa o contador de overflow
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_ClearOverflowCount(void) {
	Timer1CountOverflow = 0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador de overflow do timer 1
// Parametros:	Nenhum
// Retorna:		Valor do contador de overflow do timer 1
// ----------------------------------------------------------------------------------------------------------------
u32 timer1_GetOverflowCount(void) {
	return Timer1CountOverflow;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador do timer 1 junto com os overflows 
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer 1
// ----------------------------------------------------------------------------------------------------------------
inline u64 timer1_GetCountAll(void) {
	return (u64)T1TC + ((u64)Timer1CountOverflow * 0xFFFFFFFF);
}


// ################################################################################################################
// TIMER 1 MATCH
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

#if (TIMER1_USE_MATCH0 == pdON)
// ################################################################################################################
// n�o necess�riamente precisa da interrup��o para fazer a compara��o, por exemplo
//		podemos usar a troca do pino externo sem o uso de interrup��es

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa o pino para match 0
// Parametros:	action: Define o tipo de a��o no bit EM0 quando TC1=MR0 (reg de compara��o 0)
//					T1EMR_EMC0_DONOTHING:  	Faz nada quando chegar na compara��o de MR0
//					T1EMR_EMC0_CLRPIN:		Colocar o bit em n�vel baixo
//					T1EMR_EMC0_SETPIN:		Coloca o bit em n�vel alto
//					T1EMR_EMC0_TOGGLEPIN:	Inverte o bit
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetPinOutputMatch0(int action) {
	#if defined(arm7tdmi)
	T1_MATCH0_PINSEL = (T1_MATCH0_PINSEL & ~T1_MATCH0_PINMASK) | T1_MATCH0_PIN; // Seleciona o pino externo para a fun��o de compara��o
	#else
	MATCH1_ConfigPort0();
	#endif
	T1EMR = (T1EMR & ~T1EMR_EMC0_MASK) | action; // ajusta o comportamento do pino e ativa o pino externo para essa fun��o
}

void timer1_SetMatch0(u32 value) {
	T1MR0 = value;
}

#endif

#if (TIMER1_USE_MATCH1 == pdON)
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa o pino para match 1
// Parametros:	action: Define o tipo de a��o no bit EM1 quando TC1=MR1 (reg de compara��o 1)
//					T1EMR_EMC1_DONOTHING:  	Faz nada quando chegar na compara��o de MR0
//					T1EMR_EMC1_CLRPIN:		Colocar o bit em n�vel baixo
//					T1EMR_EMC1_SETPIN:		Coloca o bit em n�vel alto
//					T1EMR_EMC1_TOGGLEPIN:	Inverte o bit
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetPinOutputMatch1(int action) {
	#if defined(arm7tdmi)
	T1_MATCH1_PINSEL = (T1_MATCH1_PINSEL & ~T1_MATCH1_PINMASK) | T1_MATCH1_PIN; // Seleciona o pino externo para a fun��o de compara��o 1
	#else
	MATCH1_ConfigPort1();
	#endif
	T1EMR = (T1EMR & ~T1EMR_EMC1_MASK) | action; // ajusta o comportamento do pino e ativa o pino externo para essa fun��o
}

void timer1_SetMatch1(u32 value) {
	T1MR1 = value;
}
#endif

#if (TIMER1_USE_MATCH2 == pdON)
void timer1_SetMatch2(u32 value) {
	T1MR2 = value;
}
#endif

#if (TIMER1_USE_MATCH3 == pdON)
void timer1_SetMatch3(u32 value) {
	T1MR3 = value;
}
#endif

// ################################################################################################################
// TIMER 1 COMPTURE
// ################################################################################################################
// ################################################################################################################
#if (TIMER1_USE_CAPTURE0 == pdON)

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa compara��o 0 do timer 1 via pino CAPn.0
// Parametros:	Definir o tipo de borda do sinal de captura e ligar ou n�o a interrup�ao
//					(T1CCR_CR0_RISING/T1CCR_CR0_FALLING) | T1CCR_CR0_INT
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_InitCapture0(u16 sts) {
	#if defined(arm7tdmi)
	T1_CAP0_PINSEL = (T1_CAP0_PINSEL & ~T1_CAP0_PINMASK) | T1_CAP0_PIN; // Seleciona o pino externo para a fun��o de captura 0
	#else
	t1cap0_ConfigPort();
	#endif
	T1CCR |= sts; 														 // Ajusta a configura��o de captura
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna com o valor capturado do contador timer 0, TC, durante o evento CAP0
// Parametros:	Nenhum
// Retorna:		Valor capturado do contador do timer0 TC armazenado no registrador CR0
// ----------------------------------------------------------------------------------------------------------------
inline u32 timer1_GetCapture0(void) {
	return T1CR0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desliga a compara��o 0 do timer 1
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_StopCapture0(void) {
	#if defined(arm7tdmi)
	T1_CAP0_PINSEL &=~T1_CAP0_PINMASK;									// Seleciona o pino CAP para GPIO
	#endif
	T1CCR &= ~(T1CCR_CR0_INT | T1CCR_CR0_RISING | T1CCR_CR0_FALLING);	// Desliga as interrrup��o e tipo de leitura
}

#endif

#if (TIMER1_USE_CAPTURE1 == pdON)
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa compara��o 1 do timer 1 via pino CAPn.1
// Parametros:	Definir o tipo de borda do sinal de captura e ligar ou n�o a interrup�ao
//					(T1CCR_CR1_RISING/T1CCR_CR1_FALLING) | T1CCR_CR1_INT
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_InitCapture1(u16 sts) {
	T1_CAP1_PINSEL = (T1_CAP1_PINSEL & ~T1_CAP1_PINMASK) | T1_CAP1_PIN; // Seleciona o pino externo para a fun��o de captura 1
	T1CCR |= sts; 														// Ajusta a configura��o de captura
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna com o valor capturado do contador timer 0, TC, durante o evento CAP0
// Parametros:	Nenhum
// Retorna:		Valor capturado do contador do timer0 TC armazenado no registrador CR0
// ----------------------------------------------------------------------------------------------------------------
inline u32 timer1_GetCapture1(void) {
	return T1CR1;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desliga a compara��o 0 do timer 1
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_StopCapture1(void) {
	#if defined(arm7tdmi)
	T1_CAP1_PINSEL &=~T1_CAP1_PINMASK;									// Seleciona o pino CAP para GPIO
	#endif
	T1CCR &= ~(T1CCR_CR1_INT | T1CCR_CR1_RISING | T1CCR_CR1_FALLING);	// Desliga as interrrup��o e tipo de leitura
}
#endif

// ################################################################################################################
// TIMER 1 SERVI�O DE INTERRUP��O
// ################################################################################################################
// ################################################################################################################
void timer1_ISR(void) {
    ISR_ENTRY;																	// Procedimento para entrada da interrup��o
	 
	// � utilizado o mach0 para contagem de overflow
	if(T1IR & T1IR_MR0I) {												// Checa se a INT gerada � do comparador 0
		T1IR |= T1IR_MR0I;												// Limpa o flag da INT 
		
		Timer1CountOverflow++;
		
		if(timer1_IntFunctions[TIMER1_VECTOR_INT_MATCH0])				// Checa se existe uma fun��o anexada
			timer1_IntFunctions[TIMER1_VECTOR_INT_MATCH0]();				// Execute a fun��o em anexo
	}
	
	#if (TIMER1_USE_MATCH1 == pdON)
	else if(T1IR & T1IR_MR1I) {											// Checa se a INT gerada � do comparador 1
		T1IR |= T1IR_MR1I;												// Limpa o flag da INT 

		if(timer1_IntFunctions[TIMER1_VECTOR_INT_MATCH1])				// Checa se existe uma fun��o anexada
			timer1_IntFunctions[TIMER1_VECTOR_INT_MATCH1]();				// Execute a fun��o em anexo
	}
	#endif

	#if (TIMER1_USE_MATCH2 == pdON)
	else if(T1IR & T1IR_MR2I) {											// Checa se a INT gerada � do comparador 0
		T1IR |= T1IR_MR2I;												// Limpa o flag da INT 

		if(timer1_IntFunctions[TIMER1_VECTOR_INT_MATCH2])				// Checa se existe uma fun��o anexada
			timer1_IntFunctions[TIMER1_VECTOR_INT_MATCH2]();				// Execute a fun��o em anexo
	}
	#endif
	
	#if (TIMER1_USE_MATCH3 == pdON)
	else if(T1IR & T1IR_MR3I) {											// Checa se a INT gerada � do comparador 0
		T1IR |= T1IR_MR3I;												// Limpa o flag da INT 

		if(timer1_IntFunctions[TIMER1_VECTOR_INT_MATCH3])				// Checa se existe uma fun��o anexada
			timer1_IntFunctions[TIMER1_VECTOR_INT_MATCH3]();				// Execute a fun��o em anexo
	}
	#endif
	
	#if (TIMER1_USE_CAPTURE0 == pdON)
	else if(T1IR & T1IR_CR0I) {											// Checa se a INT gerada � do captura 0
		T1IR |= T1IR_CR0I;												// Limpa o flag da INT 

		if(timer1_IntFunctions[TIMER1_VECTOR_INT_CAPTURE0])				// Checa se existe uma fun��o anexada
			timer1_IntFunctions[TIMER1_VECTOR_INT_CAPTURE0]();			// Execute a fun��o em anexo
	}
	#endif
	
	#if (TIMER1_USE_CAPTURE1 == pdON)
	else if(T1IR & T1IR_CR1I) {											// Checa se a INT gerada � do captura 1
		T1IR |= T1IR_CR1I;												// Limpa o flag da INT 

		if(timer1_IntFunctions[TIMER1_VECTOR_INT_CAPTURE1])				// Checa se existe uma fun��o anexada
			timer1_IntFunctions[TIMER1_VECTOR_INT_CAPTURE1]();			// Execute a fun��o em anexo
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
