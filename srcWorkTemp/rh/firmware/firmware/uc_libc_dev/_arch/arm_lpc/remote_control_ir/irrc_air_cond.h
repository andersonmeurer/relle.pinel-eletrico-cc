#ifndef __IRRC_AIR_COND_H
#define __IRRC_AIR_COND_H

#include "_config_cpu_.h"
#include "_config_lib_irrc.h"

#define IRRC_USE_DEBUG pdON
#define IRRC_USE_SNIFFER pdON
	#define IRRC_nSAMPLES 256

#if (IRRC_AIR_COND_USE == IRRC_JAPANESE)
typedef enum {
	modeAUTO = 0,
	modeHEAT,
	modeDRY,
	modeCOLD,
	modeFAN
} tAirMode;

typedef struct {
	volatile tAirMode mode;
	volatile u8 on; 		//  Sinaliza para ligar ou desligar
	volatile u8 temp; 		//  Temperatura de trabalho
	volatile u8 fan; 		//  Valor 0 auto, valor 1, 2, 3 e 5 velocidade crescentes
	volatile u8 swing; 		// 	0x1: Aleta na horizontal, direcionando o vento totalmente para o teto
							//	0x2 a 0x4: valores de 2 a 4 aletas entre horizontal e vertical
							//	0x5: Aleta na vertical, direcionando o vento totalmente para baixo
							//	0x7: Liga o swing da aleta, aleta indo da horizontal para vertical continualmente
	volatile u8 timerOn; 	//
	volatile u8 timerOff; 	//
	volatile u8 sleep; 	 	//  Liga ou não o recurso sleep
} tAirCond, *pAir;

#define IRRC_nBITS_RX			112
#define IRRC_START_BIT_MIN 		1014 	// -30% de 3380us (3.38ms) que é o valor da especificão
#define IRRC_START_BIT_MAX 		4394	// +30% de 3380us (3.38ms) que é o valor da especificão
#define IRRC_START_SPACE_MIN 	1183	// -30% de 1690us (1.69ms) que é o valor da especificão
#define IRRC_START_SPACE_MAX 	2197	// +30% de 1690us (1.69ms) que é o valor da especificão
#define IRRC_LOW_BIT_MIN		280		// -30% de 420us que é o valor da especificão
#define IRRC_LOW_BIT_MAX		546		// +30% de 420us que é o valor da especificão
#define IRRC_HIGH_BIT_MIN		889		// -30% de 1270us que é o valor da especificão
#define IRRC_HIGH_BIT_MAX		1651	// +30% de 1270us que é o valor da especificão
#endif // #if (IRRC_AIR_COND_USE == IRRC_JAPANESE)

#if (IRRC_AIR_COND_USE == IRRC_DUGOLD)
typedef enum {
	modeCOLD = 0,
	modeDRY,
	modeHEAT,
	modeFAN,
	modeAUTO
} tAirMode;

typedef struct {
	volatile tAirMode mode;
	volatile u8 on; 		// Sinaliza para ligar ou desligar
	volatile u8 temp; 		// Temperatura de trabalho
	volatile u8 fan; 		// Valor 0 velocidade máxima, quando maior o valor menor é a velocidade
	volatile u8 swing; 		// Valor 0 auto, 1 a X sinaliza aleta de baixo para cima
	volatile u8 timerOn; 	// Sinaliza para ligar ou não o timer para desligar o ar condicionado
	volatile u8 timer; 	 	// Quantas horas para desligar o o ar condicionado
	volatile u8 sleep; 	 	// Liga ou não o recurso sleep
} tAirCond, *pAir;

#define IRRC_nBITS_RX			40
#define IRRC_START_BIT_MIN 		6300 	// -30% de 9000us (9ms) que é o valor da especificão
#define IRRC_START_BIT_MAX 		11700	// +30% de 9000us (9ms) que é o valor da especificão
#define IRRC_START_SPACE_MIN 	3150	// -30% de 4500us (4.5ms) que é o valor da especificão
#define IRRC_START_SPACE_MAX 	5850	// +30% de 4500us (4.5ms) que é o valor da especificão
#define IRRC_LOW_BIT_MIN		294		// -30% de 420us que é o valor da especificão
#define IRRC_LOW_BIT_MAX		546		// +30% de 420us que é o valor da especificão
#define IRRC_HIGH_BIT_MIN		1183	// -30% de 1690us que é o valor da especificão
#define IRRC_HIGH_BIT_MAX		2197	// +30% de 1690us que é o valor da especificão
#endif // #if (IRRC_AIR_COND_USE == IRRC_DUGOLD)

#if (IRRC_AIR_COND_USE == IRRC_SAMSUNG)
#define IRRC_nBITS_RX			48
#define IRRC_START_BIT_MIN 		3150 	// -30% de 4500us (4.5ms) que é o valor da especificão
#define IRRC_START_BIT_MAX 		5850	// +30% de 4500us (4.5ms) que é o valor da especificão
#define IRRC_START_SPACE_MIN 	3150	// -30% de 4500us (4.5ms) que é o valor da especificão
#define IRRC_START_SPACE_MAX 	5850	// +30% de 4500us (4.5ms) que é o valor da especificão
#define IRRC_LOW_BIT_MIN		392		// -30% de 560us que é o valor da especificão
#define IRRC_LOW_BIT_MAX		728		// +30% de 560us que é o valor da especificão
#define IRRC_HIGH_BIT_MIN		1183	// -30% de 1690us que é o valor da especificão
#define IRRC_HIGH_BIT_MAX		2197	// +30% de 1690us que é o valor da especificão
#endif // #if (IRRC_AIR_COND_USE == IRRC_SAMSUNG)

int irrc_Init(pAir p);
void irrc_SetIntHandler(void (*int_func)(pAir p) );
//int irrc_GetData(uint* addr, uint* data);

#if (IRRC_USE_SNIFFER == pdON)
#define pout_Init()	(GPIO2_DIR |= BIT_9)
#define pout_Off()	(GPIO2_CLR = BIT_9)
#define pout_On()	(GPIO2_SET = BIT_9)
int irrc_GetRaw(uint idx);
void irrc_ClearRaw(void);
#endif

#endif
