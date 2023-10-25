#include "servus.h"


// COMPORTAMENTO DO SERVOS - resolução em 10uS
//                       TH             TL
// Pulso para o servo: ######________________________
//
// Tempos TH	Servo normal	Servo de modo continuo
// 1000ms		0º 				Máxima velocidade sentido antihorário
// 1250ms		45º 			Média velocidade sentido antihorário
// 1500ms		90º				Parado
// 1750ms		135º 			Média velocidade sentido horário
// 2000ms		180º			Máxima velocidade sentido horário
// TL = 20ms


// COMPORTAMENTO DO SENSORES ULTRASOM
//          (10uS) trigger      (200uS) Tempo de   	    (200uS) Tempo que o  	(100us) Resposta 	(38ms) Tempo para detecção de obstáculo
//			 para o módulo	  excitação do ultrasom      módulo espera para  	 de aceitação		   Quando encontrar obstáculo pino eco = 0
//							   (8 pulsos de 40Khz)	  começar a leiturado eco      do trigger
// --------|--------------|-------------------------|-------------------------|-------------------|--------------------------------------------| 	>> Periódo. Recomendado o intervalo minimo de 50ms entre os periodos
// _________HHHHHHHHHHHHHH_____________________________________________________________________________________________________________________		>> Trigger
// ________________________XXXXXXXXXXXXXXXXXXXXXXXXX___________________________________________________________________________________________		>> 8 Pulsos de 40KHz enitidos internamente do módulo hc-sr04
// ___________________________________________________________________________HHHHHHHHHHHHHHHHHHHH_____________________________________________		>> Eco sem encontrar obstáculo (sem eco até 38ms)
// ___________________________________________________________________________HHHHHHHHHHHHHHHHHHHHHHHHHHHH_____________________________________		>> Eco encontrado obstáculo a 2 cm (eco a 150us)
// ___________________________________________________________________________HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH_		>> Eco encontrado obstáculo a 2 metros ( eco a 25ms)
//		O pino echo elevará após a exicitação do ultrasom (8 pulsos de 40Khz) e ficará em alta pelo menos em 100us

// ATENÇÃO: O período de medição no hc-sr04 é de 50ms, se houver outro trigger dentro desse intervalo o módulo ignorará a esse pedido


#if (US04_USES == pdON)
typedef struct {
	unsigned modeContinuous:1;				// Sinaliza para ficar fazendo medição de forma continua ou não
	volatile unsigned measure:1; 			// Sinaliza quando começar as medições
	volatile unsigned done:1;				// Sinaliza quando as medições estão prontas
	volatile int Fdetected;
	volatile int Ftriggerok;
	volatile uint FCont;					// Contador micro segundos para o gerenciador do US
	volatile int FModule;					// Guarda o módulo atual para medição
	volatile uint measurements[US04_N_MODULE];
} tUs04;
static tUs04 us04;
static void us04_Trigger(uint module, uint action);
static uint us04_Echo(uint module);
#endif // #if (US04_USES == pdON)

typedef struct {
	unsigned enable:1;
	uint pos;
	volatile s32 count;
} tServo;

typedef struct {
	tServo servo[SERVO_N_MODULE];
	volatile s32 intervalUs;
} tServoCtrl;
static tServoCtrl servoCtrl;

static void servus_CallSytem(void);

void servus_Init(void) {
	servoCtrl.intervalUs = 0;
	uint x;	for (x=0;x<SERVO_N_MODULE;x++) {
		servoCtrl.servo[x].count= 1500;
		servoCtrl.servo[x].pos= 1500;
		servoCtrl.servo[x].enable = 1;
	}

	#if (US04_USES == pdON)
	us04_SetupPortsModule1();
	#if (US04_N_MODULE > 1 )
	us04_SetupPortsModule2();
	#endif
	#if (US04_N_MODULE > 2 )
	us04_SetupPortsModule3();
	#endif
	#if (US04_N_MODULE > 3 )
	us04_SetupPortsModule4();
	#endif

	us04.modeContinuous = 0;	// Não medir de forma continua
	us04.measure = 0; 			// Sinaliza para não fazer as medições
	us04.done = 0;				// Nenhuma medição foi realizada ainda
	us04.FModule = 1;			// Sinaliza que o módulo 1 será o primeiro a ser medido
	us04.FCont = 0;				// Contador em micro segundos do CallSystem do US

	for (x=0;x<US04_N_MODULE;x++) {
		us04.measurements[x] = 0;
		us04_Trigger(x, pdOFF); // Desligar sinais dos triggers dos módulos
	}

	us04.Fdetected = 0;		// Sinaliza para não há objeto detectado
	us04.Ftriggerok = 0;		// Inicializar pedido de trigger

	#if US04_USE_DEBUG == pdON
	US04_DBGTRIGGER_DIR |= US04_DBGTRIGGER;
	US04_DBGTRIGGER_CLR = US04_DBGTRIGGER;
	US04_DBGTRIGGEROK_DIR |= US04_DBGTRIGGEROK;
	US04_DBGTRIGGEROK_CLR = US04_DBGTRIGGEROK;
	US04_DBGDETECTED_DIR |= US04_DBGDETECTED;
	US04_DBGDETECTED_CLR = US04_DBGDETECTED;
	#endif
	#endif //#if (US04_USES == pdON)

	servo1_SetupPorts();
	#if (SERVO_N_MODULE > 1)
	servo2_SetupPorts();
	#endif
	#if (SERVO_N_MODULE > 2)
	servo3_SetupPorts();
	#endif
	#if (SERVO_N_MODULE > 3)
	servo4_SetupPorts();
	#endif
	#if (SERVO_N_MODULE > 4)
	servo5_SetupPorts();
	#endif
	#if (SERVO_N_MODULE > 5)
	servo6_SetupPorts();
	#endif

	servus_TimeInit();
	servus_SetIntHandler(servus_CallSytem);
}

void servus_Finish(void) {
	servus_TimeStop();		// Desligo o timer para não gerar int para servus_CallSytem
}


#if (US04_USES == pdON)

// Inicializa a medição com ultrasom com modo continuo ou uma única medição
// parametro modeCont:  pdFALSE: sinaliza para fazer uma única medição
//						pdTRUE: Sinaliza para medir de forma continua
void us04_StartMeasure(uint modeCont) {
	us04.modeContinuous = modeCont;
	if (us04.measure) return;
	us04.done = 0; 			// Sinaliza que a conversão não está pronta
	us04.measure = 1; 		// Sinaliza para fazer as medições
}

// para qualquer medição seja no modo continuo ou não
void us04_StopMeasure(void) {
	us04.modeContinuous = 0;
	us04.measure = 0; 		// Sinaliza para fazer as medições
}

// Retorna em CM o valor lido pelo módulo US
// Parametro servoSel: Sinaliza qual o servo a receber a nova posição, valor de 1 a US04_N_MODULE
// Retorna: 0: Se não encontrou o ostáculo, senão retorna com o valor em centimetros
//			errDEVICE_NOT_FOUND: Se o módulo não existe
//			errDEVICE_VALUE_NOT_READY: Se o módulo ainda não terminou a medição
int us04_ReadMeasureCM(uint module) {
	if ( (module == 0) || (module > US04_N_MODULE)) return errDEVICE_NOT_FOUND;
	if (us04.done == 0) return errDEVICE_VALUE_NOT_READY;
	if (us04.measurements[module-1] == 0) return 0;
	return (us04.measurements[module-1]/58);
}

// Ligar ou desligar o trigger do módulo
static void us04_Trigger(uint module, uint action) {
	if (module == 1) us04_TriggerModule1(action);
	#if (US04_N_MODULE > 1 )
	if (module == 2) us04_TriggerModule2(action);
	#endif
	#if (US04_N_MODULE > 2 )
	if (module == 3) us04_TriggerModule3(action);
	#endif
	#if (US04_N_MODULE > 3 )
	if (module == 4) us04_TriggerModule4(action);
	#endif
}

// retorna 1 se o pino de eco = 1 senão retorna 0
static uint us04_Echo(uint module) {
	uint result = 0;
	if ((module == 1) && (us04_EchoModule1() > 0)) result = 1;
	#if (US04_N_MODULE > 1 )
	if ((module == 2) && (us04_EchoModule2() > 0)) result = 1;
	#endif
	#if (US04_N_MODULE > 2 )
	if ((module == 3) && (us04_EchoModule3() > 0)) result = 1;
	#endif
	#if (US04_N_MODULE > 3 )
	if ((module == 4) && (us04_EchoModule4() > 0)) result = 1;
	#endif

	return result;
}
#endif //#if (US04_USES == pdON)

// Determina a posição do servo motor
// Parametro servoSel: Sinaliza qual o servo a receber a nova posição, valor de 1 a SERVO_N_MODULE
// Retorna: pdPASS se o comando foi executado com sucesso
//			errDEVICE_NOT_FOUND: Se o servo não existe
//			errDEVICE_VALUE_INVALID: Valor inválido
int servo_SetPos(uint servo, int us) {
	if ( (servo == 0) || (servo > SERVO_N_MODULE) ) return errDEVICE_NOT_FOUND;
	if ( (us<100) || (us > 3000)) return errDEVICE_VALUE_INVALID;
	servoCtrl.servo[servo-1].pos = us;

	return pdPASS;
}

// Habilita não a saida PWM para o servo
// Parametro servoSel: Sinaliza qual o servo a receber a nova posição, valor de 1 a SERVO_N_MODULE
// Retorna: pdPASS se o comando foi executado com sucesso
//			errDEVICE_NOT_FOUND: Se o servo não existe
int servo_Move(uint servo, uint en) {
	if ( (servo == 0) || (servo > SERVO_N_MODULE) ) return errDEVICE_NOT_FOUND;
	servoCtrl.servo[servo-1].enable = (en == 0)?0:1;
	servoCtrl.servo[servo-1].count = 0;
	return pdPASS;
}

// Tick de sistema para controle dos servos e medições dos módulos de ultrasom.
//	Esta função será chamada a cada 10us pelo timer do uC
static void servus_CallSytem(void) {
	uint x;

	// ######################################################################################################
	// CONTROLE DOS SERVOS
	if (servoCtrl.intervalUs <= 0) {
	 	servoCtrl.intervalUs = 20000;
	 	for (x=0;x<SERVO_N_MODULE;x++) servoCtrl.servo[x].count = servoCtrl.servo[x].pos;
	 } else servoCtrl.intervalUs -= 10;

	if ( (servoCtrl.servo[0].count > 0) && (servoCtrl.servo[0].enable) ) {
			servoCtrl.servo[0].count -= 10;
			SERVO1_SET = SERVO1;
	} else	SERVO1_CLR = SERVO1;

	#if (SERVO_N_MODULE > 1)
	if ( (servoCtrl.servo[1].count > 0) && (servoCtrl.servo[1].enable) ) {
			servoCtrl.servo[1].count -= 10;
			SERVO2_SET = SERVO2;
	} else	SERVO2_CLR = SERVO2;
	#endif
	#if (SERVO_N_MODULE > 2)
	if ( (servoCtrl.servo[2].count > 0) && (servoCtrl.servo[2].enable) ) {
			servoCtrl.servo[2].count -= 10;
			SERVO3_SET = SERVO3;
	} else	SERVO3_CLR = SERVO3;

	#endif
	#if (SERVO_N_MODULE > 3)
	if ( (servoCtrl.servo[3].count > 0) && (servoCtrl.servo[3].enable) ) {
			servoCtrl.servo[3].count -= 10;
			SERVO4_SET = SERVO4;
	} else	SERVO4_CLR = SERVO4;
	#endif
	#if (SERVO_N_MODULE > 4)
	if ( (servoCtrl.servo[4].count > 0) && (servoCtrl.servo[4].enable) ) {
			servoCtrl.servo[4].count -= 10;
			SERVO5_SET = SERVO5;
	} else	SERVO5_CLR = SERVO5;
	#endif
	#if (SERVO_N_MODULE > 5)
	if ( (servoCtrl.servo[5].count > 0) && (servoCtrl.servo[5].enable) ) {
			servoCtrl.servo[5].count -= 10;
			SERVO6_SET = SERVO6;
	} else	SERVO6_CLR = SERVO6;
	#endif


	// ######################################################################################################
	// CONTROLE DO MÓDULOS ULTRASOM
	#if (US04_USES == pdON)
	#if US04_USE_DEBUG == pdOFF
	if (us04.measure == 0) return;
	#endif

	// OS VALORES SÃO BASEADOS NA CHAMADA DO TICK DE ULTRASOM A CADA 10uS
	// Passo 1 (tempo 0uS)			: Desligar trigger do módulo e manter por 10uS
	// Passo 2 (tempo 10uS)			: Ligar trigger do módulo e manter por 10uS
	// Passo 3 (tempo 20uS)			: Desligar o trigger e dar inicio do tempo para exicitação do ultrasom (200us) e tempo de resposta (200uS)
	// Passo 4 (tempo 420uS a 38ms)	: Inicio da verificação do sinal do eco. Se a primeira leitura for 0 é porque o módulo não aceitou o pedido do trigger
	// Passo 5 (tempo MIN 60ms)		: Reiniciar o gerenciador para a próxima leitura. Acrescentado um tempo de

	// EMITIR SINAL DE TRIGGER. Iniciar em 1 para que os triggers fiquem pelo menos 10us em nível baixo antes das medições
	if (us04.FCont == 1) {
		us04_Trigger(us04.FModule, pdON); 				// Emitir sinal de trigger para o módulo atual
		if (us04.modeContinuous == 0) us04.done = 0;	// Medições não pronta
		us04.measurements[us04.FModule-1] = 0;			// limpar distancia registrada do módulo atual

		#if US04_USE_DEBUG == pdON
		US04_DBGTRIGGER_SET = US04_DBGTRIGGER;
		#endif

	// DESLIGAR TRIGGER E ESPERAR PELA EXICITAÇÃO DO ULTRASOM
	} else if (us04.FCont == 2) {
		us04_Trigger(us04.FModule, pdOFF); 		// Desligar sinal de trigger

		#if US04_USE_DEBUG == pdON
		US04_DBGTRIGGER_CLR = US04_DBGTRIGGER;
		#endif

	// INICIO DA VERIFICAÇÃO DA DETECÇÃO DE OBTÁCULO (entre 420us a 38000us)
	} else if ( (us04.FCont > 42) && (us04.FCont <= 3800) ) {
		if (us04.Ftriggerok == 0) 										// Checa se o módulo aceitou o pedido do trigger
			us04.Ftriggerok = us04_Echo(us04.FModule); 						// Verificar se o módulo aceitou o trigger (echo = 1)
		else if (us04.Fdetected == 0) { 									// Checa se ainda não registramos a distancia do objeto detectado
			if (us04_Echo(us04.FModule) == 0) { 					// Checa se há objeto detectado (echo = 0)
				us04.measurements[us04.FModule-1] = (us04.FCont-41)*10;		// Registra o tempo da deteção.
																		// 		Diminuir por 40 (400uS) pois cotarmos a partir da excitação e tempo de inicio de leitura do ultrasom
																		// 		Multiplicar por 10 porque estamos contando em 10 e 10 us
				us04.Fdetected = 1;											// Trava a leitura
			}
		}

		#if US04_USE_DEBUG == pdON
		if (us04.Ftriggerok == 1)
				US04_DBGTRIGGEROK_SET = US04_DBGTRIGGEROK;
		else	US04_DBGTRIGGEROK_CLR = US04_DBGTRIGGEROK;

		if (us04.Fdetected == 1)
				US04_DBGDETECTED_SET = US04_DBGDETECTED;
		else 	US04_DBGDETECTED_CLR = US04_DBGDETECTED;
		#endif
	}
	#if US04_USE_DEBUG == pdON
	else if (us04.FCont > 3800) {
		US04_DBGTRIGGEROK_CLR = US04_DBGTRIGGEROK;
		US04_DBGDETECTED_CLR = US04_DBGDETECTED;
	}
	#endif

	// ESPERAR O TEMPO PARA QUE O MÓDULO FINALIZE O PROCESSO DE MEDIÇÃO (MIN 50MS, MAS ESTAMOS DANDO MAIS 10MS DE TOLERANCIA)
	if (us04.FCont == US04_END_MEASURE) {
		// Ir para o próximo módulo ou finalizar a leitura
		if (us04.FModule == US04_N_MODULE) {
			us04.FModule = 1;		// Sinaliza para meidor módulo 1
			us04.done = 1;		// Sinaliza que a medição está completa
			// Caso não for medição de forma continua, sinaliza para parar as medições e esperar pelo próximo pedido de medição
			if (us04.modeContinuous == 0) us04.measure = 0;
		} else us04.FModule++;		// Sinaliza para medir o próximo módulo

		us04.FCont = 0;
		us04.Fdetected = 0;;		// Sinaliza para não há objeto detectado
		us04.Ftriggerok = 0;		// Inicializar pedido de trigger
	} else
		us04.FCont++;
	#endif //#if (US04_USES == pdON)
}

