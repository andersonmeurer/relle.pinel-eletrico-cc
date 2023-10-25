// TODO ainda não testado após mudaças no dia 16-10-2013

#include "adc0.h" 

#if (AD0_USE_INT_FUNCTIONS == pdON)
	typedef void(*pFunc)(u16);
	static volatile pFunc adc0_IntFunc;
#endif

static volatile n8 adc0_completed;
	
// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa os registradores de conversão AD
// Parametros: 	Prescale definie o clock para conversão, quanto maior o clock mais preciso será a conversão
// 					AD0_PRESCALE_DIV(2, 4, 8, 16, 32, 64, 128)
//				Referencia para conversão
//					AD0_REFERENCE_AREF ou AD0_REFERENCE_AVCC ou AD0_REFERENCE_256V
//				Tipo de dados lidos do ADC
//					AD0_CONVERT_RIGHT ou AD0_CONVERT_LEFT
// Retorna: 	Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_Init(u8 prescale, u8 reference, u8 result) {
	#ifdef ADLAR							// at90s8535 não suporta
	if (result) sbi(ADMUX, ADLAR);		// Resultado da conversão será justificado a esquerda
	else		cbi(ADMUX, ADLAR);		// Resultado da conversão será justificado a direita
	#endif
	
	adc0_SetPrescaler(prescale);				// Ajustar o prescaler do clock de conversão
	adc0_SetReference(reference);			// Ajustar a referencia do adc externa ao VCC
	
	#if (AD0_USE_INT_FUNCTIONS == pdON) 	// Se esta usando funções para interrupção
	adc0_ClearIntHandler();				// Tirar qualquer indereço de função
	#endif
	
	adc0_completed = pdFALSE;				// Limpa o flag de converção completa
	sbi(ADCSRA, ADEN);						// Habilita ADC
	sbi(ADCSRA, ADIE);						// Habilita int
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: Desliga o conversor AD
// Parametros: Nenhum
// Retorna: Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_Off(void)	{
	cbi(ADCSRA, ADIE);				// Desabilita int
	cbi(ADCSRA, ADEN);				// Desabilita int ADC
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: Ajusta o tempo de conversão do AD
// Parametros: 	Prescale definie o clock para conversão
// 					AD0_PRESCALE_DIV(2, 4, 8, 16, 32, 64, 128)
// Retorna: Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_SetPrescaler(u8 prescale)	{
	ADCSRA = ((ADCSRA & ~AD0_PRESCALE_MASK) | prescale);
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: Ajusta o tipo de tensão de referencia para a converção
// Parametros: 	Referencia para conversão
//					AD0_REFERENCE_AREF ou AD0_REFERENCE_AVCC ou AD0_REFERENCE_256V
// Retorna: Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_SetReference(u8 ref) {
	ADMUX =  ((ADMUX & ~AD0_REFERENCE_MASK) | (ref<<6));
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: Ajusta o canal a ser convertido
// Parametros: O canal a ser convertido
//				AD0_CHANNEL(1,2 ... 7)
// Retorna: Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_SetChannel(u8 ch)	{
	ADMUX = (ADMUX & ~AD0_MUX_MASK) | (ch & AD0_MUX_MASK);
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: Inicializa a conversão do canal desejado
// Parametros: O canal a ser convertido
//				AD0_CHANNEL(1,2 ... 7)
// Retorna: Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_Start(u8 Channel) {
	adc0_SetChannel(Channel);			// Ajustar o canal para conversão
	adc0_completed = pdFALSE;				// Limpo o flag de converção completa
	sbi(ADCSRA, ADIF);					// Limpo o flag de hardware de converção completa
	sbi(ADCSRA, ADSC);					// Inicio converção 
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Checa se a conversão do canal ADC desejado já foi feita. Retorna TRUE se foi feita
// Parametro:	value: Ponteiro da variável para retorno do valor adc convertido
// Retorna:		Retorna pdPASS se o canal desejado já foi convertido, senão roda pdFAIL
// -------------------------------------------------------------------------------------------------------------------
int adc0_ReadNoWait(u16 *value) {
	*value = ADC;
	return adc0_completed;
}

//// -----------------------------------------------------------------------------------------------------------------
//// Descrição: Verifica se o conversão foi concluida
//// Parametros: Nenhum
//// Retorna: True sea conversão foi concluida senão retorna falso
//// -----------------------------------------------------------------------------------------------------------------
//int adc0_IsComplete(void) {
//	return adc0_completed;
//	//return bit_is_set(ADCSRA, ADSC);
//}
//
//// -----------------------------------------------------------------------------------------------------------------
//// Descrição: Le o valor de 10bits do canal convertido
//// Parametros: nenhum
//// Retorna: o valor lido
//// -----------------------------------------------------------------------------------------------------------------
//u16 adc0_GetValue10bits(void) {
//	return ADC;
//}
//
//// -----------------------------------------------------------------------------------------------------------------
//// Descrição: Le o valor de 8bits do canal convertido
//// Parametros: nenhum
//// Retorna: o valor lido
//// -----------------------------------------------------------------------------------------------------------------
//u8 adc0_GetValue8bits(void)	{
//	return (u8) (ADC >> 2);
//}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: Interrupção de fim de conversão
// Parametros:
// Retorna:
// -----------------------------------------------------------------------------------------------------------------
AD0_INTERRUPT_HANDLER(ADC_vect)	{
	sbi(ADCSRA, ADIF);					// Limpo o flag de hardware de converção completa
	
	#if (AD0_USE_INT_FUNCTIONS == pdON)
		if(adc0_IntFunc)					// Checo se existe uma função anexada ao Handler de int
			adc0_IntFunc(ADC);			// Chama a função anexada ao Handler de int
	#endif
	
	adc0_completed = pdTRUE;
}


#if (AD0_USE_INT_FUNCTIONS == pdON)
// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retira as funções do Handler de interrupções
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void adc0_ClearIntHandler(void) {
	adc0_IntFunc = 0;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Anexa a função ao handler de interrupçao do fim de conversão do ADC
// Parametros:	Função do tipo int Func(uchar Data)
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
