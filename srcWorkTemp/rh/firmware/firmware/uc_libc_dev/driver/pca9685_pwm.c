// Escrito em 29/03/2016 baseado DevXP https://www.ccsinfo.com/forum/viewtopic.php?t=47908

// O pca9685 � um controlador de 16 canais para LED RGBA (Red, Green, Blue and Amber) com interface i2c.
// Cada canal tem uma resolu��o de 12 bits obtendo 4096 n�veis de ajuste. O controlador trabalha com uma
// 	frequ�ncia entre 24Hz a 1526Hz e o duty ciclo pode ser ajust�vel de 0% a 100%.
// Cada sa�da pode fornecer at� 25mA de corrente para uma alimenta��o de 5V. PCA9685 pode ser alimentado entre 2.3V a 5.5V
//  e as entradas e sa�das s�o tolerantes a 5.5V. O CI utiliza um clock interno de 25MHz, mas pode aceitar clock
//	externo de at� 50MHz para deixar tudo sincronizado.
// A velocidade do barramento i2c pode chegar at� 1Mhz e o endere�o base � de 0x80 quando todos os pinos de endere�o
//	est�o em zero.
// O endere�amento do pca9685 � flex�vel e pode ser acessado individual ou em grupos de uma s� vez.
// H� no m�ximo 64 poss�veis endere�amentos utilizando os 6 pinos externos do pca9685. Por�m,
//	desses 64 endere�os dois s�o reservados para acessar todos os dispositivos de uma s� vez, s�o os acessos
//	de LED ALL CALL e Software Reset.
// Emitido comando de Software Reset endere�o (0x6) todos os pca9685 conectados ao barramento aceitar�o
//	independente da configura��o dos pinos externos de endere�o.
// Emitindo um comando LED ALL CALL todos os pca9685 conectados ao barramento responder�o, independente
//	da configura��o dos pinos externos de endere�o, se:
//		�	O bit ALLCALL do registrador MODE1 estiver habilitado;
//		�	O endere�o do comado for igual ao do registrador ALLCALLADR que por padr�o � 0xE0.
// Podemos acessar os pca9685 endere�os alternativos em grupos, algo semelhante ao comando
//	LED ALL CALL, para isto s�o usados os registradores SUBADRx e os bits SUBx do registrador MODE1
//	devem estarem habilitados. Isto permite criar subgrupos de pcs9685 para acessos distintos.

// A comunica��o com o pca9685 pode chegar at� 1MHz

//	Resumo dos registradores
//	Registrador	Endere�o	Valor padr�o	Descri��o
//	MODE1		0x0			0x11			Oscilador desligado e aceita comando LED ALL CALL e Software Reset. Utiliza o oscilador interno de 25Mhz, n�o aceita acessos em subendere�os e o registrador de auto incremento desligado.
//	MODE2		0x1	0x4	O sinal PWM n�o invertido, a mudan�a no PWM � feita depois do ACK, as sa�das PWM s�o em dreno aberto e o pino #OE tem a fun��o de colocar as sa�das PWM em 0.
//	SUBADR1		0x2	0xE2	Responde via subendere�o 1 caso o bit SUB1 do registrador MODE1 estiver ligado.
//	SUBADR2		0x3	0xE4	Responde via subendere�o 2 caso o bit SUB2 do registrador MODE1 estiver ligado.
//	SUBADR3		0x4	0xE8	Responde via subendere�o 3 caso o bit SUB3 do registrador MODE1 estiver ligado.
//	ALLCALLADR	0x5	0xE0	Responde caso o endere�o de aceso coincidir com esse registrador e o bit ALLCALL do registrador MODE1 estiver habilitado
//	PRE_SCALE	0xFE		Prescala para programar a frequ�ncia do PWM

//	LEDx_ON_L (8bits)
//	LEDx_ON_H (4bits)
//	LEDx_OFF_L (8bits)
//	LEDx_ON_H (4bits)	endere�os de 0x6 a 0x45	Valores de 12bits para determinar o tempo do sinal ficar em cima. LEDx_ON quando vai ligar e LEDX_OFF quando vai desligar

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
// 		ATEN��O: Essa fun��o deve ser chamada depois do barramentos i2c e a interrup��o for iniciados.
//		bitrate: pode chegar a 1MHz
// Parametro:
//		addr: Endere�o do pca9685 no barramdento I2C
// Retorna:
//		pdPASS se inicilizou corretamente
//		errPCA9685_NO_CONNECTED: Caso n�o foi possivel ler o ID do adxl345
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
// Descri��o: L� uma registrador do pca9685
// Parametros:
//		reg: Endere�o do registrador
//		*data: ponteiro para o retorno do byte lido
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		errI2C_ERROR onde o c�digo de erro pode ser lido com a fun��o i2c2_CodeError()
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
// Descri��o: Escreve dados no registrador desejado no pca9685
// Parametros:
//		reg: Endere�o do dispositivo
//		data: Valor a ser escrito no registrador
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o c�digo de erro pode ser lido com a fun��o i2c2_CodeError()
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
// Descri��o: L� o status do pca9685
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		C�digo de erro I2C lido com a fun��o i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int pca9685_Status(void) {
	if (pca9685.status == errI2C_ERROR) return i2c2_CodeError();
	else return pca9685.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Descri��o: Ajusta o intervalo entre os pulsos, ou seja, a frequencia
// Parametro: frequencia desejada. Para controle de servos motores uma frequencia de 50Hz
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		C�digo de erro I2C lido com a fun��o i2c2_CodeError()
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
// Descri��o: Determinar a largura de pulso em us para um determinado canal
// Parametros:
//		servo: Servo a ser acessado, valor de 0 a 15
//		pulse: tempo em us (n�o � preciso) = 1000 us = 1ms
//-----------------------------------------------------------------------------------------------------------------------
void pca9685_Servo(u8 servo, int pulse) {
  	double pulselength;

  	pulselength = 1000000;   // 1,000,000 us per second
  	pulselength /= (pca9685.pwmFrequency);
  	pulselength /= 4096;  // 12 bits of resolution
  	//pulse *= 1000; // usar isto caso usar pulse em segundos, neste caso ser� do tipo double
  	pulse /= pulselength;

	#if (PCA9685_USE_DEBUG == pdON)
	plognp("PCA - SET SERVO - [servo:%d] [pulse:%d] [pulselength:%f] [frequency:%f]"CMD_TERMINATOR, servo, pulse, pulselength, pca9685.pwmFrequency);
	#endif

  	pca9685_SetPWM(servo, 0, pulse);
}
