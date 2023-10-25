#include "dac.h"
#if (DAC_USE_DEBUG == pdON)
#include "stdio_uc.h"
#endif

void dac_Init(void) {
	#if defined (cortexm3)
	DAC_DesablePort(); // é preciso caso usar dac_Init() e dac_Finish() muitas vezes
	DAC_ConfigPort();
	DAC_DisablePulls();
	#else
	DAC_PINSEL = (DAC_PINSEL & ~DAC_PINMASK) | DAC_PIN; // Seleciona o pino externo para a função DAC
	#endif
 	DACR = 0;

	#if (DAC_USE_DEBUG == pdON)
  	plog("DA: Init DACR=0x%x"CMD_TERMINATOR, DACR);
  	plog("DA: Init DACCTRL=0x%x"CMD_TERMINATOR, DACCTRL);
  	plog("DA: Init DACCNTVAL=0x%x"CMD_TERMINATOR, DACCNTVAL);
	#endif
}

void dac_Finish(void) {
	#if defined (cortexm3)
	DAC_DesablePort();
	#else
	DAC_PINSEL &= ~((DAC_PINSEL & ~DAC_PINMASK) | DAC_PIN); // Seleciona o pino externo para a função DAC
	#endif
}


// retona 	pdPASS se a conversão feita com sucesso
//			errDEVICE_VALUE_INVALID: se o valor a se convertido está fora de faixa
int dac_Set(uint new_value) {
	if (new_value > 1023) return errDEVICE_VALUE_INVALID;
  	u16 dac_cr;
  	u16 dac_value;

  	dac_cr = DACR;																					// Capturo valor do registrador DAC
  	dac_value = (dac_cr & DACR_VALUE_MASK) >> DACR_VALUE_SHIFT; 									// Captura o valor dac convertido
  	dac_cr = (dac_cr & ~DACR_VALUE_MASK) | ((new_value << DACR_VALUE_SHIFT) & DACR_VALUE_MASK);		// limpa o valor convertido e escreve um novo valor
  	DACR = dac_cr;																					// Atualiza o registrador DAC

  	return dac_value;
}
