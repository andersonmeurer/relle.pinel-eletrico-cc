#include "irrc_air_cond.h"
#include "vic.h"

#if (IRRC_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

// biblioteca escrita em 2016-04-15

// O receptor NEC utiliza um timer e a entrada CAP0.
//	Esta entrada ger interrupções tanto na borda de descida com na borda de subida,
//		na borda de descida o valor do timer atual é capturado para ser comparado na borda de subida.
//	Além disso, é preciso ler um pino GPIO para determinar se foi uma
//	interrupção de queda ou subida da porta.
//
//
//  IR RX -------- CAP0  ARM lpc
//            |--- PX_T

// Procedimento de adicionar um novo protocolo
// Coloque IRRC_USE_SNIFFER para pdON e analise os tempos de nível alto e baixo dos sinais caso não usar osciloscópio
//		no prompt limpe RAW, e transmite com controle remoto e depois veja os dados e colete o tempo min e max tanto para o pulso curto como longo
//			repita o processo para diversas teclas até ficar seguro dos tempos minimos e máximos
// Coloque IRRC_nBITS_RX no máximo permitido
// habilite modo debug e analise os recebimentos

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

typedef void (*irrcpFunc)(pAir);
volatile irrcpFunc irrc_IntFunction;
static pAir protocol;

typedef enum {
	stIDLE = 0,
	stSTART,
	stGET,
	stERROR,
} tNecSt;

#define IRRC_nBUFFER_RX (IRRC_nBITS_RX/8)+1
typedef struct {
	uint bitidx;
	u8 buff[IRRC_nBUFFER_RX];
} tRXcmd;

tRXcmd rxcmd;
static volatile tNecSt necST;
volatile int newData;

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

#if (IRRC_AIR_COND_USE == IRRC_DUGOLD)
// PROTOCOLO
// 	byte 4: 39 38 37 36 35 34 33 32
//			assinatura = 0xC

//	byte 3: | 31 30 29 28  | 27 26 25 24
//			|	timer hour | multiplier
//		timer hour: tempo em horas para desligar o aparelho
//		multiplier: quando 0 assune timer_hour, se for 1 assume timer_hour*10


//	byte 2: | 23    | 22 21 20     | 19 18 | 17 16
//			| sleep | timer on/off | swing | fan
//		fan: 0 valor máximo, valor aumentando a velocidade cai
//		swing: 0 aleta parada, valor 8 aleta movimentação continua
//		timer on/off: Liga ou desliga o timer para desligar o ar condicionado
//		sleep: 0 desliga o modo sleep, senão liga


//	byte 1: 15 14 13 12 | 11 10 09 08 |
//		              	| temperature |
//		temperature: valor 0 representa 15 graus, e vai até 30 graus


//	byte 0: 07 06 05 04 |   03   | 02 01 00 |
//                      | on/off |   mode   |
//		mode: 0 modo de refrigeração, 1 modo desmudificador, 2 modo aquecer, 3 modo ventilador, 4 modo ?
//		on/off: 0 aparelho ar cond desligado, 1 aparelho ligado
static void irrc_ProcessPacket(void) {
	u8* p = rxcmd.buff;
	u8 b;

	// apontar para a último byte recebido e compara com a assinatura
	if (rxcmd.buff[(rxcmd.bitidx/8)-1] != 0xc) {
		#if (IRRC_USE_DEBUG == pdON)
		plognp("ID error: %d [%d]"CMD_TERMINATOR, rxcmd.buff[(rxcmd.bitidx/8)-1], (rxcmd.bitidx/8)-1);
		#endif
		return; // cai fora se a assinatura não condiz
	}

	#if (IRRC_USE_DEBUG == pdON)
	plognp("RX: ");
	#endif

	int x; for (x=0; x<IRRC_nBUFFER_RX; x++) {
		#if (IRRC_USE_DEBUG == pdON)
		plognp("[ b%d: 0x%x ] ", x, *p);
		#endif

		b = *p++; // pegar conteúdo do byte e apontar para o próximo byte
		if (x==0) { // byte 0 do buffer
			protocol->on = (b&BIT_3)?pdON:pdOFF;
			protocol->mode = b&0x7;
		} else if (x==1) { // byte 1 do buffer
			protocol->temp = (b&0xf)+15;
		} else if (x==2) { // byte 2 do buffer
			protocol->swing = (b>>2)&0x3;
			protocol->fan = b & 0x3;
			protocol->timerOn = (b>>4)&0x3;
			protocol->sleep = (b>>7)&1;
		} else if (x==3) { // byte 3 do buffer
			protocol->timer = (b>>4)&0xf;
			if ((b&1)>0) protocol->timer*=10;
		}
	}

	#if (IRRC_USE_DEBUG == pdON)
	plognp("[nbits: %d]"CMD_TERMINATOR, rxcmd.bitidx);
	#endif

	if (irrc_IntFunction) irrc_IntFunction(protocol);
	newData = pdTRUE;
}
#endif // #if (IRRC_AIR_COND_USE == IRRC_DUGOLD)

#if (IRRC_AIR_COND_USE == IRRC_JAPANESE)

// PROTOCOLO
//	byte 0: 07 06 05 04 03 02 01 00
//			manufacturers code = 0x23

//	byte 1: 15 14 13 12 11 10 09 08
//			manufacturers code = 0xCB

//	byte 2: 23 22 21 20 | 19 18 17 16
//			  parity      system code   = 0x26

//	byte 3: 31 30 29 28 27 26 25 24
//			product code = 0x1

// 	byte 4: 39 38 37 36 35 34 33 32
//			function code = 0x0

//	byte 5: 07 06 05 |  04  |  03     |   02   | 01 00
//                   | TON  |  TOFF   | on/off |
//		on/off: 0 aparelho ar cond desligado, 1 aparelho ligado
//		TON: Flag para ajuste de timer on, ver o valor no byte9
//		TOFF: Flag para ajuste de timer off, ver o valor no byte10
// 			OBS: Para esta função se habilitada no controle remoto a hora deve estar ajustada corretamente nele

//	byte 6: 07 06 05 04 | 03 02 01 00 |
//                      |    mode     |
//		mode: 0x1 modo aquecer, 0x2 modo desmudificador, 0x3 refrigeração, 0x7 modo ventilador, 0x8 auto

//	byte 7: 7 6 5 4 |   3 2 1 0     |
//						temperature
//		temperature: valor 0 corresponde a 31 graus Celcius, valor 0xF corresponde a 16 graus Celcius

//	byte 8: 7 6 |       5 4 3       |    2 1 0    |
//				| pos aleta e swing	|  fan/sleep  |
//		fan: 	0x0: modo auto
//				0x1: modo sleep e velocidade baixa,
//				0x2: velocidade baixa
//				0x3: velcidade média
//				0x5: velocidade alta

//		swing: 	0x1: Aleta na horizontal, direcionando o vento totalmente para o teto
//				0x2 a 0x4: valores de 2 a 4 aletas entre horizontal e vertical
//				0x5: Aleta na vertical, direcionando o vento totalmente para baixo
//				0x7: Liga o swing da aleta, aleta indo da horizontal para vertical continualmente

// 	byte 9:  7 6 5 4 3 2 1 0
//				Horário do timer on (*1)

// 	byte 10: 7 6 5 4 3 2 1 0
//				Horário do timer off (*1)

// 	(*1) Valor em minutos multiplicado por 10
//		O horário é enviado do controle remoto ao receptor já contando com a hora atual,
//			considerando que o receptor não tenha RTC

// 	byte 11: 7 6 5 4 3 2 1 0
// 	byte 12: 7 6 5 4 3 2 1 0

// 	byte 13: 7 6 5 4 3 2 1 0
//			      CRC

// OBS: Não vamos calcular CRC e nem comparar paridade, pois checaremos os 4 primeiros bytes que sempre são fixo
static void irrc_ProcessPacket(void) {
	u8* p = rxcmd.buff;
	u8 b;

	// checa a assinatira do controle remoto
	u32 pid = rxcmd.buff[3];
	pid = (pid << 8) | rxcmd.buff[2];
	pid = (pid << 8) | rxcmd.buff[1];
	pid = (pid << 8) | rxcmd.buff[0];

	if (pid != 0x126cb23) {
		#if (IRRC_USE_DEBUG == pdON)
		plognp("ID error: 0x%x"CMD_TERMINATOR, pid);
		#endif
		return; // cai fora que a assinatura não condiz
	}

	#if (IRRC_USE_DEBUG == pdON)
	plognp("RX: ");
	#endif

	int f = 0;
	int x; for (x=0; x<IRRC_nBUFFER_RX; x++) {
		#if (IRRC_USE_DEBUG == pdON)
		plognp("b%dx%02x ", x, *p);
		#endif

		b = *p++; // pegar conteúdo do byte e apontar para o próximo byte
		if (x==5) { // byte 5 do buffer
			protocol->on = (b&BIT_2)?pdON:pdOFF;
			f = b;
		} else if (x==6) { // byte 6 do buffer
			protocol->mode = b&0x7;
		} else if (x==7) { // byte 7 do buffer
			protocol->temp = 31-(b&0xf);
		} else if (x==8) { // byte 8 do buffer
			if ( (b&07) == 1) {
				protocol->sleep = pdON;
				protocol->fan = 1;
			} else {
				protocol->sleep = pdOFF;
				protocol->fan = b & 0x7;
			}
			protocol->swing = (b>>3)&0x7;
		} else if ( (x==9) && (f&BIT_4) ) { // byte 9 do buffer
			protocol->timerOn = b*10;
		} else if ( (x==10) && (f&BIT_3) ){ // byte 10 do buffer
			protocol->timerOff = b*10;
		}
	}

	#if (IRRC_USE_DEBUG == pdON)
	plognp(" [nbits: %d]"CMD_TERMINATOR, rxcmd.bitidx);
	#endif

	if (irrc_IntFunction) irrc_IntFunction(protocol);
	newData = pdTRUE;
}
#endif // #if (IRRC_AIR_COND_USE == IRRC_JAPANESE)

static void irrc_InitBuffRX(void) {
	rxcmd.bitidx = 0;
	int x; for(x=0;x<IRRC_nBUFFER_RX;x++) rxcmd.buff[x] = 0;
}

// ################################################################################################################
// GLOBAL
// ################################################################################################################
int irrc_Init(pAir p) {
	#if (IRRC_USE_SNIFFER == pdON)
	pout_Init();
	ir_Sniffer.idx = 0;
	#endif

	protocol = p;

	necST = stIDLE;
	irrc_InitBuffRX();
	newData = pdFALSE;
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

void irrc_SetIntHandler(void (*int_func)(pAir p) ) {
	irrc_IntFunction = int_func;
}

// retorna pdTRUE caso recebeu um novo comando, quando ler essa função o flag de novo comando é limpo
//int irrc_GetData(uint* addr, uint* data) {
//	int nd = newData;
//	newData = pdFALSE;
//	*addr = protocol.addr;
//	*data = protocol.data;
//
//	return nd;
//}

// ################################################################################################################
// SERVIÇO DE INTERRUPÇÃO
// ################################################################################################################
// se for adicionar o bit 1 vamos ligar o bit no buffer, senão somente apontamos para o próximo bit porque o buffer
//	já está limpo com valores zeros
static void	irrc_ShiftBit(int bit) {

	if (bit) { // se for adicionar o bit 1 vamos ligar o bit no buffer
		u8* p = rxcmd.buff;
		u8 b = (u8)(rxcmd.bitidx%8); 	// Apontar o bit do byte apontado pelo idxbit
		p += (u8)(rxcmd.bitidx/8); 		// Apontar qual byte o idxbit está apontando
		*p |= _BV(b);					// Ligar bit
	}

	rxcmd.bitidx++;
}

void irrc_ISR(void) {
    ISR_ENTRY;															// Procedimento para entrada da interrupção

	TTC = 0;  // Reset timer

    // Se não houve mais interrupção da borda do sinal o contador vai estourar a 12ms para checar se houve fim de transmissão
    // a cada 12ms vamos checar se recebemos o código completo
	if (TIR & TIR_MR0I) {	// Checa se a interrupção gerada é do comparador MR0
		TIR |= TIR_MR0I;	// Limpa o flag da INT
		if (necST == stGET) irrc_ProcessPacket();

		irrc_InitBuffRX();
		necST = stIDLE;
	} else {
		TIR = TIR_CR0I; 	// Limpa o flag da INT

		static uint space = 0;
		// Sempre se processa na borda de subida
		if (IR_GPIO_PIN & IR_PIN) { // checa se subida do pino
			// estado de espera de recepção do pimeiro pulso, esperando o startbit
			if (necST == stIDLE) {
				// vamos checar se a largura do pulso de starbit está dentro dos limites para padrão NEC
				if ( (TCR0 > IRRC_START_BIT_MIN) && (TCR0 < IRRC_START_BIT_MAX) )
					necST = stSTART;
				else {
					necST = stERROR;
					#if (IRRC_USE_DEBUG == pdON)
					plognp("error startbit %d min:%d max:%d"CMD_TERMINATOR, space, IRRC_START_BIT_MIN, IRRC_START_BIT_MAX);
					#endif
				}

			// checa se passamos o espaçador após o starbit
			} else if (necST == stSTART) {
				if ( (space > IRRC_START_SPACE_MIN) && (space < IRRC_START_SPACE_MAX) )
					necST = stGET;
				else {
					necST = stERROR;
					#if (IRRC_USE_DEBUG == pdON)
					plognp("error space %d min:%d max:%d"CMD_TERMINATOR, space, IRRC_START_SPACE_MIN, IRRC_START_SPACE_MAX);
					#endif
				}

			// capturar os pulsos subsequentes
			} else if (necST == stGET) {
				// vamos checar se é um pulso de bit 1 (distancia longa)
				if ( (space > IRRC_HIGH_BIT_MIN) && (space < IRRC_HIGH_BIT_MAX)) {
					irrc_ShiftBit(1); // shift system add 1
				// vamos checar se é um pulso de bit 0 (distancia curta)
				} else if ( (space > IRRC_LOW_BIT_MIN) && (space < IRRC_LOW_BIT_MAX)) {
					irrc_ShiftBit(0); // shift system add 0
				} else {
					necST = stERROR;
					#if (IRRC_USE_DEBUG == pdON)
					plognp("erro rx bit space: %d bitidx:%d"CMD_TERMINATOR, space, rxcmd.bitidx);
					#endif
				}
			}
		} else space = TCR0;

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
