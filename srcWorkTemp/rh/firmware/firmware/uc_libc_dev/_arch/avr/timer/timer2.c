#include "timer2.h"

volatile u16 timer2_count_overflow;
static u16 __attribute__ ((progmem)) timer2_prescale_factor[] = {0,1,8,64,256,1024};
//static u16 __attribute__ ((progmem)) timer2_rtc_prescale_factor[] = {0,1,8,32,64,128,256,1024};// STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024

typedef void (*void_pFunc)(void);										// Criando um tipo de ponteiro para função	
static volatile void_pFunc timer2_IntFunctions [TIMER2_NUM_INTERRUPTS];	// Criando um vetor de ponteiros para função para atender as interrupções

//###################################################################################################################
// TIMER 2
//###################################################################################################################
//###################################################################################################################
// CONFIGURA TIMER

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa Timer 2
// Parametros:	Prescaler
//					TIMER2_PRESCALE_STOP ou
//					atmega128 = TIMER0_PRESCALE_DIV(1,8,64,256,1024) ou TIMER2_PRESCALE_EXT_T2_FALL ou TIMER2_PRESCALE_EXT_T2_RISE
//					outros avrs = TIMER0_PRESCALE_DIV(1,8,32,64,128,256,1024)
// 				Interrupções: 
//					TIMER2_NO_INT ou (TIMER2_ENABLE_INT_OVERFLOW | TIMER2_ENABLE_INT_COMPARE)
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void timer2_Init(u8 prescale, u8 ints) {
	// RETIRO OS VETORES DE FUNÇÕES EM ANEXOS REFERENTE A INT DO TIMER 0
	timer2_ClearIntHandler(TIMER2_VETOR_INT_OVERFLOW);
	timer2_ClearIntHandler(TIMER2_VETOR_INT_COMPARE);

	timer2_SetPrescaler(prescale);						// Ajusto pre escala
	timer2_ClearCount();								// Inicializo contador	
	TIMSK = (TIMSK & ~(TIMER2_ENABLE_INT_OVERFLOW | TIMER2_ENABLE_INT_COMPARE)) | ints;
}	

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa o prescaler do timer 0. Ou parar o contador
// Parametros:	Prescaler
//					TIMER2_PRESCALE_STOP ou
//					atmega128 = TIMER0_PRESCALE_DIV(1,8,64,256,1024) ou TIMER2_PRESCALE_EXT_T2_FALL ou TIMER2_PRESCALE_EXT_T2_RISE
//					outros avrs = TIMER0_PRESCALE_DIV(1,8,32,64,128,256,1024)
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void timer2_SetPrescaler(u8 prescale)	{
	TCCR2 = (TCCR2 & ~TIMER2_PRESCALE_MASK) | prescale;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Captura o prescaler do timer0
// Parametros:	Nenhum
// Retorna:		O prescaler do timer0
// -------------------------------------------------------------------------------------------------------------------
u16 timer2_GetPrescaler(void) {
	return (pgm_read_word(timer2_prescale_factor+(TCCR2 & TIMER2_PRESCALE_MASK)));
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa os contadores do timer2, tanto o registrador de contagem com o de overflow
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
inline void timer2_ClearCount(void) {
	TCNT2 = 0;
	timer2_count_overflow = 0;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o valor do contador do timer2
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer2
// -------------------------------------------------------------------------------------------------------------------
inline u8 timer2_GetCount(void) {
	return TCNT2;
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
void timer2_SetIntHandler(u8 interruptNum, void (*userFunc)(void) )  {
	if(interruptNum < TIMER2_NUM_INTERRUPTS) {
		timer2_IntFunctions[interruptNum] = userFunc; 
	}
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retira a routina da inetrrupçao desejada do vetor de interrupçao
// Parametros:	Indece da interrupçao
//					TIMER2_VETOR_INT_OVERFLOW ou TIMER2_VETOR_INT_COMPARE
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void timer2_ClearIntHandler(u8 interruptNum) {
	if(interruptNum < TIMER2_NUM_INTERRUPTS) {
		timer2_IntFunctions[interruptNum] = 0;
	}
}

//###################################################################################################################
// TIMER2 OVERFLOW
//###################################################################################################################
//###################################################################################################################
// EMITE UM INTERRUPÇÃO QUANDO O CONTADOR DO TIMER ESTOURAR

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o valor do contador do timer2 junto com os overflows
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer 0
// -------------------------------------------------------------------------------------------------------------------
inline u32 timer2_GetCountAll(void) {
	//return ((u32)timer2_count_overflow * 0xFFFF) + (u32)TCNT2;
	return (u32)TCNT2 + ((u32)timer2_count_overflow * 0xFFFF);
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o valor do contador de overflow do timer2
// Parametros:	Nenhum
// Retorna:		Valor do contador de overflow do timer2
// -------------------------------------------------------------------------------------------------------------------
u16 timer2_GetOverflowCount(void) {
	return timer2_count_overflow;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Interrupção de overflow do timer2
// -------------------------------------------------------------------------------------------------------------------
TIMER2_INTERRUPT_HANDLER(TIMER2_OVF_vect) {
	timer2_count_overflow++;					// Incrementa overflow
	
	//Se a função está definida então execute
	if(timer2_IntFunctions[TIMER2_VETOR_INT_OVERFLOW])
		timer2_IntFunctions[TIMER2_VETOR_INT_OVERFLOW]();
}

//###################################################################################################################
// TIMER2 COMPARE
//###################################################################################################################
//###################################################################################################################
// EMITE UM INTERRUPÇÃO QUANDO O CONTADOR DO TIMER É IGUAL AO REGISTRADOR DE COMPARAÇÃO OCR

#if (TIMER2_USE_COMPARE == pdON)

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Configura o valor de comparação do Timer2
// Parametros:	Valor para comparação
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void timer2_SetCompare (u8 Value) {
	OCR2 = Value;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Interrupção de comparação do timer2. A int é geranda quando o Contador2 = OCR2
// -------------------------------------------------------------------------------------------------------------------
TIMER2_INTERRUPT_HANDLER(TIMER2_COMP_vect) {
	//Se a função está definida então execute
	if(timer2_IntFunctions[TIMER2_VETOR_INT_COMPARE])
		timer2_IntFunctions[TIMER2_VETOR_INT_COMPARE]();
}
#endif
