#include "arm.h"
#include "_config_cpu_.h"

#if defined(USE_EXT_SDRAM)
#if defined(cortexm3)
void xdelay(u32 delayval);
void xdelay(u32 delayval) {
	asm volatile (
		"L_LOOPUS_%=: 		\r\n\t" \
		"subs	%0, %0, #1 	\r\n\t" \
		"bne	L_LOOPUS_%=	\r\n\t" \
		:  /* no outputs */ : "r" (delayval)
	);
}

#define INS_DIV 3 // 3 instruções no loop em assembler
#define xdelay_us(delayval) xdelay( (u32)( (CCLK) / (1000000UL/delayval) /3) )
#define xdelay_ms(delayval) xdelay( (u32)( (CCLK) / (1000000UL/delayval) /3)*1000UL )
#endif

#define SDRAM_DELAY_1MS 	1
#define SDRAM_DELAY_100MS 100
#define SDRAM_DELAY_200MS 200
#define SDRAM_TUNE_FAC 1
#endif

// COMPILADOR YAGARTO REQUER FUNÇÕES DE CHAMADA DE SISTEMAS (syscalls) EM  SUA LIBC.A
#if defined (COMPILER_TYPE_YAGARTO)
void _exit(int n __attribute__ ((unused)) ) {
    while(1); // Loop until reset
}
#endif


#if defined(cortexm3) && defined (LPC1788)
// ########################################################################################
// FUNÇÕES DE SELEÇÃO DE PINOS DO ARM CORTEX-M3
// ########################################################################################
// ########################################################################################

// -----------------------------------------------------------------------------------------------
// Descrição:	Ajusta a função do pino
// Parametros:	addrPort: Endereço da porta
// 					PINSEL_P0 para porta 0
// 					PINSEL_P1 para porta 1 e assim sucessivamente
//				numPin: Número do pino da porta, valores de 0 a 31
//				funcnum: Número da função da porta, valores de 0 (GPIO) a quantidade de funções do ARM
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------
void pinsel_setPinFunc(reg32 *addrPort, u8 numPin, u8 numFunc) {
	addrPort+=numPin;					// Captura o endereço do registrador do determinado pino
	*addrPort &= ~(PINSEL_MASK); 		// Limpa o campo de funções no registrador do pino
	*addrPort |= numFunc &PINSEL_MASK;	// Atribui a nova função do pino
}

// -----------------------------------------------------------------------------------------------
// Descrição:	Ajusta o modo de resistor de cada pino
// Parametros:	addrPort: Endereço da porta
// 					PINSEL_P0 para porta 0
// 					PINSEL_P1 para porta 1 e assim sucessivamente
//				numPin: Número do pino da porta, valores de 0 a 31
//				pinMode: Modo do pino
//					PINSEL_PINMODE_INACTIVE: Sem resistor pull-up ou pull-down
//					PINSEL_PINMODE_PULLUP  : Internal pull-up resistor
//					PINSEL_PINMODE_PULLDOWN: Internal pull-down resistor
//					PINSEL_PINMODE_REPEATER: Repeater mode.
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------
void pinsel_setResistorMode(reg32 *addrPort, u8 numPin, u8 pinMode) {
	addrPort+=numPin;							// Captura o endereço do registrador do determinado pino
	*addrPort &= ~(PINSEL_PINMODE_MASK); 		// Limpa o campo de modos no registrador do pino
	*addrPort |= pinMode&PINSEL_PINMODE_MASK;	// Atribui o novo modo do pino
}

// -----------------------------------------------------------------------------------------------
// Descrição:	Ajusta se o pino vai ficar em dreno aberto ou não
// Parametros:	addrPort: Endereço da porta
// 					PINSEL_P0 para porta 0
// 					PINSEL_P1 para porta 1 e assim sucessivamente
//				numPin: Número do pino da porta, valores de 0 a 31
//				openDrain: Ajusta se o pino vai fica em dreno aberto ou não
// 					0: Pin is in the normal (not open drain) mode
//					1: Pin is in the open drain mode
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------
void pinsel_setOpenDrainMode(reg32 *addrPort, u8 numPin, u8 openDrain) {
	addrPort+=numPin;								// Captura o endereço do registrador do determinado pino
	*addrPort &= ~(PINSEL_OPENDRAIN_MASK); 			// Limpa o campo de modos no registrador do pino
	*addrPort |= openDrain&PINSEL_OPENDRAIN_MASK;	// Atribui o novo modo do pino
}

// -----------------------------------------------------------------------------------------------
// Descrição:	Configura o pino para uma determinada função, se vai assumir GPIO, UART, PWM, I2C ...
//					Os valores devem ser passados por parametros pela estrutura pinsel_cfg_t, onde os campos são:
//						addrPort: Endereço da porta do pino a ser configurado. PINSEL_Px (x de 0 a quantidade de portas do LPC)
//						numPin: Número do pino a ser configurado. PINSEL_PINx (x de 0 a 31)
//						nunFunc: Qual função o pino vai assumir: GPIO função 0 (padrão), UART, PWM, I2C ...
//						pinMode: Modo do pino
//							PINSEL_PINMODE_INACTIVE: Sem resistor pull-up ou pull-down
//							PINSEL_PINMODE_PULLUP  : Internal pull-up resistor
//							PINSEL_PINMODE_PULLDOWN: Internal pull-down resistor
//							PINSEL_PINMODE_REPEATER: Repeater mode.
//						openDrain: Ajusta se o pino vai fica em dreno aberto ou não
// 							0: Pin is in the normal (not open drain) mode
//							1: Pin is in the open drain mode
// Parametros:	Ponteiro da estrutura de configuração do pino
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------
// Cada registrador é para cada pinos, e podemos configurar a sua função e modo
// 	BITS	Tipo D			Tipo A			Tipo U			Tipo I			Tipo W
//	31:17	Reservado		Reservado		Reservado		Reservado		Reservado
//	16		Reservado		DACEN			Reservado		Reservado		Reservado
//	14:11	Reservado		Reservado		Reservado		Reservado		Reservado
//	10		OD				OD				Reservado		Reservado		OD
//	9		SLEW			Reservado		Reservado		HIDRIVE			SLEW
//	8		Reservado		FILTER			Reservado		HS				FILTER
//	7		Reservado		ADMODE			Reservado		Reservado		1
//	6		INV				INV				Reservado		INV				INV
//	5 		HYS				Reservado		Reservado		Reservado		HYS
//	4:3		MODE			MODE			Reservado		Reservado		MODE
//	2:0		FUNC			FUNC			FUNC			FUNC			FUNC

// FUNC = Função do pino de 1 a quantidade de funções do ARM (lpc1788 = 7)
// MODE = Modo do pino para saída.
//		00 = Sem PULLUP ou PULlDOWN,
//		01 = Resistor de pulldown habilitado
//		10 = Resistor de pullup habilitado
//		11 = modo repitidor
// HYS = Histeresse.
//		0 = deligado,
//		1 = ligado
// INV = Modo invertido de leitura.
//		0 = Não invertido (Nível alto leremos como 1)
//		1 = invertido (Nível alto leremos como 0)
// ADMODE = Selecione modo digital ou analógico
//		0 = Modo analógico
//		1 = Modo digital
// FILTER = Controle de filtro contra glitch.
//		0 = Filtro ligado, pulsos de ruídos abaixo de 10ns são filtrados,
//		1 = Desliga filtros
// HS = Modo fast para I2C
// HIDRIVE = Controle de dreno de corrente
// SLEW = Driver Slew Rate
//		0 = Modo padrão, controle de slew rate habilitado, mais de uma saída pode ser
//			mudada aos mesmo tempo
//		1 = Modo rápido, controle de slew rate desabilitado
// OD = Controle do modo de dreno aberto
//		0 = Dreno aberto desavivado
//		1 - Simula uma saída em dreno aberto (high drive desable)
// DACEN = Controle de habilitação do DAC
//		0 = DAC desabilitado
//		1 = DAC habilitado
void pinsel_configPin(pinsel_cfg_t *pinCFG) {
	pinsel_setPinFunc(pinCFG->addrPort, pinCFG->numPin, pinCFG->nunFunc);
	pinsel_setResistorMode(pinCFG->addrPort, pinCFG->numPin, pinCFG->pinMode);
	pinsel_setOpenDrainMode(pinCFG->addrPort, pinCFG->numPin, pinCFG->openDrain);
}
#endif // cortexm3


static void pll_Init(void );
#if defined(arm7tdmi)
// -----------------------------------------------------------------------------------------------
// Descrição:	Incializa o clock do sistema
// Parametros: 	nenhum
// -----------------------------------------------------------------------------------------------
static void pll_Init(void ) {
  	// DESCONETA O PLLC CASO ESTEJA CONECTADO E O MANTENHA HABILITADO
  	if ( PLLSTAT & PLLSTAT_PLLC )  {					// Checa se o PLL está conectado
		PLLCON = PLLCON_PLLE;							// Desconecta o PLL mas o mantêm habilitado
  		PLLFEED = 0xAA;	PLLFEED = 0x55;					// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG
  	}

  	// DESCONETA E DESABILITA O PLL
	PLLCON = 0;											// Desconecta e desabilita o PLL
  	PLLFEED = 0xAA;	PLLFEED = 0x55;						// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG

  	// HABILITANDO FAST GPIO CASO TENHA SUPORTE
  	#if defined(LPC2468)
  	SCS |= 0x01;
	#elif defined(LPC2148)
  	SCS |= 0x03;
	#endif


	//#############################################################################################
	// Das três opções de fonte de clock (OSC interno, RTC e OSC externo).
	// 	Vamos trabalhar somente com o oscilador externo conectado ao PLL

	// LIGANDO O OSCILADOR PRINCIPAL COMO FONTE DE CLOCK PARA O PLL. SOMENTE PARA O LPC2400
	#if defined(LPC2468)
	SCS |= SCS_OSCEN;								// Liga o oscilador principal
	while( !(SCS & SCS_OSCSTAT) );					// Espero até que o oscilador principal esteja pronto para uso como fonte de clock para o PLL
	PCLKSRCSEL = 0x1;								// Define como oscilador principal como fonte de clock para o PLL
	#endif

	// CONFIGURANDO MULTIPLICADOR E DIVISOR PLL.
 	#if defined(LPC2468)
 	PLLCFG = PLLCFG_MSEL | (PLLCFG_NSEL);			// Ajustando o multiplicador e divisor do PLL
	PLLFEED = 0xAA;	PLLFEED = 0x55;					// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG
 	#else
 	PLLCFG = PLLCFG_MSEL | (PLLCFG_PSEL);			// Ajustando o multiplicador e divisor do PLL
 	PLLFEED = 0xAA;	PLLFEED = 0x55;					// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG
 	#endif

	// HABILITANDO A PLL MAS AINDA MANTÉM DESCONECTADA
  	PLLCON = PLLCON_PLLE;								// Habilitando PLL
  	PLLFEED = 0xAA;	PLLFEED = 0x55;						// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG

 	// PARA O LPC2400 AJUSTA A SAIDA DE CLOCK DA CPU E USB
  	#if defined(LPC2468)
  	CCLKCFG	= (FCCO/CCLK)-1;						// Ajusta a velocidade do clock da CPU dividindo a frequencia Fcco vinda do PLL
	USBCLKCFG = (FCCO/48000000ul)-1;			    // Ajusta a velocidade do clock do bloco USB dividindo a frequencia Fcco vinda do PLL
  	#endif

	// ESPERAR ATÉ QUE O PLL ESTEJA TRAVADO NA FREQUENCIA SOLICITADA
  	#if !defined(SIM_SKYEYE)
  	while (!(PLLSTAT & PLLSTAT_LOCK)) continue;		// Espera que o PLL trave. No Skyeye não se usa
  	#endif

  	// CONECTA O PLL AO SISTEMA DE CLOCK
  	PLLCON = PLLCON_PLLE | PLLCON_PLLC;					// Habilita PLL e conecta a CPU
  	PLLFEED = 0xAA;	PLLFEED = 0x55;						// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG

	// ESPERAR ATÉ QUE O PLL ESTEJA CONECTADO
  	#if !defined(SIM_SKYEYE)
  	while (!(PLLSTAT & PLLSTAT_PLLC)) continue;		// Espera que o PLL trave. No Skyeye não se usa
  	#endif

	//#############################################################################################
	// AJUSTANDO O VELOCIDADE DO VPB. RELAÇÃO ENTRE A VELOCIDADE DO PROCESSADOR COM OS PERIFÉRICOS
	#if defined(LPC2468)
  		//	Cada dispositivo pode selecionar de forma independete, mas aqui estamos tratando todos por iguais
  		#if (PBSD == PBSD_1_4_CCLK)
  			PCLKSEL0 = 0x00000000;						// Pclk roda a 1/4 da velocidade do clock da CPU (Cclk)
  			PCLKSEL1 = 0x00000000;						// Pclk roda a 1/4 da velocidade do clock da CPU (Cclk)
  		#elif (PBSD == PBSD_1_2_CCLK)
		  	PCLKSEL0 = 0xAAAAAAAA;						// Pclk roda a 1/2 da velocidade do clock da CPU (Cclk)
  			PCLKSEL1 = 0xAAAAAAAA;	 					// Pclk roda a 1/2 da velocidade do clock da CPU (Cclk)
  		#else
  		  	PCLKSEL0 = 0x55595555;						// Pclk roda a mesma velocidade do clock da CPU (Cclk). RTC[19:18] não pode assumir 01
  			PCLKSEL1 = 0x55555555;						// Pclk roda a mesma velocidade do clock da CPU (Cclk)
  		#endif

	#else
		VPBDIV = VPBDIV_VALUE;                			// Ajusta a velocidade de acesso aos perifercos em relação a velocidade da CPU
	#endif
}

#endif //arm7tdmi

#if defined(cortexm3)
// -----------------------------------------------------------------------------------------------
// Descrição:	Incializa o clock do sistema
// Parametros: 	nenhum
// -----------------------------------------------------------------------------------------------
static void pll_Init(void ) {

	// Define CCLk
	#if defined(LPC1788)
	cclksel_sysClk();  // Seleciona o oscilador RC para o divisor do CCLK, PCLK e MEMCLK
	cclksel_clkDiv(1); // Divisor 1:1 para o CCLK
	#endif

	#if defined(LPC1751) || defined(LPC1752)
	cclksel_clkDiv(2); 						// Divisor 1:1 para o CCLK
	#endif

	#if defined(OSC_XTAL)
	clksrcsel_oscRC(); 						// habilitar o oscilador RC interno para o sistema
	scs_oscOFF(); 							// Desabiltar o osc MAIN
	#if (F_CPU > 20000000)
	SCS |= SCS_OSCRANGE_15MHZ_25MHZ;
	#endif
  	scs_oscON();							// Liga o oscilador MAIN
  	while( !(SCS & SCS_OSCSTAT) );			// Espero até que o oscilador principal esteja pronto para uso como fonte de clock para o PLL
  	clksrcsel_oscMAIN(); 					// Habilita o oscilador MAIN para o sistema
  	#endif

  	pll_OFF(); 								// Desabilita PLL
  	PLLFEED = 0xAA;	PLLFEED = 0x55;			// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG
  	PLLCFG = PLLCFG_MSEL | PLLCFG_PSEL;		// Ajustando o multiplicador e divisor do PLL
  	PLLFEED = 0xAA;	PLLFEED = 0x55;			// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG
  	pll_ON();								// Habilita PLL
  	PLLFEED = 0xAA;	PLLFEED = 0x55;			// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG

  	while (!(PLLSTAT & PLLSTAT_LOCK));		// Espera que o PLL trave. No Skyeye não se usa
  	#if defined(LPC1751) || defined(LPC1752)
  	pll_Connect();							// Conecta PLL
  	PLLFEED = 0xAA;	PLLFEED = 0x55;			// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG
  	while (!(PLLSTAT & PLLSTAT_PLLC));		// Espera que o PLL se conecte ao sistema
  	#endif

  	#if defined(OSC_XTAL)
  	pll1_OFF(); 							// Desabilita PLL
  	PLL1FEED = 0xAA;	PLL1FEED = 0x55;	// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG
  	PLL1CFG = PLL1CFG_MSEL | PLL1CFG_PSEL;	// Ajustando o multiplicador e divisor do PLL
  	PLL1FEED = 0xAA;	PLL1FEED = 0x55;	// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG
  	pll1_ON();								// Habilita PLL
  	PLL1FEED = 0xAA;	PLL1FEED = 0x55;	// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG

  	while (!(PLL1STAT & PLL1STAT_LOCK));	// Espera que o PLL trave. No Skyeye não se usa
  	#if defined(LPC1751) || defined(LPC1752)
  	pll1_Connect();							// Conecta PLL
  	PLL1FEED = 0xAA;	PLL1FEED = 0x55;	// Sequencia para validar as mudanças nos registradores PLLCON e PLLCFG
	while (!(PLL1STAT & PLL1STAT_PLLC));	// Espera que o PLL se conecte ao sistema
  	#endif
  	usbksel_clkDiv(USBCLKDIV);				// Aplicar divisor obter o USBCLK do PLL1_OUT
  	#endif

  	cclksel_clkDiv(CCLKDIV);				// Aplicar divisor obter o CCLK do PLL_OUT

  	#if defined(LPC1788)
  	pclksel_clkDiv(PCLKDIV);				// Aplicar divisor obter o PCLK do PLL_OUT
  	USBCLKSEL |= USBCLKSEL;
  	cclksel_PLL();							// seleciona o oscilador MAIN para o divisor do CCLK, PCLK e MEMCLK
  											// Tem que ser depois de aplicar o divisor
  	usbksel_PLL1();
  	#endif


	#if defined(LPC1751) || defined(LPC1752)
	#if ((CCLK / PCLK) == 2)
		// Pclk roda 1/2 da velocidade do clock da CPU (Cclk)
	  	PCLKSEL  = 0xaaa2a2aa;
  		PCLKSEL1 = 0xa8aaa8aa;
	#elif ((CCLK / PCLK) == 4)
		// Pclk roda 1/4 da velocidade do clock da CPU (Cclk)
	  	PCLKSEL = 0x0;
  		PCLKSEL1 = 0x0;
	#elif ((CCLK / PCLK) == 8)
		// Pclk roda 1/8 da velocidade do clock da CPU (Cclk)
	  	PCLKSEL = 0xfff3f3ff;
  		PCLKSEL1 = 0xfcfffcff;
	#else
		// Pclk roda a mesma velocidade do clock da CPU (Cclk)
	  	PCLKSEL = 0x55515155;
  		PCLKSEL1 = 0x54555455;
	#endif
	#endif // #if defined(LPC1751) || defined(LPC1752)
}
#endif //cortexm3

// -----------------------------------------------------------------------------------------------
// Descrição:	Incialiaza clock do processador e dos barramentos através do STARTUP
// ajustes no makefile 	
// -----------------------------------------------------------------------------------------------
void arm_Init (void) {

  	//#############################################################################################
	// AJUSTANDO O MAPA DE INTERRUPÇÃO. ISTO DEVIDO A DIFERENTES CONFIGURAÇÕES DE MEMÓRIA

	#if defined(RUN_MODE3) || defined(RUN_MODE2)
		// MAPA DOS VETORES DE INTERRUPÇÃO NO ESPAÇO DA SRAM
		// -------------------------------------------------
		#if defined (arm7tdmi)
		MEMMAP = MEMMAP_SRAM;
		#endif
		#if defined (cortexm3)
		vtor_SetInCode(0x10000000); // no inicio da sram interna de 64Kb
		#endif
	#elif defined(RUN_MODE1) || defined(RUN_MODEBOOT1) || defined(RUN_MODEBOOT2)
		// MAPA DOS VETORES DE INTERRUPÇÃO NO ESPAÇO DA FLASH
		// --------------------------------------------------
		#if defined (arm7tdmi)
		MEMMAP = MEMMAP_FLASH;
		#endif
		#if defined (cortexm3)
		vtor_SetInCode(0); // Definir a tabela de ints no endereço 0 da flash
		#endif
	#else
		#error NÃO FOI DEFINIDO O RUN_MODE no makefile
	#endif

	//#############################################################################################
	// 	DESLIGA TODOS OS PERIFÉRICOS DA CPU.
	//	Os periféricos que serão usados serão re-ligados nas funções init dos mesmos,
	//		Pois, isto economiza energia
	#if defined (arm7tdmi)
	PCONP = 0;
	#endif
	#if defined (cortexm3)
	PCONP = PCGPIO; // Vamos deixar somente o GPIO habilitado, pois este habilita também os IOCONS (pinsel)
					// 	para configuração dos pinos
	#endif


	//#############################################################################################
	// INICIALIZANDO PLL
	pll_Init();

	#if defined (arm7tdmi)
  	//#############################################################################################
	// CONFIGURANDO E HABILITA MAM (MEMORY ACCELERATOR MODULE)
  	MAMCR = 0;											// Desliga MAM
  	#if (ARM_MAMTIM_CYCLES == 0) 						// Ajusta automaticamente a velocidade de acesso a RAM de acordo com a frequencia da CPU
		#if 	CCLK < 20000000
			MAMTIM = MAMTIM_1_CYCLES;
		#elif	CCLK < 40000000
			MAMTIM = MAMTIM_2_CYCLES;
		#else
			MAMTIM = MAMTIM_3_CYCLES;
		#endif
  	#else
  		MAMTIM = ARM_MAMTIM_CYCLES;
  	#endif
	MAMCR = ARM_MAM; // liga mam forma completa
	#endif // arm7tdmi

	#if defined (cortexm3)
	// O power boost (PBOOST) é ativado após o reset, permitindo trabalhar com frequencia da CPU de até
	//	seu limite máximo. Vamos deixar desse modo, mas se usar gestão de economia de energia teremos
	//	que rever isto
	#if (FLASHTIM == FLASHTIM_AUTO)
		#if (CCLK >= 100000000)
		FLASHCFG = FLASHTIM_100MHZ | 0x3a; // definir a velocidade de acesso a flash (consulte _config_cpu_.h)
		#elif (CCLK >= 80000000)
		FLASHCFG = FLASHTIM_80MHZ | 0x3a; // definir a velocidade de acesso a flash (consulte _config_cpu_.h)
		#elif (CCLK >= 60000000)
		FLASHCFG = FLASHTIM_60MHZ | 0x3a; // definir a velocidade de acesso a flash (consulte _config_cpu_.h)
		#elif (CCLK >= 40000000)
		FLASHCFG = FLASHTIM_40MHZ | 0x3a; // definir a velocidade de acesso a flash (consulte _config_cpu_.h)
		#elif (CCLK >= 20000000)
		FLASHCFG = FLASHTIM_20MHZ | 0x3a; // definir a velocidade de acesso a flash (consulte _config_cpu_.h)
		#else
		FLASHCFG = FLASHTIM_SAFE | 0x3a; // definir a velocidade de acesso a flash (consulte _config_cpu_.h)
		#endif
	#else
	FLASHCFG = FLASHTIM | 0x3a; // definir a velocidade de acesso a flash (consulte _config_cpu_.h)
	#endif
	#endif // cortexm3
}

// -----------------------------------------------------------------------------------------------
// Descrição:	Incialiaza as memórias externas através do STARTUP
// ajustes no makefile 	
// -----------------------------------------------------------------------------------------------
#if defined(USE_EXT_SRAM) || defined(USE_EXT_SDRAM)
#if defined(LPC1788)
void ramext_Init_lpc1788(void);
void ramext_Init_lpc1788(void) {
	// AJUSTANDO PINOS PARA ACESSAR MEMÓRIAS EXTERNAS OU DISPOSITIVOS MAPEADOS POR RAM
	// Consultar datasheet página 113 e 164

	// ctrl pins
	IOCON_P2_16 = 0x21; //CAS
	IOCON_P2_17 = 0x21; //RAS
	IOCON_P2_18 = 0x21; //CLK0
	IOCON_P2_20 = 0x21; //DYCS0
	IOCON_P2_24 = 0x21; //CKE0
	IOCON_P2_28 = 0x21; //DQM0
	IOCON_P2_29 = 0x21; //DQM1
	IOCON_P4_25 = 0x21; //WE

	//data pins
	IOCON_P3_00 = 0x21; //D[0]
	IOCON_P3_01 = 0x21; //D[1]
	IOCON_P3_02 = 0x21; //D[2]
	IOCON_P3_03 = 0x21; //D[3]
	IOCON_P3_04 = 0x21; //D[4]
	IOCON_P3_05 = 0x21; //D[5]
	IOCON_P3_06 = 0x21; //D[6]
	IOCON_P3_07 = 0x21; //D[7]
	IOCON_P3_08 = 0x21; //D[8]
	IOCON_P3_09 = 0x21; //D[9]
	IOCON_P3_10 = 0x21; //D[10]
	IOCON_P3_11 = 0x21; //D[11]
	IOCON_P3_12 = 0x21; //D[12]
	IOCON_P3_13 = 0x21; //D[13]
	IOCON_P3_14 = 0x21; //D[14]
	IOCON_P3_15 = 0x21; //D[15]

	//#if defined (__ARCH_ARCH_KITLPC1788PR111_H) && defined(USE_EXT_SDRAM)
	//	data 31:16
	//#endif

	//address pins
	IOCON_P4_00 = 0x21; //A[0]
	IOCON_P4_01 = 0x21; //A[1]
	IOCON_P4_02 = 0x21; //A[2]
	IOCON_P4_03 = 0x21; //A[3]
	IOCON_P4_04 = 0x21; //A[4]
	IOCON_P4_05 = 0x21; //A[5]
	IOCON_P4_06 = 0x21; //A[6]
	IOCON_P4_07 = 0x21; //A[7]
	IOCON_P4_08 = 0x21; //A[8]
	IOCON_P4_09 = 0x21; //A[9]
	IOCON_P4_10 = 0x21; //A[10]
	IOCON_P4_11 = 0x21; //A[11]
	IOCON_P4_12 = 0x21; //A[12]
	IOCON_P4_13 = 0x21; //A[13]
	IOCON_P4_14 = 0x21; //A[14]

	#if defined(USE_EXT_SRAM)
	IOCON_P4_24 = 0x21; // OE
	IOCON_P2_14 = 0x21; // CS2
	IOCON_P2_15 = 0x21; // CS3
	#endif

	// LIGA EMC
	PCONP |= PCEMC;

	#if (SDRAM_BANK0_USE != SDRAM_BANK_USE_NONE)
	// DETERMINAR A VELOCIDADE DO BARRAMENTO DE MEMÓRIA
	EMCCLKSEL = MCLK;
	// CLKOUTCFG = 0x00000100;  // enables clock, sets clock source as CPU clk / 1...
	// AJUSTE FINO DO DELAY DE CLOCK ENTRE O EMC PARA A SDRAM
	EMC_DYN_DLYCTL = SDRAM_CLK_DELAY;
	#endif

	// Habilita EMC, sem espelhamento de endereçamento e modo de operação normal, ou seja, sem redução de consumo
	EMC_CTRL = 1;

	// -------------------------------------------------------------------------
	// CONFIGURAÇÕES PARA AS MEMÓRIAS ESTÁTICAS
	#if defined(USE_EXT_SRAM)
	EMC_STA_EXT_WAIT = SRAM_MEMORY_EXTENDED_WAIT;

	#if (SRAM_BANK0_USE != SRAM_BANK_USE_NONE)
	EMC_STA_CFG0 		= SRAM_BANK0_CFG;
	EMC_STA_WAITWEN0	= SRAM_BANK0_WAITWEN;
	EMC_STA_WAITOEN0	= SRAM_BANK0_WAITOEN;
	EMC_STA_WAITRD0		= SRAM_BANK0_WAITRD;
	EMC_STA_WAITPAGE0	= SRAM_BANK0_WAITPAGE;
	EMC_STA_WAITWR0		= SRAM_BANK0_WAITWR;
	EMC_STA_WAITTURN0	= SRAM_BANK0_WAITTURN;
	#endif

	#if (SRAM_BANK1_USE != SRAM_BANK_USE_NONE)
	EMC_STA_CFG1 		= SRAM_BANK1_CFG;
	EMC_STA_WAITWEN1	= SRAM_BANK1_WAITWEN;
	EMC_STA_WAITOEN1	= SRAM_BANK1_WAITOEN;
	EMC_STA_WAITRD1		= SRAM_BANK1_WAITRD;
	EMC_STA_WAITPAGE1	= SRAM_BANK1_WAITPAGE;
	EMC_STA_WAITWR1		= SRAM_BANK1_WAITWR;
	EMC_STA_WAITTURN1	= SRAM_BANK1_WAITTURN;
	#endif

	#if (SRAM_BANK2_USE != SRAM_BANK_USE_NONE)
	EMC_STA_CFG2 		= SRAM_BANK2_CFG;
	EMC_STA_WAITWEN2	= SRAM_BANK2_WAITWEN;
	EMC_STA_WAITOEN2	= SRAM_BANK2_WAITOEN;
	EMC_STA_WAITRD2		= SRAM_BANK2_WAITRD;
	EMC_STA_WAITPAGE2	= SRAM_BANK2_WAITPAGE;
	EMC_STA_WAITWR2		= SRAM_BANK2_WAITWR;
	EMC_STA_WAITTURN2	= SRAM_BANK2_WAITTURN;
	#endif

	#if (SRAM_BANK3_USE != SRAM_BANK_USE_NONE)
	EMC_STA_CFG3 		= SRAM_BANK3_CFG;
	EMC_STA_WAITWEN3	= SRAM_BANK3_WAITWEN;
	EMC_STA_WAITOEN3	= SRAM_BANK3_WAITOEN;
	EMC_STA_WAITRD3		= SRAM_BANK3_WAITRD;
	EMC_STA_WAITPAGE3	= SRAM_BANK3_WAITPAGE;
	EMC_STA_WAITWR3		= SRAM_BANK3_WAITWR;
	EMC_STA_WAITTURN3	= SRAM_BANK3_WAITTURN;
	#endif
	#endif //USE_EXT_SRAM

	// -------------------------------------------------------------------------
	// CONFIGURAÇÕES PARA AS MEMÓRIAS DINAMICAS
	#if defined(USE_EXT_SDRAM)
	#if (SDRAM_BANK0_USE != SDRAM_BANK_USE_NONE)
		//xdelay_ms(200);
		EMC_DYN_RD_CFG 	= 1; // Configure a memória dinamica para estratégica de leitura (command delayed strategy)

		EMC_DYN_RASCAS0 = ( reg32 ) SDRAM_LATENCY;	// Ajusta a letencia de ras e cas
		EMC_DYN_RP    	= ( reg32 ) SDRAM_TRP;		// Comando de periodo precharge
		EMC_DYN_RAS    	= ( reg32 ) SDRAM_TRAS;		// RAS command period
		EMC_DYN_SREX   	= ( reg32 ) SDRAM_TXSR;		// Self-refresh period
		EMC_DYN_APR    	= ( reg32 ) SDRAM_TAPR;		// Data out to active
		EMC_DYN_DAL   	= ( reg32 ) SDRAM_TDAL;		// Data in to active: 5(n+1) clock cycles
		EMC_DYN_WR    	= ( reg32 ) SDRAM_TWR;		// Write recovery: 2(n+1) clock cycles
		EMC_DYN_RC     	= ( reg32 ) SDRAM_TRC;		// Active to Active cmd: 6(n+1) clock cycles
		EMC_DYN_RFC    	= ( reg32 ) SDRAM_TRFC;		// Auto-refresh: 6(n+1) clock cycles
		EMC_DYN_XSR    	= ( reg32 ) SDRAM_TXSR;		// Exit self-refresh: 8(n+1) clock cycles
		EMC_DYN_RRD    	= ( reg32 ) SDRAM_TRRD;		// Active bank A->B: 2(n+1) clock cycles
		EMC_DYN_MRD    	= ( reg32 ) SDRAM_TMRD;		// Load Mode to Active cmd: 3(n+1) clock cycles

		// CONFIGURAR O ARM A ACESSAR QUE TIPO DE MEMÓRIA SDRAM
		EMC_DYN_CFG0 	= ( reg32 ) SDRAM_CONFIG;

		// INCIALIZANDO SDRAM
		// JEDEC General SDRAM Initialization Sequence
		// DELAY to allow power and clocks to stabilize ~100 us
		// 	Isto habilita todos os clocks, operação normal de refresh, estado da SDRAM em NOP (sem operação)
		EMC_DYN_CTRL = 0x183;
		xdelay_us(200);

		// EMITIR COMANDO PRECHARGE-ALL PARA SDRAM
		EMC_DYN_CTRL = 0x103; 				// Send command: PRECHARGE-ALL
		EMC_DYN_RFSH = SDRAM_AUTO_REFRESH; 	// Emitir auto refresh para configuração da SDRAM
		volatile u32 dummy;
		for (dummy = 256; dummy; --dummy); // > 128 clk
		EMC_DYN_RFSH = ( reg32 )  (((SDRAM_REFRESH_PERIOD_US/1000000) * (CCLK))/16); // Ajusta o tempo ciclos refresh cycles

		// EMITIR O COMANDO PARA SDRAM PARA TRABALHAR NO MODO NORMAL
		EMC_DYN_CTRL = 0x83; 			// Todos os clocks habilitados e rodando de forma interrupta. sdram operando no modo normal e mando comando MODE

		// Burst 4, Sequential, CAS-3
		dummy = *((volatile u32 *)(SDRAM_BANK0_BASE_ADDR | SDRAM_MODE_CMD)); // config modo de trabalho na sdram através do comando de leitura da sdram (SDRAM_MODE_CMD)

		// NORMAL
		EMC_DYN_CTRL = 0;	  				// Send command: NORMAL
		EMC_DYN_CFG0 |= 0x80000;			// Habilita buffer para acesso a SDRAM
		xdelay_ms(100);

		#endif //SDRAM_BANK0_USE_NONE
		#endif // USE_EXT_SDRAM)
}
#endif

#if defined(LPC2468)
void ramext_Init_lpc2468(void);
void ramext_Init_lpc2468(void) {
	// Consultar datasheet página 75

	// AJUSTANDO PINOS PARA ACESSAR MEMÓRIAS EXTERNAS OU DISPOSITIVOS MAPEADOS POR RAM
	PINSEL4  = 0x50000000;			// Ajusta os pinos P2[15] = CS3 e P2[14] = CS2
	PINMODE4 = 0xa0000000;			// Resistores em pull up
	PINSEL5  = 0x55555555;			// Ajusta os pinos P2[31:28] = DQMOUT[3:0], P2[27:24] = CLKEOUT[3:0], P2[23:20] = DYCS[3:0], P2[~19:18] = CLKOUT[1:0], P2[17] = RAS, P2[16] = CAS
	PINMODE5 = 0xaaaaaaaa;			// Resistores em pull up
	PINSEL6  = 0x55555555;			// Ajusta os pinos P3[15:0] = D[15:0]
	PINMODE6 = 0xaaaaaaaa;			// Resistores em pull up
	PINSEL7  = 0x55555555;			// Ajusta os pinos P3[31:16] = D[31:16]
	PINMODE7 = 0xaaaaaaaa;			// Resistores em pull up
	PINSEL8  = 0x55555555; 			// Ajusta os pinos P4[15:0] = A[15:0]
	PINMODE8 = 0xaaaaaaaa;			// Resistores em pull up
	PINSEL9  = 0x55555555; 			// Ajusta os pinos P4[31] = CS1, P4[30] = CS0, P4[29:36] = BLS[3:0], P4[25] = WE, P4[24] = OE, P4[23:16] = A[23:16]
	PINMODE9 = 0xaaaaaaaa;			// Resistores em pull up

	// LIGA EMC
	PCONP |= PCEMC;

	// CONFIGURA EMC E HABILITA-O
	EMC_CTRL 	= 0x00000001;		// Habilita EMC, desabilita espelhamento de endereçamento, modo de operação não baixo consumo
	//EMC_CONFIG 	=0;				// Modo little-endian, razão do clock 1:1

	// -------------------------------------------------------------------------
	// CONFIGURAÇÕES PARA AS MEMÓRIAS ESTÁTICAS
	#if defined(USE_EXT_SRAM)
	EMC_STA_EXT_WAIT = SRAM_MEMORY_EXTENDED_WAIT;

	#if (SRAM_BANK0_USE != SRAM_BANK_USE_NONE)
	EMC_STA_CFG0 		= SRAM_BANK0_CFG;
	EMC_STA_WAITWEN0	= SRAM_BANK0_WAITWEN;
	EMC_STA_WAITOEN0	= SRAM_BANK0_WAITOEN;
	EMC_STA_WAITRD0		= SRAM_BANK0_WAITRD;
	EMC_STA_WAITPAGE0	= SRAM_BANK0_WAITPAGE;
	EMC_STA_WAITWR0		= SRAM_BANK0_WAITWR;
	EMC_STA_WAITTURN0	= SRAM_BANK0_WAITTURN;
	#endif

	#if (SRAM_BANK1_USE != SRAM_BANK_USE_NONE)
	EMC_STA_CFG1 		= SRAM_BANK1_CFG;
	EMC_STA_WAITWEN1	= SRAM_BANK1_WAITWEN;
	EMC_STA_WAITOEN1	= SRAM_BANK1_WAITOEN;
	EMC_STA_WAITRD1		= SRAM_BANK1_WAITRD;
	EMC_STA_WAITPAGE1	= SRAM_BANK1_WAITPAGE;
	EMC_STA_WAITWR1		= SRAM_BANK1_WAITWR;
	EMC_STA_WAITTURN1	= SRAM_BANK1_WAITTURN;
	#endif

	#if (SRAM_BANK2_USE != SRAM_BANK_USE_NONE)
	EMC_STA_CFG2 		= SRAM_BANK2_CFG;
	EMC_STA_WAITWEN2	= SRAM_BANK2_WAITWEN;
	EMC_STA_WAITOEN2	= SRAM_BANK2_WAITOEN;
	EMC_STA_WAITRD2		= SRAM_BANK2_WAITRD;
	EMC_STA_WAITPAGE2	= SRAM_BANK2_WAITPAGE;
	EMC_STA_WAITWR2		= SRAM_BANK2_WAITWR;
	EMC_STA_WAITTURN2	= SRAM_BANK2_WAITTURN;
	#endif

	#if (SRAM_BANK3_USE != SRAM_BANK_USE_NONE)
	EMC_STA_CFG3 		= SRAM_BANK3_CFG;
	EMC_STA_WAITWEN3	= SRAM_BANK3_WAITWEN;
	EMC_STA_WAITOEN3	= SRAM_BANK3_WAITOEN;
	EMC_STA_WAITRD3		= SRAM_BANK3_WAITRD;
	EMC_STA_WAITPAGE3	= SRAM_BANK3_WAITPAGE;
	EMC_STA_WAITWR3		= SRAM_BANK3_WAITWR;
	EMC_STA_WAITTURN3	= SRAM_BANK3_WAITTURN;
	#endif
	#endif //USE_EXT_SRAM

	// -------------------------------------------------------------------------
	// CONFIGURAÇÕES PARA AS MEMÓRIAS DINAMICAS
	#if defined(USE_EXT_SDRAM)
	#if (SDRAM_BANK0_USE != SDRAM_BANK_USE_NONE)
		// delay 100ms.
	//  A SDRAM NECESSITA A MENOS DESSE TEMPO PARA ESTABILIZAR O CLOCK
		asm volatile (
			"L_LOOPUS_%=: 		\r\n\t" \
			"subs	%0, %0, #1 	\r\n\t" \
			"bne	L_LOOPUS_%=	\r\n\t" \
			:: "r" ((u32)( (CCLK) / (1000000/SDRAM_DELAY_100MS) /4 * SDRAM_TUNE_FAC)*1000)
		);

		EMC_DYN_RD_CFG 	= 1;						// Configure a memória dinamica para estratégica de leitura (command delayed strategy)

		// AJUSTA A LETENCIA DE RAS E CAS
		EMC_DYN_RASCAS0 = ( reg32 ) SDRAM_LATENCY;

		EMC_DYN_RP    	= ( reg32 ) SDRAM_TRP;		// Comando de periodo precharge
		EMC_DYN_RAS    	= ( reg32 ) SDRAM_TRAS;		// RAS command period
		EMC_DYN_SREX   	= ( reg32 ) SDRAM_TSREX;	// Self-refresh period
		EMC_DYN_APR    	= ( reg32 ) SDRAM_TAPR;		// Data out to active
		EMC_DYN_DAL   	= ( reg32 ) SDRAM_TDAL;		// Data in to active: 5(n+1) clock cycles
		EMC_DYN_WR    	= ( reg32 ) SDRAM_TWR;		// Write recovery: 2(n+1) clock cycles
		EMC_DYN_RC     	= ( reg32 ) SDRAM_TRC;		// Active to Active cmd: 6(n+1) clock cycles
		EMC_DYN_RFC    	= ( reg32 ) SDRAM_TRFC;		// Auto-refresh: 6(n+1) clock cycles
		EMC_DYN_XSR    	= ( reg32 ) SDRAM_TXSR;		// Exit self-refresh: 8(n+1) clock cycles
		EMC_DYN_RRD    	= ( reg32 ) SDRAM_TRRD;		// Active bank A->B: 2(n+1) clock cycles
		EMC_DYN_MRD    	= ( reg32 ) SDRAM_TMRD;		// Load Mode to Active cmd: 3(n+1) clock cycles

		// AJUSTA A TIPO DE MEMÓRIAS DINAMICAS
		EMC_DYN_CFG0 	= ( reg32 ) SDRAM_CONFIG;

		// INCIALIZANDO SDRAM

		EMC_DYN_CTRL = 0x00000183; 					// Todos os clocks habilitados e rodando de forma interrupta. sdram operando no modo normal e comando de NOP

		// delay 100ms
  		asm volatile (
			"L_LOOPUS_%=: 		\r\n\t" \
			"subs	%0, %0, #1 	\r\n\t" \
			"bne	L_LOOPUS_%=	\r\n\t" \
			:: "r" ((u32)( (CCLK) / (1000000/SDRAM_DELAY_100MS) /4 * SDRAM_TUNE_FAC)*1000)
		);

		EMC_DYN_CTRL = 0x00000103; 			// Send command: SDRAM-PALL: PRECHARGE-ALL, shortest possible refresh period
		EMC_DYN_RFSH = SDRAM_AUTO_REFRESH; 	// Emitir auto refresh para configuração da SDRAM

  		// delay 100ms
  		asm volatile (
			"L_LOOPUS_%=: 		\r\n\t" \
			"subs	%0, %0, #1 	\r\n\t" \
			"bne	L_LOOPUS_%=	\r\n\t" \
			:: "r" ((u32)( (CCLK) / (1000000/SDRAM_DELAY_100MS) /4 * SDRAM_TUNE_FAC)*1000)
		);

		EMC_DYN_RFSH = ( reg32 )  (((SDRAM_REFRESH_PERIOD_US/1000000) * (CCLK))/16); // ajusta o tempo entre SDRAM refresh cycles
		EMC_DYN_CTRL = 0x00000083;	// Todos os clocks habilitados e rodando de forma interrupta. sdram operando no modo normal e mando comando MODE

		volatile u32 dummy;
		dummy = *((volatile u32 *)(SDRAM_BANK0_BASE_ADDR | SDRAM_MODE_CMD)); // config modo de trabalho na sdram através do comando de leitura da sdram (SDRAM_MODE_CMD)
		(u32) dummy; // para evitar avisos do compilador por não usar essa var
		EMC_DYN_CTRL = 0x00000000;	  				// Send command: NORMAL
  		EMC_DYN_CFG0 |= 0x00080000;					// Habilita buffer
	#endif //SDRAM_BANK0_USE_NONE
	#endif // USE_EXT_SDRAM)
}
#endif

#if defined(LPC2294) || defined(LPC2214)
void ramext_Init_lpc(void);
void ramext_Init_lpc(void) {
	PINSEL2 = 0x0F814920; 	// Registrador resposável de configurar os pinos para acesso a RAM externa e deixa desabilitado trace port e debug port
	// Bits 	27 	26 	25		P3.23/A23/XCLK and P3.22:2/A2.22:2
	//			1	0	0		A11:2 são linhas de endereços e demais GPIO
	//			1	0	1		A15:2 são linhas de endereços e demais GPIO
	//			1	1	0		A19:2 são linhas de endereços e demais GPIO
	//			1	1	1		A23:2 são linhas de endereços e demais GPIO
	// Bit24: 0 = GPIO / 1 = A1
	// Bit23: 0 = GPIO / 1 = A0
	// Bit3: 0 >> P1.25:16 = GPIO   / 1 >> P1.25:16 = Trace port
	// Bit2: 0 >> P1.36:26 = GPIO   / 1 >> P1.36:26 = Debug port

	// LIGA EMC
	PCONP |= PCEMC;

	// valores ajustados no makefile
	#if (SRAM_BANK0_USE != SRAM_BANK_USE_NONE)
	BCFG0 = ( reg32 ) SRAM_BANK0_CONFIG;
	#endif
	#if (SRAM_BANK1_USE != SRAM_BANK_USE_NONE)
	BCFG1 = ( reg32 ) SRAM_BANK1_CONFIG;
	#endif
	#if (SRAM_BANK2_USE != SRAM_BANK_USE_NONE)
	BCFG2 = ( reg32 ) SRAM_BANK2_CONFIG;
	#endif
	#if (SRAM_BANK3_USE != SRAM_BANK_USE_NONE)
	BCFG3 = ( reg32 ) SRAM_BANK3_CONFIG;
	#endif
#endif


void ramext_Init (void) {
	// #############################################
	// LPC1788 com suporte a memória externa (SRAM/SDRAM)
	#if defined(LPC1788)
		ramext_Init_lpc1788();

	// #############################################
	// LPC2468 com suporte a memória externa (SRAM/SDRAM)
	#elif defined(LPC2468)
		ramext_Init_lpc2468();

	// #############################################
	// LPC2294 E LPC2214 TEM SUPORTE SOMENTE A SRAM
	#elif defined(LPC2294) || defined(LPC2214)
		ramext_Init_lpc();
	#else
		#error Dispositivo não tem suporte a memória externa
	#endif	
}
#endif //  defined(USE_EXT_SRAM) || defined(USE_EXT_SDRAM)
