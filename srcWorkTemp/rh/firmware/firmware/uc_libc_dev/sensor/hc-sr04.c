#include "hc-sr04.h"

static pUs04 us04;
static volatile int	Fdetected;
static volatile int	Ftriggerok;
volatile uint FCont;					// Contador micro segundos para o gerenciador do US
volatile int FModule;					// Guarda o módulo atual para medição

static void us04_CallSytem(void);

void us04_Init( pUs04 us ) {
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

	us04 = us;
	us04->measure = 0; 		// Sinaliza para não fazer as medições
	us04->done = 0;			// Nenhuma medição foi realizada ainda
	FModule = 1;			// Sinaliza que o módulo 1 será o primeiro a ser medido
	FCont = 0;				// Contador em micro segundos do CallSystem do US

	uint x;
	for (x=0;x<US04_N_MODULE;x++) {
		us->measurements[x] = 0;
		us04_Trigger(x, pdOFF); // Desligar sinais dos triggers dos módulos
	}

	us04_TimeInit();
	us04_SetIntHandler(us04_CallSytem);

	Fdetected = 0;		// Sinaliza para não há objeto detectado
	Ftriggerok = 0;		// Inicializar pedido de trigger

	#if US04_USE_DEBUG == pdON
	US04_DBGTRIGGER_DIR |= US04_DBGTRIGGER;
	US04_DBGTRIGGER_CLR = US04_DBGTRIGGER;
	US04_DBGTRIGGEROK_DIR |= US04_DBGTRIGGEROK;
	US04_DBGTRIGGEROK_CLR = US04_DBGTRIGGEROK;
	US04_DBGDETECTED_DIR |= US04_DBGDETECTED;
	US04_DBGDETECTED_CLR = US04_DBGDETECTED;
	#endif
}

// retorna em CM o valor lido pelo módulo US
uint us04_ReadMeasureCM(int module) {
	if (module > US04_N_MODULE) return 0;
	if (us04->measurements[module-1] == 0) return 0;
	return (us04->measurements[module-1]/58);
}

void us04_StartMeasure(void) {
	if (us04->measure) return;
	us04_TimeStop();		// Desligo o timer para não gerar int para us04_CallSytem
	us04->done = 0; 		// Sinaliza que a conversão não está pronta
	us04->measure = 1; 		// Sinaliza para fazer as medições
	us04_TimeEnable();		// Ligar timer para gerar int para us04_CallSytem
}

// Ligar ou desligar o trigger do módulo
void us04_Trigger(uint module, uint action) {
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
uint us04_Echo(uint module) {
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


// Tick de sistema para controle de medição dos módulos de ultrasom.
//	Esta função será chamada a cada 10us pelo timer do uC
static void us04_CallSytem(void) {
	#if US04_USE_DEBUG == pdOFF
	if (us04->measure == 0) return;
	#endif

	// OS VALORES SÃO BASEADOS NA CHAMADA DO TICK DE ULTRASOM A CADA 10uS
	// Passo 1 (tempo 0uS)			: Desligar trigger do módulo e manter por 10uS
	// Passo 2 (tempo 10uS)			: Ligar trigger do módulo e manter por 10uS
	// Passo 3 (tempo 20uS)			: Desligar o trigger e dar inicio do tempo para exicitação do ultrasom (200us) e tempo de resposta (200uS)
	// Passo 4 (tempo 420uS a 38ms)	: Inicio da verificação do sinal do eco. Se a primeira leitura for 0 é porque o módulo não aceitou o pedido do trigger
	// Passo 5 (tempo MIN 60ms)		: Reiniciar o gerenciador para a próxima leitura. Acrescentado um tempo de

	// EMITIR SINAL DE TRIGGER. Iniciar em 1 para que os triggers fiquem pelo menos 10us em nível baixo antes das medições
	if ( FCont == 1) {
		us04_Trigger(FModule, pdON); 		// Emitir sinal de trigger para o módulo atual
		us04->done = 0;						// Medições não pronta
		us04->measurements[FModule-1] = 0;	// limpar distancia registrada do módulo atual

		#if US04_USE_DEBUG == pdON
		US04_DBGTRIGGER_SET = US04_DBGTRIGGER;
		#endif

	// DESLIGAR TRIGGER E ESPERAR PELA EXICITAÇÃO DO ULTRASOM
	} else if ( FCont == 2) {
		us04_Trigger(FModule, pdOFF); 		// Desligar sinal de trigger

		#if US04_USE_DEBUG == pdON
		US04_DBGTRIGGER_CLR = US04_DBGTRIGGER;
		#endif

	// INICIO DA VERIFICAÇÃO DA DETECÇÃO DE OBTÁCULO (entre 420us a 38000us)
	} else if ( ( FCont > 42) && (FCont <= 3800) ) {
		if (Ftriggerok == 0) 										// Checa se o módulo aceitou o pedido do trigger
			Ftriggerok = us04_Echo(FModule); 						// Verificar se o módulo aceitou o trigger (echo = 1)
		else if (Fdetected == 0) { 									// Checa se ainda não registramos a distancia do objeto detectado
			if (us04_Echo(FModule) == 0) { 							// Checa se há objeto detectado (echo = 0)
				us04->measurements[FModule-1] = (FCont-41)*10;		// Registra o tempo da deteção.
																		// 		Diminuir por 40 (400uS) pois cotarmos a partir da excitação e tempo de inicio de leitura do ultrasom
																		// 		Multiplicar por 10 porque estamos contando em 10 e 10 us
				Fdetected = 1;											// Trava a leitura
			}
		}

		#if US04_USE_DEBUG == pdON
		if (Ftriggerok == 1)
				US04_DBGTRIGGEROK_SET = US04_DBGTRIGGEROK;
		else	US04_DBGTRIGGEROK_CLR = US04_DBGTRIGGEROK;

		if (Fdetected == 1)
				US04_DBGDETECTED_SET = US04_DBGDETECTED;
		else 	US04_DBGDETECTED_CLR = US04_DBGDETECTED;
		#endif
	}
	#if US04_USE_DEBUG == pdON
	else if (FCont > 3800) {
		US04_DBGTRIGGEROK_CLR = US04_DBGTRIGGEROK;
		US04_DBGDETECTED_CLR = US04_DBGDETECTED;
	}
	#endif

	// ESPERAR O TEMPO PARA QUE O MÓDULO FINALIZE O PROCESSO DE MEDIÇÃO (MIN 50MS, MAS ESTAMOS DANDO MAIS 10MS DE TOLERANCIA)
	if (FCont == US04_END_MEASURE) {
		// Ir para o próximo módulo ou finalizar a leitura
		if (FModule == US04_N_MODULE) {
			FModule = 1;		// Sinaliza para meidor módulo 1
			us04->done = 1;		// Sinaliza que a medição está completa
			us04->measure = 0;	// Sinaliza para parar as medições e esperar o próximo pedido
		} else FModule++;		// Sinaliza para medir o próximo módulo

		FCont = 0;
		Fdetected = 0;;		// Sinaliza para não há objeto detectado
		Ftriggerok = 0;		// Inicializar pedido de trigger
	} else
		FCont++;
}
