#include "amperimeter.h"

typedef struct {
	double vcc, r_shunt, r_load;
}tAmp;

static tAmp sensors[AMP_N_SENSORS]; // vetor de tipos de modelos de sensores a ser gerenciados

// inicializa os parametros para calculos para o modelo de termistor especificado
// parametros:	tidx: Descrimina o sensor a ser gerencicado
//				rs: Valor do resistor em ohms de shunt
//				rl: Valor do resistor em kohms de carga
void amp_Init(uint tidx, double vcc, double rs, double rl) {
	if (tidx >= AMP_N_SENSORS) return;
	sensors[tidx].vcc = vcc;
	sensors[tidx].r_shunt = rs;
	sensors[tidx].r_load = rl;
}

// para mais detalhes veja D:\meus_conhecimentos\_devices_misc__\sensor_tensão
// retorna com o valor da tensão lida no circuito correspondente ao ADC
double amp_ValueVoltage(uint tidx, uint dec) {
	return (dec*sensors[tidx].vcc)/4095;
}

// converte para amperagem o valor lido na ponta de prova
//	tidx: Indice do sensor a ser lido: 0=sensor 1, 1=sensor2 e assim sucessivamente
//	adc: Valor adc lido do respectivo sensor
// retorna em microamperes
double amp_ConvertToAmperage(uint tidx, uint adc) {
	double vout;
	vout = amp_ValueVoltage(tidx, adc);
	return  (vout/(sensors[tidx].r_shunt*sensors[tidx].r_load))*1000000;
}
