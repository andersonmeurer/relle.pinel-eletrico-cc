#include "led_error.h"
#ifndef LED_ERROR_DIR
#include "_config_cpu_device.h"
#endif

static void led_SetupPorts(void);
static void led_State(int st);

#if defined(CPU_ARM_LPC)
static void led_SetupPorts(void) {
	LED_ERROR_DIR |= LED_ERROR;
	LED_ERROR_OFF |= LED_ERROR;
}

static void led_State(int st) {
	if (st == pdON) LED_ERROR_ON |= LED_ERROR; else LED_ERROR_OFF |= LED_ERROR;
}
#endif

void ledError_Init(void) {
	led_SetupPorts();
}

// função de sistema para ser chamado a cada 1ms
// o numero do erro representa a quantidade de vezes que led vai piscar. Se for valor 0 deixa o led apagado
void ledError_Tick(int error) {
	//if (error == 0) { led_State(pdOFF); return; }

	static int count1 = 0;
	static int count2 = 0;
	static int ff = 0;
	static int nTimes = 0;

	if (count1==0) {
		if (count2==0) { // Prepara o intervalo entre as piscadas do led
			count2 = 1000;
			nTimes = 2*(error);
		}

		if (nTimes>0) { // Pisca o led por 250 ms vezes nTimes
			if (ff)	led_State(pdOFF); else led_State(pdON);
			count1 = 250;
			ff ^= 1;
			nTimes--;
		} else { // Mantém o led apagado por 1 segundo
			led_State(pdOFF);
			if (count2) count2--;
		}
	}

	if (count1) count1--;
}

