#include "timer3.h"

volatile u16 timer3_count_overflow;
static u16 __attribute__ ((progmem)) timer3_prescale_factor[] = {0,1,8,64,256,1024};
//static u16 __attribute__ ((progmem)) timer3_rtc_prescale_factor[] = {0,1,8,32,64,128,256,1024};// STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024

typedef void (*void_pFunc)(void); 										// Criando um tipo de ponteiro para fun��o
static volatile void_pFunc timer3_IntFunctions[TIMER3_NUM_INTERRUPTS];	// Criando um vetor de ponteiros para fun��o para atender as interrup��es

#if (TIMER3_USE_PWM == pdON)
	static u8 timer3_pwm_res;
#endif
	

// ################################################################################################################
// TIMER 3
// ################################################################################################################
// ################################################################################################################
// CONFIGURA TIMER

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa Timer 3
// Parametros:	Prescaler
//					TIMER1_PRESCALE_STOP ou 
//					TIMER0_PRESCALE_DIV(1,64,256,1024) ou 
//					TIMER1_PRESCALE_EXT_T1_FALL ou TIMER1_PRESCALE_EXT_T1_RISE
// 				Interrup��es: 
//					TIMER1_NO_INT ou 
//					TIMER1_ENABLE_INT_OVERFLOW | TIMER1_ENABLE_INT_COMPARE_B | TIMER1_ENABLE_INT_COMPARE_A | 
//						TIMER1_ENABLE_INT_CAPTURE | TIMER1_ENABLE_INT_COMPARE_C
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_Init(u8 prescale, u8 ints) {
	// RETIRO DO VETOR DE FUN��ES EM ANEXOS REFERENTE A INT DO TIMER 3
	timer3_ClearIntHandler(TIMER3_VETOR_INT_OVERFLOW);
	timer3_ClearIntHandler(TIMER3_VETOR_INT_COMPARE_A);
	timer3_ClearIntHandler(TIMER3_VETOR_INT_COMPARE_B);
	timer3_ClearIntHandler(TIMER3_VETOR_INT_COMPARE_C);
	timer3_ClearIntHandler(TIMER3_VETOR_INT_CAPTURE);
	
	timer3_SetPrescaler(prescale);						// Ajusto escala
	timer3_ClearCount();								// Inicializo contador	
			
	// Habilito a interrup��es se setados
	ETIMSK = (  (ETIMSK & ~(TIMER3_ENABLE_INT_OVERFLOW | TIMER3_ENABLE_INT_COMPARE_A | TIMER3_ENABLE_INT_COMPARE_B |TIMER3_ENABLE_INT_COMPARE_C | TIMER3_ENABLE_INT_CAPTURE)) | ints);	
}	

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa o prescaler do timer 3. Ou parar o contador
// Parametros:	Prescaler
//					TIMER3_PRESCALE_STOP ou 
//					TIMER3_PRESCALE_DIV(1,64,256,1024) ou 
//					TIMER3_PRESCALE_EXT_T1_FALL ou TIMER1_PRESCALE_EXT_T1_RISE
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_SetPrescaler(u8 prescale) {
	TCCR3B = (TCCR3B & ~TIMER3_PRESCALE_MASK) | prescale;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Captura o prescaler do timer 3
// Parametros:	Nenhum
// Retorna:		O prescaler do timer3
// -------------------------------------------------------------------------------------------------------------------
u16 timer3_GetPrescaler(void) {
	return (pgm_read_word(timer3_prescale_factor+(TCCR3B & TIMER3_PRESCALE_MASK)));
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa os contador do TIMER 3
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
inline void timer3_ClearCount(void) {
	TCNT3 = 0;
	timer3_count_overflow = 0;	
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador do TIMER 3
// Parametros:	Nenhum
// Retorna:		Valor do contador do TIMER 3
// ----------------------------------------------------------------------------------------------------------------
inline u16 timer3_GetCount(void) {
	return TCNT3;
}

// ################################################################################################################
// VETOR DE INTERRUP��O
// ################################################################################################################
// ################################################################################################################
// ANEXA OU RETIRA FUN��ES AO VETOR DE INTERRUP��ES

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Anexa uma routina ao vetor de interrup�ao
// Parametros:	Indece da interrup�ao
//					TIMER3_VETOR_INT_OVERFLOW ou TIMER3_VETOR_INT_COMPARE_A ou TIMER3_VETOR_INT_COMPARE_A ou
//						TIMER3_VETOR_INT_COMPARE_C ou TIMER3_VETOR_INT_CAPTURE
//				Ponteiro da routina. Exemplo:
//					Timer_Attach(TIMER3_VETOR_INT_OVERFLOW, myOverflowFunction);
//					Timer_DelIntHandler(TIMER3_VETOR_INT_OVERFLOW)
//					void myOverflowFunction(void) { ... }
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_SetIntHandler(u8 interruptNum, void (*userFunc)(void) )	{
	if(interruptNum < TIMER3_NUM_INTERRUPTS) {
		timer3_IntFunctions[interruptNum] = userFunc; 
	}
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retira a routina da inetrrup�ao desejada do vetor de interrup�ao
// Parametros:	Indece da interrup�ao
//					TIMER3_VETOR_INT_OVERFLOW ou TIMER3_VETOR_INT_COMPARE_A ou TIMER3_VETOR_INT_COMPARE_A ou
//						TIMER3_VETOR_INT_COMPARE_C ou TIMER3_VETOR_INT_CAPTURE
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_ClearIntHandler(u8 interruptNum) {
	if(interruptNum < TIMER3_NUM_INTERRUPTS) {
		timer3_IntFunctions[interruptNum] = 0;
	}
}

// ################################################################################################################
// TIMER 3 OVERFLOW
// ################################################################################################################
// ################################################################################################################
// EMITE UM INTERRUP��O QUANDO O CONTADOR DO TIMER ESTOURAR

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador do timer 0 junto com os overflows 
// Parametros:	Nenhum
// Retorna:		Valor do contador do timer 3
// ----------------------------------------------------------------------------------------------------------------
// OBS (INT DO OVERFLOW DEVE ESTAR ACIONADA)
inline u32 timer3_GetCountAll(void) {
	//register u16 T3 = TCNT3;
	//return ((u32)timer3_count_overflow * 0xFFFF) + (u32)T3;
	return (u32)TCNT3 + ((u32)timer3_count_overflow * 0xFFFF);
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o valor do contador de overflow do timer 3
// Parametros:	Nenhum
// Retorna:		Valor do contador de overflow do timer 3
// ----------------------------------------------------------------------------------------------------------------
u16 timer3_GetOverflowCount(void) {
	return timer3_count_overflow;
}


// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de overflow do TIMER 3
// ----------------------------------------------------------------------------------------------------------------
TIMER3_INTERRUPT_HANDLER(TIMER3_OVF_vect) {
	timer3_count_overflow++;					// Incrementa overflow
	
	//Se a fun��o est� definida ent�o execute
	if(timer3_IntFunctions[TIMER3_VETOR_INT_OVERFLOW])
		timer3_IntFunctions[TIMER3_VETOR_INT_OVERFLOW]();
}


// ################################################################################################################
// TIMER 3 COMPARE
// ################################################################################################################
// ################################################################################################################
// EMITE UM INTERRUP��O QUANDO O CONTADOR DO TIMER � IGUAL AO REGISTRADOR DE COMPARA��O OCR

#if (TIMER3_USE_COMPARE == pdON)
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	
// Parametros:	TIMER3_OC3A_OFF, TIMER3_OC3A_CHANGE, TIMER3_OC3A_NORMAL, TIMER3_OC3A_INV
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_SetComparePortOC3A(u8 StsOC3A) {
	sbi(TIMER3_OC3A_DDR, TIMER3_OC3A_PIN);// Configure o pino de compara��o para saida
	TCCR3A = (TCCR3A & ~TIMER3_OC3A_MASK) | StsOC3A;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	
// Parametros:	TIMER3_OC3B_OFF, TIMER3_OC3B_CHANGE, TIMER3_OC3B_NORMAL, TIMER3_OC3B_INV
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_SetComparePortOC3B(u8 StsOC3B) {
	// CONFIGURE O PINO DE COMPARA��O PARA SAIDA
	sbi(TIMER3_OC3B_DDR, TIMER3_OC3B_PIN);
	TCCR3A = (TCCR3A & ~TIMER3_OC3B_MASK) | StsOC3B;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	
// Parametros:	TIMER3_OC3C_OFF, TIMER3_OC3C_CHANGE, TIMER3_OC3C_NORMAL, TIMER3_OC3C_INV
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_SetComparePortOC3C(u8 StsOC3C) {
	// Configure o pino de compara��o para saida
	sbi(TIMER3_OC3C_DDR, TIMER3_OC3C_PIN);
	TCCR3A = (TCCR3A & ~TIMER3_OC3C_MASK) | StsOC3C;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Configura o valor de compara��o A do TIMER 3
// Parametros:	Valor para compara��o
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_SetCompareA (u16 Value) {
	OCR3A = Value;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Configura o valor de compara��o B do TIMER 3
// Parametros:	Valor para compara��o
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_SetCompareB (u16 Value) {
	OCR3B = Value;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa contador quando a compara��o for igual
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_SetCompareClearOnMatch(void) {
	TCCR3B |= (1<<WGM32);
}


// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Configura o valor de compara��o C do TIMER 3
// Parametros:	Valor para compara��o
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_SetCompareC (u16 Value) {
	OCR3C = Value;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de compara��o do OC3A do TIMER 3. A int � geranda quando o Contador1 = OCR3A
// ----------------------------------------------------------------------------------------------------------------
TIMER3_INTERRUPT_HANDLER(TIMER3_COMPA_vect) {
	//Se a fun��o est� definida ent�o execute
	if(timer3_IntFunctions[TIMER3_VETOR_INT_COMPARE_A])
		timer3_IntFunctions[TIMER3_VETOR_INT_COMPARE_A]();
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de compara��o do OC3B do TIMER 3. A int � geranda quando o Contador1 = OCR3B
// ----------------------------------------------------------------------------------------------------------------
TIMER3_INTERRUPT_HANDLER(TIMER3_COMPB_vect) {
	//Se a fun��o est� definida ent�o execute
	if(timer3_IntFunctions[TIMER3_VETOR_INT_COMPARE_B])
		timer3_IntFunctions[TIMER3_VETOR_INT_COMPARE_B]();
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de compara��o do OC3C do TIMER 3. A int � geranda quando o Contador1 = OCR3C
// ----------------------------------------------------------------------------------------------------------------
TIMER3_INTERRUPT_HANDLER(TIMER3_COMPC_vect) {
	//Se a fun��o est� definida ent�o execute
	if(timer3_IntFunctions[TIMER3_VETOR_INT_COMPARE_C])
		timer3_IntFunctions[TIMER3_VETOR_INT_COMPARE_C]();
}

#endif

// ################################################################################################################
// TIMER 3 CAPTURE IC3
// ################################################################################################################
// ################################################################################################################
// CAPTURA O VALOR DO TIMER3 PARA O REGISTRADOR ICR3 QUANDO HOUVER MUDAN�A DO ESTADO DO PINO ICP
#if (TIMER3_USE_ICP == pdON)

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Configura o modo de trabalho do IC
// Parametros:	Configura��o de captura 
//				( TIMER3_ICP_NOISE_CANCELER_ON ou TIMER3_ICP_NOISE_CANCELER_OFF) |
//				( TIMER3_ICP_FALLING_EDGE ou TIMER3_ICP_RISING_EDGE)
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_IcpInit(u8 Config) {
	TCCR3B = (TCCR3B & ~TIMER3_ICP_MASK) | Config;
	cbi(TIMER3_ICP_DDR, TIMER3_ICP_PIN);
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna com o valor capturado do timer3 no momento da mudan�a do pino ICP
// Parametros:	Nenhum
// Retorna:		Valor do TIMER 3
// ----------------------------------------------------------------------------------------------------------------
u16 timer3_IcpGetValue(void) {
	return ICR3;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de interrup��o de captura IC3 do timer3
// ----------------------------------------------------------------------------------------------------------------
TIMER3_INTERRUPT_HANDLER(TIMER3_CAPT_vect) {
	//Se a fun��o est� definida ent�o execute
	if(timer3_IntFunctions[TIMER3_VETOR_INT_CAPTURE])
		timer3_IntFunctions[TIMER3_VETOR_INT_CAPTURE]();
}

#endif


// ################################################################################################################
// TIMER 3 PWM
// ################################################################################################################
// ################################################################################################################
#if (TIMER3_USE_PWM == pdON)
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa o timer3 para o PWM. Este utiliza os pinos OC3A, OC3B e OC3C
// Parametros:	Bit de resolu��o do PWM 
//					TIMER3_PWM_MODE_8BITS ou TIMER3_PWM_MODE_9BITS ou TIMER3_PWM_MODE_10BITS
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmInit(u8 bit_res) {
	// QUANTO MAIOR O BIT, MAIOR SER� CONTAGEM E MAIOR SER� O TEMPO
	timer3_pwm_res = bit_res;
	
	if (bit_res == TIMER3_PWM_MODE_9BITS) { 			// Modo de 9 bits
		sbi(TCCR3A, WGM31);		
		cbi(TCCR3A, WGM30);
	} else if( bit_res == TIMER3_PWM_MODE_10BITS) {	// Modo de 10 bits
		sbi(TCCR3A, WGM31);		
		sbi(TCCR3A, WGM30);
	} else { 										// Modo de 8 bits
		cbi(TCCR3A, WGM31);		
		sbi(TCCR3A, WGM30);
	}

	OCR3A = 0; // Limpa o comparador de saida A
	OCR3B = 0; // Limpa o comparador de saida B
	OCR3C = 0; // Limpa o comparador de saida C
}

/*
void timer3_PwmInitICR(u16 TopCount) {
	// set PWM mode with ICR top-count
	cbi(TCCR3A,WGM30);
	sbi(TCCR3A,WGM31);
	sbi(TCCR3B,WGM32);
	sbi(TCCR3B,WGM33);
	
	ICR3 = TopCount;
	
	OCR3A = 0; // Limpa o comparador de saida A
	OCR3B = 0; // Limpa o comparador de saida B
	OCR3C = 0; // Limpa o comparador de saida C
}
*/
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desliga o PWM do TIMER 3
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmOff(void) {
	// Desliga opera��o de PWM no TIMER 3
	cbi(TCCR3A, WGM31);
	cbi(TCCR3A, WGM30);
	
	timer3_PwmAOff();
	timer3_PwmBOff();
	timer3_PwmCOff();
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Conecta a fun��o PWM do TIMER 3 no pino OC3A de modo invertido ou n�o
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmAOn(void) {
	#if (TIMER3_USE_PWM_OC1A_INV == pdON)
		// Pino OC3A invertido
		sbi(TCCR3A, COM3A1);	
		sbi(TCCR3A, COM3A0);
	#else	
		// Pino OC3A n�o invertido
		sbi(TCCR3A, COM3A1);
		cbi(TCCR3A, COM3A0);
	#endif
	
	sbi(TIMER3_OC3A_DDR, TIMER3_OC3A_PIN);	// Configure o pino do PWM para saida
}


// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Conecta a fun��o PWM do TIMER 3 no pino OC3B de modo invertido ou n�o
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmBOn(void) {	
	#if (TIMER3_USE_PWM_OC1B_INV == pdON)
		// Pino OC3B invertido 
		sbi(TCCR3A,COM3B1);	
		sbi(TCCR3A,COM3B0);
	#else
		// Pino OC3B n�o invertido
		sbi(TCCR3A,COM3B1);	
		cbi(TCCR3A,COM3B0);
	#endif
	
	sbi(TIMER3_OC3B_DDR, TIMER3_OC3B_PIN);// Configure o pino do PWM para saida
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Conecta a fun��o PWM do TIMER 3 no pino OC3C de modo invertido ou n�o
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmCOn(void) {
	#if (TIMER3_USE_PWM_OC1C_INV == pdON)
		// Pino OC3C invertido 
		sbi(TCCR3A,COM3C1);	
		sbi(TCCR3A,COM3C0);
	#else
		// Pino OC3C n�o invertido
		sbi(TCCR3A,COM3C1);	
		cbi(TCCR3A,COM3C0);
	#endif
		
	sbi(TIMER3_OC3C_DDR, TIMER3_OC3C_PIN);// Configure o pino do PWM para saida
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desconecta a fun��o PWM do pino OC3A
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmAOff(void) {
	cbi(TCCR3A,COM3A1);	
	cbi(TCCR3A,COM3A0);
	cbi(TIMER3_OC3A_DDR, TIMER3_OC3A_PIN);	// Configure o pino do PWM para entrada (Padr�o no reset)
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desconecta a fun��o PWM do pino OC3B
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmBOff(void) {
	cbi(TCCR3A,COM3B1);	
	cbi(TCCR3A,COM3B0);
	
	cbi(TIMER3_OC3B_DDR, TIMER3_OC3B_PIN);// Configure o pino do PWM para entrada (Padr�o no reset)
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Desconecta a fun��o PWM do pino OC3C
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmCOff(void) {
	cbi(TCCR3A,COM3C1);	
	cbi(TCCR3A,COM3C0);
	
	cbi(TIMER3_OC3C_DDR, TIMER3_OC3C_PIN);// Configure o pino do PWM para entrada (Padr�o no reset)
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta o valor em porcentagem de compara��o do PWM OC3A referente ao ciclo alto
// Parametros:	Valor da permanencia do ciclo alto. Valo de 1 a 100%. Valor 0% � desligado
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmASet(u8 pwmDuty) {
	u16 Value;
	
	Value = (1<<timer3_pwm_res)-1;
	if (pwmDuty < 100)
		Value = (pwmDuty*(Value/100));
	
	OCR3A = Value;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta o valor em porcentagem de compara��o do PWM OC3B referente ao ciclo alto
// Parametros:	Valor da permanencia do ciclo alto. Valo de 0 a 100%
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmBSet(u8 pwmDuty) {
	u32 Value;
			
	Value = (1<<timer3_pwm_res)-1;
	if (pwmDuty < 100)
		Value = (pwmDuty*(Value/100));
	
	OCR3B = Value;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta o valor em porcentagem de compara��o do PWM OC3C referente ao ciclo alto
// Parametros:	Valor da permanencia do ciclo alto. Valo de 0 a 100%
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void timer3_PwmCSet(u8 pwmDuty) {
	u32 Value;
			
	Value = (1<<timer3_pwm_res)-1;
	if (pwmDuty < 100)
		Value = (pwmDuty*(Value/100));
	
	OCR3C = Value;
}

#endif

