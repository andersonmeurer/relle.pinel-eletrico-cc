#ifndef APP_H
#define APP_H

// ###########################################################################################################################################
// DESENVOLVIMENTO
#define MODE_DEVELOPMENT	pdON

#if (MODE_DEVELOPMENT == pdON)
#define LOG_APP				pdOFF		// Usar ou não o log do controle da aplicação
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
#define FIRMWARE_VERSION "2.1"	// usar x.y só com uma casa decimal, pois estamos usando a impressão de temperatura
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

#define BAUDRATE_SBC		57600 // Não usar acima de 57600, pois há erro de recepção do raspberry.
									// Deve ser algum bug de hardware do rasp porque o baudrate do rasp não fica indentico do ARM
									// pois a comunicação com PC a 115200 funciona bem.
									// Ou a tolerancia de erro no rasp não é tão grande como no PC onde o ARM tem um erro considerável
									//	TODO Quando usar o oscilador interno do ARM refazer os testes a sabe com usando oscilador interno do ARM isso se resolve

#define BAUDRATE_MULTIMETER 57600

typedef enum {
	cmdNONE = 0,
    cmdGET_INFO,        // Comando para ler os registradores: modelo, versão firmware e modo de operações dos reles
    cmdGET_MULTIMETER 	// Comando para ler o multimetro
} tCommand;

// ###############################################################################
// CONTROLE DO SISTEMA

typedef struct {
	int stsCom;	// Status de comunicação:
				//		0: Sem comunicação com o dispositivo. O mesmo não está conectado, ou está desligado, ou não há dispositivo neste endereço.
				//		1: O multímetro recebeu uma função que não foi implementada;
				//		2: Foi acessado a um endereço de registrador inexistente;
				//		3: Foi tentado gravar um valor inválido no registrador do multímetro;
				//		4: Um irrecuperável erro ocorreu enquanto o multímetro estava tentando executar a ação solicitada;
				//		5: Comunicação estabelecida com sucesso
	u16 sts;	// Status do multimetro
				// Função assumida do multímetro bit[4]:
				//		0: Amperímetro;
				// 		1: Voltímetro.
				// Status do sensor bits[3:0]:
				// 		0: Sinaliza que o multímetro está lendo pela primeira vez o sensor. Isto somente acontece no momento que o multímetro é ligado.
				// 		1: O multímetro já contém o valor convertido;
				// 		2: Sinaliza um erro, indica que o valor está abaixo da escala permitida pelo multímetro;
				// 		3: Sinaliza um erro, indica que o valor está acima da escala permitida pelo multímetro;
	u16 valueLSW; // Word menos significativo do valor da amperagem ou voltímetro dependo da função assumida do Multímetro
	u16 valueMSW; // Word mais significativo do valor da amperagem ou voltímetro dependo da função assumida do Multímetro
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
