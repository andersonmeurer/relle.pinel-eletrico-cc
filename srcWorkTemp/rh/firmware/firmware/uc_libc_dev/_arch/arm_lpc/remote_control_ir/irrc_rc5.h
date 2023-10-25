#ifndef __IRRC_RC5_H
#define __IRRC_RC5_H

#include "_config_cpu_.h"

#define IRRC_USE_SNIFFER pdON
	#define IRRC_nSAMPLES 256

#define IRRC_START_BIT		889 	// 889 us
#define IRRC_MIN_HALF_BIT   640  	// 640 us
#define IRRC_HALF_BIT_TIME  889  	// 889 us
#define IRRC_MAX_HALF_BIT   1140  	// 1140 us
#define IRRC_MIN_FULL_BIT   1340  	// 1340 us
#define IRRC_FULL_BIT_TIME  1778  	// 1778 us
#define IRRC_MAX_FULL_BIT   2220  	// 2220 us
typedef struct {
	volatile int system;  	// Format 1E/Nts4 s3 s3 s1 s0
	volatile int command;  	// Format 0 0  c5 c4c3 c2c1 c0
	volatile int newData;
} tRC5, *pRC5;
tRC5 rc5;

int irrc_Init(void);
void irrc_SetIntHandler(void (*int_func)(uint sys, uint cmd) );
int irrc_GetData(uint* sys, uint* cmd);

#if (IRRC_USE_SNIFFER == pdON)
#define pout_Init()	(GPIO2_DIR |= BIT_9)
#define pout_Off()	(GPIO2_CLR = BIT_9)
#define pout_On()	(GPIO2_SET = BIT_9)
int irrc_GetRaw(uint idx);
void irrc_ClearRaw(void);
#endif

#endif
