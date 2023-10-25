#include "eint0.h"
#if defined(cortexm3)
#include "arm.h"
#endif

typedef void (*eint0_pfunc)(void);
static volatile eint0_pfunc  eint0_int_func;
void eint0_ISR(void);

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa o pino da interrupção, o modo de sensibilidade e o vetor da interrupção externa
// Parametros:	Modo de interrupção
// 					EXTMODE_LEVEL - Sensivel ao nivel do sinal
// 					EXTMODE_EDGE  - Sensivel a borda do sinal
//				Tipo de sensibilidade
// 					EXTPOLAR_LOW  - Sensivel ao nivel baixo ou na queda do sinal
// 					EXTPOLAR_HIGH - Sensivel ao nivel alto ou na subida do sinal
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna código de erro
// ----------------------------------------------------------------------------------------------------------------
int eint0_Init(u8 mode, u8 polar) {	
	#if defined(arm7tdmi)
	EINT0_PINSEL = (EINT0_PINSEL & ~EINT0_PINMASK) | EINT0_PINS; // Seleciona o pino externo para a função de interrupção externa
	#else
	ENTI0_ConfigPort();
	#endif
		
	eint0_int_func = 0;	
	eint0_SetMode(mode);
	eint0_SetPolar(polar);
	EXTINT |= EINT0;											// Limpa INT
	
	#if defined(arm7tdmi)
	if (!irq_Install(VIC_EINT0, eint0_ISR, EINT0_PRIO_LEVEL))
		 return errIRQ_INSTALL;
	#endif
	
	#if defined (cortexm3)
	nvic_enableIRQ(VIC_EINT0);
	nvic_setPriority(VIC_EINT0, EINT0_PRIO_LEVEL);
	#endif
	
	return pdPASS;
}


// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Anexa uma routina do usuário ao vetor de interrupção
// Parametros:	Ponteiro da função
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void eint0_SetIntHandler(void (*func)(void) ) {
	eint0_int_func = func;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retira a routina do usuário do vetor de interrupçao
// Parametro:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void eint0_ClearIntHandler() {
	eint0_int_func = 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta se a interrupção será aceita pela nivel ou borda do sinal
// Parametro:	Modo de interrupção
// 					EXTMODE_LEVEL - Sensivel ao nivel do sinal
// 					EXTMODE_EDGE  - Sensivel a borda do sinal
// Retorna nada
//------------------------------------------------------------------------------------------------------------------
#if (EINT0_USE_FUNC_INLINE == pdON)
inline void eint0_SetMode(u8 mode) {
#else
void eint0_SetMode(u8 mode) {
#endif


	irq_Disable();												// Desabilita a interrupção global
	
	if (mode)		EXTMODE |= EINT0;
	else			EXTMODE &= ~EINT0;
	
	irq_Enable();												// Restaura a interrupção global
}
	
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta a polaridade para aceitar a interrupção
// Parametros:	Tipo de sensibilidade
// 					EXTPOLAR_LOW  - Sensivel ao nivel baixo ou na queda do sinal
// 					EXTPOLAR_HIGH - Sensivel ao nivel alto ou na subida do sinal
// Retorna nada
//------------------------------------------------------------------------------------------------------------------
#if (EINT0_USE_FUNC_INLINE == pdON)
inline void eint0_SetPolar(u8 polar) {
#else
void eint0_SetPolar(u8 polar) {
#endif
	irq_Disable();												// Desabilita a interrupção global

	if (polar)		EXTPOLAR |= EINT0;
	
	// NÃO ESTA FUNCIONANDO EM COLOCAR O VALOR 0 NO EXTPOLAR. O MICRO CONTROLADOR FICA TRAVADO
	// JÁ QUE ESTAMOS USANDO SOMENTE NO INICIO DO PROGRAMA E O VALOR DESEJADO É 0, DEIXAREMOS COM O VALOR PADRÃO APÓS O RESET, QUE É 0
	// CASO PRECISE USAR NO MEIO DO PROGRAMA É PRECISO PESQUISAR PORQUE O MICRO CONTROLADOR FICA TRAVADO NO INICIO
	else			EXTPOLAR &= ~EINT0; 
		
	irq_Enable();												// Restaura a interrupção global
}

// ################################################################################################################
// SERVIÇOS DE INTERRUPÇÕES IRQ
// ################################################################################################################
// ################################################################################################################
void eint0_ISR (void) {
    ISR_ENTRY;																	// Procedimento para entrada da interrupção
    EXTINT |= EINT0;															// Limpa flag da interrupção
	       
    if (eint0_int_func)															// Checa se existe alguma função anexada
    	eint0_int_func();														// Executa a função que foi anexada a essa interrupção
     
    // Limpa interrupção
    #if defined(arm7tdmi)
    VICSoftIntClr = (1<<VIC_EINT0);												
    #endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_EINT0);
	#endif

    ISR_EXIT;																	// Procedimento para saida da interrupção
}

/*
// ################################################################################################################
// SERVIÇOS DE INTERRUPÇÕES FIQ
// ################################################################################################################
// ################################################################################################################

void EINT_FIQ_ISR (void) {

	// ISR_ENTRY;																	// Procedimento para entrada da interrupção. Útil para interrupção NESTED
	// LIMPA FLAG DE INTERRUPÇÃO
	#ifdef EINT_USE_INT0
    	EXTINT = EINT0;																// Limpa flag da interrupção
	#endif
	
	#ifdef EINT_USE_INT1
	    EXTINT = EINT1;																// Limpa flag da interrupção
	#endif
	
	#ifdef EINT_USE_INT2
    	EXTINT = EINT2;																// Limpa flag da interrupção
	#endif
	
	#ifdef EINT_USE_INT3
    	EXTINT = EINT3;																// Limpa flag da interrupção
	#endif
	

    
    #ifdef EINT_USE_INT0
    	if (eint0_int_func)															// Checa se existe alguma função anexada
    		eint0_int_func();														// Executa a função que foi anexada a essa interrupção
    #endif
    #ifdef EINT_USE_INT1
    	if (eint1_int_func)															// Checa se existe alguma função anexada
    		eint1_int_func();														// Executa a função que foi anexada a essa interrupção
    #endif
    #ifdef EINT_USE_INT2
    	if (eint2_int_func)															// Checa se existe alguma função anexada
    		eint2_int_func();														// Executa a função que foi anexada a essa interrupção
    #endif
    #ifdef EINT_USE_INT3
    	if (eint3_int_func)															// Checa se existe alguma função anexada
    		eint3_int_func();														// Executa a função que foi anexada a essa interrupção
    #endif

   //ISR_EXIT;																		// Procedimento para saida da interrupção. Útil para interrupção NESTED
    //VICVectAddr = 0;             													// Limpa o registrador de endereço do ISR, para novas interrupções
  	ISR_EXIT;
}
*/
