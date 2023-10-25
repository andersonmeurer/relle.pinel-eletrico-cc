#include "termistor.h"
#include <math.h>
#include "stdio_uc.h"

typedef struct {
	double vcc,rs,a,b,c;
}tTerm;

static tTerm term[TERM_N_SENSORS]; // vetor de tipos de modelos de sensores a ser gerenciados

// inicializa os parametros para calculos para o modelo de termistor especificado
// parametros:	termo: Descrimina o termistor a ser gerencicado
//				vcc: Tensão aplicada no resisor pullup do do circuito interface do sensor
//				rs: Valor do resistor pullup do circuito interface do sensor
//				a,b,c: Constantes da equação steinhart-hart
void term_Init(uint tidx, double vcc, double rs, double a, double b, double c) {
	if (tidx >= TERM_N_SENSORS) return;
	term[tidx].vcc = vcc;
	term[tidx].rs = rs;
	term[tidx].a = a;
	term[tidx].b = b;
	term[tidx].c = c;
}

// esta biblioteca somente tem suporte para ler termistores configurado em pullup
// para mais detalhes veja D:\meus_conhecimentos\_devices_misc__\sensor_termistor
// retorna com o valor da tensão lida no circuito correspondente ao ADC
double term_ValueVoltage(uint tidx, uint dec) {
	return (dec*term[tidx].vcc)/4095;
}

// retorna com o valor da resistencia de acordo com a tensão lida no circuito
double term_ValueRes(uint tidx, double vs) {
	return vs*term[tidx].rs/(term[tidx].vcc-vs);
}

// Equação Steinhart-Hart para encontar a temperatura de acordo com o valor resistivo do termistor
// retorna a temperatura, em kelvin, do termistor de acordo com o valor da sua resistencia
double term_SteinhartHart(uint tidx, double res) {
	double x1, x2;

	x2 = log(res); // ln
	x1 = x2;
    x2 = pow(x2, 3);
    x1*=term[tidx].b;
    x1+=term[tidx].c*x2;
    x1+=term[tidx].a;
    x1 = 1/x1;
    //x1-=273.15; // Converter de graus kelvin para graus celsius

    return x1;
}

// converte para graus celsius o valor do ADC lido da interface do termistor
//	tidx: Indice do sensor a ser lido: 0=sensor 1, 1=sensor2 e assim sucessivamente
//	adc: Valor adc lido do respectivo sensor
//	retTemp: Sinaliza em que grandeza de temperatura a retornar: tempKELVIN, tempCELSIUS ou tempFAHRENHEIT
double term_ConvertToTemp(uint tidx, uint adc, uint retTemp) {
	double vs, res, temp;
	vs = term_ValueVoltage(tidx, adc);
	res = term_ValueRes(tidx, vs);
	temp = term_SteinhartHart(tidx, res);

	if (retTemp == tempCELSIUS) temp -= 273.15;
	else if (retTemp == tempFAHRENHEIT) temp = (temp-273.15)*1.8+32;
	return temp;
}

// converte para graus celsius o valor do ADC lido da interface do termistor
void term_ConvertToTempS(uint tidx, uint adc, pchar s, uint retTemp) {
	sformat(s ,TERM_PRECISON_CONVERT, term_ConvertToTemp(tidx, adc, retTemp));
}

// arredonda para precisão de 0.5 graus
void term_TempRound(double* temp) {
	uint roundtemp;
	roundtemp = trunc(*temp*10) - trunc(*temp)*10;
	if (roundtemp < 5)
			*temp = trunc(*temp);
	else 	*temp = trunc(*temp) + 0.5;

}
