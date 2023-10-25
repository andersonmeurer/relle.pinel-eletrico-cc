#include "voltimeter.h"

typedef struct {
	double vcc, resin, resdown;
}tVolt;

static tVolt sensors[VOLT_N_SENSORS]; // vetor de tipos de modelos de sensores a ser gerenciados

// inicializa os parametros para calculos para o modelo de termistor especificado
// parametros:	tidx: Descrimina o sensor a ser gerencicado
//				vcc: Tensão do máxima de conversão do ADC
//				rin: Valor do resistor de entrada em ohms do circuito da interface do sensor
//				rdown: Valor do resistor pulldown em ohms do circuito da interface do sensor
void volt_Init(uint tidx, double vcc, double rin, double rdown) {
	if (tidx >= VOLT_N_SENSORS) return;
	sensors[tidx].vcc = vcc;
	sensors[tidx].resin = rin;
	sensors[tidx].resdown = rdown;
}

// para mais detalhes veja D:\meus_conhecimentos\_devices_misc__\sensor_tensão
// retorna com o valor da tensão lida no circuito correspondente ao ADC
double volt_ValueVoltage(uint tidx, uint dec) {
	return (dec*sensors[tidx].vcc)/4095;
}

// converte para tensão o valor lido na ponta de prova
//	tidx: Indice do sensor a ser lido: 0=sensor 1, 1=sensor2 e assim sucessivamente
//	adc: Valor adc lido do respectivo sensor
// retorna em milivolts
double volt_ConvertToVoltage(uint tidx, uint adc) {
	double u2;
	u2 = volt_ValueVoltage(tidx, adc);
	return  (((sensors[tidx].resin+sensors[tidx].resdown)*u2)/sensors[tidx].resdown)*1000;
}
