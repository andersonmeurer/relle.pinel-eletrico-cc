#include "app.h"
#include "stdio_uc.h"

// ###########################################################################################
// INIT APLICATION
void app_Init(void) {
	control.nMultimetersGeren = nMULTIMETER_GEREN;
	control.modbusID = 1; // TODO. colocar jumpers externo
	int x; for(x=0;x<nMULTIMETER;x++) {
		control.multimeter[x].stsCom = 0;
		control.multimeter[x].sts = 0;
		memset(control.multimeter[x].model, 0 , 5);
		memset(control.multimeter[x].firmware, 0 , 3);
	}

	app_SetOut(0);
	app_SetRelay(0x3d); // chaves 1, 3, 4, 5, 6 ligadas
}

void app_SetOut(uint act) {
	control.douts = act;
	// TODO acionar ou não as saida do ARM
}

void app_SetRelay(uint act) {
	control.relays = act;
	if ((act & 0x1)>0) RELAY1_ON = RELAY1; else RELAY1_OFF = RELAY1;
	if ((act & 0x2)>0) RELAY2_ON = RELAY2; else RELAY2_OFF = RELAY2;
	if ((act & 0x4)>0) RELAY3_ON = RELAY3; else RELAY3_OFF = RELAY3;
	if ((act & 0x8)>0) RELAY4_ON = RELAY4; else RELAY4_OFF = RELAY4;
	if ((act & 0x10)>0) RELAY5_ON = RELAY5; else RELAY5_OFF = RELAY5;
	if ((act & 0x20)>0) RELAY6_ON = RELAY6; else RELAY6_OFF = RELAY6;
	if ((act & 0x40)>0) RELAY7_ON = RELAY7; else RELAY7_OFF = RELAY7;
	if ((act & 0x80)>0) RELAY8_ON = RELAY8; else RELAY8_OFF = RELAY8;
}

// ###########################################################################################
// TICK
// Esta função está sendo chamada a cada 1ms
void app_Tick(void) {
	// ATENÇÃO: Tempo de execução de TODOS os TICKS dentro dessa função NÃO pode ser superior a 1ms
	TP_DBGTICK_ON = TP_DBGTICK;
	modbus_MasterTick();
	modbus_SlaveTick();
	TP_DBGTICK_OFF = TP_DBGTICK;
}
