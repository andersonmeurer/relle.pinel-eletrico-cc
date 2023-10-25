/*
	Driver da controladora de touchscreen
	Esta biblioteca somente na leitura do touch com tenseão de referencia do próprio touch (DFR)
	Não usamos outros recursos do tsc2046, como leitura de temperaturas, leitura aux e de tensão da bateria.
	Não usamos o recurso de interrupção quando a tela for tocada
*/

#include "tsc2046.h"
#include <stdlib.h>

#define useWINDOWS_SAMPES 0 	// Tempo em ms para janela de análise. Coloque valor 0 para não usar a janela
#define TSC2016_N_SAMPLES 3		// Define a quantidade de amostragem de dada canal ADC

// VALORES PARA 12 BITS
#define TSC2046_X_COORD_MAX             4090
#define TSC2046_Y_COORD_MAX             4090
#define TSC2046_Z1_COORD_MAX            4090
#define TSC2046_Z2_COORD_MAX            4090
#define TSC2046_DELTA_X_VARIANCE        (0x50)
#define TSC2046_DELTA_Y_VARIANCE        (0x50)
#define TSC2046_DELTA_Z1_VARIANCE       (0x50)
#define TSC2046_DELTA_Z2_VARIANCE       (0x50)


#if (SPI_TSC2016_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

#if (TSC2016_N_SAMPLES < 3)
#error TSC2016_N_SAMPLES deve ter o valor mínimo 3
#endif

#if (TSC2016_N_SAMPLES > 20)
#error TSC2016_N_SAMPLES não deve exceder a 20, pois vai haver um overrun da interrupção do timer
#endif

typedef struct {
	int initilized;
	int ad_top, ad_bottom;		// Valor analogico das margens do topo e embaixo
	int ad_left, ad_right; 		// Valor analogico das margens esquerda e direita
	int lcd_v_size, lcd_h_size; // Tamanho do LCD
	int swap_xy;				// Gira a tela
	volatile int state;			// Estado do toque na tela. Valor 0 = sem toque siginificativo, valor 1 a tela está sendo tocada. Atualizado a cada 40ms
	volatile int x,y;			// Posição da tela onde o usuário tocou
	volatile int x_d, y_d;			// Posição da tela onde o usuário tocou, fica registrado até que seja lida
} tTouch;

tTouch touch;

#if (useWINDOWS_SAMPES > 0)
void _init(void);
#endif

#define ADS_START         (1 << 7)
#define ADS_A2A1A0_d_y    (1 << 4)  /* differential */
#define ADS_A2A1A0_d_z1   (3 << 4)  /* differential */
#define ADS_A2A1A0_d_z2   (4 << 4)  /* differential */
#define ADS_A2A1A0_d_x    (5 << 4)  /* differential */
#define ADS_A2A1A0_temp0  (0 << 4)  /* non-differential */
#define ADS_A2A1A0_vbatt  (2 << 4)  /* non-differential */
#define ADS_A2A1A0_vaux   (6 << 4)  /* non-differential */
#define ADS_A2A1A0_temp1  (7 << 4)  /* non-differential */
#define ADS_8_BIT         (1 << 3)
#define ADS_12_BIT        (0 << 3)
#define ADS_SER           (1 << 2)  /* non-differential */
#define ADS_DFR           (0 << 2)  /* differential */
#define ADS_PD10_PDOWN    (0 << 0)  /* lowpower mode + penirq */
#define ADS_PD10_ADC_ON   (1 << 0)  /* ADC on */
#define ADS_PD10_REF_ON   (2 << 0)  /* vREF on + penirq */
#define ADS_PD10_ALL_ON   (3 << 0)  /* ADC + vREF on */


#define READ_12BIT_DFR(d, adc, vref) (ADS_START | d \
  | ADS_12_BIT | ADS_DFR | \
  (adc ? ADS_PD10_ADC_ON : 0) | (vref ? ADS_PD10_REF_ON : 0))

#define READ_Y(vref)  (READ_12BIT_DFR(ADS_A2A1A0_d_y,  1, vref))
#define READ_Z1(vref) (READ_12BIT_DFR(ADS_A2A1A0_d_z1, 1, vref))
#define READ_Z2(vref) (READ_12BIT_DFR(ADS_A2A1A0_d_z2, 1, vref))
#define READ_X(vref)  (READ_12BIT_DFR(ADS_A2A1A0_d_x,  1, vref))
#define PWRDOWN       (READ_12BIT_DFR(ADS_A2A1A0_d_y,  0, 0))  /* LAST */

/* single-ended samples need to first power up reference voltage;
 * we leave both ADC and VREF powered
 */
#define READ_12BIT_SER(x) (ADS_START | x \
  | ADS_12_BIT | ADS_SER)

#define REF_ON  (READ_12BIT_DFR(ADS_A2A1A0_d_x, 1, 1))
#define REF_OFF (READ_12BIT_DFR(ADS_A2A1A0_d_y, 0, 0))

// #################################################################################################################
// FUNÇÕES GLOBAIS
// #################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: Configura o pino CHIP SELECT
// Parametros	Nenhum
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void tsc2046_Init(void) {
	touch.initilized = pdFALSE;
	touch.state = 0;
	touch.swap_xy = 0;
	touch.x = -1;
	touch.y = -1;

	tsc2046_SetupPorts();	// Configurar o pino CS
    tsc2046_Enable();
    tsc2046_TransferByte(REF_ON);
    tsc2046_TransferByte((READ_12BIT_SER(ADS_A2A1A0_vaux) | ADS_PD10_ALL_ON));
    tsc2046_TransferByte(PWRDOWN);
	tsc2046_Disable();
	#if (useWINDOWS_SAMPES > 0)
	_init();
	#endif
	touch.initilized = pdTRUE;
}

//------------------------------------------------------------------------------------------------------------------
// Sinaliza para fazer ou não o swap da tela
//------------------------------------------------------------------------------------------------------------------
void tsc2046_SwapXY(int sw) {
	touch.swap_xy = sw;
}

//------------------------------------------------------------------------------------------------------------------
// Sinaliza qual é a dimensão do LCD
//------------------------------------------------------------------------------------------------------------------
void tsc2046_LcdSize(int h_size, int v_size) {
	touch.lcd_h_size = h_size;
	touch.lcd_v_size = v_size;
}

//------------------------------------------------------------------------------------------------------------------
// Ajusta a área ativa de toque da tela
//------------------------------------------------------------------------------------------------------------------
void tsc2046_SetArea(int top, int left, int bottom, int right) {
	touch.ad_top = top;
	touch.ad_bottom = bottom;
	touch.ad_left = left;
	touch.ad_right = right;
}

//------------------------------------------------------------------------------------------------------------------
// * @brief       Get Touch coordinates.
// * @param[out]  pX     X-Coord
// * @param[out]  pY     Y-Coord
// * @return      pdTRUE se houve um novo toque na tela, senão volta pdFALSE
//------------------------------------------------------------------------------------------------------------------
#if (useWINDOWS_SAMPES > 0)
int tsc2046_GetTouchCoord(s16* x, s16* y) {
	*x = touch.x;
	*y = touch.y;

	return pdPASS;
}

int tsc2046_GetTouchCoordST(s16* x, s16* y) {
	*x = touch.x;
	*y = touch.y;
	return touch.state;
}
#else
int tsc2046_GetTouchCoord(s16* x, s16* y) {
	if (touch.state) {
		touch.state = 0;
		*x = touch.x_d;
		*y = touch.y_d;
		return pdTRUE;
	} else {
		*x = -1;
		*y = -1;
		return pdFALSE;
	}
}

int tsc2046_GetTouchCoordST(s16* x, s16* y) {
	int st = touch.state;
	touch.state = 0;
	*x = touch.x_d;
	*y = touch.y_d;
	return st;
}
#endif


// #################################################################################################################
// FUNÇÕES AQUISIÇÃO
// #################################################################################################################
static u16 tsc2046_Transfer(u8 cmd) {
    u8 rx[2];

    tsc2046_Enable();
	tsc2046_TransferByte(cmd);			// envia comando
	rx[0] = tsc2046_TransferByte(0x0);	// Lê o dado
	rx[1] = tsc2046_TransferByte(0x0);	// Lê o dado
    tsc2046_Disable();

    return (((rx[0]&0x7F) <<8) | (rx[1]>>0)) >> 3; // para 12 bits
}

//------------------------------------------------------------------------------------------------------------------
// * @brief       Calculate the coefficient of pressure
// * @param[in]   x           X-Coordinate
// * @param[in]   z1          Z1-Coordinate
// * @param[in]   z2          Z2-Coordinate
// * @return      coefficient.
//------------------------------------------------------------------------------------------------------------------
static s16 tsc2046_CalPressureCoef(s16 x, s16 z1, s16 z2) {
    s16 z = -1;
    z = x*(z2/z1 - 1);
    return z;
	//return (x * abs(z2 - z1)) / z1;
}

//------------------------------------------------------------------------------------------------------------------
// * @brief       convert the coord received from TSC to a value on truly LCD.
// * @param[in]   Coord       received coord
// * @param[in]   MinVal    the minimum value of a coord
// * @param[in]   MaxVal     the maximum value of a coord
// * @param[in]   TrueSize   the size on LCD
// * @return      the coord after converting.
//------------------------------------------------------------------------------------------------------------------
static s16 tsc2046_ConvertCoord(s16 Coord, s16 MinVal, s16 MaxVal, s16 TrueSize) {
    s16 tmp;
    s16 ret;
    u8 convert = 0;

    if (MinVal > MaxVal) {       // Swap value
        tmp = MaxVal;
        MaxVal = MinVal;
        MinVal = tmp;
        convert = 1;
    }

    ret = (Coord - MinVal)*TrueSize/(MaxVal-MinVal);
    if (convert) ret = TrueSize - ret;

    return ret;
}

//------------------------------------------------------------------------------------------------------------------
// * @brief       Evaluate the coords received from TSC.
// * @param[in]   buf    list of coords
// * @param[in]   len   the number of entries in above list
// * @param[in]   maxVal     the maximum value of a coord
// * @param[in]   maxDelta   the maximum delta between coords
// * @return      -1: Invalid coords, coord in case it is valid.
//------------------------------------------------------------------------------------------------------------------
static s16 tsc2046_EvalCoord(volatile u16* buf, u32 len, u16 maxVal, u16 maxDelta) {
   	u32 i = 0;
   	s16 diff = 0, coord = -1;
   	u8 coord_valid = 0;

	for(i = 0; i < len; i++) {
		// ignore values are not in range
     	if(buf[i] >= maxVal) {
       		coord = -1;
       		coord_valid = 0;
       		continue;
     	}

     	// the first valid coord
     	if(coord == -1) {
         	coord = buf[i];
         	coord_valid = 0;
         	continue;
     	}

     	// evaluate coord
     	diff = buf[i] - coord;
     	if (diff < 0) diff = 0 - diff;
     	if (diff < maxDelta) {
       		coord = (coord + buf[i])/2;  // get mean value
       		coord_valid = 1;         // at least 2 continuous coords are valid
     	} else  {
       		coord = buf[i];      // new coord
       		coord_valid = 0;
     	}
   	}

   	if(coord_valid) return coord;
   	return -1;
}


#if (useWINDOWS_SAMPES > 0 )
// Faremos a leitura do touch a cada 1ms, porém assumimos que o touch foi pressionado ou não
//	quando o memso ficar no mínino useWINDOWS_SAMPES ms no mesmo estado. O 40ms é o tempo que mais rápido que um usuário mediano
//	consiga manter dedo sobre o touch e o intervalo entre os toques
#define nSAMPLY_BUF useWINDOWS_SAMPES

typedef struct {
	volatile int idx;
	volatile int buffer[nSAMPLY_BUF];
} tBuffer, *pBuffer;

typedef struct {
	volatile int ct; // contador de ticks para determinar quando o touch foi pressionado ou não
	volatile int ht; // sinaliza que o touch foi tocado ou não
	volatile int sync; // sincronizar a janela de useWINDOWS_SAMPES ms quando o touch foi tocado. Porém, o touch tem que ficar no mínimo useWINDOWS_SAMPES ms sem ser tocado
    volatile s16 x, y, z1, z2, z;
	volatile u16 x_buf[TSC2016_N_SAMPLES];
	volatile u16 y_buf[TSC2016_N_SAMPLES];
	volatile u16 z1_buf[TSC2016_N_SAMPLES];
	volatile u16 z2_buf[TSC2016_N_SAMPLES];
	tBuffer bufferX;
	tBuffer bufferY;
} tCap;

static tCap cap;

static int _addBuffer(pBuffer p, u16 tmp) {
	p->buffer[p->idx%nSAMPLY_BUF] = tmp;

	if (p->idx > 0) {
		int qtd;
		if (p->idx < nSAMPLY_BUF) qtd = p->idx+1; else qtd = nSAMPLY_BUF;

		// media normal
		int m = 0;
		int i; for (i=0;i<qtd;i++) m += p->buffer[i];
		m = m/qtd;

		// vamos remover o ponto discrepante
		int x;
		int k, d;
		k = p->idx % nSAMPLY_BUF; //automaticamente considera o último ponto como sendo ruim
		d = 0; //nao tao ruim assim, por consideramos aqui com desvio "0" da media

		// procuramos pelo ponto mais longe
		for (i=0;i<qtd;i++) {
			x = abs(m - p->buffer[i]); 	// qual a diferença deste ponto para media?
			if (x<d) continue;        					// é menor que o outro? entao esquece e vai para o proximo
			k = i;										// marca o ponto "i" como sendo o pior de todos ate agora
			d = x;										// anota quao ruim ele é
		}

		// vamor calcular novamente a media sem o ponto ruim
		m = 0;
		for (i=0;i<qtd;i++) if (i!=k) m += p->buffer[i];
		m = m/(qtd-1); // -1 porque sabemos que foi retirado apenas 1 ponto

		tmp = m;
	}

	p->idx++;
	return tmp;
}

static void _initBuffer(void) {
	cap.bufferX.idx = 0;
	cap.bufferY.idx = 0;
}

void _init(void) {
	cap.ct = 0;
	cap.ht = 0;
	cap.sync = 0;
	_initBuffer();
}

static void _readDevice(void) {
	cap.ht = 0; // sinaliza sem toque na tela

    u16 i;
    // Get X-Coordinate
    for (i = 0; i < TSC2016_N_SAMPLES; i++) cap.x_buf[i] = tsc2046_Transfer(READ_X(0));
    cap.x = tsc2046_EvalCoord(cap.x_buf,TSC2016_N_SAMPLES,TSC2046_X_COORD_MAX,TSC2046_DELTA_X_VARIANCE);
    if (cap.x<=0) { _initBuffer(); return; }

    // Get Y-Coordinate
    for (i = 0; i < TSC2016_N_SAMPLES; i++) cap.y_buf[i] = tsc2046_Transfer(READ_Y(0));
    cap.y = tsc2046_EvalCoord(cap.y_buf,TSC2016_N_SAMPLES,TSC2046_Y_COORD_MAX,TSC2046_DELTA_Y_VARIANCE);
    if (cap.y<=0) { _initBuffer(); return; }

    // Get Z1-Coordinate
    for (i = 0; i < TSC2016_N_SAMPLES; i++) cap.z1_buf[i] = tsc2046_Transfer(READ_Z1(0));
    cap.z1 = tsc2046_EvalCoord(cap.z1_buf,TSC2016_N_SAMPLES,TSC2046_Z1_COORD_MAX,TSC2046_DELTA_Z1_VARIANCE);
    if (cap.z1 <= 0) { _initBuffer(); return; }

    // Get Z2-Coordinate
    for (i = 0; i < TSC2016_N_SAMPLES; i++) cap.z2_buf[i] = tsc2046_Transfer(READ_Z2(0));
    cap.z2 = tsc2046_EvalCoord(cap.z2_buf,TSC2016_N_SAMPLES,TSC2046_Z2_COORD_MAX,TSC2046_DELTA_Z2_VARIANCE);
    if (cap.z2 <= 0) { _initBuffer(); return; }

	cap.z = tsc2046_CalPressureCoef(cap.x, cap.z1, cap.z2);
    //if ( (cap.z < 0) || (cap.z > 6000) ) { _initBuffer(); return; }
    if ( (cap.z < 0) || (cap.z > 11000) ) { _initBuffer(); return; }

	cap.ht = 1; // sinaliza que houve o toque na tela

    // Swap, adjust to truly size of LCD
   	if(touch.swap_xy) {
		cap.y = tsc2046_ConvertCoord(cap.x, touch.ad_top, touch.ad_bottom, touch.lcd_v_size);
        cap.x = tsc2046_ConvertCoord(cap.y, touch.ad_left, touch.ad_right, touch.lcd_h_size);
	} else {
	    cap.x = tsc2046_ConvertCoord(cap.x, touch.ad_top, touch.ad_bottom, touch.lcd_v_size);
	 	cap.y = tsc2046_ConvertCoord(cap.y, touch.ad_left, touch.ad_right, touch.lcd_h_size);
    }

    // adiciona o resultado no buffer circular e já traz a média dos resultados
    cap.x = _addBuffer(&cap.bufferX , cap.x);
    cap.y = _addBuffer(&cap.bufferY , cap.y);

	tsc2046_TransferByte(PWRDOWN);
}

static void _getValues(void) {
	// se houve o toque na tela após 40ms sem nada ser tocado antes, vamos sincronizar a janela de 40ms para o primeiro toque
	if (cap.sync) { cap.sync = 0; cap.ct = 0;} // Limpa o contador da janela de 40ms e sinaliza que não é preciso sincronizar
	cap.ct++;
	if (cap.ct < useWINDOWS_SAMPES) return; // Esperar pelo menos useWINDOWS_SAMPES ms para tomar decição se a tela foi tocada ou não
	cap.ct = 0; // Limpa o contador da janela de 40ms

	if (cap.ht) { // Se houve o toque durante os 40ms
		touch.x = cap.x; touch.y = cap.y; // Captura as posições de toque X e Y
		touch.state = 1; // sinaliza que o touch foi tocado
		// Não limpamos o buffer aqui porque a tela pode ser mantida pressionada além dos 40ms
	} else { // Se não houve o toque durante os 40ms
		touch.x = -1; touch.y = -1; // Sinaliza que as coordenadas são irrelevantes
		touch.state = 0; // sinaliza que o touch não foi tocado
		cap.sync = 1; // sinaliza que a janela de 40ms pode ser sincronizada quando o touch for tocado pela primeira vez
	}
}

// esta função deve ser chamada a cada 1ms
void tsc2046_Tick(void) {
	if (!touch.initilized) return;
	_readDevice();
	_getValues();
}
#else

typedef struct {
    volatile s16 x, y, z1, z2, z;
	volatile u16 x_buf[TSC2016_N_SAMPLES];
	volatile u16 y_buf[TSC2016_N_SAMPLES];
	volatile u16 z1_buf[TSC2016_N_SAMPLES];
	volatile u16 z2_buf[TSC2016_N_SAMPLES];
} tCap;
static tCap cap;


// esta função deve ser chamada a cada 1ms
void tsc2046_Tick(void) {
	if (!touch.initilized) return;

	touch.x = -1; touch.y = -1;
    u16 i;
    // Get X-Coordinate
    for (i = 0; i < TSC2016_N_SAMPLES; i++) cap.x_buf[i] = tsc2046_Transfer(READ_X(0));
    cap.x = tsc2046_EvalCoord(cap.x_buf,TSC2016_N_SAMPLES,TSC2046_X_COORD_MAX,TSC2046_DELTA_X_VARIANCE);
    if (cap.x<=0) return;

    // Get Y-Coordinate
    for (i = 0; i < TSC2016_N_SAMPLES; i++) cap.y_buf[i] = tsc2046_Transfer(READ_Y(0));
    cap.y = tsc2046_EvalCoord(cap.y_buf,TSC2016_N_SAMPLES,TSC2046_Y_COORD_MAX,TSC2046_DELTA_Y_VARIANCE);
    if (cap.y<=0) return;

    // Get Z1-Coordinate
    for (i = 0; i < TSC2016_N_SAMPLES; i++) cap.z1_buf[i] = tsc2046_Transfer(READ_Z1(0));
    cap.z1 = tsc2046_EvalCoord(cap.z1_buf,TSC2016_N_SAMPLES,TSC2046_Z1_COORD_MAX,TSC2046_DELTA_Z1_VARIANCE);
    if (cap.z1 <= 0) return;

    // Get Z2-Coordinate
    for (i = 0; i < TSC2016_N_SAMPLES; i++) cap.z2_buf[i] = tsc2046_Transfer(READ_Z2(0));
    cap.z2 = tsc2046_EvalCoord(cap.z2_buf,TSC2016_N_SAMPLES,TSC2046_Z2_COORD_MAX,TSC2046_DELTA_Z2_VARIANCE);
    if (cap.z2 <= 0) return;

	cap.z = tsc2046_CalPressureCoef(cap.x, cap.z1, cap.z2);
    if ( (cap.z < 0) || (cap.z > 6000) ) return;
   	//if ( (cap.z < 0) || (cap.z > 11000) ) return;

    // Swap, adjust to truly size of LCD
   	if(touch.swap_xy) {
		touch.y = tsc2046_ConvertCoord(cap.x, touch.ad_top, touch.ad_bottom, touch.lcd_v_size);
        touch.x = tsc2046_ConvertCoord(cap.y, touch.ad_left, touch.ad_right, touch.lcd_h_size);
	} else {
	    touch.x = tsc2046_ConvertCoord(cap.x, touch.ad_top, touch.ad_bottom, touch.lcd_v_size);
	 	touch.y = tsc2046_ConvertCoord(cap.y, touch.ad_left, touch.ad_right, touch.lcd_h_size);
    }

    touch.x_d = touch.x;
    touch.y_d = touch.y;
    touch.state = 1;

	tsc2046_TransferByte(PWRDOWN);
}
#endif
