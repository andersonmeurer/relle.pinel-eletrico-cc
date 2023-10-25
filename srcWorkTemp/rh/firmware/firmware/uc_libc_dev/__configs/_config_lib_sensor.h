#ifndef __CONFIG_LIB_SENSOR_H
#define __CONFIG_LIB_SENSOR_H

#include "uc_libdefs.h"

// ######################################################################################################
// SENSOR DE PESO USANDO hx711
#define HX711_nSAMPLY 				20		// Quantidade de amostras para calcular a média
#define HX711_USE_PROCESS			pdOFF	// Se deseja usar processo para leitura dos valores no hx711
	#define HX711_READING_TIME		10		// Tempo em ms para leitura desses valores
											// Usando o processo têm a vantagem de não ficar muito tempo em um loop lendo todas as amostras.
											// Porém, a desvantagem é que os valores demoram para estabilizar quando a há mudanças brusca de peso.
											// 	Ex: Se colocar um peso na balança, para 20 amostras a 10ms de tempo de processo, a balança vai se
											//		estabilizar depois de 200ms, e durante este período vamos receber os valores crescendo
											// Não usando processo, quando mandamos ler o valor da balança a função vai trazer o valor já estabilizado.
											//	A desvantagem é que ficamos muito tempo parado em um loop para leitura dos valores, o tempo é referente
											//		a taxa de amostragem do hx711

// ######################################################################################################
// SENSOR ACELEROMETRO
#define ADXL345_USE_PROCESS			pdON	// Se deseja usar processo para leitura dos valores no aceleremetro
	#define ADXL345_READING_TIME	10		// Tempo em ms para leitura dos eixos
#define ADXL345_USE_INCLINATION		pdON	// Se deseja calcular pitch e roll
#define ADXL345_USE_FORCE_G 		pdOFF	// Se deseja usar força g para calular aceleração, queda livre e TAP
#define ADXL345_USE_FILTER			pdON	// Liga ou desliga o filtro de leitura
	#define ADXL345_nSAMPLY 		10		// Quantidade de amostras para calcular a média

// ######################################################################################################
// SENSOR DO AMPERÍMETRO
#define AMP_N_SENSORS	1		// Define a quantidade de sensores que vai ser gerenciado
#define AMP_VCC			3.3		// Tensão máxima de conversão do ADC
#define AMP_RHUNT		1		// Valor do resistor em ohms de shunt
#define AMP_RLOAD		68		// Valor do resistor em kohms de carga

// ######################################################################################################
// SENSOR DO VOLTÍMETRO
#define VOLT_N_SENSORS	1			// Define a quantidade de sensores que vai ser gerenciado
#define VOLT_VCC		3.3			// Tensão máxima de conversão do ADC
#define VOLT_RESIN		470000.00	// Valor do resistor de entrada em ohms do circuito da interface do sensor
#define VOLT_RESDOWN	130000.00	// Valor do resistor pulldown em ohms do circuito da interface do sensor

//                  |--------------- ADC
// Vin ---/\/\/\/---|----/\/\/\/---- GND
//         RESIN         RESDOWN

// ######################################################################################################
// SENSOR DE UMIDADE RELATIVA DO AR HIH 4000
#define HIH4000_N_SENSORS	1			// Define a quantidade de sensores que vamos gerenciar


#define HIH4000_R1		30000.00		// valor do resistor entre o sensor e a entrada do ADC // Coloque valor 1 caso não usar
#define HIH4000_R2		100000.00		// valor do resistor pulldowm // Coloque valor 1 caso não usar
#define HIH4000_VCC		5.0				// Tensão de alimentação do sensor
#define HIH4000_ADCVCC	3.3				// Tensão de alimentação do ADC
#define HIH4000_ADCMAX	4095			// Valor máximo do conversão para HIH4000_ADCVCC


// ######################################################################################################
// SENSOR DE TEMPERATURA TERMISTOR
#define TERM_N_SENSORS	1				// Define a quantidade de sensores diferentes que serão gerenciados
#define TERM_PRECISON_CONVERT	"%.1f" 	// Define N casas decimais para precisão da conversão para temperatura

// para mais detalhes veja D:\meus_conhecimentos\_devices_misc__\sensor_termistor
// CONSTANTES DA EQUAÇÃO STEINHART-HART
// SENSOR 100K/25º
//#define TERM1_VCC	3.48			// Tensão aplicada no resisor pullup do do circuito interface do sensor
//#define TERM1_RS	100000.00		// valor do resistor pullup do circuito interface do sensor
//#define TERM1_A		0.000833777969165095
//#define TERM1_B		0.00020786481370718
//#define TERM1_C		8.32737350948285E-8

// SENSOR WorkTemp 10K/25º - valores -20ºC, 15ºC e 50ºC
#define TERM1_VCC	3.3			// Tensão aplicada no resisor pullup do do circuito interface do sensor
#define TERM1_RS	9530.00		// valor do resistor pullup em ohms do circuito interface do sensor

// 	ºC   ohms
//	-20 67770
//	25	10000
//	50	4160
#define TERM1_A		0.000893907855517688
#define TERM1_B		0.000250424219255434
#define TERM1_C		1.96612085902327E-7
//#define TERM1_A		0.8939078555E-3 // 0.000892877114342062
//#define TERM1_B		2.504242193E-4 	// 0.000250587594450717
//#define TERM1_C		1.966120859E-7 	//	1.9604060866488e-7

//// SENSOR 10K4/25º
//#define TERM2_VCC	3.48			// Tensão aplicada no resisor pullup do do circuito interface do sensor
//#define TERM2_RS	100000.00		// valor do resistor pullup do circuito interface do sensor
//#define TERM2_A		1.028444e-3
//#define TERM2_B		2.392435e-4
//#define TERM2_C		1.562216e-7

// ######################################################################################################
// SENSOR DE TEMPERATURA LM35 ou LM335
#define LM35_VREF 3.3  		// Vref é a tensão de de referencia para medição do lm35 (independente da tensão VCC do sensor), ou seja, a tensão do ADC do uC
#define LM35_resADC 4096.0 	// resADC é a resolução do canal ADC que vai ler o lm35: Resolução de 12 bits = 4096

#endif
