#include "tc1047.h"

// O tc1047 é um sensor de temperatura com tensão de saída linear diretamente proporcional com a temperatura.
//	O tc1047 pode medir temperatura entre -40°C a 125°C e sua tensão de alimentação de 2.7V a 4.4V para tc1047,
//	ou de 2.5V a 5.5V para tc1047a
// A saída do tc1047 é de 10mV/°C

//	DegC = (((adcVal * 3300) / resADC) – 500)10
//	Onde:
//		DegC é o valor da temperatura em graus Celsius °C;
//		valADC é o resultado da conversão ADC;
//		resADC é a resolução do canal ADC que vai ler o tc1047 = 4096
//			(2 elev 10) = 1024
//			(2 elev 12) = 4096

// retorna em graus Celsius do valor do ADC
int tc1047_ConvertToDegC(int adcVal) {
	return (int) (((adcVal*3300)/TC1047_resADC)-500)/10;
}

// retorna em graus fahrenheit do valor do ADC
int tc1047_ConvertToDegF(int adcVal) {
	return (int) ((tc1047_ConvertToDegC(adcVal)*9)/5 + 32);
}
