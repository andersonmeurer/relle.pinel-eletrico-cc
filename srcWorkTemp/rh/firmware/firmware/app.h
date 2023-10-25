#ifndef APP_H
#define APP_H

// ###########################################################################################################################################
// DESENVOLVIMENTO
#define MODE_DEVELOPMENT	pdON

#if (MODE_DEVELOPMENT == pdON)
#define LOG_APP				pdOFF		// Usar ou n�o o log do controle da aplica��o
#define LOG_MODBUS_SLAVE	pdOFF
#define LOG_MODBUS_MASTER	pdOFF
#else
#define LOG_APP				pdOFF
#define LOG_MODBUS_SLAVE	pdOFF
#define LOG_MODBUS_MASTER	pdOFF
#endif

#define	TP_DBGTICK_DIR		GPIO2_DIR
#define	TP_DBGTICK_ON 		GPIO2_SET
#define	TP_DBGTICK_OFF		GPIO2_CLR
#define	TP_DBGTICK			BIT_0

// ###########################################################################################################################################
// MODELO DO RECURSOS DE HARDAWARES
#define MODEL			"hr01"	// somente 4 chars, por causa so modbus
#define FIRMWARE_VERSION "2.1"	// usar x.y s� com uma casa decimal, pois estamos usando a impress�o de temperatura
								// usar 3 chars por causa do modbus
#define PROGAM_NAME "Hardware Resource " MODEL " quadro cc mod2 - firmware v" FIRMWARE_VERSION " [compiled in " __DATE__ " " __TIME__ "]" CMD_TERMINATOR

// ###########################################################################################################################################
// INCLUDES

#include "_config_cpu_.h"
#include "systick.h"
#include "delay.h"
#include "modbus_slave.h"
#include "modbus_master.h"
#include <math.h>


// ###############################################################################
// CONTROLE DO SISTEMA

#define RELAY1_DIR		GPIO2_DIR
#define RELAY1_ON		GPIO2_SET
#define RELAY1_OFF		GPIO2_CLR
#define RELAY1			BIT_2

#define RELAY2_DIR		GPIO1_DIR
#define RELAY2_ON		GPIO1_SET
#define RELAY2_OFF		GPIO1_CLR
#define RELAY2			BIT_24

#define RELAY3_DIR		GPIO1_DIR
#define RELAY3_ON		GPIO1_SET
#define RELAY3_OFF		GPIO1_CLR
#define RELAY3			BIT_23

#define RELAY4_DIR		GPIO2_DIR
#define RELAY4_ON		GPIO2_SET
#define RELAY4_OFF		GPIO2_CLR
#define RELAY4			BIT_6

#define RELAY5_DIR		GPIO2_DIR
#define RELAY5_ON		GPIO2_SET
#define RELAY5_OFF		GPIO2_CLR
#define RELAY5			BIT_5

#define RELAY6_DIR		GPIO2_DIR
#define RELAY6_ON		GPIO2_SET
#define RELAY6_OFF		GPIO2_CLR
#define RELAY6			BIT_4

#define RELAY7_DIR		GPIO2_DIR
#define RELAY7_ON		GPIO2_SET
#define RELAY7_OFF		GPIO2_CLR
#define RELAY7			BIT_3

#define RELAY8_DIR		GPIO1_DIR
#define RELAY8_ON		GPIO1_SET
#define RELAY8_OFF		GPIO1_CLR
#define RELAY8			BIT_22

// ###########################################################################################################################################
// MODBUS
#define rs485_ENTXS_DIR		GPIO2_DIR
#define rs485_ENTXS_ON		GPIO2_SET
#define rs485_ENTXS_OFF 	GPIO2_CLR
#define rs485_ENTXS			BIT_1

#define rs485_ENTXM_DIR		GPIO0_DIR
#define rs485_ENTXM_ON		GPIO0_SET
#define rs485_ENTXM_OFF 	GPIO0_CLR
#define rs485_ENTXM			BIT_17

#define BAUDRATE_SBC		57600 // N�o usar acima de 57600, pois h� erro de recep��o do raspberry.
									// Deve ser algum bug de hardware do rasp porque o baudrate do rasp n�o fica indentico do ARM
									// pois a comunica��o com PC a 115200 funciona bem.
									// Ou a tolerancia de erro no rasp n�o � t�o grande como no PC onde o ARM tem um erro consider�vel
									//	TODO Quando usar o oscilador interno do ARM refazer os testes a sabe com usando oscilador interno do ARM isso se resolve

#define BAUDRATE_MULTIMETER 57600

typedef enum {
	cmdNONE = 0,
    cmdGET_INFO,        // Comando para ler os registradores: modelo, vers�o firmware e modo de opera��es dos reles
    cmdGET_MULTIMETER 	// Comando para ler o multimetro
} tCommand;

// ###############################################################################
// CONTROLE DO SISTEMA

typedef struct {
	int stsCom;	// Status de comunica��o:
				//		0: Sem comunica��o com o dispositivo. O mesmo n�o est� conectado, ou est� desligado, ou n�o h� dispositivo neste endere�o.
				//		1: O mult�metro recebeu uma fun��o que n�o foi implementada;
				//		2: Foi acessado a um endere�o de registrador inexistente;
				//		3: Foi tentado gravar um valor inv�lido no registrador do mult�metro;
				//		4: Um irrecuper�vel erro ocorreu enquanto o mult�metro estava tentando executar a a��o solicitada;
				//		5: Comunica��o estabelecida com sucesso
	u16 sts;	// Status do multimetro
				// Fun��o assumida do mult�metro bit[4]:
				//		0: Amper�metro;
				// 		1: Volt�metro.
				// Status do sensor bits[3:0]:
				// 		0: Sinaliza que o mult�metro est� lendo pela primeira vez o sensor. Isto somente acontece no momento que o mult�metro � ligado.
				// 		1: O mult�metro j� cont�m o valor convertido;
				// 		2: Sinaliza um erro, indica que o valor est� abaixo da escala permitida pelo mult�metro;
				// 		3: Sinaliza um erro, indica que o valor est� acima da escala permitida pelo mult�metro;
	u16 valueLSW; // Word menos significativo do valor da amperagem ou volt�metro dependo da fun��o assumida do Mult�metro
	u16 valueMSW; // Word mais significativo do valor da amperagem ou volt�metro dependo da fun��o assumida do Mult�metro
	char model[5];
	char firmware[3];
} tMultimeter;
#define nMULTIMETER  16

typedef struct {
	uint modbusID; 		// preciso ler os jumpers
	uint douts;			// estados das saidas digitais
		#define nOUTS 		9
	uint relays;		// estados dos reles
		#define nRELAY 		5
	uint nMultimetersGeren;	// Determina a quantidade de multimetros o RH deve gerenciar, inicio pelo ID=1
		#define nMULTIMETER_GEREN 10
	tMultimeter multimeter[nMULTIMETER];
} tControl, *pControl;
tControl control;

void app_Init(void);
void app_Tick(void);
void app_SetOut(uint act);
void app_SetRelay(uint act);

void prompt_Process(void);

void modbus_Init(uint id);
void modbus_Process(void);
void modbus_SendCommand(uint slaveID, tCommand c);

#endif
