/*
	Esta biblioteca trabalha n�o no modo BURTS, ou seja, 
	� feito manualmente a convers�o de cada canal desejada.
	No BUSRT, a CPU converte todos os canais automaticamente
	
	Tamb�m n�o foi usado a interrup��o para as convers�es
	
	Atualizado em 05/04/2012
*/

#include "adc0.h"
#if (ADC0_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incialize ADC0. Somente os 4 canais para o LPC22xx e LPC24xx
// Parametros:	frequencia de clock para convers�o: 
//					M�xima 13Mhz para o cortexm3 e 4500000 para os demais
//					Deve ser multiplo inteiro de PCLK, ou seja, PCLK/adc_clk = valor inteiro
//				bit de resolu��o:
//					AD0_CLKS_[10, 9, 8, 7, 6, 5, 4, 3]BIT (para cortexm3 � sempre 0)
// Retorna:		nada
// -------------------------------------------------------------------------------------------------------------------
void adc0_Init(u32 adc_clk, u32 bit) {
    PCONP |= PCAD;										// Liga o ADC no controle de potencia. Tem que ser antes da configura��o, pois ap�s ligar o perif�ricos, seus regs assumem valores padr�es
    
    #if defined (cortexm3)
		ADC0_ConfigPort();
		ADC0_DisablePulls();
		bit = 0;
    #else
    	// AD0.0 a AD0.4
    	AD0_PINSEL = (AD0_PINSEL & ~AD0_PINMASK) | AD0_PINS; // Seleciona o pino externo para a fun��o ADC
    #endif

    AD0CR = ( 0x01 << 0 ) 	|	// Canal 0 do ADC0
			( ( (int)(PCLK / adc_clk) - 1 ) << 8 ) |  			// CLKDIV = 0 divide por 1, CLKDIV = 1 divide por 2, CLKDIV = 3 divide por 3 ... at� 255
			( 0 << 16 ) 	| 	// BURST = 0, no BURST, software controlled
			( bit ) 		|	// Resolu��o (000 11 clocks/10 bits ...  111 4 clocks/3 bits)
			( 1 << 21 ) 	| 	// PDN = 1, normal operation
			( 0 << 22 ) 	|  	// RESERVADO
			( 0 << 24 ) 	| 	// START = 0 A/D conversion stops
			( 0 << 27 );		// EDGE = 0 (CAP/MAT singal falling, trigger A/D conversion)


	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Init AD0CR[0x%x]=0x%x"CMD_TERMINATOR, &AD0CR, AD0CR);
	#endif

//	#ifdef ADC_INT_MODE
	//	if (!irq_Install(VIC_ADC, adc_ISR, priority))
		//	 return pdFALSE;
	   	//else return pdPASS;
	//#endif
}


// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incialize convers�o do ADC no modo n�o BURST
// Parametros:	Canal a ser convertido
//					AD0_CHANNEL[0..7]
//				Gatilho para iniciar a convers�o
//					AD0_START_NO 		 Somente no modo n�o BUSRT. Conversor parado
//					AD0_START_NOW	 Somente no modo n�o BUSRT. Incializa convers�o agora
//					AD0_START_MAT0_0 	 Somente no modo n�o BUSRT. Incializa convers�o pela mudan�a da borda do pino MAT0.0
//					AD0_START_MAT0_1 	 Somente no modo n�o BUSRT. Incializa convers�o pela mudan�a da borda do pino MAT0.1
//					AD0_START_MAT0_2 	 Somente no modo n�o BUSRT. Incializa convers�o pela mudan�a da borda do pino MAT0.2
//					AD0_START_MAT0_3 	 Somente no modo n�o BUSRT. Incializa convers�o pela mudan�a da borda do pino MAT0.3
//					AD0_START_MAT1_0 	 Somente no modo n�o BUSRT. Incializa convers�o pela mudan�a da borda do pino MAT1.0
//					AD0_START_MAT1_1 	 Somente no modo n�o BUSRT. Incializa convers�o pela mudan�a da borda do pino MAT1.1  	
//				Borda do gatilho (somente se usar gatinho MAT), se n�o usar coloca valor 0
//					AD0_EDGE_FALLING 	 Come�a a captura na borda de descrida do gatilho
//					AD0_EDGE_RISING		 Come�a a captura na borda de subida do gatilho


// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void adc0_Start(u8 channel, u32 start, u32 edge) {
    AD0CR &= ~(AD0_CHANNEL_MASK | AD0_START_MASK | AD0_EDGE_MASK);	// Limpa os bits de confidura��es
    AD0CR |= (edge | start | (1<<channel));							// Muda o canal ADC, configura a borda para incio de convers�ao e incializa a convers�o
	
	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Start ADC channel[%d] AD0CR[0x%x] = 0x%x"CMD_TERMINATOR, channel, &AD0CR, AD0CR);
	#endif
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Para a convers�o do ADC
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void adc0_Stop(void) {
	AD0CR &= ~AD0_START_MASK;										// Para a convers�o ADC

	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Stop. AD0CR = 0x%x"CMD_TERMINATOR, AD0CR);
	#endif
}

#if defined(LPC2468) || defined (cortexm3)
// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Espera e l� a convers�o no canal ADC desejado
// Parametros:	channel: Canal a ser convertido
//					ADC_CHANNEL[0..7]
//				value: Ponteiro da vari�vel para retorno do valor adc convertido
// Retorna:		Sempre pdTRUE
// -------------------------------------------------------------------------------------------------------------------
int adc0_Read(u8 channel, u16 *value) {
	reg32 *addr_ch = (reg32 *)(AD0_BASE_ADDR + 0x10 + 4*channel);		// Capturo o endere�o do registrador relacionado ao canal

	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Read channel[%d] reg[0x%x]"CMD_TERMINATOR, channel, addr_ch); // N�O LEIA O REGISTRADOR DE DADOS CONVERTIDOS (*addr_ch), SEN�O VAI LIMPAR O STATUS
	#endif

	while(!(AD0STAT & (1<<channel)));							// Espero at� que a convers�o esteja completa 
	#if defined(cortexm3)
	*value =  (*addr_ch>>4) & 0xFFF;
	#else
	*value =  (*addr_ch>>6) & 0x3FF;
	#endif
	
	return pdPASS;	
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Checa se a convers�o do canal ADC desejado j� foi feita. Retorna TRUE se foi feita
// Parametros:	channel: Canal a ser convertido
//					ADC_CHANNEL[0..7]
//				value: Ponteiro da vari�vel para retorno do valor adc convertido
// Retorna:		Retorna pdPASS se o canal desejado j� foi convertido, sen�o roda pdFAIL
// -------------------------------------------------------------------------------------------------------------------
int adc0_ReadNoWait(u8 channel, u16 *value) {
	reg32 *addr_ch = (reg32 *)(AD0_BASE_ADDR + 0x10 + 4*channel);		// Capturo o endere�o do registrador relacionado ao canal

	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Read channel[%d] reg[0x%x] AD0STAT = 0x%x"CMD_TERMINATOR, channel, addr_ch, AD0STAT); // N�O LEIA O REGISTRADOR DE DADOS CONVERTIDOS (*addr_ch), SEN�O VAI LIMPAR O STATUS
	#endif

	//if (! (AD0STAT & (1<<channel) )) return pdFAIL; // retorna falso caso ainda convers�o n�o esteja completa
	if (! (*addr_ch & (0x80000000) )) return pdFAIL; // retorna falso caso ainda convers�o n�o esteja completa
	if ((*addr_ch & (0x40000000) )) return pdFAIL; // retorna falso caso

	#if defined(cortexm3)
	*value =  (*addr_ch>>4) & 0xFFF;
	#else
	*value =  (*addr_ch>>6) & 0x3FF;
	#endif

	return pdPASS;
}
	
#else

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Espera e l� a convers�o do canal ADC atual
// Parametro:	value: Ponteiro da vari�vel para retorno do valor adc convertido
// Retorna:		Sempre pdTRUE
// -------------------------------------------------------------------------------------------------------------------
int adc0_Read(u16 *value) {
	
	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Read"CMD_TERMINATOR);
	#endif

	while(!(AD0DR & AD0_DONE));							// Espero at� que a convers�o esteja completa 
	*value =  (AD0DR>>6) & 0x3FF;

	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Done"CMD_TERMINATOR);
	#endif

	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Checa se a convers�o do canal ADC desejado j� foi feita. Retorna TRUE se foi feita
// Parametros:	channel: Canal a ser convertido
//					ADC_CHANNEL[0..7]
//				value: Ponteiro da vari�vel para retorno do valor adc convertido
// Retorna:		Retorna pdTRUE se o canal desejado j� foi convertido, sen�o roda pdFAIL
// -------------------------------------------------------------------------------------------------------------------
int adc0_ReadNoWait(u16 *value) {

	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Read"CMD_TERMINATOR);
	#endif

	if (!(AD0DR & AD0_DONE)) return pdFAIL; // retorna falso caso ainda convers�o n�o esteja completa
	*value =  (AD0DR>>6) & 0x3FF;

	return pdPASS;
}

#endif

