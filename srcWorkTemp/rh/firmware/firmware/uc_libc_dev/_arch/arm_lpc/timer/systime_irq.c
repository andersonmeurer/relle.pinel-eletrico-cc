// Timer de sistema com base de tempo de 1ms, ou 0.5ms, ou 0.25ms de acordo com SYSTIME_BASE_TICK

#include "systime_irq.h"
#include "vic.h"

#if (SYSTIME_USE_TIMER == 0)
	#if defined (cortexm3)
	#define  systime_ISR 					timer0_ISR
	#endif

	#define SYSTIME_VIC 					VIC_TIMER0
	#define SYSTIME_PCTIM					PCTIM0
	#define SYSTIME_IR 						T0IR
	#define SYSTIME_TCR_RESET 				T0TCR_RESET
	#define SYSTIME_PR 						T0PR
	#define SYSTIME_TCR 					T0TCR
		#define SYSTIME_TCR_RESET				T0TCR_RESET
		#define SYSTIME_TCR_ENABLE				T0TCR_ENABLE
	#define SYSTIME_MR0						T0MR0
	#define SYSTIME_MCR						T0MCR
		#define SYSTIME_MCR_MR0_INT_ENABLE 		T0MCR_MR0_INT_ENABLE
		#define SYSTIME_MCR_MR0_RESET    		T0MCR_MR0_RESET
	#define SYSTIME_CCR						T0CCR
	#define SYSTIME_EMR						T0EMR
#else
	#if defined (cortexm3)
	#define  systime_ISR 					timer1_ISR
	#endif

	#define SYSTIME_VIC 					VIC_TIMER1
	#define SYSTIME_PCTIM					PCTIM1
	#define SYSTIME_IR 						T1IR
	#define SYSTIME_TCR_RESET 				T1TCR_RESET
	#define SYSTIME_PR 						T1PR
	#define SYSTIME_TCR 					T1TCR
		#define SYSTIME_TCR_RESET				T1TCR_RESET
		#define SYSTIME_TCR_ENABLE				T1TCR_ENABLE
	#define SYSTIME_MR0						T1MR0
	#define SYSTIME_MCR						T1MCR
		#define SYSTIME_MCR_MR0_INT_ENABLE 		T1MCR_MR0_INT_ENABLE
		#define SYSTIME_MCR_MR0_RESET    		T1MCR_MR0_RESET
	#define SYSTIME_CCR						T1CCR
	#define SYSTIME_EMR						T1EMR
#endif

void systime_ISR(void);

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

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incializa o Timer para uso do tick de sistema
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
int systime_Init(void) {
	#if (SYSTIME_USE_LED == pdON)
	SYSTIME_LED_DIR |= SYSTIME_LED; 					// Colocar o pino do led para saida
	countTicksLed = 0;
	#endif

	PCONP |= SYSTIME_PCTIM;										// Ligar o timer no controle de potencia. Tem que ser antes da configura��o, pois ap�s ligar o perif�ricos, seus regs assumem valores padr�es

	SYSTIME_IR = 0xFF;											// Limpa todas as interrup��es
	SYSTIME_TCR = SYSTIME_TCR_RESET;							// Reset o contador e mantem em reset
	SYSTIME_PR = 0; 											// Sem prescaler
	SYSTIME_MR0 = SYSTIME_TICKS_PER_MS/SYSTIME_BASE_TICK;	// Definir a contagem para gerar um tick de sistema
  	SYSTIME_CCR = 0;                          					// disable compare registers
  	SYSTIME_EMR = 0;                          					// disable external match register

  	// retira qualquer fun��o anexada para interrup��o do systime
  	systime_ClearIntHandler();

	#if defined (arm7tdmi)
  	// Instala o ISR a VIC
  	#if (SYSTIME_PRIO_LEVEL > 15)
  	#error A prioridade de interrup��o SYSTIME_PRIO_LEVEL n�o pode ser maior que 15
  	#endif
  	if (!irq_Install(SYSTIME_VIC, systime_ISR, SYSTIME_PRIO_LEVEL))
		return errIRQ_INSTALL;	// Retonar falso se n�o conseguiu instalar a ISR a VIC
	#endif

	#if defined (cortexm3)
  	nvic_enableIRQ(SYSTIME_VIC);
  	nvic_setPriority(SYSTIME_VIC, SYSTIME_PRIO_LEVEL);
  	#endif

  	// Limpa o contador de ticks do sistema
  	countTicks_ms = 0;

  	// Habilita a interrup��o quando MR0 for igual a TC, e reseta TC quando ele for igual a MR0
  	SYSTIME_MCR = SYSTIME_MCR_MR0_INT_ENABLE | SYSTIME_MCR_MR0_RESET;

  	// reset timer
  	SYSTIME_TCR = SYSTIME_TCR_RESET;
  	// Habilita e reset timer
  	SYSTIME_TCR = SYSTIME_TCR_ENABLE;

	return pdPASS;
}

// -----------------------------------------------------------------------
// Descri��o: 	Anexa uma routina ao vetor de interrup��es a ser chamada a cada xticks
// Parametro: 	int_func
//					Ponteiro da rotina que tratar� a interrup��o do timer.
//					Ex:	systime_SetIntHandler(myOverflowFunction, xticks);
//				xticks: Quantidade de ticks que a fun��o deve ser chamada. Consultar SYSTIME_BASE_TICK
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void systime_SetIntHandler(void (*int_func)(void), uint xticks ) {
	if (xticks == 0) { systime_IntFunctions = 0; return;}
	systime_IntFunctions = int_func;
	callfuncTime = xticks;
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
// Descri��o: 	Inicializa o timeout para usar o timeout_Check
// Parametro:	timeout: tempo em ms para espera. Podemos usar macros abaixo
//					SYSTIME_xMS onde x = 2, 5, 10, 20, 30, 50, 100, 150, 250, 500
//					SYSTIME_xSEC onde x = 1, 2, 5, 10
// -------------------------------------------------------------------------------------------------------------------
void timeout_Init(u32 timeout) {
	timerout = timeout;
	timerout_startTime = systime_GetTick();
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Checa se o tempo de timeout desejado j� estourou
// Retorna: 	pdTRUE se o tempo de timeou j� foi alcan�ado, sen�o retorna pdFALSE
// -------------------------------------------------------------------------------------------------------------------
int timeout_Check(void) {
  	if (systime_GetElapsedTick(timerout_startTime) < timerout)
    		return pdFAIL;
  	else	return pdPASS;
}

// ################################################################################################################
// TIMER SERVI�OS DE INTERRUP��O
// ################################################################################################################
// ################################################################################################################
// ----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Cabe�aho para atender as interrup��es do timer
// ----------------------------------------------------------------------------------------------------------------
void systime_ISR(void) {
    ISR_ENTRY;								// Procedimento para entrada da interrup��o
	SYSTIME_IR = 0xFF;						// Limpa todos os flags da INTs do timer

	countTicks_ms++;

	// Checa se existe uma fun��o anexada a interrup��o e � tempo de chamar a fun��o
	if ( (systime_IntFunctions) && (countTicks_ms % callfuncTime == 0))
		systime_IntFunctions();					// Execute a fun��o anexada

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

	// Limpa interrup��o
	#if defined (arm7tdmi)
	VICSoftIntClr = (1<<SYSTIME_VIC);
	#endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(SYSTIME_VIC);
	#endif

	ISR_EXIT;								// Procedimento para saida da interrup��o
}
