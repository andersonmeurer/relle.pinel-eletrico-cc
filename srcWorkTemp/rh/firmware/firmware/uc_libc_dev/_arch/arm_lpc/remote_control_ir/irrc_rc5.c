#include "irrc_rc5.h"

#include "vic.h"

// biblioteca escrita em 2016-04-15

// O receptor RC5 utiliza um timer e a entrada CAP0.
//	Esta entrada pode capturar valor do timer atual em ambas bordas de descida e subida,
//	tal como gerar interrupções em ambas bordas. Isto faz uma medição simples nos tempo
//	altos e baixos do pulso RC5. Além disso, é preciso ler um pino GPIO para determinar se foi uma
//	interrupção de queda ou subida da porta.
//
//
//  IR RX -------- CAP0  ARM lpc
//            |--- PX_T

// Procedimento de adicionar um novo protocolo
// Coloque IRRC_USE_SNIFFER para pdON e analise os tempos de nível alto e baixo dos sinais
//		no prompt limpe RAW, e transmite com controle remoto e depois veja os dados e colete o tempo min e max tanto para o pulso curto como longo
//			repita o processo para diversas teclas até ficar seguro dos tempos minimos e máximos


#if (IRRC_USE_TIMER == 0)
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
#define irrc_ISR timer0_ISR
#endif

typedef void (*irrcpFunc)(uint addr, uint cmd);
volatile irrcpFunc irrc_IntFunction;

static volatile int low_time;
static volatile int high_time;
static volatile int half_bit;
static volatile u32 rxcmd;

// ################################################################################################################
// MISC
// ################################################################################################################
static void irrc_ShiftBit(int val) {
	rxcmd = (rxcmd << 1) | val;  // shift system
}

// ************************************************************************
//  ; rc5_Decode (we only take action at a rising edge)
//  ; Half(prev) Bit LowTime     HighTime     Action  NewHalf Bit
//  ; -------------------------------------------------------------------
//  ;    0            0           0       Shift 0      0
//  ;    0            0           1       Shift 1      1
//  ;    0            1           0       -ERROR-      *
//  ;    0            1           1       Shift 1,0     0
//  ;    1            0           0       Shift 1      1
//  ;    1            0           1       -ERROR-      *
//  ;    1            1           0       Shift 1,0     0
//  ;    1            1           1       -ERROR-      *
//  ************************************************************************
static void irrc_Decode(void) {
   	u8 action;

   	action = half_bit << 2;

   	if ( (high_time > IRRC_MIN_FULL_BIT) && (high_time < IRRC_MAX_FULL_BIT) )
    	action = action | 1;  // high_time = long
   	else if (!( (high_time > IRRC_MIN_HALF_BIT) && (high_time < IRRC_MAX_HALF_BIT) )) {
       	rxcmd = 0; // RC5 ERROR
        return;
   	}

    if ( (low_time > IRRC_MIN_FULL_BIT) && (low_time < IRRC_MAX_FULL_BIT) )
    	action = action | 2;  // low_time = long
    else if (!( (low_time > IRRC_MIN_HALF_BIT) && (low_time < IRRC_MAX_HALF_BIT)) ) {
    	rxcmd = 0; // RC5 ERROR
        return;
	}

    switch(action) {
    case 0:
    	irrc_ShiftBit(0);  // short low, short high, shift 0
    	break;
    case 1:
    	irrc_ShiftBit(1);  // short low, long high, shift 1
        half_bit = 1;  // new half bit is true
        break;
    case 2:
    	rxcmd = 0; // long low, short high, ERROR
   	case 3:
   		irrc_ShiftBit(1);  // long low, long high, shift 1,0
        irrc_ShiftBit(0);
        break;
    case 4:
    	irrc_ShiftBit(1);  // short low, short high, shift 1
        break;
    case 5:
    	rxcmd = 0; // long low, short high, ERROR
        break;
    case 6:
    	irrc_ShiftBit(1);  // long low, short high, shift 1,0
        irrc_ShiftBit(0);
        half_bit = 0;  // new half bit is false
        break;
    case 7:
    	rxcmd = 0; // long low, short high, ERROR
    default:
    	break;  // invalid
    }
}

#if (IRRC_USE_SNIFFER == pdON)
typedef struct {
	u16 idx;
	u16 raw[IRRC_nSAMPLES];
} tIR_Sniffer;
tIR_Sniffer ir_Sniffer;

int irrc_GetRaw(uint idx) {
	if ( (idx >=IRRC_nSAMPLES) || ( idx >= ir_Sniffer.idx) ) return -1;
	return ir_Sniffer.raw[idx];
}

void irrc_ClearRaw(void) {
	ir_Sniffer.idx = 0;
}
#endif


// ################################################################################################################
// GLOBAL
// ################################################################################################################
int irrc_Init(void) {
	#if (IRRC_USE_SNIFFER == pdON)
	pout_Init();
	ir_Sniffer.idx = 0;
	#endif

	rc5.command = 0;
	rc5.system = 0;
	rc5.newData = pdFALSE;
	irrc_IntFunction = 0;

	PCONP |= PCTIM;			// Ligar o timer no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões

	TIR  = 0xFF;			// Limpa todas as interrupções
	TPR = PCLK / 1000000;  	// Ajusta o prescaler do timer para rodar a 1MHz, para gerar uma contagem dp TC a cada 1uS
	TMR0 = 12000;  			// Registrador de comparação para 12ms. Tempo osciso sem receber mais sinais do controle remoto e precessar o comando
	TMCR = 3;  				// Gerar interrupção quando contador do timer for igual MR0, também zera o contador (TC)
	TCCR = 0x7;  			// Gerar interrupção na borda de subida e descida de cap0 e armazena o valor do contador (TC) em CR0 em ambos eventos
	TTC = 0;  				// Zera o contador
	TTCR = 1;  				// Inicia a contagem

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

// para RC5: O bit 5 do addr sinaliza que o a tecla esta mantida pressionada ou está sendo pressionada
void irrc_SetIntHandler(void (*int_func)(uint addr, uint cmd) ) {
	irrc_IntFunction = int_func;
}

// retorna pdTRUE caso recebeu um novo comando, quando ler essa função o flag de novo comando é limpo
// para RC5: O bit 5 do addr sinaliza que o a tecla esta mantida pressionada ou está sendo pressionada
int irrc_GetData(uint* sys, uint* cmd) {
	int nd = rc5.newData;
	rc5.newData = pdFALSE;
	*sys = rc5.system;
	*cmd = rc5.command;

	return nd;
}

// ################################################################################################################
// SERVIÇO DE INTERRUPÇÃO
// ################################################################################################################
void irrc_ISR(void) {
    ISR_ENTRY;															// Procedimento para entrada da interrupção

	TTC = 0;  // Reset timer

    // Se não houve mais interrupção da borda do sinal o contador vai estourar a 12ms para checar se houve fim de transmissão
    // a cada 12ms vamos checar se recebemos o código completo
	if (TIR & TIR_MR0I) {	// Checa se a interrupção gerada é do comparador MR0
		TIR |= TIR_MR0I;	// Limpa o flag da INT
		if (rxcmd) {
			rc5.command = rxcmd & 0x3f;
			rc5.system = (rxcmd >> 7) & 0x1f;
			rc5.newData = pdTRUE;
			if (irrc_IntFunction) irrc_IntFunction(rc5.system, rc5.command);
		}
		rxcmd = 0;
	} else {
		TIR = TIR_CR0I; 	// Limpa o flag da INT

		// Sempre se processa na borda de subida
		if (IR_GPIO_PIN & IR_PIN) { // checa se subida do pino, isto já filtra o starbit
			if (rxcmd == 0)  { // First pulse?
				low_time = IRRC_START_BIT;  // assume short low time
				high_time= IRRC_START_BIT;  // assume short high time
				half_bit = 1;  // assume half bit is true
			} else low_time = TCR0;  // rising, so capture low time

         	irrc_Decode();
		} else high_time = TCR0;  // falling, so capture high time

		#if (IRRC_USE_SNIFFER == pdON)
		if (IR_GPIO_PIN & IR_PIN) {
			pout_On();
			ir_Sniffer.raw[ir_Sniffer.idx] = TCR0;
		} else {
			pout_Off();
			ir_Sniffer.raw[ir_Sniffer.idx] = BIT_15 | TCR0;
		}
		if (ir_Sniffer.idx < IRRC_nSAMPLES) ir_Sniffer.idx++;
		#endif
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

