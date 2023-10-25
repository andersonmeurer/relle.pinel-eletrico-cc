// TODO ainda n�o testado ap�s muda�as no dia 16-10-2013

#include "adc0.h" 

#if (AD0_USE_INT_FUNCTIONS == pdON)
	typedef void(*pFunc)(u16);
	static volatile pFunc adc0_IntFunc;
#endif

static volatile n8 adc0_completed;
	
// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incializa os registradores de convers�o AD
// Parametros: 	Prescale definie o clock para convers�o, quanto maior o clock mais preciso ser� a convers�o
// 					AD0_PRESCALE_DIV(2, 4, 8, 16, 32, 64, 128)
//				Referencia para convers�o
//					AD0_REFERENCE_AREF ou AD0_REFERENCE_AVCC ou AD0_REFERENCE_256V
//				Tipo de dados lidos do ADC
//					AD0_CONVERT_RIGHT ou AD0_CONVERT_LEFT
// Retorna: 	Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_Init(u8 prescale, u8 reference, u8 result) {
	#ifdef ADLAR							// at90s8535 n�o suporta
	if (result) sbi(ADMUX, ADLAR);		// Resultado da convers�o ser� justificado a esquerda
	else		cbi(ADMUX, ADLAR);		// Resultado da convers�o ser� justificado a direita
	#endif
	
	adc0_SetPrescaler(prescale);				// Ajustar o prescaler do clock de convers�o
	adc0_SetReference(reference);			// Ajustar a referencia do adc externa ao VCC
	
	#if (AD0_USE_INT_FUNCTIONS == pdON) 	// Se esta usando fun��es para interrup��o
	adc0_ClearIntHandler();				// Tirar qualquer indere�o de fun��o
	#endif
	
	adc0_completed = pdFALSE;				// Limpa o flag de conver��o completa
	sbi(ADCSRA, ADEN);						// Habilita ADC
	sbi(ADCSRA, ADIE);						// Habilita int
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: Desliga o conversor AD
// Parametros: Nenhum
// Retorna: Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_Off(void)	{
	cbi(ADCSRA, ADIE);				// Desabilita int
	cbi(ADCSRA, ADEN);				// Desabilita int ADC
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: Ajusta o tempo de convers�o do AD
// Parametros: 	Prescale definie o clock para convers�o
// 					AD0_PRESCALE_DIV(2, 4, 8, 16, 32, 64, 128)
// Retorna: Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_SetPrescaler(u8 prescale)	{
	ADCSRA = ((ADCSRA & ~AD0_PRESCALE_MASK) | prescale);
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: Ajusta o tipo de tens�o de referencia para a conver��o
// Parametros: 	Referencia para convers�o
//					AD0_REFERENCE_AREF ou AD0_REFERENCE_AVCC ou AD0_REFERENCE_256V
// Retorna: Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_SetReference(u8 ref) {
	ADMUX =  ((ADMUX & ~AD0_REFERENCE_MASK) | (ref<<6));
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: Ajusta o canal a ser convertido
// Parametros: O canal a ser convertido
//				AD0_CHANNEL(1,2 ... 7)
// Retorna: Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_SetChannel(u8 ch)	{
	ADMUX = (ADMUX & ~AD0_MUX_MASK) | (ch & AD0_MUX_MASK);
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: Inicializa a convers�o do canal desejado
// Parametros: O canal a ser convertido
//				AD0_CHANNEL(1,2 ... 7)
// Retorna: Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_Start(u8 Channel) {
	adc0_SetChannel(Channel);			// Ajustar o canal para convers�o
	adc0_completed = pdFALSE;				// Limpo o flag de conver��o completa
	sbi(ADCSRA, ADIF);					// Limpo o flag de hardware de conver��o completa
	sbi(ADCSRA, ADSC);					// Inicio conver��o 
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Checa se a convers�o do canal ADC desejado j� foi feita. Retorna TRUE se foi feita
// Parametro:	value: Ponteiro da vari�vel para retorno do valor adc convertido
// Retorna:		Retorna pdPASS se o canal desejado j� foi convertido, sen�o roda pdFAIL
// -------------------------------------------------------------------------------------------------------------------
int adc0_ReadNoWait(u16 *value) {
	*value = ADC;
	return adc0_completed;
}

//// -----------------------------------------------------------------------------------------------------------------
//// Descri��o: Verifica se o convers�o foi concluida
//// Parametros: Nenhum
//// Retorna: True sea convers�o foi concluida sen�o retorna falso
//// -----------------------------------------------------------------------------------------------------------------
//int adc0_IsComplete(void) {
//	return adc0_completed;
//	//return bit_is_set(ADCSRA, ADSC);
//}
//
//// -----------------------------------------------------------------------------------------------------------------
//// Descri��o: Le o valor de 10bits do canal convertido
//// Parametros: nenhum
//// Retorna: o valor lido
//// -----------------------------------------------------------------------------------------------------------------
//u16 adc0_GetValue10bits(void) {
//	return ADC;
//}
//
//// -----------------------------------------------------------------------------------------------------------------
//// Descri��o: Le o valor de 8bits do canal convertido
//// Parametros: nenhum
//// Retorna: o valor lido
//// -----------------------------------------------------------------------------------------------------------------
//u8 adc0_GetValue8bits(void)	{
//	return (u8) (ADC >> 2);
//}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: Interrup��o de fim de convers�o
// Parametros:
// Retorna:
// -----------------------------------------------------------------------------------------------------------------
AD0_INTERRUPT_HANDLER(ADC_vect)	{
	sbi(ADCSRA, ADIF);					// Limpo o flag de hardware de conver��o completa
	
	#if (AD0_USE_INT_FUNCTIONS == pdON)
		if(adc0_IntFunc)					// Checo se existe uma fun��o anexada ao Handler de int
			adc0_IntFunc(ADC);			// Chama a fun��o anexada ao Handler de int
	#endif
	
	adc0_completed = pdTRUE;
}


#if (AD0_USE_INT_FUNCTIONS == pdON)
// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retira as fun��es do Handler de interrup��es
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_ClearIntHandler(void) {
	adc0_IntFunc = 0;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Anexa a fun��o ao handler de interrup�ao do fim de convers�o do ADC
// Parametros:	Fun��o do tipo int Func(uchar Data)
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_SetIntHandler(void(*Func)(u16)) {
	adc0_IntFunc = Func;
}

#endif

/*
//! Perform a 10-bit conversion
// starts conversion, waits until conversion is done, and returns result
u16 adc_Convert10bits(u8 ch)	{
	adc0_completed = cFalse;				// clear conversion complete flag
	outp((inp(ADMUX) & ~ADC_MUX_MASK) | (ch & ADC_MUX_MASK), ADMUX);	// set channel
	sbi(ADCSRA, ADIF);						// clear hardware "conversion complete" flag 
	sbi(ADCSRA, ADSC);						// start conversion
	//while(!adc0_completed);				// wait until conversion complete
	//while( bit_is_clear(ADCSRA, ADIF) );		// wait until conversion complete
	while( bit_is_set(ADCSRA, ADSC) );		// wait until conversion complete

	// CAUTION: MUST READ ADCL BEFORE ADCH!!!
	return (inp(ADCL) | (inp(ADCH)<<8));	// read ADC (full 10 bits);
}

//! Perform a 8-bit conversion.
// starts conversion, waits until conversion is done, and returns result
u8 a2dConvert8bit(u8 ch)
{
	// do 10-bit conversion and return highest 8 bits
	return a2dConvert10bit(ch)>>2;			// return ADC MSB byte
}
*/
