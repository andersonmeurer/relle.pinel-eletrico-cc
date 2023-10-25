#include "eint3.h"
#if defined(cortexm3)
#include "arm.h"
#endif

typedef void (*eint3_pfunc)(void);
static volatile eint3_pfunc  eint3_int_func;
void eint3_ISR(void);

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
int eint3_Init(u8 mode, u8 polar) {
	#if defined(arm7tdmi)
	EINT3_PINSEL = (EINT3_PINSEL & ~EINT3_PINMASK) | EINT3_PINS; // Seleciona o pino externo para a função de interrupção externa
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
// Descrição: 	Anexa uma routina do usuário ao vetor de interrupção
// Parametros:	Ponteiro da função
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void eint3_SetIntHandler(void (*func)(void) ) {
	eint3_int_func = func;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retira a routina do usuário do vetor de interrupçao
// Parametro:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void eint3_ClearIntHandler(void) {
	eint3_int_func = 0;
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta se a interrupção será aceita pela nivel ou borda do sinal
// Parametro:	Modo de interrupção
// 					EXTMODE_LEVEL - Sensivel ao nivel do sinal
// 					EXTMODE_EDGE  - Sensivel a borda do sinal
// Retorna nada
//------------------------------------------------------------------------------------------------------------------
#if (EINT_USE_FUNC_INLINE == pdON)
inline void eint3_SetMode(u8 mode) {
#else
void eint3_SetMode(u8 mode) {
#endif

	irq_Disable();												// Desabilita a interrupção global
	
	if (mode)		EXTMODE |= EINT3;
	else			EXTMODE &= ~EINT3;

	irq_Enable();												// Restaura a interrupção global
}
	
//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta a polaridade para aceitar a interrupção
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

	irq_Disable();												// Desabilita a interrupção global
	
	if (polar)		EXTPOLAR |= EINT3;
		
	// NÃO ESTA FUNCIONANDO EM COLOCAR O VALOR 0 NO EXTPOLAR. O MICRO CONTROLADOR FICA TRAVADO
	// JÁ QUE ESTAMOS USANDO SOMENTE NO INICIO DO PROGRAMA E O VALOR DESEJADO É 0, DEIXAREMOS COM O VALOR PADRÃO APÓS O RESET, QUE É 0
	// CASO PRECISE USAR NO MEIO DO PROGRAMA É PRECISO PESQUISAR PORQUE O MICRO CONTROLADOR FICA TRAVADO NO INICIO
	//else			EXTPOLAR &= ~eint; 
	
	irq_Enable();												// Restaura a interrupção global
}

// ################################################################################################################
// SERVIÇOS DE INTERRUPÇÕES IRQ
// ################################################################################################################
// ################################################################################################################
void eint3_ISR (void) {
    ISR_ENTRY;																	// Procedimento para entrada da interrupção
    EXTINT |= EINT3;															// Limpa flag da interrupção
	       
    if (eint3_int_func)															// Checa se existe alguma função anexada
    	eint3_int_func();														// Executa a função que foi anexada a essa interrupção
    
	// Limpa interrupção
    #if defined(arm7tdmi)
    VICSoftIntClr = (1<<VIC_EINT3);												
    #endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_EINT3);
	#endif
    ISR_EXIT;																	// Procedimento para saida da interrupção    
}
