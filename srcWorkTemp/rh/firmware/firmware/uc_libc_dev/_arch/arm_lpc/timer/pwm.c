#include "pwm.h"

u32 pwm_match_counter;
void pwm_ISR(void);

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Iniciliza o PWM no timer0, configura os pinos de saida do uC e a interrupção para isso
// Parametros:	Nenhum
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna código de erro
// ----------------------------------------------------------------------------------------------------------------
int pwm_Init(void) {
	PCONP |= PCPWM0;													// Ligar o PWM no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões

    pwm_match_counter = 0;

    PINSEL0 = (PINSEL0&0xFFF03FF0) | 0x000A800A;						// Ajusta todos os pinos de PWM
    PINSEL1 = (PINSEL1&0xFFFFF3FF) | 0x00000400;						// Ajusta todos os pinos de PWM
	
    PWMTCR = PWMCR_RESET;												// Limpa contador	
       
    PWMPR = 0;															// Prescaler para o PWM count frequency:Fpclk
    PWMMCR = TMCR_PWM_MR0_INT_ENABLE | TMCR_PWM_MR0_RESET;				// Liga int para o PWM0 e zera o contador quando PWMTC = PWM0
            
    pwm_Set(0, 0);
      
	if (!irq_Install(VIC_PWM, pwm_ISR, PWM_PRIO_LEVEL))
	 	 return errIRQ_INSTALL;
   	else return pdPASS;

}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Ajusta PWM
// Parametros:	Duty Cicle e OffSet
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void pwm_Set(u32 Cycle, u32 OffSet) {			
    PWMMR0 = Cycle;
    PWMMR1 = Cycle * 5/6 + OffSet;
    PWMMR2 = Cycle * 2/3 + OffSet;
    PWMMR3 = Cycle * 1/2 + OffSet;
    PWMMR4 = Cycle * 1/3 + OffSet;
    PWMMR5 = Cycle * 1/6 + OffSet;

    // O LER será limpo quando MATCH0 o capturar, para ler e executar um novo valor para o registrador de comparação,
    // todos os PWMLERs devem ser lidos, ou seja, todos os registradores PWM latch devem ser habilitados
    PWMLER = LER0_ENABLE | LER1_ENABLE | LER2_ENABLE | LER3_ENABLE | LER4_ENABLE | LER5_ENABLE | LER6_ENABLE;
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa PWM
// Parametros:	
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void pwm_Start( void ) {
    // Todos os PWM habilitados e com controle de com bordas simples
    PWMPCR = PWMPCR_PWM_ENABLE1 | PWMPCR_PWM_ENABLE2 | PWMPCR_PWM_ENABLE3 | PWMPCR_PWM_ENABLE4 | PWMPCR_PWM_ENABLE5 | PWMPCR_PWM_ENABLE6;
    //PWMPCR = PWMPCR_PWM_ENABLE1;
    PWMTCR = PWMCR_ENABLE | PWMCR_PWM_ENABLE;	// Contadores e PWM habilitados
}

// ----------------------------------------------------------------------------------------------------------------
// Descrição: 	Para PWM
// Parametros:	Nenhum
// Retorna:		Nada
// ----------------------------------------------------------------------------------------------------------------
void pwm_Stop( void ) {
    // fazer uma comparação onde todos os PWMs desligados desligar tudo
    
    PWMPCR = 0;			// Desabilita a saida do PWM para o pino do uC
    PWMTCR = 0x00;		// Desliga tudo
}


// ################################################################################################################
// PWM SERVIÇO DE INTERRUPÇÃO
// ################################################################################################################
// ################################################################################################################
void pwm_ISR(void) {
 	ISR_ENTRY;															// Procedimento para entrada da interrupção
    u32 regVal;
 
    regVal = PWMIR;														// Captura os valores do registradore de int    
        
    if ( regVal & PWMIR_MR0I )
		pwm_match_counter++;	

    PWMIR |= regVal;													// Limpa int flag MATCH0
    
    VICSoftIntClr = (1<<VIC_PWM);										// Limpa interrupção de software
    ISR_EXIT;															// Procedimento para saida da interrupção
}

