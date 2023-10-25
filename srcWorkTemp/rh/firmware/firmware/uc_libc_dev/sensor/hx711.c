// Biblioteca para ler sensores de peso
// 		Escrita em 20/08/2015
// 		Esta biblioteca trabalha com hx711 usando o modo padrão do circuito:
//			Amostragem dos valores a 10SPS
//			Usando canal A com ganho de 128

#include "hx711.h"
#include "hx711_drv.h"
#include "delay.h"

#if (HX711_USE_PROCESS == pdON)
#include "systime_irq.h"
#endif

#include <stdlib.h>
void hx711_ClearBuf(void);

typedef struct {
	s32 offset;			// Valor para desconsiderar qualquer objeto que fica sobre o sensor para dar apoio ao objeto a ser pesado
	float scale;		// Ajusta a escala de medida para seu respectivo tipo de sensor
	s32 data;			// Valor em gramas
} tHx711, *pHx711;
static tHx711 hx711;

static uint timeoutLoad;
static volatile uint timeout; // Contador do timeout

static int hx711_ReadData(int calib);
static int hx711_Average(int calib);

//-----------------------------------------------------------------------------------------------------------------------
// Inicia o hx711 usando canal A com ganho de 128
// 	Paramentro:
//		tm: tempo em ms na espera da resposta do hx711
//-----------------------------------------------------------------------------------------------------------------------
void hx711_Init(uint tm) {
	hx711_SetupPorts();
	timeoutLoad = tm;

	hx711.offset = 0;
	hx711.scale = 1;

	hx711_Reset();
}

// emitir um reset ao hx711 para colocar nas condições padrões
void hx711_Reset(void) {
	hx711_ClockHigh();
	delay_us(100); // Espera para que o hx711 perceba o sinal de reset
	hx711_ClockLow();

	hx711.data = 0;
	hx711_ClearBuf();
}

//-----------------------------------------------------------------------------------------------------------------------
// Configura o modo de trabalho do hx711.   AINDA NÃO TESTATO
// 	De acordo com a quantidade de pulsos enviados ao hx711 configuramos o mesmo
//		25 pulsos seleciona o canal A para medição com ganho de 128
//		27 pulsos seleciona o canal A para medição com ganho de 64
//		26 pulsos seleciona o canal B para medição com ganho de 32
// 	Paramentros:
//		nPulse: HX711_SET_CHANNEL_A_128, HX711_SET_CHANNEL_A_64, HX711_SET_CHANNEL_B_32
// 	Retorna:
//		pdPASS se a configuração foi feita com sucesso
//		pdFAIL se não foi possivel configurar o hx711
//-----------------------------------------------------------------------------------------------------------------------
int hx711_Config(int nPulse) {
	timeout = timeoutLoad;

	while (hx711_DOUT()) {
		if (timeout == 0) return pdFAIL;
	}

	int x; for (x=0; x<nPulse; x++) {
		hx711_ClockHigh();
		delay_us(1);
		hx711_ClockLow();
		delay_us(1);
	}

	return pdPASS;
}

static uint idxBuf;
static s32 buf[HX711_nSAMPLY];
void hx711_ClearBuf(void) {
	int x; for (x=0;x<HX711_nSAMPLY;x++) buf[x]=0;
	idxBuf = 0;
}

//-----------------------------------------------------------------------------------------------------------------------
// Lê os valores no hx711
// 	Paramentro:
//		calib: pdTRUE sinaliza que a leitura é para calibração, ou seja, vamos desconsiderar o offset
// 	Retorna:
//		pdPASS se a leitura foi feito com sucesso
//		pdFAIL se não foi possivel ler os dados
//-----------------------------------------------------------------------------------------------------------------------
static int hx711_ReadData(int calib) {
	 // Se o hx711 colocar esse pino em baixa, sinaliza que há dados a ser lidos
	if (hx711_DOUT()) return pdFAIL;

	// Emitir 24 pulsos de clock e ler os bits de dados. O primeiro bit é MSB
	hx711.data = 0;
	int x; for (x=0; x<24; x++) {
		hx711_ClockHigh();
		delay_us(1);
		if (hx711_DOUT()) hx711.data |= _BV(23-x);
		hx711_ClockLow();
		delay_us(1);
	}

	// Emitir 25° clock para colocar o hx711 pronto para a próxima atualização
	hx711_ClockHigh();
	delay_us(1);
	hx711_ClockLow();
	delay_us(1);

	// se o bit 23 estiver ligado se trata de um número negativo, então vamos cololocar os demais bit em 1
	if ((hx711.data & BIT_23) > 0) hx711.data |= 0xFF000000;

	// fazer filtro
	buf[idxBuf % HX711_nSAMPLY] = hx711.data;
	if (idxBuf > 0) {
		int qtd;
		if (idxBuf < HX711_nSAMPLY) qtd = idxBuf+1; else qtd = HX711_nSAMPLY;

		// media normal
		s32 m = 0;
		int i; for (i=0;i<qtd;i++) m += buf[i];
		m = m/qtd;

		// vamos remover o ponto discrepante
		s32 xx;
		s32 k, d;
		k = idxBuf % HX711_nSAMPLY; //automaticamente considera o último ponto como sendo ruim
		d = 0; //nao tao ruim assim, por consideramos aqui com desvio "0" da media

		// procuramos pelo ponto mais longe
		for (i=0;i<qtd;i++) {
			xx = abs(m - buf[i]); 	// qual a diferença deste ponto para media?
			if (xx<d) continue;    		// é menor que o outro? entao esquece e vai para o proximo
			k = i;						// marca o ponto "i" como sendo o pior de todos ate agora
			d = xx;						// anota quao ruim ele é
		}

		// vamor calcular novamente a media sem o ponto ruim
		m = 0;
		for (i=0;i<qtd;i++) if (i!=k) m += buf[i];
		m = m/(qtd-1); // -1 porque sabemos que foi retirado apenas 1 ponto

		hx711.data =(s32)m;
	}

	idxBuf++;

	hx711.data /= hx711.scale;
	// se não for calibração
	if (!calib) hx711.data -= hx711.offset;

	return pdPASS;
}

//-----------------------------------------------------------------------------------------------------------------------
// Fazer a média de leituras. Função útil para ajuste de zero ou quando não estamos usando o filtro
// 	Paramentro:
//		calib: pdTRUE sinaliza que a leitura é para calibração, ou seja, vamos desconsiderar o offset
// Retorna:	pdPASS se a configuração foi feita com sucesso com o valor da média em hx711.data
//			pdFAIL se não foi possivel configurar o hx711
//-----------------------------------------------------------------------------------------------------------------------
static int hx711_Average(int calib) {
	int x; for(x=0; x<HX711_nSAMPLY; x++) {
		timeout = timeoutLoad;
		while (!hx711_ReadData(calib)) {
			if (timeout == 0) return pdFAIL;
		}
	}

	return pdPASS;
}

//-----------------------------------------------------------------------------------------------------------------------
// Ajusta o offset de forma automática
// 	Sempre que a balança estiver sem objeto a medir é o valor do peso for diferente de 0 é preciso fazer o ajuste de zero
// Retorna:	pdPASS se a configuração foi feita com sucesso
//			pdFAIL se não foi possivel configurar o hx711
//-----------------------------------------------------------------------------------------------------------------------
int hx711_ZeroAdjustment(void) {
	int ret = hx711_Average(pdTRUE);
	hx711.offset = hx711.data;
	return ret;
}

//-----------------------------------------------------------------------------------------------------------------------
// retorna o valor em gramas do peso lido do hx711
// retorna pdPASS se a leitura foi feito com sucesso senão retorna pdFAIL
//-----------------------------------------------------------------------------------------------------------------------
int hx711_GetValue(s32 *data) {
	#if (HX711_USE_PROCESS == pdOFF)
	hx711_ClearBuf();
	int ret = hx711_Average(pdFALSE);
	*data = hx711.data;
	return ret;
	#endif
	*data = hx711.data;
	return pdPASS;
}

//-----------------------------------------------------------------------------------------------------------------------
// OffSet é o valor a desconsiderar qualquer objeto que fica sobre o sensor para dar apoio ao objeto a ser pesado
//-----------------------------------------------------------------------------------------------------------------------
void hx711_SetOffSet(s32 ofs) { hx711.offset = ofs; }
s32 hx711_GetOffSet(void) { return hx711.offset; }

//-----------------------------------------------------------------------------------------------------------------------
// Escala ajusta a escala de medida de acordo com o tipo de sensor. Valor 1999.0 usado para sensor de 750gramas
// A escala não pode ser 0
//-----------------------------------------------------------------------------------------------------------------------
void hx711_SetScale(float scl) { hx711.scale = scl; }
float hx711_GetScale(void) { return hx711.scale; }

static volatile uint timeRead = 0;
//-----------------------------------------------------------------------------------------------------------------------
// Esta função deve ser chamada a cada 1ms
//-----------------------------------------------------------------------------------------------------------------------
void hx711_Tick(void) {
	if (timeRead) timeRead--;
	if (timeout) timeout--;
}

#if (HX711_USE_PROCESS == pdON)
// Coleta dados do hx711 a cada X ms
// Usando o processo têm a vantagem de não ficar muito tempo em um loop lendo todas as amostras.
// Porém, a desvantagem é que os valores demoram para estabilizar quando a há mudanças brusca de peso.
// 	Ex: Se colocar um peso na balança, para 20 amostras a 10ms de tempo de processo, a balança vai se
//		estabilizar depois de 200ms, e durante este período vamos receber os valores crescendo
// Não usando processo, quando mandamos ler o valor da balança a função vai trazer o valor já estabilizado.
//	A desvantagem é que ficamos muito tempo parado em um loop para leitura dos valores, o tempo é referente
//		a taxa de amostragem do hx711 e tempo de comunicação
void hx711_Process(void) {
	if (timeRead) return;
	timeRead = HX711_READING_TIME;

	hx711_ReadData(pdFALSE);
}
#endif
