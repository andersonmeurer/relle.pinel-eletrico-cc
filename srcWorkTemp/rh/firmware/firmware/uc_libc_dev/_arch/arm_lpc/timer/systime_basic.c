#include "systime_basic.h"

#if (SYSTIME_USE_TIMER == 0)
	#define SYS_TIME_PCTIM	PCTIM0
	#define SYS_TIME_TCR 	T0TCR
	#define SYS_TIME_PR 	T0PR
	#define SYS_TIME_MCR	T0MCR
	#define SYS_TIME_CCR	T0CCR
	#define SYS_TIME_EMR	T0EMR
	#define SYS_TIME_TC		T0TC
	#define SYS_TCR_RESET	T0TCR_RESET	
	#define SYS_TCR_ENABLE	T0TCR_ENABLE
#else
	#define SYS_TIME_PCTIM	PCTIM1
	#define SYS_TIME_TCR 	T1TCR
	#define SYS_TIME_PR 	T1PR
	#define SYS_TIME_MCR	T1MCR
	#define SYS_TIME_CCR	T1CCR
	#define SYS_TIME_EMR	T1EMR
	#define SYS_TIME_TC		T1TC
	#define SYS_TCR_RESET	T1TCR_RESET	
	#define SYS_TCR_ENABLE	T1TCR_ENABLE
#endif

static u32 timerout;
static u32 timerout_startTime;
static u32 countTicks;						// Contador de ticks de sistema
static u32 last_TC;

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa o Timer para uso do tick de sistema
// Parametros:	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void systime_Init(void) {
	PCONP |= SYS_TIME_PCTIM;							// Ligar o timer no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões
	
  	SYS_TIME_TCR = SYS_TCR_RESET;                  		// Reset o contador do timer
  	SYS_TIME_PR = SYS_TIME_PCLK_DIV - 1;          		// Ajusta o prescaler do timer
  	SYS_TIME_MCR = 0;                            		// disable match registers
  	#if !defined(SIM_SKYEYE) 
  		SYS_TIME_CCR = 0;                          		// disable compare registers
  		SYS_TIME_EMR = 0;                          		// disable external match register
  	#endif
  	SYS_TIME_TCR = SYS_TCR_ENABLE;                 		// Habilita timer 0
  	countTicks = 0;										// Limpa o contador de ticks do sistema
  	timerout = 0;
  	timerout_startTime = 0;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna a quantidade de ticks de sistema
// Parametros:	Nenhum
// Retorna:		O tempo em ticks de sistema
// -------------------------------------------------------------------------------------------------------------------
u32 systime_GetTick(void) {
  	u32 now = SYS_TIME_TC;

  	countTicks += (u32)(now - last_TC);
  	last_TC = now;
  
  	return countTicks;
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
// Parametros:	Tempo para pausa
//					SYS_TIME_xMS   [x = 2, 5, 10, 20, 30, 50, 100, 150, 250, 500]
//					SYS_TIME_xSEC   [x = 1, 2, 5, 10]
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void systime_Pause(u32 duration) {
  	u32 start_time = systime_GetTick();

  	while (systime_GetElapsedTick(start_time) < duration)
    	CRITICAL_WAIT;
}

void systime_delay_ms(u32 delayval) {
	u32 d = (delayval * 1e-3 * SYS_TIME_TICKS_PER_SEC) + .5;
	systime_Pause(d);
}

// NÃO MUITO PRECISO EM CONTAGEM ABAIXO DE 30us. Tempo minimo de 10uS a CPU de 58Mhz
void systime_delay_us(u32 delayval) {
	u32 d = (delayval * 1e-6 * SYS_TIME_TICKS_PER_SEC) + .5;
	systime_Pause(d);
}

// INICIZALIZA O TIMEROUT PARA SER CHECADO PELA FUNÇÃO ABAIXO
// VALOR EM MS
void timerout_Init(u32 val) {
	timerout = (val * 1e-3 * SYS_TIME_TICKS_PER_SEC) + .5;
	timerout_startTime = systime_GetTick();
}

// RETORNA pdTRUE SE O VALOR DO TIMEROUT FOI ALCANÇADO
int timerout_Check(void) {
  	if (systime_GetElapsedTick(timerout_startTime) < timerout)
    		return pdFAIL;
  	else	return pdPASS;
}
