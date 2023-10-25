#include "servo2.h"
#include "_config_lib_motors.h"

// Controlador de servo motor
//
// Usamos os registradores MR0 para definir o tempo alto e o MR1 para definir o tempo baixo
//                       TH             TL
// Pulso para o servo: ######________________________
//
// Tempos	Servo normal	Servo de modo continuo
// 1000ms	0� 				M�xima velocidade sentido antihor�rio
// 1250ms	45� 			M�dia velocidade sentido antihor�rio
// 1500ms	90�				Parado
// 1750ms	135� 			M�dia velocidade sentido hor�rio
// 2000ms	180�			M�xima velocidade sentido hor�rio

#if defined(CPU_ARM_LPC)

// Usamos MR0 do timer para compara��o do TC para baixar o nivel de sinal, e o MR1 para elevar o sinal e resetar o TC
// Configuramos o registrador EMR do timer para que o pino externo v� em baixa na interrup��o, entretanto chamamos
//	a fun��o proc_MacthMR1 para elevar o pino na interrup��o de MR1
//                       TH             TL
// Pulso para o servo: ######________________________
//                           MR0                     MR1
// Tempo de TC de 0 at� MR0 para os servos entre 1ms a 2ms
// Tempo de TC de 0 at� MR1 = 20ms

#if (SERVO2_USE_TIMER == 1)
	#include "timer1.h"
	#define TEMR_EMC0_DONOTHING	T1EMR_EMC0_DONOTHING
	#define TEMR_EMC0_CLRPIN T1EMR_EMC0_CLRPIN
	#define TIMER_VECTOR_INT_MATCH1 TIMER1_VECTOR_INT_MATCH1
	#define timer_Init timer1_Init
	#define timer_SetPinOutputMatch0 timer1_SetPinOutputMatch0
	#define timer_SetIntHandler	timer1_SetIntHandler
	#define timer_SetMatch0	timer1_SetMatch0
	#define timer_SetMatch1	timer1_SetMatch1
	#define TEMR T1EMR
	#define timer_Set timer1_Set
		#define TTCR_ENABLE T1TCR_ENABLE
		#define TTCR_STOP T1TCR_STOP
#elif (SERVO2_USE_TIMER == 2)
	#include "timer2.h"
	#define TEMR_EMC0_DONOTHING	T2EMR_EMC0_DONOTHING
	#define TEMR_EMC0_CLRPIN T2EMR_EMC0_CLRPIN
	#define TIMER_VECTOR_INT_MATCH1 TIMER2_VECTOR_INT_MATCH1
	#define timer_Init timer2_Init
	#define timer_SetPinOutputMatch0 timer2_SetPinOutputMatch0
	#define timer_SetIntHandler	timer2_SetIntHandler
	#define timer_SetMatch0	timer2_SetMatch0
	#define timer_SetMatch1	timer2_SetMatch1
	#define TEMR T2EMR
	#define timer_Set timer2_Set
		#define TTCR_ENABLE T2TCR_ENABLE
		#define TTCR_STOP T2TCR_STOP
#endif // #if (SERVO2_USE_TIMER)

static void proc_MacthMR1(void);

// inicializa o servo j� na posi��o desejada
void servo2_Init(int pos) {
	timer_Init(
		1,											// Timer 0 sem prescaler
		T1TCR_ENABLE,								// Iniciar contador do timer
		T1MCR_MR0_INT_ENABLE |						// Gerar interrup��o quanto TC=MR0
		T1MCR_MR1_INT_ENABLE |						// Gerar interrup��o quanto TC=MR1
		T1MCR_MR1_RESET								// Resetar o contador quando TC=MR1
	);

	timer_SetPinOutputMatch0(TEMR_EMC0_CLRPIN);						// Ajusta o pino externo ir ao n�vel baixo quando chegar na compara��o MR0
	timer_SetIntHandler(TIMER_VECTOR_INT_MATCH1, proc_MacthMR1);	// Anexar fun��o a ser chamado a interrup��o de compara��o 1 do timer
	timer_SetMatch0(us2cnt_Perif(pos));								// Ajusta o valor para compara��o 0 do timer em ms
	timer_SetMatch1(us2cnt_Perif(20000));							// Ajusta o valor para compara��o 1 do timer (20mS)
}

void servo2_SetPos(int us) {
	timer_SetMatch0(us2cnt_Perif(us));							// Ajusta o valor para compara��o 0 do timer (1mS)
}

void servo2_Enable(void) {
	timer_Set(TTCR_ENABLE);
}

void servo2_Disable(void) {
	timer_Set(TTCR_STOP);
}

static void proc_MacthMR1(void) {
	TEMR |= 1; // Colocar o pino em nivel 1
}

#endif // #if defined(CPU_ARM_LPC)
