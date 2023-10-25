#include "eint0.h"
#if defined(cortexm3)
#include "arm.h"
#endif

typedef void (*eint0_pfunc)(void);
static volatile eint0_pfunc  eint0_int_func;
void eint0_ISR(void);

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializa o pino da interrup��o, o modo de sensibilidade e o vetor da interrup��o externa
// Parametros:	Modo de interrup��o
// 					EXTMODE_LEVEL - Sensivel ao nivel do sinal
// 					EXTMODE_EDGE  - Sensivel a borda do sinal
//				Tipo de sensibilidade
// 					EXTPOLAR_LOW  - Sensivel ao nivel baixo ou na queda do sinal
// 					EXTPOLAR_HIGH - Sensivel ao nivel alto ou na subida do sinal
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna c�digo de erro
// ----------------------------------------------------------------------------------------------------------------
int eint0_Init(u8 mode, u8 polar) {	
	#if defined(arm7tdmi)
	EINT0_PINSEL = (EINT0_PINSEL & ~EINT0_PINMASK) | EINT0_PINS; // Seleciona o pino externo para a fun��o de interrup��o externa
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
// Descri��o: 	Anexa uma routina do usu�rio ao vetor de interrup��o
// Parametros:	Ponteiro da fun��o
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void eint0_SetIntHandler(void (*func)(void) ) {
	eint0_int_func = func;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retira a routina do usu�rio do vetor de interrup�ao
// Parametro:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void eint0_ClearIntHandler() {
	eint0_int_func = 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta se a interrup��o ser� aceita pela nivel ou borda do sinal
// Parametro:	Modo de interrup��o
// 					EXTMODE_LEVEL - Sensivel ao nivel do sinal
// 					EXTMODE_EDGE  - Sensivel a borda do sinal
// Retorna nada
//------------------------------------------------------------------------------------------------------------------
#if (EINT0_USE_FUNC_INLINE == pdON)
inline void eint0_SetMode(u8 mode) {
#else
void eint0_SetMode(u8 mode) {
#endif


	irq_Disable();												// Desabilita a interrup��o global
	
	if (mode)		EXTMODE |= EINT0;
	else			EXTMODE &= ~EINT0;
	
	irq_Enable();												// Restaura a interrup��o global
}
	
//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta a polaridade para aceitar a interrup��o
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
	irq_Disable();												// Desabilita a interrup��o global

	if (polar)		EXTPOLAR |= EINT0;
	
	// N�O ESTA FUNCIONANDO EM COLOCAR O VALOR 0 NO EXTPOLAR. O MICRO CONTROLADOR FICA TRAVADO
	// J� QUE ESTAMOS USANDO SOMENTE NO INICIO DO PROGRAMA E O VALOR DESEJADO � 0, DEIXAREMOS COM O VALOR PADR�O AP�S O RESET, QUE � 0
	// CASO PRECISE USAR NO MEIO DO PROGRAMA � PRECISO PESQUISAR PORQUE O MICRO CONTROLADOR FICA TRAVADO NO INICIO
	else			EXTPOLAR &= ~EINT0; 
		
	irq_Enable();												// Restaura a interrup��o global
}

// ################################################################################################################
// SERVI�OS DE INTERRUP��ES IRQ
// ################################################################################################################
// ################################################################################################################
void eint0_ISR (void) {
    ISR_ENTRY;																	// Procedimento para entrada da interrup��o
    EXTINT |= EINT0;															// Limpa flag da interrup��o
	       
    if (eint0_int_func)															// Checa se existe alguma fun��o anexada
    	eint0_int_func();														// Executa a fun��o que foi anexada a essa interrup��o
     
    // Limpa interrup��o
    #if defined(arm7tdmi)
    VICSoftIntClr = (1<<VIC_EINT0);												
    #endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_EINT0);
	#endif

    ISR_EXIT;																	// Procedimento para saida da interrup��o
}

/*
// ################################################################################################################
// SERVI�OS DE INTERRUP��ES FIQ
// ################################################################################################################
// ################################################################################################################

void EINT_FIQ_ISR (void) {

	// ISR_ENTRY;																	// Procedimento para entrada da interrup��o. �til para interrup��o NESTED
	// LIMPA FLAG DE INTERRUP��O
	#ifdef EINT_USE_INT0
    	EXTINT = EINT0;																// Limpa flag da interrup��o
	#endif
	
	#ifdef EINT_USE_INT1
	    EXTINT = EINT1;																// Limpa flag da interrup��o
	#endif
	
	#ifdef EINT_USE_INT2
    	EXTINT = EINT2;																// Limpa flag da interrup��o
	#endif
	
	#ifdef EINT_USE_INT3
    	EXTINT = EINT3;																// Limpa flag da interrup��o
	#endif
	

    
    #ifdef EINT_USE_INT0
    	if (eint0_int_func)															// Checa se existe alguma fun��o anexada
    		eint0_int_func();														// Executa a fun��o que foi anexada a essa interrup��o
    #endif
    #ifdef EINT_USE_INT1
    	if (eint1_int_func)															// Checa se existe alguma fun��o anexada
    		eint1_int_func();														// Executa a fun��o que foi anexada a essa interrup��o
    #endif
    #ifdef EINT_USE_INT2
    	if (eint2_int_func)															// Checa se existe alguma fun��o anexada
    		eint2_int_func();														// Executa a fun��o que foi anexada a essa interrup��o
    #endif
    #ifdef EINT_USE_INT3
    	if (eint3_int_func)															// Checa se existe alguma fun��o anexada
    		eint3_int_func();														// Executa a fun��o que foi anexada a essa interrup��o
    #endif

   //ISR_EXIT;																		// Procedimento para saida da interrup��o. �til para interrup��o NESTED
    //VICVectAddr = 0;             													// Limpa o registrador de endere�o do ISR, para novas interrup��es
  	ISR_EXIT;
}
*/
