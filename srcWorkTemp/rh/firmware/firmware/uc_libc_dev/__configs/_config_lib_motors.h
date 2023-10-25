#ifndef __CONFIG_LIB_MOTORS_H
#define __CONFIG_LIB_MOTORS_H

// ######################################################################################################
// SERVO MOTORES

// DEFINE QUAL O TIMER QUE VAMOS USAR PARA CONTROLAR O SERVO1
//	Para cada servo requer um timer dedicado
#define SERVO1_USE_TIMER	1	// Cuidado para não usar o mesmo timer de outros módulos como SYSTIME ou UIP,
								//	para isso consulte archXXXX.h da arquitetura que estais usando

#define SERVO2_USE_TIMER	2	// Cuidado para não usar o mesmo timer de outros módulos como SYSTIME ou UIP,
								//	para isso consulte archXXXX.h da arquitetura que estais usando

#endif
