// Timer de sistema com base de tempo de 1ms, ou 0.5ms, ou 0.25ms de acordo com SYSTIME_BASE_TICK

#include "systick.h"
#include "vic.h"

void sysTick_ISR(void);

typedef void (*systick_pFunc_t)(void);
static volatile systick_pFunc_t systime_IntFunctions;
static volatile uint callfuncTime = 0;

static volatile u32 timerout;
static volatile u32 timerout_startTime;
static volatile u32 countTicks_ms;						// Contador de ticks de sistema


#if (SYSTIME_USE_LED == pdON)
// countTicksLed 	0 -- SYSTIME_LED_OFF_MS ----- SYSTIME_LED_OFF_MS + SYSTIME_LED_ON_MS  0
// LED 			 	_____#####################################################################______________
static volatile u32 countTicksLed;
#endif

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configINTERRUPT_PRIORITY 		( SYSTIME_PRIO_LEVEL << (3) )

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o Timer para uso do tick de sistema
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
int systime_Init(void) {
	#if (SYSTIME_USE_LED == pdON)
	SYSTIME_LED_DIR |= SYSTIME_LED; 					// Colocar o pino do led para saida
	countTicksLed = 0;
	#endif

	// Make PendSV and SysTick the lowest priority interrupts
	NVIC_SYSPRI2_REG |= ( ( ( u32 ) configINTERRUPT_PRIORITY ) << 16UL );
	NVIC_SYSPRI2_REG |= ( ( ( u32 ) configINTERRUPT_PRIORITY ) << 24UL );

	// Configure SysTick to interrupt at the requested rate.
	STRELOAD = ( CCLK / 1000 / SYSTIME_BASE_TICK ) - 1UL;
	STCTRL = ( STCTRL_CLKSOURCE | STCTRL_TICKINT | STCTRL_ENABLE );

  	systime_ClearIntHandler(); // retira qualquer função anexada para interrupção do systime
  	countTicks_ms = 0; // Limpa o contador de ticks do sistema

	return pdPASS;
}

// -----------------------------------------------------------------------
// Descrição: 	Anexa uma routina ao vetor de interrupções a ser chamada a cada xticks
// Parametro: 	int_func
//					Ponteiro da rotina que tratará a interrupção do timer.
//					Ex:	systime_SetIntHandler(myOverflowFunction, xticks);
//				xticks: Quantidade de ticks que a função deve ser chamada. Consultar SYSTIME_BASE_TICK
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void systime_SetIntHandler(void (*int_func)(void), uint xticks ) {
	if (xticks == 0) { systime_IntFunctions = 0; return;}
	systime_IntFunctions = int_func;
	callfuncTime = xticks;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retira a routina da inetrrupçao desejada do vetor de interrupções
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void systime_ClearIntHandler(void) {
	systime_IntFunctions = 0;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna a quantidade de ticks de sistema
// Parametros:	Nenhum
// Retorna:		O tempo em ticks de sistema
// -------------------------------------------------------------------------------------------------------------------
u32 systime_GetTick(void) {
  	return countTicks_ms;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna a diferença entre o tempo atual com o tempo passado por parametro
// Parametros:	Tempo que deseja diminuir do tempo de sistema
// Retorna:		A diferença de tempos
// -------------------------------------------------------------------------------------------------------------------
u32 systime_GetElapsedTick(u32 start_time) {
  	return systime_GetTick() - start_time;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Fornece uma pausa no sistema. Fica no loop até o tempo desejado
// Parametros:	Tempo para pausa em ms. Qualquer valor ou usar as macros abaixo
//					SYSTIME_xMS onde x = 2, 5, 10, 20, 30, 50, 100, 150, 250, 500
//					SYSTIME_xSEC onde x = 1, 2, 5, 10
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void systime_Pause(u32 duration) {
  	u32 start_time = systime_GetTick();
 	while (systime_GetElapsedTick(start_time) < duration) CRITICAL_WAIT;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa o timeout para usar o timeout_Check
// Parametro:	timeout: tempo em ms para espera. Podemos usar macros abaixo
//					SYSTIME_xMS onde x = 2, 5, 10, 20, 30, 50, 100, 150, 250, 500
//					SYSTIME_xSEC onde x = 1, 2, 5, 10
// -------------------------------------------------------------------------------------------------------------------
void timeout_Init(u32 timeout) {
	timerout = timeout;
	timerout_startTime = systime_GetTick();
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Checa se o tempo de timeout desejado já estourou
// Retorna: 	pdTRUE se o tempo de timeou já foi alcançado, senão retorna pdFALSE
// -------------------------------------------------------------------------------------------------------------------
int timeout_Check(void) {
  	if (systime_GetElapsedTick(timerout_startTime) < timerout)
    		return pdFAIL;
  	else	return pdPASS;
}

// ################################################################################################################
// SYSTEM TIMER INTERRUPÇÃO
// ################################################################################################################
// ################################################################################################################
void sysTick_ISR (void) {
    ISR_ENTRY;	// Procedimento para entrada da interrupção

	countTicks_ms++;

	// Checa se existe uma função anexada a interrupção e é tempo de chamar a função
	if ( (systime_IntFunctions) && (countTicks_ms % callfuncTime == 0))
		systime_IntFunctions();					// Execute a função anexada

	#if (SYSTIME_USE_LED == pdON)
	if (countTicksLed >= SYSTIME_LED_OFF_MS*SYSTIME_BASE_TICK + SYSTIME_LED_ON_MS*SYSTIME_BASE_TICK)
		countTicksLed = 0;
		countTicksLed++;
		if (countTicksLed > SYSTIME_LED_OFF_MS*SYSTIME_BASE_TICK) {
			SYSTIME_LED_ON = SYSTIME_LED;
		} else {
			SYSTIME_LED_OFF = SYSTIME_LED;
		}
	#endif

	ISR_EXIT; // Procedimento para saida da interrupção
}
