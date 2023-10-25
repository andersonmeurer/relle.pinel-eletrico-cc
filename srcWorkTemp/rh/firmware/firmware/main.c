/**
 * Criado em 22/04/2015
 * Atualizado em 21/01/2016
 * O recurso de hardware fica na espera de comandos vindo do SBC
 * A comunicação com o SBC é via UART 57600 bps com protocolo modbus
 */

#include "vic.h"
#include "app.h"
#if (MODE_DEVELOPMENT == pdON)
#include "uart0_irq.h"
#endif
#include "stdio_uc.h"

static void showVicError(int n) {
	irq_Disable();

	static int x;
	if (n>10) n=10;
	while(1) {
		for (x=0;x<n;x++) {	ledERROR_On(); buzzer_On(); delay_ms(250); ledERROR_Off(); buzzer_Off(); delay_ms(250);}
		delay_ms(1000);
	}
}

int main(void) {
	nvic_SetIntHandler(showVicError);

	// NÃO USAR UART COM INTERRUPÇÃO PARA O LOG, POIS CONSTANTEMENTE A
  	// POSIÇÃO DOS VETORES DE INTERRUPÇÕES COM O USO DO BOOTLOADER

  	// REGISTRANDO AS ENTRADAS E SAÍDAS PADRÃO AOS SEUS RESPECTIVOS DISPOSTIVOS
	// PORTA PARA CLI DEVE SER COM INTERRUPÇÃO
  	#if (MODE_DEVELOPMENT == pdON)
  	lgetchar_RegisterDev(uart0_GetChar);
	lputchar_RegisterDev(uart0_PutChar);
	ldebug_RegisterDev(uart0_BasicPutChar);
	uart0_Init(115200);
	#endif

	RELAY1_DIR |= RELAY1;
	RELAY2_DIR |= RELAY2;
	RELAY3_DIR |= RELAY3;
	RELAY4_DIR |= RELAY4;
	RELAY5_DIR |= RELAY5;
	RELAY6_DIR |= RELAY6;
	RELAY7_DIR |= RELAY7;
	RELAY8_DIR |= RELAY8;

	TP_DBGTICK_DIR |= TP_DBGTICK;
	TP_DBGTICK_OFF = TP_DBGTICK;
	ledERROR_Init();
	app_Init();
	systime_Init();
	systime_SetIntHandler(app_Tick, 1); // Chamar o a função callsystem a cada 1ms OBS: Levando em consideração o SYSTIME_IRQ_TICK_DIV
	modbus_Init(control.modbusID);

	irq_Enable(); 	// Habilitar interrupções

	#if (MODE_DEVELOPMENT == pdON)
	lprintf(CMD_TERMINATOR);
	lprintf(PROGAM_NAME);
	lprintf("ucLibc version "__VERSION CMD_TERMINATOR);
	#endif

	while (1) {
		prompt_Process();
		modbus_Process();
	}

	return (0);
}
