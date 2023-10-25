#include "timer0.h"

volatile u16 timer0_count_overflow;
static u16 __attribute__ ((progmem)) timer0_prescale_factor[] = {0,1,8,64,256,1024};
//static u16 __attribute__ ((progmem)) timer0_rtc_prescale_factor[] = {0,1,8,32,64,128,256,1024};// STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024

typedef void (*void_pFunc)(void);										// Criando um tipo de ponteiro para função
static volatile void_pFunc timer0_IntFunctions[TIMER0_NUM_INTERRUPTS]; 	// Criando um vetor de ponteiros para função para atender as interrupções

//###################################################################################################################
// TIMER0
//###################################################################################################################
//###################################################################################################################
// CONFIGURA TIMER

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa Timer0
// Parametros:	Prescaler
//					TIMER0_PRESCALE_STOP ou
//					para o atmega128 	= TIMER0_PRESCALE_DIV(1,32,64,128,256,1024)
//					para os demais avrs = TIMER0_PRESCALE_DIV(1,64,256,1024) ou TIMER0_PRESCALE_EXT_T0_FALL ou TIMER0_PRESCALE_EXT_T0_RISE
// 				Interrupções: 
//					TIMER0_NO_INT ou (TIMER0_ENABLE_INT_OVERFLOW | TIMER0_ENABLE_INT_COMPARE)
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void timer0_Init(u8 prescale, u8 ints) {
	// RETIRO OS VETORES DE FUNÇÕES EM ANEXOS REFERENTE A INT DO TIMER 0
	timer0_ClearIntHandler(TIMER0_VETOR_INT_OVERFLOW);

	#if defined (OCIE0) && (TIMER0_USE_COMPARE == pdON) 	// Se existe comparação no timer0 do AVR e foi habilitado essa função
	timer0_ClearIntHandler(TIMER0_VETOR_INT_COMPARE);
	#endif
	
	timer0_SetPrescaler(prescale);								// Ajusto pre escala
	timer0_ClearCount();										// Inicializo contador	

	#if defined (OCIE0)  	// Se existe comparação no timer0 do AVR
		TIMSK = (TIMSK & ~(TIMER0_ENABLE_INT_OVERFLOW | TIMER0_ENABLE_INT_COMPARE)) | ints;
	#else
		TIMSK = (TIMSK & ~TIMER0_ENABLE_INT_OVERFLOW) | ints;
	#endif
}	

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa o prescaler do timer 0. Ou parar o contador
// Parametros:	Prescaler 
//					TIMER0_PRESCALE_STOP ou
//					para o atmega128 	= TIMER0_PRESCALE_DIV(1,32,64,128,256,1024)
//					para os demais avrs = TIMER0_PRESCALE_DIV(1,64,256,1024) ou TIMER0_PRESCALE_EXT_T0_FALL ou TIMER0_PRESCALE_EXT_T0_RISE
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void timer0_SetPrescaler(u8 prescale)	{
	TCCR0 = (TCCR0 & ~TIMER0_PRESCALE_MASK) | prescale;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Captura o prescaler do timer0
// Parametros:	Nenhum
// Retorna:		O prescaler do timer0
// -------------------------------------------------------------------------------------------------------------------
u16 timer0_GetPrescaler(void) {
	return (pgm_read_word(timer0_prescale_factor+(TCCR0 & TIMER0_PRESCALE_MASK)));
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa os contadores do timer0, tanto o registrador de contagem com o de overflow
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
inline void timer0_ClearCount(void) {
	TCNT0 = 0;
	timer0_count_overflow = 0;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o valor do contador do timer0
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer0
// -------------------------------------------------------------------------------------------------------------------
inline u8 timer0_GetCount(void) {
	return TCNT0;
}

//###################################################################################################################
// VETOR DE INTERRUPÇÂO
//###################################################################################################################
//###################################################################################################################
// ANEXA OU RETIRA FUNÇÕES AO VETOR DE INTERRUPÇÕES

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Anexa uma routina ao vetor de interrupçao
// Parametros:	Indece da interrupçao
//					TIMER0_VETOR_INT_OVERFLOW ou TIMER0_VETOR_INT_COMPARE
//				Ponteiro da routina. Exemplo:
//					Timer_Attach(TIMER0_VETOR_INT_OVERFLOW, myOverflowFunction);
//					Timer_DelIntHandler(TIMER0_VETOR_INT_OVERFLOW)
//					void myOverflowFunction(void) { ... }
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void timer0_SetIntHandler(u8 interruptNum, void (*userFunc)(void) ) {
	if(interruptNum < TIMER0_NUM_INTERRUPTS) {
		timer0_IntFunctions[interruptNum] = userFunc; 
	}
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retira a routina da inetrrupçao desejada do vetor de interrupçao
// Parametros:	Indece da interrupçao
//					TIMER0_VETOR_INT_OVERFLOW ou TIMER0_VETOR_INT_COMPARE
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void timer0_ClearIntHandler(u8 interruptNum) {
	if(interruptNum < TIMER0_NUM_INTERRUPTS) {
		timer0_IntFunctions[interruptNum] = 0;
	}
}

//###################################################################################################################
// TIMER 0 OVERFLOW
//###################################################################################################################
//###################################################################################################################
// EMITE UM INTERRUPÇÃO QUANDO O CONTADOR DO TIMER ESTOURAR

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o valor do contador do timer0 junto com os overflows
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer0
// -------------------------------------------------------------------------------------------------------------------
inline u32 timer0_GetCountAll(void) {
	//return ((u32)timer0_count_overflow * 0xFFFF) + (u32)TCNT0;
	return (u32)TCNT0 + ((u32)timer0_count_overflow * 0xFFFF);
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o valor do contador de overflow do timer0
// Parametros:	Nenhum
// Retorna:		Valor do contador de overflow do timer0
// -------------------------------------------------------------------------------------------------------------------
u16 timer0_GetOverflowCount(void) {
	return timer0_count_overflow;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Interrupção de overflow do timer0
// -------------------------------------------------------------------------------------------------------------------
TIMER0_INTERRUPT_HANDLER(TIMER0_OVF_vect) {
	timer0_count_overflow++;	// Incrementa overflow
	
	//Se a função está definida então execute
	if(timer0_IntFunctions[TIMER0_VETOR_INT_OVERFLOW])
		timer0_IntFunctions[TIMER0_VETOR_INT_OVERFLOW]();
}

//###################################################################################################################
// TIMER 0 COMPARE
//###################################################################################################################
//###################################################################################################################
// EMITE UM INTERRUPÇÃO QUANDO O CONTADOR DO TIMER É IGUAL AO REGISTRADOR DE COMPARAÇÃO OCR

#if defined(OCIE0) && (TIMER0_USE_COMPARE == pdON) // se existe comparação no timer0

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Configura o valor de comparação do Timer 0
// Parametros:	Valor para comparação
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void timer0_SetCompare (u8 value) {
	OCR0 = value;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Interrupção de comparação do timer 0. A int é geranda quando o Contador1 = OCR0
// -------------------------------------------------------------------------------------------------------------------
TIMER0_INTERRUPT_HANDLER(TIMER0_COMP_vect) {
	//Se a função está definida então execute
	if(timer0_IntFunctions[TIMER0_VETOR_INT_COMPARE])
		timer0_IntFunctions[TIMER0_VETOR_INT_COMPARE]();
}
#endif
