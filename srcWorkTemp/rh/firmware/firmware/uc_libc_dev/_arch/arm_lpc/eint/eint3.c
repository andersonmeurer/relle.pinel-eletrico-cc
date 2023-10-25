#include "eint3.h"
#if defined(cortexm3)
#include "arm.h"
#endif

typedef void (*eint3_pfunc)(void);
static volatile eint3_pfunc  eint3_int_func;
void eint3_ISR(void);

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
int eint3_Init(u8 mode, u8 polar) {
	#if defined(arm7tdmi)
	EINT3_PINSEL = (EINT3_PINSEL & ~EINT3_PINMASK) | EINT3_PINS; // Seleciona o pino externo para a fun��o de interrup��o externa
	#else
	ENTI3_ConfigPort();
	#endif
			
	eint3_int_func = 0;
	eint3_SetMode(mode);
	eint3_SetPolar(polar);
	EXTINT = EINT3;												// Limpa INT
		
	#if defined(arm7tdmi)
	if (!irq_Install(VIC_EINT3, eint3_ISR, EINT3_PRIO_LEVEL))
		 return errIRQ_INSTALL;
	#endif

	#if defined (cortexm3)
	nvic_enableIRQ(VIC_EINT3);
	nvic_setPriority(VIC_EINT3, EINT3_PRIO_LEVEL);
	#endif
	
	return pdPASS;
}


// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Anexa uma routina do usu�rio ao vetor de interrup��o
// Parametros:	Ponteiro da fun��o
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void eint3_SetIntHandler(void (*func)(void) ) {
	eint3_int_func = func;
}

// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retira a routina do usu�rio do vetor de interrup�ao
// Parametro:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void eint3_ClearIntHandler(void) {
	eint3_int_func = 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta se a interrup��o ser� aceita pela nivel ou borda do sinal
// Parametro:	Modo de interrup��o
// 					EXTMODE_LEVEL - Sensivel ao nivel do sinal
// 					EXTMODE_EDGE  - Sensivel a borda do sinal
// Retorna nada
//------------------------------------------------------------------------------------------------------------------
#if (EINT_USE_FUNC_INLINE == pdON)
inline void eint3_SetMode(u8 mode) {
#else
void eint3_SetMode(u8 mode) {
#endif

	irq_Disable();												// Desabilita a interrup��o global
	
	if (mode)		EXTMODE |= EINT3;
	else			EXTMODE &= ~EINT3;

	irq_Enable();												// Restaura a interrup��o global
}
	
//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta a polaridade para aceitar a interrup��o
// Parametros:	Tipo de sensibilidade
// 					EXTPOLAR_LOW  - Sensivel ao nivel baixo ou na queda do sinal
// 					EXTPOLAR_HIGH - Sensivel ao nivel alto ou na subida do sinal
// Retorna nada
//------------------------------------------------------------------------------------------------------------------
#if (EINT_USE_FUNC_INLINE == pdON)
inline void eint3_SetPolar(u8 polar) {
#else
void eint3_SetPolar(u8 polar) {
#endif

	irq_Disable();												// Desabilita a interrup��o global
	
	if (polar)		EXTPOLAR |= EINT3;
		
	// N�O ESTA FUNCIONANDO EM COLOCAR O VALOR 0 NO EXTPOLAR. O MICRO CONTROLADOR FICA TRAVADO
	// J� QUE ESTAMOS USANDO SOMENTE NO INICIO DO PROGRAMA E O VALOR DESEJADO � 0, DEIXAREMOS COM O VALOR PADR�O AP�S O RESET, QUE � 0
	// CASO PRECISE USAR NO MEIO DO PROGRAMA � PRECISO PESQUISAR PORQUE O MICRO CONTROLADOR FICA TRAVADO NO INICIO
	//else			EXTPOLAR &= ~eint; 
	
	irq_Enable();												// Restaura a interrup��o global
}

// ################################################################################################################
// SERVI�OS DE INTERRUP��ES IRQ
// ################################################################################################################
// ################################################################################################################
void eint3_ISR (void) {
    ISR_ENTRY;																	// Procedimento para entrada da interrup��o
    EXTINT |= EINT3;															// Limpa flag da interrup��o
	       
    if (eint3_int_func)															// Checa se existe alguma fun��o anexada
    	eint3_int_func();														// Executa a fun��o que foi anexada a essa interrup��o
    
	// Limpa interrup��o
    #if defined(arm7tdmi)
    VICSoftIntClr = (1<<VIC_EINT3);												
    #endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_EINT3);
	#endif
    ISR_EXIT;																	// Procedimento para saida da interrup��o    
}
