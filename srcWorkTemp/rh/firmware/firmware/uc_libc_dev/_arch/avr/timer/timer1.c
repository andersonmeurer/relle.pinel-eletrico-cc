#include "timer1.h"

volatile u16 timer1_count_overflow;
static u16 __attribute__ ((progmem)) timer1_prescale_factor[] = {0,1,8,64,256,1024};
//static u16 __attribute__ ((progmem)) timer1_rtc_prescale_factor[] = {0,1,8,32,64,128,256,1024};// STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024

typedef void (*void_pFunc)(void); 										// Criando um tipo de ponteiro para fun��o
static volatile void_pFunc timer1_IntFunctions[TIMER1_NUM_INTERRUPTS]; 	// Criando um vetor de ponteiros para fun��o para atender as interrup��es

#if (TIMER1_USE_PWM == pdON)
	static u8 timer1_pwm_res;
#endif
	

// ################################################################################################################
// TIMER 1
// ################################################################################################################
// ################################################################################################################
// CONFIGURA TIMER

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa Timer 1
// Parametros:	Prescaler
//					TIMER1_PRESCALE_STOP ou 
//					TIMER1_PRESCALE_DIV(1,8,64,256,1024) ou 
//					TIMER1_PRESCALE_EXT_T1_FALL ou TIMER1_PRESCALE_EXT_T1_RISE
// 				Interrup��es: 
//					TIMER1_NO_INT ou 
//					TIMER1_ENABLE_INT_OVERFLOW | TIMER1_ENABLE_INT_COMPARE_B | TIMER1_ENABLE_INT_COMPARE_A | 
//						TIMER1_ENABLE_INT_CAPTURE | TIMER1_ENABLE_INT_COMPARE_C
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_Init(u8 prescale, u16 ints) {
	// RETIRO OS VETORES DE FUN��ES EM ANEXOS REFERENTE A INT DO TIMER 0
	timer1_ClearIntHandler(TIMER1_VETOR_INT_OVERFLOW);
	timer1_ClearIntHandler(TIMER1_VETOR_INT_COMPARE_A);
	timer1_ClearIntHandler(TIMER1_VETOR_INT_COMPARE_B);
	timer1_ClearIntHandler(TIMER1_VETOR_INT_CAPTURE);
		
	#ifdef OCR1C
		timer1_ClearIntHandler(TIMER1_VETOR_INT_COMPARE_C);
		ETIMSK = (ETIMSK & ~(u8)(TIMER1_ENABLE_INT_COMPARE_C>>8)) | (ints & TIMER1_ENABLE_INT_COMPARE_C)>>8;
	#endif
	
	timer1_SetPrescaler(prescale);	// Ajusto pre escala
	timer1_ClearCount();			// Inicializo contador	
			
	// Habilito a interrup��es se setados
	TIMSK = (  (TIMSK & ~(TIMER1_ENABLE_INT_OVERFLOW | TIMER1_ENABLE_INT_COMPARE_A | TIMER1_ENABLE_INT_COMPARE_B | TIMER1_ENABLE_INT_CAPTURE)) | (u8)ints);
}	

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa o prescaler do timer 1. Ou parar o contador
// Parametros:	Prescaler
//					TIMER1_PRESCALE_STOP ou 
//					TIMER1_PRESCALE_DIV(1,64,256,1024) ou 
//					TIMER1_PRESCALE_EXT_T1_FALL ou TIMER1_PRESCALE_EXT_T1_RISE
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetPrescaler(u8 prescale) {
	TCCR1B = (TCCR1B & ~TIMER1_PRESCALE_MASK) | prescale;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Captura o prescaler do timer1
// Parametros:	Nenhum
// Retorna:		O prescaler do timer1
// -------------------------------------------------------------------------------------------------------------------
u16 timer1_GetPrescaler(void) {
	return (pgm_read_word(timer1_prescale_factor+(TCCR1B & TIMER1_PRESCALE_MASK)));
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa os contador do timer 1
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
inline void timer1_ClearCount(void) {
	TCNT1 = 0;
	timer1_count_overflow = 0;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador do timer 1
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer 1
// ----------------------------------------------------------------------------------------------------------------
inline u16 timer1_GetCount(void) {
	return TCNT1;
}

// ################################################################################################################
// VETOR DE INTERRUP��O
// ################################################################################################################
// ################################################################################################################
// ANEXA OU RETIRA FUN��ES AO VETOR DE INTERRUP��ES

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Anexa uma routina ao vetor de interrup�ao
// Parametros:	Indece da interrup�ao
//					TIMER1_VETOR_INT_OVERFLOW ou TIMER1_VETOR_INT_COMPARE_A ou TIMER1_VETOR_INT_COMPARE_A ou
//						TIMER1_VETOR_INT_COMPARE_C ou TIMER1_VETOR_INT_CAPTURE
//				Ponteiro da routina. Exemplo:
//					Timer_Attach(TIMER1_VETOR_INT_OVERFLOW, myOverflowFunction);
//					Timer_DelIntHandler(TIMER1_VETOR_INT_OVERFLOW)
//					void myOverflowFunction(void) { ... }
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetIntHandler(u8 interruptNum, void (*userFunc)(void) ) {
	if(interruptNum < TIMER1_NUM_INTERRUPTS) {
		timer1_IntFunctions[interruptNum] = userFunc; 
	}
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retira a routina da inetrrup�ao desejada do vetor de interrup�ao
// Parametros:	Indece da interrup�ao
//					TIMER1_VETOR_INT_OVERFLOW ou TIMER1_VETOR_INT_COMPARE_A ou TIMER1_VETOR_INT_COMPARE_A ou
//						TIMER1_VETOR_INT_COMPARE_C ou TIMER1_VETOR_INT_CAPTURE
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_ClearIntHandler(u8 interruptNum) {
	if(interruptNum < TIMER1_NUM_INTERRUPTS) {
		timer1_IntFunctions[interruptNum] = 0;
	}
}

// ################################################################################################################
// TIMER 1 OVERFLOW
// ################################################################################################################
// ################################################################################################################
// EMITE UM INTERRUP��O QUANDO O CONTADOR DO TIMER ESTOURAR

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador do timer 1 junto com os overflows 
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer 1
// ----------------------------------------------------------------------------------------------------------------
// OBS (INT DO OVERFLOW DEVE ESTAR ACIONADA)
inline u32 timer1_GetCountAll(void)  {
	//register u16 T1 = TCNT1;
	//return ((u32)timer1_count_overflow * 0xFFFF) + (u32)T1;
	return (u32)TCNT1 + ((u32)timer1_count_overflow * 0xFFFF);
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador de overflow do timer 1
// Parametros:	Nenhum
// Retorna:		Valor do contador de overflow do timer 1
// ----------------------------------------------------------------------------------------------------------------
u16 timer1_GetOverflowCount(void) {
	return timer1_count_overflow;
}


// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de overflow do timer 1
// ----------------------------------------------------------------------------------------------------------------
TIMER1_INTERRUPT_HANDLER(TIMER1_OVF_vect) {
	timer1_count_overflow++;					// Incrementa overflow
	
	//Se a fun��o est� definida ent�o execute
	if(timer1_IntFunctions[TIMER1_VETOR_INT_OVERFLOW])
		timer1_IntFunctions[TIMER1_VETOR_INT_OVERFLOW]();
}


// ################################################################################################################
// TIMER 1 COMPARE
// ################################################################################################################
// ################################################################################################################
// EMITE UM INTERRUP��O QUANDO O CONTADOR DO TIMER � IGUAL AO REGISTRADOR DE COMPARA��O OCR

#if (TIMER1_USE_COMPARE == pdON)
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	
// Parametros:	TIMER1_OC1A_OFF, TIMER1_OC1A_CHANGE, TIMER1_OC1A_NORMAL, TIMER1_OC1A_INV
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetComparePortOC1A(u8 StsOC1A) {
	sbi(TIMER1_OC1A_DDR, TIMER1_OC1A_PIN); // Configure o pino de compara��o para saida
	TCCR1A = (TCCR1A & ~TIMER1_OC1A_MASK) | StsOC1A;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	
// Parametros:	TIMER1_OC1B_OFF, TIMER1_OC1B_CHANGE, TIMER1_OC1B_NORMAL, TIMER1_OC1B_INV
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetComparePortOC1B(u8 StsOC1B) {
	#ifdef TIMER1_OC1B_PIN
		sbi(TIMER1_OC1B_DDR, TIMER1_OC1B_PIN); // Configure o pino de compara��o para saida
	#endif

	TCCR1A = (TCCR1A & ~TIMER1_OC1B_MASK) | StsOC1B;
}

#ifdef OCR1C
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	
// Parametros:	TIMER1_OC1C_OFF, TIMER1_OC1C_CHANGE, TIMER1_OC1C_NORMAL, TIMER1_OC1C_INV
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetComparePortOC1C(u8 StsOC1C) {
	#ifdef TIMER1_OC1C_PIN
		sbi(TIMER1_OC1C_DDR, TIMER1_OC1C_PIN);// Configure o pino de compara��o para saida
	#endif

	TCCR1A = (TCCR1A & ~TIMER1_OC1C_MASK) | StsOC1C;
}
#endif

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Configura o valor de compara��o A do Timer 1
// Parametros:	Valor para compara��o
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetCompareA (u16 Value) {
	OCR1A = Value;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Configura o valor de compara��o B do Timer 1
// Parametros:	Valor para compara��o
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetCompareB (u16 Value) {
	OCR1B = Value;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa contador quando a compara��o for igual
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
#ifdef OCR1C
void timer1_SetCompareClearOnMatch(void) {
	TCCR1B |= (1<<WGM12);
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Configura o valor de compara��o C do Timer 1
// Parametros:	Valor para compara��o
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_SetCompareC (u16 Value) {
	OCR1C = Value;
}
#endif

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de compara��o do OC1A do timer 1. A int � geranda quando o Contador1 = OCR1A
// ----------------------------------------------------------------------------------------------------------------
TIMER1_INTERRUPT_HANDLER(TIMER1_COMPA_vect) {
	//Se a fun��o est� definida ent�o execute
	if(timer1_IntFunctions[TIMER1_VETOR_INT_COMPARE_A])
		timer1_IntFunctions[TIMER1_VETOR_INT_COMPARE_A]();
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de compara��o do OC1B do timer 1. A int � geranda quando o Contador1 = OCR1B
// ----------------------------------------------------------------------------------------------------------------
TIMER1_INTERRUPT_HANDLER(TIMER1_COMPB_vect) {
	//Se a fun��o est� definida ent�o execute
	if(timer1_IntFunctions[TIMER1_VETOR_INT_COMPARE_B])
		timer1_IntFunctions[TIMER1_VETOR_INT_COMPARE_B]();
}

#ifdef OCR1C
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de compara��o do OC1V do timer 1. A int � geranda quando o Contador1 = OCR1C
// ----------------------------------------------------------------------------------------------------------------
TIMER1_INTERRUPT_HANDLER(TIMER1_COMPC_vect) {
	//Se a fun��o est� definida ent�o execute
	if(timer1_IntFunctions[TIMER1_VETOR_INT_COMPARE_C])
		timer1_IntFunctions[TIMER1_VETOR_INT_COMPARE_C]();
}

#endif


#endif

// ################################################################################################################
// TIMER 1 CAPTURE ICP
// ################################################################################################################
// ################################################################################################################
// CAPTURA O VALOR DO TIMER1 PARA O REGISTRADOR ICR1 QUANDO HOUVER MUDAN�A DO ESTADO DO PINO ICP
#if (TIMER1_USE_ICP == pdON)

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Configura o modo de trabalho do IC
// Parametros:	Configura��o de captura 
//				( TIMER1_ICP_NOISE_CANCELER_ON ou TIMER1_ICP_NOISE_CANCELER_OFF) |
//				( TIMER1_ICP_FALLING_EDGE ou TIMER1_ICP_RISING_EDGE)
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_IcpInit(u8 config) {
	TCCR1B = (TCCR1B & ~TIMER1_ICP_MASK) | config;

	#ifdef TIMER1_ICP_PIN
		cbi(TIMER1_ICP_DDR, TIMER1_ICP_PIN);
	#endif
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna com o valor capturado do timer1 no momento da mudan�a do pino IC1
// Parametros:	Nenhum
// Retorna:		Valor do timer 1
// ----------------------------------------------------------------------------------------------------------------
u16 timer1_IcpGetValue(void) {
	return ICR1;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de interrup��o de captura IC1 do timer1
// ----------------------------------------------------------------------------------------------------------------
TIMER1_INTERRUPT_HANDLER(TIMER1_CAPT_vect) {
	//Se a fun��o est� definida ent�o execute
	if(timer1_IntFunctions[TIMER1_VETOR_INT_CAPTURE])
		timer1_IntFunctions[TIMER1_VETOR_INT_CAPTURE]();
}

#endif


// ################################################################################################################
// TIMER 1 PWM
// ################################################################################################################
// ################################################################################################################
#if (TIMER1_USE_PWM == pdON)
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa o timer1 para o PWM. Este utiliza os pinos OC1A, OC1B e OC1C
// Parametros:	Bit de resolu��o do PWM 
//					TIMER1_PWM_MODE_8BITS ou TIMER1_PWM_MODE_9BITS ou TIMER1_PWM_MODE_10BITS
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_PwmInit(u8 bit_res) {
	//QUANTO MAIOR O BIT, MAIOR SER� CONTAGEM E MAIOR SER� O TEMPO
	timer1_pwm_res = bit_res;
	
	if (bit_res == TIMER1_PWM_MODE_9BITS) { 			// Modo de 9 bits
		sbi(TCCR1A,WGM11);		
		cbi(TCCR1A,WGM10);
	} else if( bit_res == TIMER1_PWM_MODE_10BITS ) { 	// Modo de 10 bits
		sbi(TCCR1A,WGM11);		
		sbi(TCCR1A,WGM10);
	} else { 											// Modo de 8 bits
		cbi(TCCR1A,WGM11);		
		sbi(TCCR1A,WGM10);
	}

	OCR1A = 0; // Limpa o comparador de saida A
	OCR1B = 0; // Limpa o comparador de saida B
	
	#ifdef OCR1C
		OCR1C = 0; // Limpa o comparador de saida C
	#endif
}

/*
#ifdef WGM10
void timer1_PwmInitICR(u16 TopCount) {
	// set PWM mode with ICR top-count
	cbi(TCCR1A,WGM10);
	sbi(TCCR1A,WGM11);
	sbi(TCCR1B,WGM12);
	sbi(TCCR1B,WGM13);
	
	ICR1 = TopCount;
	
	OCR1A = 0; // Limpa o comparador de saida A
	OCR1B = 0; // Limpa o comparador de saida B
	
	#ifdef OCR1C
		OCR1C = 0; // Limpa o comparador de saida C
	#endif
}
#endif
*/

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desliga o PWM do timer 1
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_PwmOff(void) {
	// Desliga opera��o de PWM no timer 1
	cbi(TCCR1A,WGM11);
	cbi(TCCR1A,WGM10);
	
	timer1_PwmAOff();
	timer1_PwmBOff();
	
	#ifdef OCR1C
		timer1_PwmCOff();
	#endif
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Conecta a fun��o PWM do timer 1 no pino OC1A de modo invertido ou n�o
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_PwmAOn(void) {
	#if (TIMER1_USE_PWM_OC1A_INV == pdON)
		// Pino OC1A invertido
		sbi(TCCR1A,COM1A1);	
		sbi(TCCR1A,COM1A0);			
	#else
		// Pino OC1A n�o invertido
		sbi(TCCR1A,COM1A1);	
		cbi(TCCR1A,COM1A0);
	#endif
	
	sbi(TIMER1_OC1A_DDR, TIMER1_OC1A_PIN); 	// Configure o pino do PWM para saida
}


// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Conecta a fun��o PWM do timer 1 no pino OC1B de modo n�o invertido
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_PwmBOn(void) {
	#if (TIMER1_USE_PWM_OC1B_INV  == pdON)
		// Pino OC1B invertido 
		sbi(TCCR1A,COM1B1);	
		sbi(TCCR1A,COM1B0);
	#else
		// Pino OC1B n�o invertido
		sbi(TCCR1A,COM1B1);	
		cbi(TCCR1A,COM1B0);
	#endif
	
	#ifdef TIMER1_OC1B_PIN
		sbi(TIMER1_OC1B_DDR, TIMER1_OC1B_PIN); // Configure o pino do PWM para saida
	#endif
}


#ifdef OCR1C
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Conecta a fun��o PWM do timer 1 no pino OC1C de modo n�o invertido
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_PwmCOn(void) {
	#if (TIMER1_USE_PWM_OC1C_INV == pdON)
		// Pino OC1C invertido 
		sbi(TCCR1A,COM1C1);	
		sbi(TCCR1A,COM1C0);
	#else
		// Pino OC1C n�o invertido
		sbi(TCCR1A,COM1C1);	
		cbi(TCCR1A,COM1C0);
	#endif
	
	#ifdef TIMER1_OC1C_PIN
		sbi(TIMER1_OC1C_DDR, TIMER1_OC1C_PIN); // Configure o pino do PWM para saida
	#endif
}
#endif


// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desconecta a fun��o PWM do pino OC1A
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_PwmAOff(void) {
	cbi(TCCR1A,COM1A1);	
	cbi(TCCR1A,COM1A0);
	cbi(TIMER1_OC1A_DDR, TIMER1_OC1A_PIN); // Configure o pino do PWM para entrada (Padr�o no reset)
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desconecta a fun��o PWM do pino OC1B
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_PwmBOff(void) {
	cbi(TCCR1A,COM1B1);	
	cbi(TCCR1A,COM1B0);
	
	#ifdef TIMER1_OC1B_PIN
		cbi(TIMER1_OC1B_DDR, TIMER1_OC1B_PIN); // Configure o pino do PWM para entrada (Padr�o no reset)
	#endif
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desconecta a fun��o PWM do pino OC1C
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
#ifdef OCR1C
void timer1_PwmCOff(void) {
	cbi(TCCR1A,COM1C1);	
	cbi(TCCR1A,COM1C0);
	
	#ifdef TIMER1_OC1C_PIN
		cbi(TIMER1_OC1C_DDR, TIMER1_OC1C_PIN); // Configure o pino do PWM para entrada (Padr�o no reset)
	#endif
}
#endif

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta o valor em porcentagem de compara��o do PWM OC1A referente ao ciclo alto
// Parametros:	Valor da permanencia do ciclo alto. Valo de 1 a 100%. Valor 0% � desligado
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_PwmASet(u8 pwmDuty) {
	u16 Value;
	
	Value = (1<<timer1_pwm_res)-1;
	if (pwmDuty < 100)
		Value = (pwmDuty*(Value/100));

	OCR1A = Value;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta o valor em porcentagem de compara��o do PWM OC1B referente ao ciclo alto
// Parametros:	Valor da permanencia do ciclo alto. Valo de 0 a 100%
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_PwmBSet(u8 pwmDuty) {
	u32 Value;
			
	Value = (1<<timer1_pwm_res)-1;
	if (pwmDuty < 100)
		Value = (pwmDuty*(Value/100));
	
	OCR1B = Value;
}

#ifdef OCR1C
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta o valor em porcentagem de compara��o do PWM OC1C referente ao ciclo alto
// Parametros:	Valor da permanencia do ciclo alto. Valo de 0 a 100%
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer1_PwmCSet(u8 pwmDuty) {
	u32 Value;
			
	Value = (1<<timer1_pwm_res)-1;
	if (pwmDuty < 100)
		Value = (pwmDuty*(Value/100));
	
	OCR1C = Value;
}
#endif


#endif
