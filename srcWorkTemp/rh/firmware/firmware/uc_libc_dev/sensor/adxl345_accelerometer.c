// Escrito em 20/08/2015

// Este driver utliza o barramento I2C para comunicação com o dispositivo
// TODO Melhorar o filtro, há muito ruído nas leituras dos eixos (implementar o filtro kalman para ver se melhora)
//	o melhor teste se funciona e usar o código abaixo no loop main
//		#define TILT_TOLERANCE 1
//		if ( (adxl345.x >= -TILT_TOLERANCE) && (adxl345.x <= TILT_TOLERANCE)) LED1_ON = LED1; else LED1_OFF = LED1;
//		if ( (adxl345.y >= -TILT_TOLERANCE) && (adxl345.y <= TILT_TOLERANCE)) LED2_ON = LED2; else LED2_OFF = LED2;

#include "adxl345_accelerometer.h"
#include <math.h>
#include <stdlib.h>

#if (ADX345_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif
#if (ADXL345_USE_PROCESS == pdON)
#include "systime_irq.h"
#endif

typedef struct {
	int status;
	int addrDevice;
	s16 x,y,z;
	#if (ADXL345_USE_INCLINATION == pdON)
	double roll;
	double pitch;
	int angle; // precisão de 1 em 1 grau
	//double xAngle, yAngle, zAngle;
	//double angle;
	#endif

	#if (ADXL345_USE_FORCE_G == pdON)
	double ax,ay,az;
	#endif
} tAdxl345, *pAdxl345;
static tAdxl345 adxl345;

static void adxl345_ClearBuf(void);
static int adxl345_ReadXYZ(void);

#if (ADXL345_USE_INCLINATION == pdON)
static void adxl345_CalcRollPitch(void);
#endif
static int adxl345_SetRegisterBit(u8 regAdress, int bitPos, bool state);
static int _adxl345_ReadXYZ(s16 *x, s16 *y, s16 *z);

#if (ADXL345_USE_PROCESS == pdOFF)
static int updatedXYZ;
#endif

#if (ADXL345_USE_FORCE_G == pdON)
static void adxl345_CalcAcceleration(void);
#endif

//-----------------------------------------------------------------------------------------------------------------------
// Inicializa o driver para acessar o acelerômetro
// 		ATENÇÃO: Essa função deve ser chamada depois do barramentos i2c e a interrupção for iniciados,
//			e após 1.1ms que o adxl345 seja ligado
// Parametro:
//		addr: Endereço do adxl345 no barramdento I2C
// Retorna:
//		pdPASS se inicilizou corretamente
//		errADXL345_NO_CONNECTED: Caso não foi possivel ler o ID do adxl345
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_Init(int addr) {
	#if (ADX345_USE_DEBUG == pdON)
	plognp("ADX - INIT"CMD_TERMINATOR);
	#endif

	u8 id = 0;

	adxl345.x = 0;
	adxl345.y = 0;
	adxl345.z = 0;
	#if (ADXL345_USE_FORCE_G == pdON)
	adxl345.ax = 0;
	adxl345.ay = 0;
	adxl345.az = 0;
	#endif

	#if (ADXL345_USE_INCLINATION == pdON)
	adxl345.roll = 0;
	adxl345.pitch = 0;
	adxl345.angle = 0;
	#endif

	adxl345.addrDevice = addr;
	adxl345_ReadReg(ADXL345_DEVID, &id);

	#if (ADX345_USE_DEBUG == pdON)
	if (id == ADXL345_ID) plognp("    id = 0x%x"CMD_TERMINATOR, id);
	else plognp("	id error [0x%x]"CMD_TERMINATOR, id);
	#endif

	if (id == ADXL345_ID) {
		adxl345.status = pdPASS;
	} else adxl345.status = errADXL345_NO_CONNECTED;

	adxl345_ClearBuf();
	#if (ADXL345_USE_PROCESS == pdOFF)
	updatedXYZ = pdFALSE;
	#endif

	return adxl345.status;
}


//-----------------------------------------------------------------------------------------------------------------------
// Descrição: Lê o status do adxl345
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		Código de erro I2C lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_Status(void) {
	if (adxl345.status == errI2C_ERROR) return i2c2_CodeError();
	else return adxl345.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: Lê uma registrador do ADXL345
// Parametros:
//		reg: Endereço do registrador
//		*data: ponteiro para o retorno do byte lido
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_ReadReg(u8 reg, u8 *data) {
	adxl345.status = adxl345_Send(adxl345.addrDevice, &reg, 1);
	if (adxl345.status == pdPASS) adxl345.status = adxl345_Receive(adxl345.addrDevice, data, 1);

	#if (ADX345_USE_DEBUG == pdON)
	plognp("ADX - READ REG"CMD_TERMINATOR);
	plognp("	device 0x%x, reg 0x%x, value 0x%x, adxl345.status %d"CMD_TERMINATOR, adxl345.addrDevice, reg, *data, adxl345.status);
	#endif

	return adxl345.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Reads num bytes starting from address register on device in to _buff array
//		reg: Endereço do registrador
//		len: tamanho do buffer
//		*data: ponteiro do bufer para o retorno do byte lido
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_ReadBuffer(u8 reg, int len, u8* buff) {
	adxl345.status = adxl345_Send(adxl345.addrDevice, &reg, 1);
	if (adxl345.status == pdPASS) adxl345.status = adxl345_Receive(adxl345.addrDevice, buff, len);

	#if (ADX345_USE_DEBUG == pdON)
	plognp("ADX - READ REG"CMD_TERMINATOR);
	plognp("	device 0x%x, reg init 0x%x, adxl345.status %d value: ", adxl345.addrDevice, reg, adxl345.status);
	int x; for (x=0;x<len;x++)  plognp("0x%x", buff[x]);
	plognp(CMD_TERMINATOR);
	#endif

	return adxl345.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Descrição: Escreve dados no registrador desejado
// Parametros:
//		reg: Endereço do dispositivo
//		data: Valor a ser escrito no registrador
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_WriteReg(u8 reg, u8 data) {
	u8 buffer[2];

	buffer[0] = reg;
	buffer[1] = data;

	adxl345.status = adxl345_Send(adxl345.addrDevice, buffer, 2);
	return adxl345.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Função para ligar ou desligar um determinado bit de um determinado registrador
// Parametro:
//		regAdress: Registrador alvo para ligar ou desligar o bit desejado
//		bitPos:	O bit a ser ligado ou desligado
//		state: Liga (pdON) ou desliga (pdOFF) o bit desejado do registrador
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
static int adxl345_SetRegisterBit(u8 regAdress, int bitPos, bool state) {
	u8 _b;
	adxl345_ReadBuffer(regAdress, 1, &_b);
	if (state) 	_b |= (1 << bitPos);  // forces nth bit of _b to be 1.  all other bits left alone.
	else 		_b &= ~(1 << bitPos); // forces nth bit of _b to be 0.  all other bits left alone.
	adxl345_WriteReg(regAdress, _b);
	return adxl345.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Ajusta o o limite de aceleração de trabalho do adxl345
// Parametro: res:
//		ADXL345_RES_2: Sensibilidade de 2g
//		ADXL345_RES_4: Sensibilidade de 4g
//		ADXL345_RES_8: Sensibilidade de 8g
//		ADXL345_RES_16: Sensibilidade de 16g
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_SetRangeSetting(int res) {
	u8 _s, _b;

	switch (res) {
	case 2:	_s = 0;	break;
	case 4:	_s = 1;	break;
	case 8:	_s = 2;	break;
	case 16:_s = 3;	break;
	default:_s = 0;
	}
	adxl345_ReadBuffer(ADXL345_DATA_FORMAT, 1, &_b);
	_s |= (_b & 0xec);
	adxl345_WriteReg(ADXL345_DATA_FORMAT, _s);

	return adxl345.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Ajusta a resolução ADC do adxl345
// Parametro:
//		fullResBit:
//			pdON: the device is in full resolution mode, where the output resolution increases with the g range
//				set by the range bits to maintain a 4mg/LSB scal factor
//			pdOFF: the device is in 10-bit mode, and the range buts determine the maximum g range and scale factor
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_SetFullResBit(bool fullResBit) {
	return adxl345_SetRegisterBit(ADXL345_DATA_FORMAT, 3, fullResBit);
}

//-----------------------------------------------------------------------------------------------------------------------
// Liga ou desliga o modo econômico do adxl345
// Parametro:
//		state: pdON liga modo econômico, pdOFF desliga
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_SetLowPower(bool state) {
	return adxl345_SetRegisterBit(ADXL345_BW_RATE, 4, state);
}

//-----------------------------------------------------------------------------------------------------------------------
// Ajusta a taxa de atualização da FIFO quando usada ou dos registradores XYZ quando não usada a FIFO
// Parametro:
// 		br: Taxa de atualização: ADXL345_BW_1600, ADXL345_BW_800,  ADXL345_BW_400,  ADXL345_BW_200, ADXL345_BW_100
// 			ADXL345_BW_50, ADXL345_BW_25, ADXL345_BW_12, ADXL345_BW_6, ADXL345_BW_3
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_SetRate(int br) {
	u8 b;
	adxl345_ReadBuffer(ADXL345_BW_RATE, 1, &b);
	b &= ~0xf;
	b |= br;
	adxl345_WriteReg(ADXL345_BW_RATE, b);

	return adxl345.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Liga ou desliga o uso da FIFO e connfigura o seu modo de trabalho
// Parametro:
// 	fifoMode:
//			ADXL345_FIFO_MODE_BYPASS: Sem uso da FIFO, os registradores XYZ são atualziados direto
//			ADXL345_FIFO_MODE_FIFO: Os dados dos três eixos são armazenados na FIFO. Quando o nível da FIFO é igual
//				ao registrador FIFO_CTL (addr 0x38) uma interrupção é emitida. Se a FIFO ficar cheia os dados novos serão descartados
//			ADXL345_FIFO_MODE_STREAM:Os dados dos três eixos são armazenados na FIFO. Quando o nível da FIFO é igual
//				ao registrador FIFO_CTL (addr 0x38) uma interrupção é emitida. Se a FIFO ficar cheia os dados antigos serão descartados
//			ADXL345_FIFO_MODE_TRIGGER: Ver datasheet
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_SetFIFO(int fifoMode) {
	u8 b;
	adxl345_ReadBuffer(ADXL345_FIFO_CTL, 1, &b);
	b &= ~0xf0;
	b |= fifoMode;
	adxl345_WriteReg(ADXL345_FIFO_CTL, b);
	return adxl345.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Liga ou desliga determinadas interrupções
// Parametros
//		interrupBit: aponta a intderrupção que vamos ativar ou não:
//				ADXL345_INT_DATA_READY, ADXL345_INT_SINGLE_TAP, ADXL345_INT_DOUBLE_TAPT, ADXL345_INT_ACTIVITY
//				ADXL345_INT_INACTIVITY, ADXL345_INT_FREE_FALL,  ADXL345_INT_WATERMARK,   ADXL345_INT_OVERRUNY
// 		state: Ativa (pdON) ou não ativa (pdOFF) a interrupção desejada
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_SetInterrupt(u8 interruptBit, bool state) {
	return adxl345_SetRegisterBit(ADXL345_INT_ENABLE, interruptBit, state);
}

#if (ADXL345_USE_FORCE_G == pdON)
// Sets the THRESH_ACT u8 which holds the threshold value for detecting activity.
// The data format is unsigned, so the magnitude of the activity event is compared
// with the value is compared with the value in the THRESH_ACT register. The scale
// factor is 62.5mg/LSB. A value of 0 may result in undesirable behavior if the
// activity interrupt is enabled. The maximum value is 255.
int adxl345_SetActivityThreshold(int activityThreshold) {
	activityThreshold = constrain(activityThreshold,0,255);
	u8 _b = (u8)(activityThreshold);
	return adxl345_WriteReg(ADXL345_THRESH_ACT, _b);
}

// Sets the THRESH_INACT u8 which holds the threshold value for detecting inactivity.
// The data format is unsigned, so the magnitude of the inactivity event is compared
// with the value is compared with the value in the THRESH_INACT register. The scale
// factor is 62.5mg/LSB. A value of 0 may result in undesirable behavior if the
// inactivity interrupt is enabled. The maximum value is 255.
int adxl345_SetInactivityThreshold(int inactivityThreshold) {
	inactivityThreshold = constrain(inactivityThreshold,0,255);
	u8 _b = (u8)(inactivityThreshold);
	return adxl345_WriteReg(ADXL345_THRESH_INACT, _b);
}

// Sets the TIME_INACT register, which contains an unsigned time value representing the
// amount of time that acceleration must be less thant the value in the THRESH_INACT
// register for inactivity to be declared. The scale factor is 1sec/LSB. The value must
// be between 0 and 255.
int adxl345_SetTimeInactivity(int timeInactivity) {
	timeInactivity = constrain(timeInactivity,0,255);
	u8 _b = (u8)(timeInactivity);
	return adxl345_WriteReg(ADXL345_TIME_INACT, _b);
}

// state pdON ou pdOFF
int adxl345_SetActivityX(bool state) {
	return adxl345_SetRegisterBit(ADXL345_ACT_INACT_CTL, 6, state);
}

// state pdON ou pdOFF
int adxl345_SetActivityY(bool state) {
	return adxl345_SetRegisterBit(ADXL345_ACT_INACT_CTL, 5, state);
}

// state pdON ou pdOFF
int adxl345_SetActivityZ(bool state) {
	return adxl345_SetRegisterBit(ADXL345_ACT_INACT_CTL, 4, state);
}

// state pdON ou pdOFF
int adxl345_SetInactivityX(bool state) {
	return adxl345_SetRegisterBit(ADXL345_ACT_INACT_CTL, 2, state);
}

// state pdON ou pdOFF
int adxl345_SetInactivityY(bool state) {
	return adxl345_SetRegisterBit(ADXL345_ACT_INACT_CTL, 1, state);
}

// state pdON ou pdOFF
int adxl345_SetInactivityZ(bool state) {
	return adxl345_SetRegisterBit(ADXL345_ACT_INACT_CTL, 0, state);
}

// state pdON ou pdOFF
int adxl345_SetTapDetectionOnX(bool state) {
	return adxl345_SetRegisterBit(ADXL345_TAP_AXES, 2, state);
}

// state pdON ou pdOFF
int adxl345_SetTapDetectionOnY(bool state) {
	return adxl345_SetRegisterBit(ADXL345_TAP_AXES, 1, state);
}

// state pdON ou pdOFF
int adxl345_SetTapDetectionOnZ(bool state) {
	return adxl345_SetRegisterBit(ADXL345_TAP_AXES, 0, state);
}

// Sets the THRESH_TAP u8 value
// it should be between 0 and 255
// the scale factor is 62.5 mg/LSB
// A value of 0 may result in undesirable behavior
int adxl345_SetTapThreshold(int tapThreshold) {
	tapThreshold = constrain(tapThreshold,0,255);
	u8 _b = (u8)(tapThreshold);
	return adxl345_WriteReg(ADXL345_THRESH_TAP, _b);
}

// Sets the DUR u8
// The DUR u8 contains an unsigned time value representing the maximum time
// that an event must be above THRESH_TAP threshold to qualify as a tap event
// The scale factor is 625µs/LSB
// A value of 0 disables the tap/double tap funcitons. Max value is 255.
int adxl345_SetTapDuration(int tapDuration) {
	tapDuration = constrain(tapDuration,0,255);
	u8 _b = (u8)(tapDuration);
	return adxl345_WriteReg(ADXL345_DUR, _b);
}

// Sets the latency (latent register) which contains an unsigned time value
// representing the wait time from the detection of a tap event to the start
// of the time window, during which a possible second tap can be detected.
// The scale factor is 1.25ms/LSB. A value of 0 disables the double tap function.
// It accepts a maximum value of 255.
int adxl345_SetDoubleTapLatency(int doubleTapLatency) {
	u8 _b = (u8)(doubleTapLatency);
	return adxl345_WriteReg(ADXL345_LATENT, _b);
}

// Sets the Window register, which contains an unsigned time value representing
// the amount of time after the expiration of the latency time (Latent register)
// during which a second valud tap can begin. The scale factor is 1.25ms/LSB. A
// value of 0 disables the double tap function. The maximum value is 255.
int adxl345_SetDoubleTapWindow(int doubleTapWindow) {
	doubleTapWindow = constrain(doubleTapWindow,0,255);
	u8 _b = (u8)(doubleTapWindow);
	return adxl345_WriteReg(ADXL345_WINDOW, _b);
}

// Sets the THRESH_FF register which holds the threshold value, in an unsigned format, for
// free-fall detection. The root-sum-square (RSS) value of all axes is calculated and
// compared whith the value in THRESH_FF to determine if a free-fall event occured. The
// scale factor is 62.5mg/LSB. A value of 0 may result in undesirable behavior if the free-fall
// interrupt is enabled. The maximum value is 255.
int adxl345_SetFreeFallThreshold(int freeFallThreshold) {
	freeFallThreshold = constrain(freeFallThreshold,0,255);
	u8 _b = (u8)(freeFallThreshold);
	return adxl345_WriteReg(ADXL345_THRESH_FF, _b);
}

// Sets the TIME_FF register, which holds an unsigned time value representing the minimum
// time that the RSS value of all axes must be less than THRESH_FF to generate a free-fall
// interrupt. The scale factor is 5ms/LSB. A value of 0 may result in undesirable behavior if
// the free-fall interrupt is enabled. The maximum value is 255.
int adxl345_SetFreeFallDuration(int freeFallDuration) {
	freeFallDuration = constrain(freeFallDuration,0,255);
	u8 _b = (u8)(freeFallDuration);
	return adxl345_WriteReg(ADXL345_TIME_FF, _b);
}
#endif // #if (ADXL345_USE_FORCE_G == pdON)

//-----------------------------------------------------------------------------------------------------------------------
// Coloca o adxl345 para o modo de medição, saindo do modo stanby
// Retorna:
//		pdPASS se a escrita foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_PowerOn(void) {
	return adxl345_WriteReg(ADXL345_POWER_CTL, 8);
}

static uint idxBuf;
static int buf[3][ADXL345_nSAMPLY];
void adxl345_ClearBuf(void) {
	int axis; int x;
	for (axis=0;axis<3;axis++)
		for (x=0;x<ADXL345_nSAMPLY;x++) buf[axis][x]=0;
	idxBuf = 0;
}

//-----------------------------------------------------------------------------------------------------------------------
// Lê os eixos x, y e z do adxl345
// Parametro: ponteiros da variáveis x, y e z para retornar com os valores
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
static int _adxl345_ReadXYZ(s16 *x, s16 *y, s16 *z) {
	u8 _buff[6];

	adxl345_ReadBuffer(ADXL345_DATAX0, 6, _buff); //read the acceleration data from the ADXL345

	if (adxl345.status != pdPASS) return adxl345.status;

	*x = (((int)_buff[1]) << 8) | _buff[0];
	*y = (((int)_buff[3]) << 8) | _buff[2];
	*z = (((int)_buff[5]) << 8) | _buff[4];

	int axis;
	s16* pval;
	for (axis=0;axis<3;axis++) {
		if (axis == 0) pval = x;
		else if (axis == 1) pval = y;
		else pval = z;

		if (idxBuf > 0) {
			int qtd;
			if (idxBuf < ADXL345_nSAMPLY) qtd = idxBuf+1; else qtd = ADXL345_nSAMPLY;
			buf[axis][idxBuf % ADXL345_nSAMPLY] = (s16)*pval;

			// media normal
			int m = 0;
			int i; for (i=0;i<qtd;i++) m += buf[axis][i];
			m = m/qtd;

			// vamos remover o ponto discrepante
			int xx;
			int k, d;
			k = idxBuf % ADXL345_nSAMPLY; //automaticamente considera o último ponto como sendo ruim
			d = 0; //nao tao ruim assim, por consideramos aqui com desvio "0" da media

			// procuramos pelo ponto mais longe
			for (i=0;i<qtd;i++) {
				xx = abs(m - buf[axis][i]); 	// qual a diferença deste ponto para media?
				if (xx<d) continue;    		// é menor que o outro? entao esquece e vai para o proximo
				k = i;						// marca o ponto "i" como sendo o pior de todos ate agora
				d = xx;						// anota quao ruim ele é
			}

			// vamor calcular novamente a media sem o ponto ruim
			m = 0;
			for (i=0;i<qtd;i++) if (i!=k) m += buf[axis][i];
			m = m/(qtd-1); // -1 porque sabemos que foi retirado apenas 1 ponto

			*pval =(s16)m;
		}
	}
	idxBuf++;

	return adxl345.status;
}

//-----------------------------------------------------------------------------------------------------------------------
// Atualiza Lê os eixos x, y e z
// Parametros: ponteiros das vars x,y,z para retorno dos valores
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
static int adxl345_ReadXYZ(void) {
	#if (ADXL345_USE_PROCESS == pdOFF)
	adxl345_ClearBuf();
	int i; for(i=0; i<ADXL345_nSAMPLY; i++) {
		if (_adxl345_ReadXYZ(&adxl345.x, &adxl345.y, &adxl345.z) != pdPASS) return adxl345.status;
	}

	updatedXYZ = pdTRUE; // sinaliza para as funções adxl345_ReadAngles e adxl345_ReadAcceleration não executam leituras XYZ denovo

	return pdPASS;
	#else
	return _adxl345_ReadXYZ(&adxl345.x, &adxl345.y, &adxl345.z); //read the accelerometer values and store them in variables  x,y,z
	#endif
}

//-----------------------------------------------------------------------------------------------------------------------
// Retorna com os valores atuais dos eixos x,y,z
// Parametros: ponteiros das vars x,y,z para retorno dos valores
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_GetXYZ(s16* x, s16* y, s16* z) {
	int ret = adxl345_ReadXYZ();
	*x = adxl345.x;
	*y = adxl345.y;
	*z = adxl345.z;
	return ret;
}


#if (ADXL345_USE_INCLINATION == pdON)
//-----------------------------------------------------------------------------------------------------------------------
// Calcula roll, pitch e angulo de inclinação
//-----------------------------------------------------------------------------------------------------------------------
static void adxl345_CalcRollPitch(void) {
	double fx = (s16)adxl345.x;
	double fy = (s16)adxl345.y;
	double fz = (s16)adxl345.z;

  	//Roll & Pitch Equations
    adxl345.roll  = (atan2(-fy, fz)*180.0)/M_PI;
    adxl345.pitch = (atan2(fx, sqrt(fy*fy + fz*fz))*180.0)/M_PI;
    adxl345.angle = round(adxl345.roll);
}

//-----------------------------------------------------------------------------------------------------------------------
// Retorna com o ângulo de inclinação
// Parametros: ponteiro da var angle para retorno do valor
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_GetAngle(int* angle) {
	int ret = pdPASS;
	#if (ADXL345_USE_PROCESS == pdOFF)
	if (!updatedXYZ) ret = adxl345_ReadXYZ();
	updatedXYZ = pdFALSE;
	#endif

	adxl345_CalcRollPitch();
	*angle = adxl345.angle;

	return ret;
}

//-----------------------------------------------------------------------------------------------------------------------
// Retorna com o roll e pitch
// Parametros: ponteiros das vars para retorno dos valores
// Retorna:
//		pdPASS se a leitura foi feita com sucesso
//		errI2C_ERROR onde o código de erro pode ser lido com a função i2c2_CodeError()
//		errI2C_NO_RESPONSE: Sem resposta do dispositivo por um determinado tempo
//-----------------------------------------------------------------------------------------------------------------------
int adxl345_GetRollPitch(double* roll, double *pitch) {
	int ret = pdPASS;
	#if (ADXL345_USE_PROCESS == pdOFF)
	if (!updatedXYZ) ret = adxl345_ReadXYZ();
	updatedXYZ = pdFALSE;
	#endif

	adxl345_CalcRollPitch();

	*roll = adxl345.roll;
	*pitch = adxl345.pitch;

	return ret;
}

#endif


#if (ADXL345_USE_FORCE_G == pdON)
//-----------------------------------------------------------------------------------------------------------------------
// Calcula a aceleração
//-----------------------------------------------------------------------------------------------------------------------
static void adxl345_CalcAcceleration(void) {
	adxl345.ax = adxl345.x * ADXL345_GAINS_X;
	adxl345.ay = adxl345.y * ADXL345_GAINS_Y;
	adxl345.az = adxl345.z * ADXL345_GAINS_Z;
}

int adxl345_GetAcceleration(double *ax, double *ay, double *az) {
	int ret = pdPASS;
	#if (ADXL345_USE_PROCESS == pdOFF)
	if (!updatedXYZ) ret = adxl345_ReadXYZ();
	updatedXYZ = pdFALSE;
	#endif

	adxl345_CalcAcceleration();

	*ax = adxl345.ax;
	*ay = adxl345.ay;
	*az = adxl345.az;
	return ret;
}
#endif


#if (ADXL345_USE_PROCESS == pdON)
// Coleta dados do adxl345 a cada X ms
// Usando o processo têm a vantagem de não ficar muito tempo em um loop lendo todas as amostras.
// Porém, a desvantagem é que os valores demoram para estabilizar quando a há mudanças brusca de inclinação.
// 	Ex: Se mudamos a inclinação, para 20 amostras a 10ms de tempo de processo, acelerômetro vai se
//		estabilizar depois de 200ms, e durante este período vamos receber os valores sendo alterados
// Não usando processo, quando mandamos ler o valor da inclinação a função vai trazer o valor já estabilizado.
//	A desvantagem é que ficamos muito tempo parado em um loop para leitura dos valores, o tempo é referente
//		a taxa de amostragem do adxl345 e tempo de comunicação
void adxl345_Process(void) {
	static tTime timeRead = 0;
	if (now() <= timeRead) return;
	timeRead = now() + ADXL345_READING_TIME;

	adxl345_ReadXYZ();
	#if (ADXL345_USE_INCLINATION == pdON)
	adxl345_CalcRollPitch();
	#endif

	#if (ADXL345_USE_FORCE_G == pdON)
	adxl345_CalcAcceleration();
	#endif
}
#endif
