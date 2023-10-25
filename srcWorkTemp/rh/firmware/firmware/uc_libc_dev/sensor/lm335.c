#include "lm335.h"

// O lm335 � um sensor de precis�o que opera na faixa de -40�C a 100�C com erro de 1�C.
//	A resposta da varia��o da tens�o em rela��o a temperatura � em graus Kelvin e opera entre 400uA a 5mA.

// A sa�da do lm335 � de 10mV/�K

//	DegC = ((Vref * valADC * 100) / resADC) � 273
//	DegF = DegC*9/5 + 32
//	Onde:
//		�	DegC � o valor da temperatura em graus Celsius �C;
//		�	DegF � o valor da temperatura em graus fahrenheit �F;
//		�	100 para converter volts para milivolts;
//		�	valADC � o resultado da convers�o ADC;
//		�	Vref � a tens�o de de referencia para medi��o do lm35 (independente da tens�o VCC do sensor) = 3.3 do uC;
//		�	resADC � a resolu��o do canal ADC que vai ler o lm35 = 4096
//			o	(2 elev 10) = 1024
//			o	(2 elev 12) = 4096
// Circuito
//	O valor de R1 n�o afeta no valor da tens�o de sa�da que corresponde a temperatura mensurada,
//		pois � s� um limitador de corrente, mas seu valor afeta na faixa de temperatura a ser medido.
//	O valor de R1 depende da tens�o que vamos aplicar no sensor, como o sensor trabalha de 0.4mA a 5mA
//		devemos colocar o valor do resistor limitador de corrente que para metade da corrente de trabalho.
//	Para 5V >> R1 = VCC/I   R1 = 5/0.0025� = 2K

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
// |  lm335  ADJ | ----- n�o conectado
// |   GND       |
//  ------------- 
//      |
//      |
//     GND
// O pino ADJ � para ajuste fino da temperatura, no momento n�o estamos usando, pois se for preciso faremos via software.

// retorna em graus Celsius do valor do ADC
int lm335_ConvertToDegC(int adcVal) {
	return (int) (((LM35_VREF*adcVal*100.0)/LM35_resADC)-273.15);
}

// retorna em graus fahrenheit do valor do ADC
int lm335_ConvertToDegF(int adcVal) {
	return (int) ((lm335_ConvertToDegC(adcVal)*9)/5 + 32);
}
