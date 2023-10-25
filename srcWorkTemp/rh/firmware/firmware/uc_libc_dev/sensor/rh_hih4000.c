#include "rh_hih4000.h"
//#include "stdio_uc.h"

typedef struct {
	double svcc, adcvcc;
	uint adcmax, r1,r2;
}tHih4000;

static tHih4000 hih[HIH4000_N_SENSORS]; // vetor de tipos de modelos de sensores a ser gerenciados

static double ihi4000_trueVout(uint sidx, uint adc);

// inicializa os paramentros para calculos para o sensor especificado
// parametros:	sidx: Descrimina o sensor a ser gerencicado
//				adcvcc:
void ihi4000_Init(uint sidx, double svcc, uint r1, uint r2, double adcvcc, uint adcmax) {
	if (sidx >= HIH4000_N_SENSORS) return;
	hih[sidx].svcc = svcc;
	hih[sidx].r1 = r1;
	hih[sidx].r2 = r2;
	hih[sidx].adcvcc = adcvcc;
	hih[sidx].adcmax = adcmax;
}

static double ihi4000_trueVout(uint sidx, uint adc) {
	double vout;
	vout = (double)(hih[sidx].adcvcc*adc/hih[sidx].adcmax); 	// Calcular a tensão na entrada do ADC
	return 	(hih[sidx].r1+hih[sidx].r2)*vout/hih[sidx].r2;		// Vout real do sensor
}

int ihi4000_ConvertToRH(uint sidx, uint adc, double temperature) {
	double trueVout, rh;
	trueVout = ihi4000_trueVout(sidx, adc);
	// RH = (Vo – 0,16*Vs)/(Vs*0,0062)
	rh = (trueVout - (0.16*hih[sidx].svcc))/(hih[sidx].svcc*0.0062);
	// trueRH = RH/(1.0546 - 0.00216*T) , T em ºC

//	plognp("[idx=%d] [R1=%d] [R2=%d] [adcmax=%d] [svcc=%f] [adcvcc=%f]"CMD_TERMINATOR,
//		sidx, hih[sidx].r1, hih[sidx].r2, hih[sidx].adcmax, hih[sidx].svcc, hih[sidx].adcvcc
//	);
//
//	plognp("[trueVour=%f]"CMD_TERMINATOR, trueVout);
//	plognp("rh_auxr=%f]"CMD_TERMINATOR, rh);

	return (int)(rh)/(1.0546 - (0.00216*temperature));
}
