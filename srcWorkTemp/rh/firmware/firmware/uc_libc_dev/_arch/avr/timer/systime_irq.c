#include <avr/interrupt.h>
#include "systime_irq.h"

// FREE RTOS
#if defined(FREE_RTOS) // N�o se usa essa lib com freertos
#error N�O UTILIZA ESSA LIB COM FREERTOS, SOMENTE USAMOS AS MACROS DO system.h
#else
// SEM SISTEMA OPERACIONAL

#if (SYSTIME_USE_TIMER == 0)
#define SYSTIME_TC 					TCNT0
#define SYSTIME_TCCR 				TCCR0B
#define SYSTIME_PRESCALE_MASK 		TIMER0_PRESCALE_MASK
#define SYSTIME_ENABLE_INT_COMPARE 	TIMER0_ENABLE_INT_COMPARE_A
#define SYSTIME_OCR 				OCR0A
#define SYSTIME_INTERRUPT_HANDLER	TIMER0_INTERRUPT_HANDLER
#define SYSTIME_COMPA_vect			TIMER0_COMPA_vect
#elif (SYSTIME_USE_TIMER == 1)
#if defined (at90s2343) || defined (at90s2343)
#error Esse AVR n�o tem suporte a Timer1
#endif
#define SYSTIME_TC 					TCNT1
#define SYSTIME_TCCR 				TCCR1B
#define SYSTIME_PRESCALE_MASK 		TIMER1_PRESCALE_MASK
#define SYSTIME_ENABLE_INT_COMPARE 	TIMER1_ENABLE_INT_COMPARE_A
#define SYSTIME_OCR 				OCR1A
#define SYSTIME_INTERRUPT_HANDLER	TIMER1_INTERRUPT_HANDLER
#define SYSTIME_COMPA_vect			TIMER1_COMPA_vect
#endif

typedef void (*void_pFunc_t)(void);
static volatile void_pFunc_t systime_IntFunctions;
static volatile tTime callfuncTime = 0;

static volatile u32 timerout;
static volatile u32 timerout_startTime;
static volatile u32 countTicks_ms;						// Contador de ticks de sistema

#if (SYSTIME_USE_LED == pdON)
// countTicksLed 	0 -- SYSTIME_LED_OFF_MS ----- SYSTIME_LED_OFF_MS + SYSTIME_LED_ON_MS  0
// LED 			 	_____#####################################################################______________
static volatile u32 countTicksLed;
#endif

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incializa o Timer para uso do tick de sistema
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
int systime_Init(void) {
	#if (SYSTIME_USE_LED == pdON)
	SYSTIME_LED_DIR |= _BV(SYSTIME_LED); 						// Colocar o pino do led para saida
	cbi(SYSTIME_LED_PORT, SYSTIME_LED);
	countTicksLed = 0;
	#endif

	SYSTIME_TC = 0;												// Zerar contador
	SYSTIME_TCCR = (SYSTIME_TCCR & ~SYSTIME_PRESCALE_MASK); 	// Limpa o prescaler e contador parado
	SYSTIME_TCCR |= SYSTIME_PRESCALER;
	SYSTIME_OCR = (SYSTIME_TICKS_PER_MS/(SYSTIME_BASE_TICK*_BV(SYSTIME_PRESCALER-1)));	// Definir quantos ticks de sistema � para ser gerado em 1 ms

	systime_ClearIntHandler();	// retira qualquer fun��o anexada para interrup��o do systime
  	countTicks_ms = 0;			// Limpa o contador de ticks do sistema

	TIMSK |= SYSTIME_ENABLE_INT_COMPARE; // Habilita a interrup��o quando REG for igual ao contador

  	return pdPASS;
}

// -----------------------------------------------------------------------
// Descri��o: 	Anexa uma routina ao vetor de interrup��es a ser chamada a cada calltime
//				OBS: Levar em considera��o o SYSTIME_BASE_TICK
// Parametro: 	int_func
//					Ponteiro da rotina que tratar� a interrup��o do timer.
//					Ex:	systime_SetIntHandler(myOverflowFunction);
//				calltime: Tempo em ms que o a fun��o ser� chamada, valor 0 � ignorado
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void systime_SetIntHandler(void (*int_func)(void), tTime calltime ) {
	systime_IntFunctions = int_func;
	callfuncTime = calltime;
}


// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retira a routina da inetrrup�ao desejada do vetor de interrup��es
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void systime_ClearIntHandler(void) {
	systime_IntFunctions = 0;
}


// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna a quantidade de ticks de sistema
// Parametros:	Nenhum
// Retorna:		O tempo em ticks de sistema
// -------------------------------------------------------------------------------------------------------------------
u32 systime_GetTick(void) {
  	return countTicks_ms;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna a diferen�a entre o tempo atual com o tempo passado por parametro
// Parametros:	Tempo que deseja diminuir do tempo de sistema
// Retorna:		A diferen�a de tempos
// -------------------------------------------------------------------------------------------------------------------
u32 systime_GetElapsedTick(u32 start_time) {
  	return systime_GetTick() - start_time;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Fornece uma pausa no sistema. Fica no loop at� o tempo desejado
// Parametros:	Tempo para pausa em ms
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void systime_Pause(u32 duration) {
  	u32 start_time = systime_GetTick();
  	while (systime_GetElapsedTick(start_time) < duration)
    	CRITICAL_WAIT;
}


// Inicizaliza o timerout para ser checado pela fun��o timerout_Check
// Valor em ms
void timerout_Init(u32 val) {
	timerout = val;
	timerout_startTime = systime_GetTick();
}

// Retorna pdTRUE se o valor do timerout foi alcan�ado
int timerout_Check(void) {
  	if (systime_GetElapsedTick(timerout_startTime) < timerout)
    		return pdFAIL;
  	else	return pdPASS;
}

// ################################################################################################################
// SERVI�OS DE INTERRUP��O
// ################################################################################################################
// ################################################################################################################

// Cabe�alho para atender as interrup��es do timer da compara��o A
SYSTIME_INTERRUPT_HANDLER(SYSTIME_COMPA_vect) {
	countTicks_ms++;

	if(systime_IntFunctions)					// Checa se existe uma fun��o anexada a interrup��o
	if ( (callfuncTime == 0) || !(countTicks_ms % callfuncTime) )
		systime_IntFunctions();					// Execute a fun��o anexada

	#if (SYSTIME_USE_LED == pdON)
	if (countTicksLed >= SYSTIME_LED_OFF_MS*SYSTIME_BASE_TICK + SYSTIME_LED_ON_MS*SYSTIME_BASE_TICK)
		countTicksLed = 0;
	countTicksLed++;

	if (countTicksLed > SYSTIME_LED_OFF_MS*SYSTIME_BASE_TICK)
			sbi(SYSTIME_LED_PORT, SYSTIME_LED);
	else	cbi(SYSTIME_LED_PORT, SYSTIME_LED);
	#endif

	SYSTIME_TC = 0;												// zerar contador
}
#endif

