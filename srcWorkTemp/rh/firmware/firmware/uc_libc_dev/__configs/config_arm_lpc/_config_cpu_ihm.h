#ifndef __CONFIG_CPU_IHM_H
#define __CONFIG_CPU_IHM_H

#include "_config_lib_.h"

// ###################################################################################################################
// CONFIGURAÇÕES DO PAINEL PAINEL DO LASER 403
#define PNL403_USE_UART 1		// Define qual UART a usar. Valor 0 usar UART0 e assim sucessivamente
	#define PN403_DATAFLOW_DIR	GPIO0_DIR
	#define PN403_DATAFLOW_SET	GPIO0_SET
	#define PN403_DATAFLOW_CLR	GPIO0_CLR
	#define PN403_DATAFLOW		BIT_17

#endif
