/*
 * Esta lib controla somente uma saida PWM no controle de borda modo simples sem uso de interrupções
 *
 * Para fornecer uma saída PWM simples em um pino é requerido dois registradores
 * de comparações (MATCH). Um registrador (MR0) para controlar a taxa o ciclo do PWM,
 * reiniciando a contagem na comparação.
 * O outro registrador controla a posição da borda do PWM.
 * A saídas de PWM nos pinos sempre subirão no início de cada ciclo PWM,
 * quando o contador se iguala ao registrador MR0.
 *
 * controle e borda REG PWM1PCR bits PWMSELx [x=2..6] Para pwm1 não tem opção de controle de borda dupla
 *
 * Canal PWM 1 para saida PWM0_1 usar MR0 para set com MR1 para reset
 * Canal PWM 2 para saida PWM0_2 usar MR0 para set com MR2 para reset
 * Canal PWM 3 para saida PWM0_3 usar MR0 para set com MR3 para reset
 * Canal PWM 4 para saida PWM0_4 usar MR0 para set com MR4 para reset
 * Canal PWM 5 para saida PWM0_5 usar MR0 para set com MR5 para reset
 * Canal PWM 6 para saida PWM0_6 usar MR0 para set com MR6 para reset
 *
 * Exemplo: iniciando um PWM com duty cycle de 50%
 * 	pwm0_Init(1);
 *	pwm0_DutyCycle(80);
 *	pwm0_On();
 *
 * */

#include "pwm0.h"

static int pwmPulseWidth = 20000;
static int pwmPercentage = 50;

// Configura o PWM para 20Khz com 50% de duty cicle, e ajusta o pino de saida para PWM.
//	Porém, o pwm permanece desligado
// Podemos alterar a frequência e o duty cicle simultaneamente de acordo com a necessidade
int pwm0_Init(void) {
	PCONP |= PCPWM0;				// Ligar o pwm no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões

	pwm0_ConfigPort();				// Habilita o pino do ARM para saida PWM
	pwm0_SetPrescale(1);			// Ajusta o precaler do contador TC
	PWM0MR0 = PCLK/pwmPulseWidth;
	PWM0MCR = 2; 					// Reset on PWMMR0: the PWMTC will be reset if PWMMR0 matches it.
	PWM0PCR = PWM0PCR_PWM_ENABLE6; 	// habilita pwm0[6]

	return pdPASS;
}

// Para o PWM
void pwm0_Off(void) {
	PWM0TCR = PWM0TCR_RESET; // Reset o contador e mantem em reset
}

// Liga o PWM
void pwm0_On(void) {
	PWM0TCR = PWM0TCR_ENABLE | PWM0TCR_PWMEN; // Habilita contandor e o timer assume modo PWM
}

// Ajusta o prescale do timer. Valor do prescale de 1 a 2^^32bits
void pwm0_SetPrescale(u32 prescale) {
	PWM0PR = prescale - 1;
}

// Ajuste a frequencia do PWM em Hertz
// Se prescaler for diferente de 1, a frequencia não é ajustada corretamente
int pwm0_Frequency (int frequency) {
  	if ((frequency < 1) || (frequency >= (int) PCLK)) return -1;

  	PWM0MR0 = (PCLK / (pwmPulseWidth = frequency));
  	PWM0MR6 = ((PCLK / pwmPulseWidth) * pwmPercentage) / 100;
  	PWM0LER = PWM0LER6_ENABLE | PWM0LER0_ENABLE;

  	return 0;
}

//// ajuste PWM por largura de pulso
//int pwm0_PulseWidthInMicroseconds (int us) {
//  	u32 count = (PCLK / 1000000) * us;
//
//  	if (count > PWM0MR0)  return -1;
//
//  	PWM0MR6 = count;
//  	PWM0LER = PWM0LER6_ENABLE;
//  	return 0;
//}

// Ajuste o duty cycle do PWM
int pwm0_DutyCycle (int percentage) {
  	if ((percentage < 0) || (percentage > 100))  return -1;

  	PWM0MR6 = ((PCLK / pwmPulseWidth) * (pwmPercentage = percentage)) / 100;
  	PWM0LER = PWM0LER6_ENABLE;
  	return 0;
}

