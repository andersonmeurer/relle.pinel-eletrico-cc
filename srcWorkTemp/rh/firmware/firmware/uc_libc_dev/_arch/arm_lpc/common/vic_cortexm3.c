#include "vic.h"

#if (VIC_USE_DEBUG == pdON) || (VICERR_USE_DEBUG == pdON)
#include"stdio_uc.h"
#endif


typedef void (*void_pFunc_t)(int);
static volatile void_pFunc_t vicerr_Functions = NULL;

extern u32 _stack; // Definido no linkscript
extern void _start (void); // definido no startup.c

void nmi_ISR (void) __attribute__((weak));
void hardFault_ISR (void) __attribute__((weak));
void memManage_ISR (void) __attribute__((weak));
void busFault_ISR (void) __attribute__((weak));
void usageFault_ISR (void) __attribute__((weak));
void svc_ISR (void) __attribute__((weak));
void debugMon_ISR (void) __attribute__((weak));
void pendSV_ISR (void) __attribute__((weak));
void sysTick_ISR (void) __attribute__((weak));

void wdt_ISR (void) __attribute__((weak));
void timer0_ISR (void) __attribute__((weak));
void timer1_ISR (void) __attribute__((weak));
void timer2_ISR (void) __attribute__((weak));
void timer3_ISR (void) __attribute__((weak));
void uart0_ISR (void) __attribute__((weak));
void uart1_ISR (void) __attribute__((weak));
void uart2_ISR (void) __attribute__((weak));
void uart3_ISR (void) __attribute__((weak));
void pwm_ISR (void) __attribute__((weak));
void i2c0_ISR (void) __attribute__((weak));
void i2c1_ISR (void) __attribute__((weak));
void i2c2_ISR (void) __attribute__((weak));
void spi_ISR (void) __attribute__((weak));
void ssp0_ISR (void) __attribute__((weak));
void ssp1_ISR (void) __attribute__((weak));
void pll0_ISR (void) __attribute__((weak));
void rtc_ISR (void) __attribute__((weak));
void eint0_ISR (void) __attribute__((weak));
void eint1_ISR (void) __attribute__((weak));
void eint2_ISR (void) __attribute__((weak));
void eint3_ISR (void) __attribute__((weak));
void adc_ISR (void) __attribute__((weak));
void bod_ISR (void) __attribute__((weak));
void usbHost_ISR (void) __attribute__((weak));
void can_ISR (void) __attribute__((weak));
void dma_ISR (void) __attribute__((weak));
void i2s_ISR (void) __attribute__((weak));
void enet_ISR (void) __attribute__((weak));
void rit_ISR (void) __attribute__((weak));
void mcpwm_ISR (void) __attribute__((weak));
void qei_ISR (void) __attribute__((weak));
void pll1_ISR (void) __attribute__((weak));
void usbAct_ISR (void) __attribute__((weak));
void canAct_ISR (void) __attribute__((weak));
void uart4_ISR (void) __attribute__((weak));
void ssp2_ISR (void) __attribute__((weak));
void lcd_ISR (void) __attribute__((weak));
void gpio_ISR (void) __attribute__((weak));
void pwm0_ISR (void) __attribute__((weak));
void eeprom_ISR (void) __attribute__((weak));

__attribute__ ((section(".vectors"), used))
void (* const gVectors[])(void) =
{
   // 16 VETORES DE INTERRUPÇÃO DE SISTEMA
	(void (*)(void))((unsigned long)&_stack),
   _start,
   nmi_ISR,
   hardFault_ISR,
   memManage_ISR,
   busFault_ISR,
   usageFault_ISR,
   0, 0, 0, 0,
   svc_ISR,
   debugMon_ISR,
   0,
   pendSV_ISR,
   sysTick_ISR,

   // 16 VETORES DE INTERRUPÇÃO DE SISTEMA
   wdt_ISR,
   timer0_ISR,
   timer1_ISR,
   timer2_ISR,
   timer3_ISR,
   uart0_ISR,
   uart1_ISR,
   uart2_ISR,
   uart3_ISR,
   pwm_ISR,
   i2c0_ISR,
   i2c1_ISR,
   i2c2_ISR,
   spi_ISR,
   ssp0_ISR,
   ssp1_ISR,
   pll0_ISR,
   rtc_ISR,
   eint0_ISR,
   eint1_ISR,
   eint2_ISR,
   eint3_ISR,
   adc_ISR,
   bod_ISR,
   usbHost_ISR,
   can_ISR,
   dma_ISR,
   i2s_ISR,
   enet_ISR,
   rit_ISR,
   mcpwm_ISR,
   qei_ISR,
   pll1_ISR,
   usbAct_ISR,
   canAct_ISR,
   uart4_ISR,
   ssp2_ISR,
   lcd_ISR,
   gpio_ISR,
   pwm0_ISR,
   eeprom_ISR
}; /* gVectors */

/*
 * And here are the weak interrupt handlers.
 */
void nmi_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr NMI");
	#endif
	if (vicerr_Functions) vicerr_Functions(1);
	while(1);
}
void hardFault_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr hardFault"CMD_TERMINATOR);
	plognp("Possiveis causas:"CMD_TERMINATOR);
	plognp("  Acesso a um hardware não ligado ou com clock desligado"CMD_TERMINATOR);
	plognp("  Acesso a memória inexistente"CMD_TERMINATOR);
	plognp("  Registrador acessando pino I/O apontando para duas funções diferentes"CMD_TERMINATOR);
	#endif
	if (vicerr_Functions) vicerr_Functions(2);
	while(1);
}
void memManage_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr memManage");
	#endif
	if (vicerr_Functions) vicerr_Functions(3);
	while(1);
}
void busFault_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr busFault");
	#endif
	if (vicerr_Functions) vicerr_Functions(4);
	while(1);
}
void usageFault_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr usageFault_ISR");
	#endif
	if (vicerr_Functions) vicerr_Functions(5);
	while(1);
}
void svc_ISR (void) {
#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr svc");
	#endif
	if (vicerr_Functions) vicerr_Functions(6);
	while(1);
}
void debugMon_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr debugMon");
	#endif
	if (vicerr_Functions) vicerr_Functions(7);
	while(1);
}
void pendSV_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr pendSV");
	#endif
	if (vicerr_Functions) vicerr_Functions(8);
	while(1);
}
void sysTick_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr sysTick");
	#endif
	if (vicerr_Functions) vicerr_Functions(9);
	while(1);
}
void wdt_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr wdt");
	#endif
	if (vicerr_Functions) vicerr_Functions(10);
	while(1);
}
void timer0_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr timer0");
	#endif
	if (vicerr_Functions) vicerr_Functions(11);
	while(1);
}
void timer1_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr timer1");
	#endif
	if (vicerr_Functions) vicerr_Functions(12);
	while(1);
}
void timer2_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr timer2");
	#endif
	if (vicerr_Functions) vicerr_Functions(13);
	while(1);
}
void timer3_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr timer3");
	#endif
	if (vicerr_Functions) vicerr_Functions(14);
	while(1);
}
void uart0_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr uart0");
	#endif
	if (vicerr_Functions) vicerr_Functions(15);
	while(1);
}
void uart1_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr uart1");
	#endif
	if (vicerr_Functions) vicerr_Functions(16);
	while(1);
}
void uart2_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr uart2");
	#endif
	if (vicerr_Functions) vicerr_Functions(17);
	while(1);
}
void uart3_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr uart3");
	#endif
	if (vicerr_Functions) vicerr_Functions(18);
	while(1);
}
void pwm_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr pwm");
	#endif
	if (vicerr_Functions) vicerr_Functions(19);
	while(1);
}
void i2c0_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr i2c0");
	#endif
	if (vicerr_Functions) vicerr_Functions(20);
	while(1);
}
void i2c1_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr i2c1");
	#endif
	if (vicerr_Functions) vicerr_Functions(21);
	while(1);
}
void i2c2_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr i2c2");
	#endif
	if (vicerr_Functions) vicerr_Functions(22);
	while(1);
}
void spi_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr spi");
	#endif
	if (vicerr_Functions) vicerr_Functions(23);
	while(1);
}
void ssp0_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr spp0");
	#endif
	if (vicerr_Functions) vicerr_Functions(24);
	while(1);
}
void ssp1_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr spp1");
	#endif
	if (vicerr_Functions) vicerr_Functions(25);
	while(1);
}
void pll0_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr pll0");
	#endif
	if (vicerr_Functions) vicerr_Functions(26);
	while(1);
}
void rtc_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr ");
	#endif
	if (vicerr_Functions) vicerr_Functions(27);
	while(1);
}
void eint0_ISR (void) {
#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr eint0");
	#endif
	if (vicerr_Functions) vicerr_Functions(28);
	while(1);
}
void eint1_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr eint1");
	#endif
	if (vicerr_Functions) vicerr_Functions(29);
	while(1);
}
void eint2_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr eint2");
	#endif
	if (vicerr_Functions) vicerr_Functions(30);
	while(1);
}
void eint3_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr eint3");
	#endif
	if (vicerr_Functions) vicerr_Functions(31);
	while(1);
}
void adc_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr adc");
	#endif
	if (vicerr_Functions) vicerr_Functions(32);
	while(1);
}
void bod_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr bod");
	#endif
	if (vicerr_Functions) vicerr_Functions(33);
	while(1);
}
void usbHost_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr usb");
	#endif
	if (vicerr_Functions) vicerr_Functions(34);
	while(1);
}
void can_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr can");
	#endif
	if (vicerr_Functions) vicerr_Functions(35);
	while(1);
}
void dma_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr dma");
	#endif
	if (vicerr_Functions) vicerr_Functions(36);
	while(1);
}
void i2s_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr i2s");
	#endif
	if (vicerr_Functions) vicerr_Functions(37);
	while(1);
}
void enet_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr enet");
	#endif
	if (vicerr_Functions) vicerr_Functions(38);
	while(1);
}
void rit_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr rit");
	#endif
	if (vicerr_Functions) vicerr_Functions(39);
	while(1);
}
void mcpwm_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr mcpwm");
	#endif
	if (vicerr_Functions) vicerr_Functions(40);
	while(1);
}
void qei_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr qei");
	#endif
	if (vicerr_Functions) vicerr_Functions(41);
	while(1);
}

void pll1_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr pll1");
	#endif
	if (vicerr_Functions) vicerr_Functions(42);
	while(1);
}

void usbAct_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr USB ACT");
	#endif
	if (vicerr_Functions) vicerr_Functions(43);
	while(1);
}
void canAct_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr CAN ACT");
	#endif
	if (vicerr_Functions) vicerr_Functions(44);
	while(1);
}
void uart4_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr uart4");
	#endif
	if (vicerr_Functions) vicerr_Functions(45);
	while(1);
}
void ssp2_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr ssp2");
	#endif
	if (vicerr_Functions) vicerr_Functions(46);
	while(1);
}
void lcd_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr lcd");
	#endif
	if (vicerr_Functions) vicerr_Functions(47);
	while(1);
}
void gpio_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr gpio");
	#endif
	if (vicerr_Functions) vicerr_Functions(48);
	while(1);
}
void pwm0_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr pwm0");
	#endif
	if (vicerr_Functions) vicerr_Functions(49);
	while(1);
}
void eeprom_ISR (void) {
	#if (VICERR_USE_DEBUG == pdON)
	plognp("vicErr eeprom");
	#endif
	if (vicerr_Functions) vicerr_Functions(50);
	while(1);
}

// -----------------------------------------------------------------------------------------------
// Descrição:	Habilita a interrupção de um determinado periférico
// Parametro:	ID da interrupção do periférico
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------
void nvic_enableIRQ(u8 numIRQ) {
	#if (VIC_USE_DEBUG == pdON)
	plognp("enableIRQ [%u][%u]"CMD_TERMINATOR, numIRQ, (u32)numIRQ/VIC_SIZE);
	#endif
	reg32 *addrPort = &ISER0; 		// Pegar o endereço do registrador para habilitar a int
	#if (VIC_USE_DEBUG == pdON)
	plognp("addrPort 0x%x"CMD_TERMINATOR, addrPort);
	#endif

	addrPort+=(reg32)(numIRQ/VIC_SIZE); 	// Determinar em que registrador pertence ao periférico
	#if (VIC_USE_DEBUG == pdON)
	plognp("addrPort 0x%x = 0x%x "CMD_TERMINATOR, addrPort, (1 << (numIRQ%VIC_SIZE)));
	#endif
	*addrPort = (1 << (numIRQ%VIC_SIZE));	// Habilitar interrupção do periférico
}

// -----------------------------------------------------------------------------------------------
// Descrição:	Desabilita a interrupção de um determinado periférico
// Parametro:	ID da interrupção do periférico
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------
void nvic_disableIRQ(u8 numIRQ) {
	reg32 *addrPort = &ICER0; 		// Pegar o endereço do registrador do periférico
	addrPort+=(reg32)(numIRQ/VIC_SIZE); 	// Determinar em que registrador pertence ao periférico
	*addrPort = (1 << (numIRQ%VIC_SIZE));	// Desabilitar a  interrupção do periférico
}

// -----------------------------------------------------------------------------------------------
// Descrição:	Verifica se o a interrupção de um determinado periférico está pendente ou não
// Parametro:	ID da interrupção do periférico
// Retorna:		Retorna pdTRUE e a interrupção do periférico está pendente, senão pdFALSE
// -----------------------------------------------------------------------------------------------
u8 nvic_getPendingIRQ(u8 numIRQ) {
	reg32 *addrPort = &ISER0; 			// Pegar o endereço do registrador do periférico
	addrPort+=(reg32)(numIRQ/VIC_SIZE); // Determinar em que registrador pertence ao periférico
	return ( *addrPort & (1 << (numIRQ%VIC_SIZE)) )?pdTRUE:pdFALSE;
}

// -----------------------------------------------------------------------------------------------
// Descrição:	Habilita a pendencia da interrupção de um determinado periférico
// Parametro:	ID da interrupção do periférico
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------
void nvic_setPendingIRQ(u8 numIRQ) {
	reg32 *addrPort = &ISER0; 		// Pegar o endereço do registrador do periférico
	addrPort+=(reg32)(numIRQ/VIC_SIZE); 	// Determinar em que registrador pertence ao periférico
	*addrPort = (1 << (numIRQ%VIC_SIZE));	// Habilitar a pendencia interrupção do periférico
}

// -----------------------------------------------------------------------------------------------
// Descrição:	Desabilita a pendencia da interrupção de um determinado periférico
// Parametro:	ID da interrupção do periférico
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------
void nvic_clearPendingIRQ(u8 numIRQ) {
	reg32 *addrPort = &ICPR0; 		// Pegar o endereço do registrador do periférico
	addrPort+=(reg32)(numIRQ/VIC_SIZE); 	// Determinar em que registrador pertence ao periférico
	*addrPort = (1 << (numIRQ%VIC_SIZE));	// Desabilitar a pendencia interrupção do periférico
}


// -----------------------------------------------------------------------------------------------
// Descrição:	Verifica se o a interrupção de um determinado periférico está ativa ou não
// Parametro:	ID da interrupção do periférico
// Retorna:		Retorna pdTRUE e a interrupção do periférico está ativa, senão pdFALSE
// -----------------------------------------------------------------------------------------------
u8 nvic_getActiveIRQ(u8 numIRQ) {
	reg32 *addrPort = &IABR0; 			// Pegar o endereço do registrador do periférico
	addrPort+=(reg32)(numIRQ/VIC_SIZE); // Determinar em que registrador pertence ao periférico
	return ( *addrPort & (1 << (numIRQ%VIC_SIZE)) )?pdTRUE:pdFALSE;
}


// -----------------------------------------------------------------------------------------------
// Descrição:	Ajusta a prioridade das interrupções dos periféricos ou do núcleo do ARM
// Parametro:	ID da interrupção
//					Valores negativos são do núcleo do ARM, e os positivos são dos periféricos
//				Nível da interrupção
//					Para os periféricos de 0 a 31
// Retorna:		Retorna pdTRUE e a interrupção do periférico está ativa, senão pdFALSE
// -----------------------------------------------------------------------------------------------
void nvic_setPriority(s32 numIRQ, u8 priority) {
	#if (VIC_USE_DEBUG == pdON)
	plognp("set pri %d %u"CMD_TERMINATOR, numIRQ, priority);
	#endif
	if(numIRQ < 0) {
		reg32 *addrPort = &SHPR1; 				// Pegar o endereço do registrador do periférico
		addrPort+=(reg32)(((numIRQ&0xf)-4)/4); 	// Determinar em que registrador pertence ao periférico.
												//   (numIRQ&0xf)-4 para converte o valor negativo
		// cada registrador de 32 bits, guarda a prioridade de 4 periféricos
		// logo cada byte é para cada periférico, então precisamo encontrar em que local [e
		reg8 *offset = (reg8*) addrPort;
		offset+=(reg8)((numIRQ&0xf)-4)%4;
		*offset = ((priority << (8 - VIC_PRIO_BITS)) & 0xff);
	} else {
		reg32 *addrPort = &IPR_BASE; 		// Pegar o endereço do registrador do periférico
		#if (VIC_USE_DEBUG == pdON)
		plognp("addrPort 0x%x"CMD_TERMINATOR, addrPort);
		#endif
		addrPort+=(reg32)(numIRQ/4); 		// Determinar em que registrador pertence ao periférico
		#if (VIC_USE_DEBUG == pdON)
		plognp("addrPort 0x%x"CMD_TERMINATOR, addrPort);
		#endif
		// cada registrador de 32 bits, guarda a prioridade de 4 periféricos
		// logo cada byte é para cada periférico, então precisamo encontrar em que local [e
		reg8 *offset = (reg8*) addrPort;
		#if (VIC_USE_DEBUG == pdON)
		plognp("offset init 0x%x"CMD_TERMINATOR, offset);
		#endif
		offset+=(reg8)numIRQ%4;
		#if (VIC_USE_DEBUG == pdON)
		plognp("offset 0x%x = 0x%x"CMD_TERMINATOR, offset, ((priority << (8 - VIC_PRIO_BITS)) & 0xff));
		#endif
		*offset = ((priority << (8 - VIC_PRIO_BITS)) & 0xff);
	}
}


// -----------------------------------------------------------------------
// Descrição: 	Anexa uma routina para exibição de erro de interrupção
// Parametro: 	int_func
//					Ponteiro da rotina que tratará o erro de vetor de interrupção
//					Ex:	nvic_SetIntHandler(showVicError);
//
//					Ex1:
//						void showVicError(int n) {
//							register_error_eeprom(n);
//							pisca_cod_led_xsegundos();
//							watchdogOn();
//							EsperarPeloResetDoWatchDog()
//						}

//					Ex2:
//						void showVicError(int n) {
//							static int x;
//							if (n>10) n=10;
//							while(1) {
//								for (x=0;x<n;x++) {	LED1_ON = LED1; delay_ms(250); LED1_OFF = LED1; delay_ms(250);}
//								delay_ms(1000);
//							}
//						}
//						ATENÇÃO: NUNCA USA O DELAY COM INTERRUPÇÃO, USE A LIB COM LOOP, pois se este é um procedimento de erro de
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void nvic_SetIntHandler(void (*func)(int) ) {
	vicerr_Functions = func;
}
