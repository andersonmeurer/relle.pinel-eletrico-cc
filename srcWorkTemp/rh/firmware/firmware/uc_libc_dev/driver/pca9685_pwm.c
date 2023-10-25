// Escrito em 29/03/2016 baseado DevXP https://www.ccsinfo.com/forum/viewtopic.php?t=47908

// O pca9685 é um controlador de 16 canais para LED RGBA (Red, Green, Blue and Amber) com interface i2c.
// Cada canal tem uma resolução de 12 bits obtendo 4096 níveis de ajuste. O controlador trabalha com uma
// 	frequência entre 24Hz a 1526Hz e o duty ciclo pode ser ajustável de 0% a 100%.
// Cada saída pode fornecer até 25mA de corrente para uma alimentação de 5V. PCA9685 pode ser alimentado entre 2.3V a 5.5V
//  e as entradas e saídas são tolerantes a 5.5V. O CI utiliza um clock interno de 25MHz, mas pode aceitar clock
//	externo de até 50MHz para deixar tudo sincronizado.
// A velocidade do barramento i2c pode chegar até 1Mhz e o endereço base é de 0x80 quando todos os pinos de endereço
//	estão em zero.
// O endereçamento do pca9685 é flexível e pode ser acessado individual ou em grupos de uma só vez.
// Há no máximo 64 possíveis endereçamentos utilizando os 6 pinos externos do pca9685. Porém,
//	desses 64 endereços dois são reservados para acessar todos os dispositivos de uma só vez, são os acessos
//	de LED ALL CALL e Software Reset.
// Emitido comando de Software Reset endereço (0x6) todos os pca9685 conectados ao barramento aceitarão
//	independente da configuração dos pinos externos de endereço.
// Emitindo um comando LED ALL CALL todos os pca9685 conectados ao barramento responderão, independente
//	da configuração dos pinos externos de endereço, se:
//		•	O bit ALLCALL do registrador MODE1 estiver habilitado;
//		•	O endereço do comado for igual ao do registrador ALLCALLADR que por padrão é 0xE0.
// Podemos acessar os pca9685 endereços alternativos em grupos, algo semelhante ao comando
//	LED ALL CALL, para isto são usados os registradores SUBADRx e os bits SUBx do registrador MODE1
//	devem estarem habilitados. Isto permite criar subgrupos de pcs9685 para acessos distintos.

// A comunicação com o pca9685 pode chegar até 1MHz

//	Resumo dos registradores
//	Registrador	Endereço	Valor padrão	Descrição
//	MODE1		0x0			0x11			Oscilador desligado e aceita comando LED ALL CALL e Software Reset. Utiliza o oscilador interno de 25Mhz, não aceita acessos em subendereços e o registrador de auto incremento desligado.
//	MODE2		0x1	0x4	O sinal PWM não invertido, a mudança no PWM é feita depois do ACK, as saídas PWM são em dreno aberto e o pino #OE tem a função de colocar as saídas PWM em 0.
//	SUBADR1		0x2	0xE2	Responde via subendereço 1 caso o bit SUB1 do registrador MODE1 estiver ligado.
//	SUBADR2		0x3	0xE4	Responde via subendereço 2 caso o bit SUB2 do registrador MODE1 estiver ligado.
//	SUBADR3		0x4	0xE8	Responde via subendereço 3 caso o bit SUB3 do registrador MODE1 estiver ligado.
//	ALLCALLADR	0x5	0xE0	Responde caso o endereço de aceso coincidir com esse registrador e o bit ALLCALL do registrador MODE1 estiver habilitado
//	PRE_SCALE	0xFE		Prescala para programar a frequência do PWM

//	LEDx_ON_L (8bits)
//	LEDx_ON_H (4bits)
//	LEDx_OFF_L (8bits)
//	LEDx_ON_H (4bits)	endereços de 0x6 a 0x45	Valores de 12bits para determinar o tempo do sinal ficar em cima. LEDx_ON quando vai ligar e LEDX_OFF quando vai desligar

// EXEMPLO DE USO
//	if (pca9685_Init(0x80) == pdPASS) {
//		pca9685_PWMFrequency(50); // 50Hz Frequency - can go up to 400Hz max for servo's
//		uint x;	for (x=0;x<15;x++) 	pca9685_Servo(x,1500);
//	}

#include <math.h>
#include "pca9685_pwm.h"
#include "delay.h"

#define PCA9685_USE_DEBUG_ALL pdON
#if (PCA9685_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

typedef struct {
	int status;
	int addrDevice;
	float pwmFrequency;
	u8 prescale;
} tPca9685, *pPca9685;
static tPca9685 pca9685;

//-----------------------------------------------------------------------------------------------------------------------
// Inicializa o driver para acessar o controlador de PWM
// 		ATENÇÃO: Essa função deve ser chamada depois do barramentos i2c e a interrupção for iniciados.
//		bitrate: pode chegar a 1MHz
// Parametro:
//		addr: Endereço do pca9685 no barramdento I2C
// Retorna:
//		pdPASS se inicilizou corretamente
//		errPCA9685_NO_CONNECTED: Caso não foi possivel ler o ID do adxl345
//-----------------------------------------------------------------------------------------------------------------------
int pca9685_Init(int addr) {
	pca9685.addrDevice = addr;
	pca9685.status = pca9685_WriteReg(PCA9685_MODE1_REG, 0x0);

	#define PCA_VAL_TEST 0x5A
	u8 id = 0;

	if (pca9685.status == pdPASS) pca9685.status = pca9685_WriteReg(PCA9685_PORT0_REG, PCA_VAL_TEST);
	if (pca9685.status == pdPASS) pca9685.status = pca9685_ReadReg(PCA9685_PORT0_REG, &id);
	if (pca9685.status == pdPASS) pca9685.status = pca9685_WriteReg(PCA9685_PORT0_REG, 0);
	if ( (pca9685.status == pdPASS) && (id != PCA_VAL_TEST) ) pca9685.status = errPCA9685_NO_CONNECTED;

	#if (PCA9685_USE_DEBUG == pdON)
	if (pca9685.status == pdPASS)
		plognp("PCA - INIT"CMD_TERMINATOR);
	else  plognp("PCA - INIT ERROR [%d]"CMD_TERMINATOR, pca9685.status);
	#endif

	pca9685_ReadReg(PCA9685_PRESCALE_REG, &pca9685.prescale);

	return pca9685.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: Lê uma registrador do pca9685
// Parametros:
//		reg: Endereço do registrador
//		*data: ponteiro para o retorno do byte lido
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int pca9685_ReadReg(u8 reg, u8 *data) {
	pca9685.status = pca9685_Send(pca9685.addrDevice, &reg, 1);
	if (pca9685.status == pdPASS) pca9685.status = pca9685_Receive(pca9685.addrDevice, data, 1);

	#if ( (PCA9685_USE_DEBUG == pdON) && (PCA9685_USE_DEBUG_ALL == pdON) )
	plognp("PCA - READ REG - ");
	plognp("addr:0x%x reg:0x%x value:0x%x status:%d"CMD_TERMINATOR, pca9685.addrDevice, reg, *data, pca9685.status);
	#endif

	return pca9685.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: Escreve dados no registrador desejado no pca9685
// Parametros:
//		reg: Endereço do dispositivo
//		data: Valor a ser escrito no registrador
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int pca9685_WriteReg(u8 reg, u8 data) {
	u8 buffer[2];

	buffer[0] = reg;
	buffer[1] = data;

	pca9685.status = pca9685_Send(pca9685.addrDevice, buffer, 2);

	#if ( (PCA9685_USE_DEBUG == pdON) && (PCA9685_USE_DEBUG_ALL == pdON) )
	plognp("PCA - WRITE REG - ");
	plognp("addr:0x%x reg:0x%x value:0x%x status:%d"CMD_TERMINATOR, pca9685.addrDevice, reg, data, pca9685.status);
	#endif

	return pca9685.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: Lê o status do pca9685
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		Código de erro I2C lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int pca9685_Status(void) {
	if (pca9685.status == errI2C_ERROR) return i2c2_CodeError();
	else return pca9685.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: Ajusta o intervalo entre os pulsos, ou seja, a frequencia
// Parametro: frequencia desejada. Para controle de servos motores uma frequencia de 50Hz
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		Código de erro I2C lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
void pca9685_PWMFrequency(float freq) {
	// Set PWM Frequency by setting the Prescale value (min 3, max 255)
	// Equates to max frequency approx 1.5kHz - varies depending on internal osc
	//            min frequency 24 Hz
	// Setting the PWM Frequency also turns the device on

  	//freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
  	pca9685.pwmFrequency = freq;
  	float prescaleval = 25000000;
  	prescaleval /= 4096;
  	prescaleval /= freq;
  	prescaleval -= 1;
  	pca9685.prescale = floor(prescaleval + 0.5);

	#if (PCA9685_USE_DEBUG == pdON)
	plognp("PCA - SET FREQ - addr:0x%x frequency:%f prescale:%d"CMD_TERMINATOR, pca9685.addrDevice, pca9685.pwmFrequency, pca9685.prescale);
	#endif

  	u8 oldmode;
  	pca9685_ReadReg(PCA9685_MODE1_REG, &oldmode);
  	u8 newmode = (oldmode&0x7F) | 0x10; // sleep
  	pca9685_WriteReg(PCA9685_MODE1_REG, newmode); // go to sleep
  	pca9685_WriteReg(PCA9685_PRESCALE_REG, pca9685.prescale); // set the prescaler
  	pca9685_WriteReg(PCA9685_MODE1_REG, oldmode);
	delay_ms(1);											// Wait minimo 500 uS
	pca9685_WriteReg(PCA9685_MODE1_REG, oldmode | 0x81);  //  This sets the MODE1 register to turn off auto increment.
  	//pca9685_WriteReg(PCA9685_MODE1_REG, oldmode | 0xa1);  //  This sets the MODE1 register to turn on auto increment.
}


void pca9685_SetPWM(u8 ch, u16 on, u16 off) {
	pca9685_WriteReg(PCA9685_PORT0_REG+(4*ch), (u8)on);
	pca9685_WriteReg(PCA9685_PORT0_REG+1+(4*ch), (u8)(on >>8));
	pca9685_WriteReg(PCA9685_PORT0_REG+2+(4*ch), (u8)off); // Send Low Byte
	pca9685_WriteReg(PCA9685_PORT0_REG+3+(4*ch), (u8)(off >> 8)); // Send High Byte

	#if (PCA9685_USE_DEBUG == pdON)
	plognp("PCA - SET PWM - [channel:%d] [on:%d] [off:%d] - ", ch, on, off);
	plognp("[0x%x:%d] [0x%x:%d] [0x%x:%d] [0x%x:%d]"CMD_TERMINATOR,
		PCA9685_PORT0_REG+(4*ch), (u8)on,
		PCA9685_PORT0_REG+1+(4*ch), (u8)(on >> 8),
		PCA9685_PORT0_REG+2+(4*ch), (u8)off,
		PCA9685_PORT0_REG+3+(4*ch), (u8)(off >> 8)
	);
	#endif
}

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: Determinar a largura de pulso em us para um determinado canal
// Parametros:
//		servo: Servo a ser acessado, valor de 0 a 15
//		pulse: tempo em us (não é preciso) = 1000 us = 1ms
//-----------------------------------------------------------------------------------------------------------------------
void pca9685_Servo(u8 servo, int pulse) {
  	double pulselength;

  	pulselength = 1000000;   // 1,000,000 us per second
  	pulselength /= (pca9685.pwmFrequency);
  	pulselength /= 4096;  // 12 bits of resolution
  	//pulse *= 1000; // usar isto caso usar pulse em segundos, neste caso será do tipo double
  	pulse /= pulselength;

	#if (PCA9685_USE_DEBUG == pdON)
	plognp("PCA - SET SERVO - [servo:%d] [pulse:%d] [pulselength:%f] [frequency:%f]"CMD_TERMINATOR, servo, pulse, pulselength, pca9685.pwmFrequency);
	#endif

  	pca9685_SetPWM(servo, 0, pulse);
}
