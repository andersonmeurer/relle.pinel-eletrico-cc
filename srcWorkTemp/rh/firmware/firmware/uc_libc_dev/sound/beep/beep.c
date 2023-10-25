#include "beep.h"

typedef struct {
	unsigned power:1;	// aciona ON/OFF
	uint type;			// Determina o tipo sonoro de alarme
	uint ntimes;		// Repeti��es do beep
	uint interval;		// Intervalo minimo entre cada emiss�o de comandos de beep
	uint intervalBeep;	// Intervalo entre as repeti��es
	uint intervalOn;	// Intervalo ligado
} tBeep, *pBeep;

static tBeep beep;

static void beep_SetType(uint type, uint adjNTimes);

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Inicializa o gerenciador de beep
// Parametro 1: Ponteiro para estrutura beep de controle, deve ser declaradum uma var struct tBeep externo a lib
//					para que esse gerenciador possa controlar o beep
// Retorna:		nada
// -------------------------------------------------------------------------------------------------------------------
void beep_Init(void) {
	beep_SetupPorts();
	beep.power = pdOFF; // sinaliza para contolar o alarme
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Liga o beep para emitir um tipo sonoro
// Parametro Type: tipo de som vai ser gerado
					// beepCONTINUOUS 			- Buzzer ligado direto
					// beepCONTINUOUS_ONOFF 	- Buzzer liga e desliga em periodos reguladores e mantem nessa condi��o at� ser desligado
					// beep1SHORTBEEP			- Buzzer � emitido 1 beep de teclado
					// beep3SHORTBEEP 			- Buzzer � emitido 3 beeps em intevalos curtos e depois � deligado
// Retorna:		nada
// -------------------------------------------------------------------------------------------------------------------
void beep_Play(uint type) {
	beep_SetType(type, pdTRUE);
	beep.power = pdON;
}

// retorna o que est� sendo tocado no buzzer
uint beep_Playing (void) {
	if (beep.power == pdON) return beep.type;
	else return 0;
}

// Para qualquer emiss�o de som
void beep_Stop(void) {
	beep.power = pdOFF;
}


// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Ajusta o controlador a emitir o tipo sonoro de alarme
// Parametro 1: tipo de som vai ser gerado
					// beepCONTINUOUS 			- Buzzer ligado direto
					// beepCONTINUOUS_ONOFF 	- Buzzer liga e desliga em periodos reguladores e mantem nessa condi��o at� ser desligado
					// beep1SHORTBEEP			- Buzzer � emitido 1 beep de teclado
					// beep3SHORTBEEP 			- Buzzer � emitido 3 beeps em intevalos curtos e depois � deligado
// Parametro 2: pdTRUE para atualizar var nTimes, sen�o coloque pdFALSE para n�o ser atualizada
// Retorna:		nada
// -------------------------------------------------------------------------------------------------------------------
static void beep_SetType(uint type, uint adjNTimes) {
	beep.type = type;

	switch(type) {
	case beep1SHORTBEEP:
		if (adjNTimes) beep.ntimes = 1; 			// um beep
		beep.intervalBeep = 50; 					// intervalo entre beeps em ms
		beep.intervalOn = 50; 						// intervalo com o beep ligado
		beep.interval = beepMINTIME_INTERVAL;
		break;
	case beep3SHORTBEEP:
		if (adjNTimes) beep.ntimes = 3; 			// tres beeps
		beep.intervalBeep = 50; 					// intervalo entre beeps em ms
		beep.intervalOn = 50; 						// intervalo ligado
		beep.interval = beepMINTIME_INTERVAL;
		break;
	case beepCONTINUOUS_ONOFF:
		beep.ntimes = 0;
		beep.intervalBeep = 900;
		beep.intervalOn = 100;
		beep.interval = 0;
		break;
	default: //beepCONTINUOUS
		beep.ntimes = 0;
		beep.intervalBeep = 0;
		beep.intervalOn = 0;
		beep.interval = 0;
	}
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Evento a ser chamado a cada 1ms para control de emiss�o sonora
// -------------------------------------------------------------------------------------------------------------------
void beep_Tick(void) {
	// retorna se o controlador de alarme estiver desligado
	if (beep.power == pdOFF) {
		beep_BuzzerOFF();
		if (beep.interval) beep.interval--;
		return;
	}

	switch(beep.type) {
	case beepCONTINUOUS:
		beep_BuzzerON();
		break;
	default:
		if (beep.intervalOn>1) {
			beep_BuzzerON();
			beep.intervalOn--;
		} else {
			beep_BuzzerOFF();
			if (beep.intervalBeep>1) {
				beep.intervalBeep--;
			} else {
				beep_SetType(beep.type, pdFALSE);
				if (beep.type != beepCONTINUOUS_ONOFF) {
					if (beep.ntimes>1) beep.ntimes--; else beep.power = pdOFF;
				}
			}
		}
	}
}
