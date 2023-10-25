/*
	Esta biblioteca trabalha não no modo BURTS, ou seja, 
	é feito manualmente a conversão de cada canal desejada.
	No BUSRT, a CPU converte todos os canais automaticamente
	
	Também não foi usado a interrupção para as conversões
	
	Atualizado em 05/04/2012
*/

#include "adc0.h"
#if (ADC0_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incialize ADC0. Somente os 4 canais para o LPC22xx e LPC24xx
// Parametros:	frequencia de clock para conversão: 
//					Máxima 13Mhz para o cortexm3 e 4500000 para os demais
//					Deve ser multiplo inteiro de PCLK, ou seja, PCLK/adc_clk = valor inteiro
//				bit de resolução:
//					AD0_CLKS_[10, 9, 8, 7, 6, 5, 4, 3]BIT (para cortexm3 é sempre 0)
// Retorna:		nada
// -------------------------------------------------------------------------------------------------------------------
void adc0_Init(u32 adc_clk, u32 bit) {
    PCONP |= PCAD;										// Liga o ADC no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões
    
    #if defined (cortexm3)
		ADC0_ConfigPort();
		ADC0_DisablePulls();
		bit = 0;
    #else
    	// AD0.0 a AD0.4
    	AD0_PINSEL = (AD0_PINSEL & ~AD0_PINMASK) | AD0_PINS; // Seleciona o pino externo para a função ADC
    #endif

    AD0CR = ( 0x01 << 0 ) 	|	// Canal 0 do ADC0
			( ( (int)(PCLK / adc_clk) - 1 ) << 8 ) |  			// CLKDIV = 0 divide por 1, CLKDIV = 1 divide por 2, CLKDIV = 3 divide por 3 ... até 255
			( 0 << 16 ) 	| 	// BURST = 0, no BURST, software controlled
			( bit ) 		|	// Resolução (000 11 clocks/10 bits ...  111 4 clocks/3 bits)
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
// Descrição: 	Incialize conversão do ADC no modo não BURST
// Parametros:	Canal a ser convertido
//					AD0_CHANNEL[0..7]
//				Gatilho para iniciar a conversão
//					AD0_START_NO 		 Somente no modo não BUSRT. Conversor parado
//					AD0_START_NOW	 Somente no modo não BUSRT. Incializa conversão agora
//					AD0_START_MAT0_0 	 Somente no modo não BUSRT. Incializa conversão pela mudança da borda do pino MAT0.0
//					AD0_START_MAT0_1 	 Somente no modo não BUSRT. Incializa conversão pela mudança da borda do pino MAT0.1
//					AD0_START_MAT0_2 	 Somente no modo não BUSRT. Incializa conversão pela mudança da borda do pino MAT0.2
//					AD0_START_MAT0_3 	 Somente no modo não BUSRT. Incializa conversão pela mudança da borda do pino MAT0.3
//					AD0_START_MAT1_0 	 Somente no modo não BUSRT. Incializa conversão pela mudança da borda do pino MAT1.0
//					AD0_START_MAT1_1 	 Somente no modo não BUSRT. Incializa conversão pela mudança da borda do pino MAT1.1  	
//				Borda do gatilho (somente se usar gatinho MAT), se não usar coloca valor 0
//					AD0_EDGE_FALLING 	 Começa a captura na borda de descrida do gatilho
//					AD0_EDGE_RISING		 Começa a captura na borda de subida do gatilho


// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void adc0_Start(u8 channel, u32 start, u32 edge) {
    AD0CR &= ~(AD0_CHANNEL_MASK | AD0_START_MASK | AD0_EDGE_MASK);	// Limpa os bits de confidurações
    AD0CR |= (edge | start | (1<<channel));							// Muda o canal ADC, configura a borda para incio de conversçao e incializa a conversão
	
	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Start ADC channel[%d] AD0CR[0x%x] = 0x%x"CMD_TERMINATOR, channel, &AD0CR, AD0CR);
	#endif
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Para a conversão do ADC
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void adc0_Stop(void) {
	AD0CR &= ~AD0_START_MASK;										// Para a conversão ADC

	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Stop. AD0CR = 0x%x"CMD_TERMINATOR, AD0CR);
	#endif
}

#if defined(LPC2468) || defined (cortexm3)
// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Espera e lê a conversão no canal ADC desejado
// Parametros:	channel: Canal a ser convertido
//					ADC_CHANNEL[0..7]
//				value: Ponteiro da variável para retorno do valor adc convertido
// Retorna:		Sempre pdTRUE
// -------------------------------------------------------------------------------------------------------------------
int adc0_Read(u8 channel, u16 *value) {
	reg32 *addr_ch = (reg32 *)(AD0_BASE_ADDR + 0x10 + 4*channel);		// Capturo o endereço do registrador relacionado ao canal

	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Read channel[%d] reg[0x%x]"CMD_TERMINATOR, channel, addr_ch); // NÃO LEIA O REGISTRADOR DE DADOS CONVERTIDOS (*addr_ch), SENÃO VAI LIMPAR O STATUS
	#endif

	while(!(AD0STAT & (1<<channel)));							// Espero até que a conversão esteja completa 
	#if defined(cortexm3)
	*value =  (*addr_ch>>4) & 0xFFF;
	#else
	*value =  (*addr_ch>>6) & 0x3FF;
	#endif
	
	return pdPASS;	
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Checa se a conversão do canal ADC desejado já foi feita. Retorna TRUE se foi feita
// Parametros:	channel: Canal a ser convertido
//					ADC_CHANNEL[0..7]
//				value: Ponteiro da variável para retorno do valor adc convertido
// Retorna:		Retorna pdPASS se o canal desejado já foi convertido, senão roda pdFAIL
// -------------------------------------------------------------------------------------------------------------------
int adc0_ReadNoWait(u8 channel, u16 *value) {
	reg32 *addr_ch = (reg32 *)(AD0_BASE_ADDR + 0x10 + 4*channel);		// Capturo o endereço do registrador relacionado ao canal

	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Read channel[%d] reg[0x%x] AD0STAT = 0x%x"CMD_TERMINATOR, channel, addr_ch, AD0STAT); // NÃO LEIA O REGISTRADOR DE DADOS CONVERTIDOS (*addr_ch), SENÃO VAI LIMPAR O STATUS
	#endif

	//if (! (AD0STAT & (1<<channel) )) return pdFAIL; // retorna falso caso ainda conversão não esteja completa
	if (! (*addr_ch & (0x80000000) )) return pdFAIL; // retorna falso caso ainda conversão não esteja completa
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
// Descrição: 	Espera e lê a conversão do canal ADC atual
// Parametro:	value: Ponteiro da variável para retorno do valor adc convertido
// Retorna:		Sempre pdTRUE
// -------------------------------------------------------------------------------------------------------------------
int adc0_Read(u16 *value) {
	
	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Read"CMD_TERMINATOR);
	#endif

	while(!(AD0DR & AD0_DONE));							// Espero até que a conversão esteja completa 
	*value =  (AD0DR>>6) & 0x3FF;

	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Done"CMD_TERMINATOR);
	#endif

	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Checa se a conversão do canal ADC desejado já foi feita. Retorna TRUE se foi feita
// Parametros:	channel: Canal a ser convertido
//					ADC_CHANNEL[0..7]
//				value: Ponteiro da variável para retorno do valor adc convertido
// Retorna:		Retorna pdTRUE se o canal desejado já foi convertido, senão roda pdFAIL
// -------------------------------------------------------------------------------------------------------------------
int adc0_ReadNoWait(u16 *value) {

	#if (ADC0_USE_DEBUG == pdON)
	plog("AD: Read"CMD_TERMINATOR);
	#endif

	if (!(AD0DR & AD0_DONE)) return pdFAIL; // retorna falso caso ainda conversão não esteja completa
	*value =  (AD0DR>>6) & 0x3FF;

	return pdPASS;
}

#endif

