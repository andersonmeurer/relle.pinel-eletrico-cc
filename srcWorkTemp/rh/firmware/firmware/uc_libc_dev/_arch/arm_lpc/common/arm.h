#ifndef __ARM_H
#define __ARM_H
#include "_config_cpu_.h"

// COMPILADOR YAGARTO REQUER FUNÇÕES DE CHAMADA DE SISTEMAS (syscalls) EM  SUA LIBC.A
#if defined(COMPILER_TYPE_YAGARTO)
void _exit(int n);
#endif

#if defined(USE_EXT_SRAM) || defined(USE_EXT_SDRAM)
#include "mem_map.h"
#endif

// TESTES DAS CONFIGURAÇÕES DO CLOCK DO SISTEMA PARA ARM7TDMI
#if ((F_CPU < F_CPU_MIN) || (F_CPU > F_CPU_MAX))
	#error F_CPU FORA DOS LIMITES. Corrija F_CPU em arch_X.h (X = arquitetura do hardware)
#endif

// ###################################################################################################################
#if defined(LPC2468)
	#if ((PLL_M < PLL_M_MIN) || (PLL_M > PLL_M_MAX))
	#error PLL_M FORA DOS LIMITES. Corrija PLL_M em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ((PLL_N < PLL_N_MIN) || (PLL_N > PLL_N_MAX))
	#error PLL_N FORA DOS LIMITES. Corrija PLL_N em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ((FCCO < FCCO_MIN) || (FCCO > FCCO_MAX))
	#error FCCO FORA DOS LIMITES. Corrija PLL_M or PLL_P em arch_X.h (X = arquitetura do hardware)
	#endif

	#if (CCLK < CCLK_MIN)
	#error CCLK ESTÁ BAIXO DO LIMITE. Corrija PLL_M em arch_X.h (X = arquitetura do hardware)
	#endif
#endif

#if defined(LPC2214) || defined(LPC2294)  || defined(LPC2101) || defined(LPC2102) || defined(LPC2103)
	#if ((PLL_M < PLL_M_MIN) || (PLL_M > PLL_M_MAX))
	#error PLL_M FORA DOS LIMITES. Corrija PLL_M em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ((PLL_P != 1) && (PLL_P != 2) && (PLL_P != 4) && (PLL_P != 8))
	#error PLL_P FORA DOS LIMITES. Corrija PLL_P em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ((PBSD != PBSD_1_4_CCLK) && (PBSD != PBSD_1_2_CCLK) && (PBSD != PBSD_1_1_CCLK))
	#error VELOCIDADE do BARRAMENTO DE PERIFÉRICOS (PBSD) CONTÉM VALOR ILEGAL. Corrija em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ((FCCO < FCCO_MIN) || (FCCO > FCCO_MAX))
	#error FCCO FORA DOS LIMITES. Corrija PLL_M or PLL_P em arch_X.h (X = arquitetura do hardware)
	#endif
#endif


#if defined (LPC1751) || (LPC1752)
	#if ((PLL_M < PLL_M_MIN) || (PLL_M > PLL_M_MAX))
	#error PLL_M FORA DOS LIMITES. Corrija PLL_M em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ((PLL_N < PLL_N_MIN) || (PLL_N > PLL_N_MAX))
	#error PLL_N FORA DOS LIMITES. Corrija PLL_N em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ((FCCO < FCCO_MIN) || (FCCO > FCCO_MAX))
	#error FCCO FORA DOS LIMITES. Corrija PLL_M or PLL_P em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ( (CCLK < CCLK_MIN) || (CCLK > CCLK_MAX) )
	#error CCLK ESTÁ BAIXO DO LIMITE. Corrija PLL_M em arch_X.h (X = arquitetura do hardware)
	#endif

	#if defined(OSC_XTAL)
	#if ((PLL1_P != 1) && (PLL1_P != 2) && (PLL1_P != 4) && (PLL1_P != 8))
	#error PLL_P FORA DOS LIMITES. Corrija PLL1_P em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ((PLL1_M < PLL1_M_MIN) || (PLL1_M > PLL1_M_MAX))
	#error PLL1_M FORA DOS LIMITES. Corrija PLL1_M em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ( (USBCLKDIV < 1) || (USBCLKDIV > 31) )
	#error USBCLKDIV FORA DOS LIMITES. Corrija em arch_X.h (X = arquitetura do hardware)
	#endif
	#endif
#endif

#if defined (LPC1788)
	#if ((PLL_M < PLL_M_MIN) || (PLL_M > PLL_M_MAX))
	#error PLL_M FORA DOS LIMITES. Corrija PLL_M em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ((PLL_P != 1) && (PLL_P != 2) && (PLL_P != 4) && (PLL_P != 8))
	#error PLL_P FORA DOS LIMITES. Corrija PLL_P em arch_X.h (X = arquitetura do hardware)
	#endif

	#if (((PLL_OUT * 2 * PLL_P) < FCCO_MIN) || ((PLL_OUT * 2 * PLL_P) > FCCO_MAX))
	#error FCCO FORA DOS LIMITES. Corrija PLL_OUT or PLL_P em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ( (CCLK < CCLK_MIN) || (CCLK > CCLK_MAX) )
	#error CCLK FORA DOS LIMITES. Corrija em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ( (PCLK < PCLK_MIN) || (PCLK > PCLK_MAX) )
	#error PCLK FORA DOS LIMITES. Corrija em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ( (PLL_OUT < PLL_OUT_MIN) || (PLL_OUT > PLL_OUT_MAX) )
	#error PLL_OUT FORA DOS LIMITES. Corrija em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ( (PLL1_OUT < PLL_OUT_MIN) || (PLL1_OUT > PLL_OUT_MAX) )
	#error PLL1_OUT FORA DOS LIMITES. Corrija em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ( (CCLKDIV < 1) || (CCLKDIV > 31) )
	#error CCLKDIV FORA DOS LIMITES. Corrija em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ( (PCLKDIV < 1) || (PCLKDIV > 31) )
	#error PCLKDIV FORA DOS LIMITES. Corrija em arch_X.h (X = arquitetura do hardware)
	#endif

	#if ( (USBCLKDIV < 1) || (USBCLKDIV > 31) )
	#error USBCLKDIV FORA DOS LIMITES. Corrija em arch_X.h (X = arquitetura do hardware)
	#endif
#endif // lpc1788

void arm_Init ( void );
#if defined(USE_EXT_SRAM) || defined(USE_EXT_SDRAM)
void ramext_Init (void);
#endif

#if defined (cortexm3)
// ESTRUTURA DE CONFIFURAÇÕES DAS FUNÇÕES DOS PINOS
typedef struct {
	reg32* addrPort;// Endereço da porta do pino a ser configurado. PINSEL_Px (x de 0 a quantidade de portas do LPC)
	u8 numPin;		// Número do pino a ser configurado. PINSEL_PINx (x de 0 a 31)
	u8 nunFunc;		// Qual função o pino vai assumir: GPIO função 0 (padrão), UART, PWM, I2C ...
	u8 pinMode;		// Modo do pino
					//		PINSEL_PINMODE_INACTIVE: Sem resistor pull-up ou pull-down
					//		PINSEL_PINMODE_PULLUP  : Internal pull-up resistor
					//		PINSEL_PINMODE_PULLDOWN: Internal pull-down resistor
					//		PINSEL_PINMODE_REPEATER: Repeater mode.
	u8 openDrain;	// Ajusta se o pino vai fica em dreno aberto ou não
					//		0: Pin is in the normal (not open drain) mode
					//		1: Pin is in the open drain mode

} pinsel_cfg_t;

void pinsel_setPinFunc(reg32 *addrPort, u8 numPin, u8 numFunc);
void pinsel_setResistorMode(reg32 *addrPort, u8 numPin, u8 pinMode);
void pinsel_setOpenDrainMode(reg32 *addrPort, u8 numPin, u8 openDrain);
void pinsel_configPin(pinsel_cfg_t *cfgPIN);

#endif // cortexm3

#endif

