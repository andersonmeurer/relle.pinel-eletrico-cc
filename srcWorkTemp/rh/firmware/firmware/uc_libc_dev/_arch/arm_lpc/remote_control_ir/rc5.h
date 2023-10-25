#ifndef __IRRC_RC5_H
#define __IRRC_RC5_H

#include "_config_cpu_.h"

typedef struct {
	volatile int system;  	// Format 1E/Nts4 s3 s3 s1 s0
	volatile int command;  	// Format 0 0  c5 c4c3 c2c1 c0
	volatile int newData;
} tRC5, *pRC5;

tRC5 rc5;

int rc5_Init (void);
void rc5_SetIntHandler(void (*int_func)(int sys, int cmd) );
int rc5_GetData(int* system, int* command);

#endif
