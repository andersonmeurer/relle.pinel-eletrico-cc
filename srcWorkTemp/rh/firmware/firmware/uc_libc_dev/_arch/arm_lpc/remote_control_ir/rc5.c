#include "rc5.h"
#include "vic.h"

// O receptor RC5 utiliza um timer e a entrada CAP0.
//	Esta entrada pode capturar valor do timer atual em ambas bordas de descida e subida,
//	tal como gerar interrupções em ambas bordas. Isto faz uma medição simples nos tempo
//	altos e baixos do pulso RC5. Além disso, é preciso ler um pino GPIO para determinar se foi uma
//	interrupção de queda ou subida da porta.
//
//
//  IR RX -------- CAP0  ARM lpc
//            |--- PX_T


//#define pout_Init()	(GPIO2_DIR |= BIT_9)
//#define pout_Off()	(GPIO2_CLR = BIT_9)
//#define pout_On()	(GPIO2_SET = BIT_9)

#if (IR_USE_TIMER == 0)
#define PCTIM PCTIM0
#define TIR T0IR
#define TPR T0PR
#define TMR0 T0MR0
#define TMCR T0MCR
#define TCCR T0CCR
#define TTC T0TC
#define TTCR T0TCR
#define TIMER_PRIO_LEVEL TIMER0_PRIO_LEVEL
#define VIC_TIMER VIC_TIMER0
#define TIR_MR0I T0IR_MR0I
#define TIR_CR0I T0IR_CR0I
#define TCR0 T0CR0
#define tcap0_ConfigPort() t0cap0_ConfigPort()
void timer0_ISR (void);
#define rc5_ISR timer0_ISR
#endif

typedef void (*rc5pFunc)(int sys, int cmd);
volatile rc5pFunc rc5_IntFunction;

#define MIN_HALF_BIT      640  	// 640 us
#define HALF_BIT_TIME     889  	// 889 us
#define MAX_HALF_BIT      1140  // 1140 us
#define MIN_FULL_BIT      1340  // 1340 us
#define FULL_BIT_TIME     1778  // 1778 us
#define MAX_FULL_BIT      2220  // 2220 us

static volatile int low_time;
static volatile int high_time;
static volatile int half_bit;
static volatile int sys;  // temp system byte
static volatile int cmd;  // temp Command byte

static void rc5_ShiftBit(char val) {
	if (sys & 0x80) {
    	if(cmd & 0x80) { // command full ?
         	sys = 0;  // yes, ERROR
          	cmd = 0;
        } else	cmd = (cmd << 1) | val;  // shift command
	} else sys = (sys << 1) | val;  // shift system
}

/************************************************************************
  ; rc5_Decode (we only take action at a rising edge)
  ; Half(prev) Bit LowTime     HighTime     Action  NewHalf Bit
  ; -------------------------------------------------------------------
  ;    0            0           0       Shift 0      0
  ;    0            0           1       Shift 1      1
  ;    0            1           0       -ERROR-      *
  ;    0            1           1       Shift 1,0     0
  ;    1            0           0       Shift 1      1
  ;    1            0           1       -ERROR-      *
  ;    1            1           0       Shift 1,0     0
  ;    1            1           1       -ERROR-      *
  ************************************************************************/
static void rc5_Decode(void) {
   	unsigned char action;

   	action = half_bit << 2;

   	if ( (high_time > MIN_FULL_BIT) && (high_time < MAX_FULL_BIT) )
    	action = action | 1;  // high_time = long
   	else if (!( (high_time > MIN_HALF_BIT) && (high_time < MAX_HALF_BIT) )) {
       	sys = 0;  // RC5 ERROR
        cmd = 0;
        return;
   	}

    if ( (low_time > MIN_FULL_BIT) && (low_time < MAX_FULL_BIT) )
    	action = action | 2;  // low_time = long
    else if (!( (low_time > MIN_HALF_BIT) && (low_time < MAX_HALF_BIT)) ) {
    	sys = 0;  // RC5 ERROR
        cmd = 0;
        return;
	}

    switch(action) {
    case 0:
    	rc5_ShiftBit(0);  // short low, short high, shift 0
    	break;
    case 1:
    	rc5_ShiftBit(1);  // short low, long high, shift 1
        half_bit = 1;  // new half bit is true
        break;
    case 2:
    	sys = 0;  // long low, short high, ERROR
        cmd = 0;
   	case 3:
   		rc5_ShiftBit(1);  // long low, long high, shift 1,0
        rc5_ShiftBit(0);
        break;
    case 4:
    	rc5_ShiftBit(1);  // short low, short high, shift 1
        break;
    case 5:
    	sys = 0;  // short low, long high, ERROR
        cmd = 0;
        break;
    case 6:
    	rc5_ShiftBit(1);  // long low, short high, shift 1,0
        rc5_ShiftBit(0);
        half_bit = 0;  // new half bit is false
        break;
    case 7:
    	sys = 0;  // long low, long high, ERROR
        cmd = 0;
    default:
    	break;  // invalid
    }
}

int rc5_Init (void) {
	//pout_Init();

	rc5_IntFunction = 0;
	rc5.system = 0;
	rc5.command = 0;
	rc5.newData = pdFALSE;

	PCONP |= PCTIM;				// Ligar o timer no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões

	TIR  = 0xFF;				// Limpa todas as interrupções
	TPR = PCLK / 1000000;  		// Ajusta o prescaler do timer para rodar a 1MHz
	TMR0 = 12000;  				// Registrador de comparação para 12ms. High(idle) Timeout
	TMCR = 3;  					// Gerar interrupção quando contador do timer for igual MR0, também zera o contador (TC)
	TCCR = 0x7;  				// Gerar interrupção na borda de subida e descida de cap0 e armazenar o valor do contador (TC) em CR0 em ambos enventos
	TTC = 0;  					// Zera o contador
	TTCR = 1;  					// Inicia a contagem

	#if defined(arm7tdmi)
	if (!irq_Install(VIC_TIMER, timer0_ISR, TIMER_PRIO_LEVEL))		// Instala o ISR a VIC
		return errIRQ_INSTALL;											// Retonar falso se não conseguiu instalar a ISR a VIC
	#endif

	#if defined (cortexm3)
	tcap0_ConfigPort();
	nvic_enableIRQ(VIC_TIMER);
	nvic_setPriority(VIC_TIMER, TIMER_PRIO_LEVEL);
	#endif

	return pdPASS;
}

// bit 5 do system sinaliza que o a tecla esta mantida pressionada ou está sendo pressionada
void rc5_SetIntHandler(void (*int_func)(int sys, int cmd) ) {
	rc5_IntFunction = int_func;
}

// retorna pdTRUE caso recebeu um novo comando, quando ler essa função o flag de novo comando é limpo
// bit 5 do system sinaliza que o a tecla esta mantida pressionada ou está sendo pressionada
int rc5_GetData(int* system, int* command) {
	int nd = rc5.newData;
	rc5.newData = pdFALSE;
	*system = rc5.system;
	*command = rc5.command;
	return nd;
}

// ################################################################################################################
// SERVIÇO DE INTERRUPÇÃO
// ################################################################################################################
void rc5_ISR(void) {
    ISR_ENTRY;															// Procedimento para entrada da interrupção

	TTC = 0;  // Reset timer // REDUDANTE ????????

    // É utilizado o mach0 para contagem de overflow
    // a cada 12ms vamos checar se recebemos o código completo
	if (TIR & TIR_MR0I) {	// Checa se a interrupção gerada é do comparador MR0
		TIR |= TIR_MR0I;	// Limpa o flag da INT
		if (cmd & 0x80)  { // command full ?
        	rc5.command = cmd & 0x7F;  	// Captura o comando recebido
        	rc5.system = sys;  			// Captura o sistema recebido
			rc5.newData = pdTRUE;  		// Sinaliza que recebemos um novo comando

			if (rc5_IntFunction) rc5_IntFunction(rc5.system, rc5.command);
			// TODO colocar função de chamada
		}
		sys = 0;
		cmd = 0;
	} else {
		TIR = TIR_CR0I; 	// Limpa o flag da INT
		if (IR_GPIO_PIN & IR_PIN) { // check P1_25 rising or falling edge
			if (sys == 0)  { // First pulse?
				low_time = HALF_BIT_TIME;  // assume short low time
				high_time= HALF_BIT_TIME;  // assume short high time
				half_bit = 1;  // assume half bit is true
	            cmd = 0x02;  // = 00000010, prepare command byte
			} else low_time = TCR0;  // rising, so capture low time

         	rc5_Decode();
		} else high_time = TCR0;  // falling, so capture high time
		//if (GPIO1_PIN & BIT_25) pout_On(); else pout_Off();
	}

	// Limpa interrupção
	#if defined(arm7tdmi)
	VICSoftIntClr = (1<<VIC_TIMER);
	#endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_TIMER);
	#endif

    ISR_EXIT;															// Procedimento para saida da interrupção
}

