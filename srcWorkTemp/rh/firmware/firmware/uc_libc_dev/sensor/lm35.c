#include "lm35.h"

// LM35 Temperature Sensor
// The LM35 temperature sensor is a 'Precision Centigrade Temperature Sensor with an
//		accuracy of +/- 1°C, whose output voltage is linear proportional to the Celsius temperature'.
//		With the circuit we will be using, we will have an accurate measurement range of +2°C to +150°C,
//		adequate for measuring room temperatures.
// Although with a slightly more advanced circuit, it can have a range of -55° to +150°C
//		(but the circuit doesn't support the negative voltage reading). Refer to the LM35 datasheet here for more info.
// The LM35 we will be using is a TO-92 plastic package, perfect for our breadboard with res pulldown of the 100K

// DegC = (Vref * valADC * 100) / resADC
// Onde:
//    DegC é o valor da temperatura em °C.
//    valADC é o resultado da conversão ADC
// 	  Vref é a tensão de de referencia para medição do lm35 (independente da tensão VCC do sensor), ou seja, a tensão do ADC do uC
//    resADC é a resolução do canal ADC que vai ler o lm35 = 4096
// 			(2 elev 10) = 1024
// 			(2 elev 12) = 4096

//      5V
//      |
//      |
//  ---------
// |         |
// |  lm35   | ----------> ADC do uC
// |         |     |
//  ---------     | | res 100K
//      |          |
//      |-----------
//     GND

// retorna em graus Celsius do valor do ADC
int lm35_ConvertToDegC(int adcVal) {
	return (int) ((LM35_VREF*adcVal*100.0)/LM35_resADC);
}

// retorna em graus fahrenheit do valor do ADC
int lm35_ConvertToDegF(int adcVal) {
	return (int) ((lm35_ConvertToDegC(adcVal)*9)/5 + 32);
}
