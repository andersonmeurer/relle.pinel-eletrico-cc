#include "lm335.h"

// O lm335 é um sensor de precisão que opera na faixa de -40ºC a 100ºC com erro de 1ºC.
//	A resposta da variação da tensão em relação a temperatura é em graus Kelvin e opera entre 400uA a 5mA.

// A saída do lm335 é de 10mV/°K

//	DegC = ((Vref * valADC * 100) / resADC) – 273
//	DegF = DegC*9/5 + 32
//	Onde:
//		•	DegC é o valor da temperatura em graus Celsius ºC;
//		•	DegF é o valor da temperatura em graus fahrenheit ºF;
//		•	100 para converter volts para milivolts;
//		•	valADC é o resultado da conversão ADC;
//		•	Vref é a tensão de de referencia para medição do lm35 (independente da tensão VCC do sensor) = 3.3 do uC;
//		•	resADC é a resolução do canal ADC que vai ler o lm35 = 4096
//			o	(2 elev 10) = 1024
//			o	(2 elev 12) = 4096
// Circuito
//	O valor de R1 não afeta no valor da tensão de saída que corresponde a temperatura mensurada,
//		pois é só um limitador de corrente, mas seu valor afeta na faixa de temperatura a ser medido.
//	O valor de R1 depende da tensão que vamos aplicar no sensor, como o sensor trabalha de 0.4mA a 5mA
//		devemos colocar o valor do resistor limitador de corrente que para metade da corrente de trabalho.
//	Para 5V >> R1 = VCC/I   R1 = 5/0.0025ª = 2K

//     VCC
//      |
//      |
//		|
//	   | |
//	   | |  R1 1%
//	   | |
//		|
//		|------------------> ADC do uC
//		|
//  -------------
// |   OUT       |
// |  lm335  ADJ | ----- não conectado
// |   GND       |
//  ------------- 
//      |
//      |
//     GND
// O pino ADJ é para ajuste fino da temperatura, no momento não estamos usando, pois se for preciso faremos via software.

// retorna em graus Celsius do valor do ADC
int lm335_ConvertToDegC(int adcVal) {
	return (int) (((LM35_VREF*adcVal*100.0)/LM35_resADC)-273.15);
}

// retorna em graus fahrenheit do valor do ADC
int lm335_ConvertToDegF(int adcVal) {
	return (int) ((lm335_ConvertToDegC(adcVal)*9)/5 + 32);
}
