#include "qei.h"
#include "vic.h"
#if defined(cortexm3)
#include "arm.h"
#endif

// Quadrature Encoder Interface (QEI)

// Biblioteca escrita em 2016-03-31

// Esta lib utiliza os pinos externos PHA e PHB para controle de um encoder
// As interrupções são usadas para:
// - Sinalizar a ativação do pino externo index, este pino é usado para zerar os contadores
// - Sinalizar overflow na velocidade
// - Sinalizar mudanças de direção
// - Sinalizar outras situações de comparações e clocks

typedef void (*qei_pFunc_t)(int);
static volatile qei_pFunc_t qei_IntFunctions;
void qei_ISR (void);

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Inicializa o periférico qei com configurações padrões e sem interruções. Configurações padrões:
//				QEI_DIRINV_NONE: A direção QEI é normal
//				QEI_SIGNALMODE_QUAD: Modo do sinal por fase
//				QEI_CAPMODE_4X: Modo de captura utilizando as bordas dos sinais PhA and PhB para contagem (4X)
//				QEI_INVINX_NONE: Sem inversão do sinal INDEX
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_Init(void) {
	PCONP |= PCQEI; // Ligar hardware QEI
	qei_ConfigPort(); // Ajusta os pinos do ARM assunir função de qei

	// Reset all remaining value in QEI peripheral
	QEICON = QEI_CON_RESP | QEI_CON_RESV | QEI_CON_RESI;
	QEIMAXPOS = 0x00;
	CMPOS0 = 0x00;
	CMPOS1 = 0x00;
	CMPOS2 = 0x00;
	INXCMP = 0x00;
	QEILOAD = 0x00;
	VELCOMP = 0x00;
	FILTER = 0x00;

	#if defined (cortexm3)
	nvic_enableIRQ(VIC_QEI);
	nvic_setPriority(VIC_QEI, QEI_PRIO_LEVEL);
	#endif

	#if defined(arm7tdmi)
  	if (!irq_Install(VIC_QEI, qei_ISR, QEI_PRIO_LEVEL))
		return errIRQ_INSTALL;
	#endif

	QEIIEC = QEI_IECLR_BITMASK; 	// Desliga todas as interrupções
	QEICLR = QEI_INTCLR_BITMASK; 	// Limpa as interrupções pendentes
	QEICONF = QEI_INVINX_NONE | QEI_CAPMODE_4X | QEI_SIGNALMODE_QUAD | QEI_DIRINV_NONE; // Set QEI configuration
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Reconfigura o QEI:
// Parametro: 	(QEI_DIRINV_CMPL ou QEI_DIRINV_NONE) |
//				(QEI_SIGNALMODE_CLKDIR ou QEI_SIGNALMODE_QUAD) |
//				(QEI_CAPMODE_2X ou QEI_CAPMODE_4X) |
//				(QEI_INVINX_NONE ou QEI_INVINX_EN)
//					QEI_DIRINV_CMPL: A direção QEI é complementado
//					QEI_DIRINV_NONE: A direção QEI é normal
//					QEI_SIGNALMODE_CLKDIR: Modo do sinal por clock/direção, onde PHA usado para determinar a direção e PHB para coletar o pulso
//					QEI_SIGNALMODE_QUAD: Modo do sinal por fase, usando PHA e PHB para detectar a fase
//					QEI_CAPMODE_2X: Modo de captura utilizando somente a brdas do sinal PhA para contagem (2X)
//					QEI_CAPMODE_4X: Modo de captura utilizando as bordas dos sinais PhA and PhB para contagem (4X)
//					QEI_INVINX_NONE: Sem inversão do sinal INDEX
//					QEI_INVINX_EN: Com inversão do sinal INDEX
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_Config(uint config) {
	QEICONF = config;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Reinicia os valores como velocidade, contadores, posição
// Parametro:	resetType: ( QEI_RESET_POS |  QEI_RESET_POSOnIDX | QEI_RESET_VEL | QEI_RESET_IDX)
//  				QEI_RESET_POS: Reset Position Counter
//   				QEI_RESET_POSOnIDX: Reset Position Counter on Index signal
//  				QEI_RESET_VEL: Reset Velocity
// 					QEI_RESET_IDX: Reset Index Counter
//				Usar o operador OR caso usar mais de um alvo de reset
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_Reset(uint resetType) {
	QEICON = resetType;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Aplica um filtro para coleta de amostras nas entradas PHA, PHB e INDEX.
//				O Filtro determina a quantidade	de clocks para requerer as amostras (debounce)
// Parametro: samplingPulse: Valor do filtro. Valor 0 não se utilza o filtro
//  		  reloadType: Determina o tipo de carregamento:
//					qeiVALUE_ABSOLUTE: Sinaliza que o registrador será atualizado com um valor absoluto
//					qeiVALUE_IN_US: Sinaliza que o registrador será atualizado com valor em micro segundos
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetFilter(uint samplingPulse, int reloadType) {
	if (reloadType)
			FILTER = (u32)( (PCLK) / (1000000UL/samplingPulse));
	else 	FILTER = samplingPulse;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Ajusta o valor de velocidade do timer. Quando haver um overflow no temporizador de velocidade,
//				o valor fixado no QEILOAD será carregado para o temporizador de velocidade
//				para o próximo período. A velocidade calculada em RPM, portanto, será afetado por esse valor.
// Parametro: reloadValue: Valor de carregamento para o registrador de velcodiade do timer (QEILOAD)
// retorna:  reloadType: Determina o tipo de carregamento:
//				qeiVALUE_ABSOLUTE: Sinaliza que o registrador será atualizado com um valor absoluto
//				qeiVALUE_IN_US: Sinaliza que o registrador será atualizado com valor em micro segundos
// -------------------------------------------------------------------------------------------------------------------
void qei_SetTimerReload(uint reloadValue, int reloadType) {
	if (reloadType)
			QEILOAD = (u32)((PCLK /(1000000/reloadValue)) - 1);
	else 	QEILOAD = reloadValue - 1;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Ajusta a posição máxima
// Parametro: maxPos: Máxima posição
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetMaxPosition(uint maxPos) {
	QEIMAXPOS = maxPos;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Ajusta a comparação de posição
// Parametro: posCompCh: Registrador de comparação
// 				QEI_COMPPOS_CH_0: Registrador de comparação de posição 0
// 				QEI_COMPPOS_CH_1: Registrador de comparação de posição 1
// 			 	QEI_COMPPOS_CH_2: Registrador de comparação de posição 2
//			  value: Valor para comparação
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetPositionComp(u8 posCompCh, u32 value) {
	//u32 *tmp = (reg32 *) (&(CMPOS0) + bPosCompCh * 4);
	reg32 *tmp = (reg32 *) (QEI_BASE_ADDR + 0x14 + (posCompCh * 4));
	*tmp = value;
}


// -------------------------------------------------------------------------------------------------------------------
// Descrição: Valor de comparação para o INDEX
// Parametro: value: Valor para comparação
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetIndexComp(uint value) {
	INXCMP = value;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Ajusta o valor de comparação para velocidade
// Parametro: value: Valor para comparação
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetVelocityComp(uint value) {
	VELCOMP = value;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Retorna com a contagem de acionamento do pino externo INDEX quando habilitado
// -------------------------------------------------------------------------------------------------------------------
uint qei_GetIndex(void) {
	return INXCNT;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Pega o valor atual do contador timer
// -------------------------------------------------------------------------------------------------------------------
uint qei_GetTimer(void) {
	return QEITIME;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Pega o valor da velocidade de pulso do momento
// -------------------------------------------------------------------------------------------------------------------
uint qei_GetVelocity(void) {
	return QEIVEL;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Retorna com o mais recente medição de velocidade. Quando haver uma overflow na velocidade do timer
//				a velocidade corrente será lido do registrador QEICAP
// -------------------------------------------------------------------------------------------------------------------
uint qei_GetVelocityCap(void) {
	return QEICAP;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Retorna com a direção
// -------------------------------------------------------------------------------------------------------------------
int qei_GetDirection(void) {
	return ((QEISTAT & 0x1)?1:0);
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Retorna com a posição atual do encoder. Esta posição é incrementada ou decrementada quadno as contagens
//				ocorrerem dependendo da direção
// -------------------------------------------------------------------------------------------------------------------
uint qei_GetPosition(void) {
	return QEIPOS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Calcula a velocidade atual em RPM através do valor da velocidade e o ppr (Pulse Per Round) do encoder
// Parametros: 	velCapValue: Velocidade média
//				ppr: Pulse Per Round do encoder. Quantidade de pulsos por volta * 4 por te 2 sinais contando em 4 bordas
// retorna: Volar em RPM
// -------------------------------------------------------------------------------------------------------------------
uint qei_CalculateRPM(uint velCapValue, uint ppr) {
	u32 rpm, load, edges;

	// Get Timer load value (velocity capture period)
	load  = (QEILOAD + 1);
	// Get Edge
	edges = ((QEICONF & QEI_CONF_CAPMODE) ? 4 : 2);
	// Calculate RPM
	rpm = (((PCLK) * velCapValue * 60) / (load * ppr * edges));

	return (uint)rpm;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Habilita as interrupções e aponta a função que será chamada
// Parametros: intFunc: ponteiro da função a ser chamada pelas interrupções.
//					Quando a interrupção for executada a função será chamada com parametro sinalizando quais interrupções foram geradas
//			   intType: determina as interrupções a serem habilitadas ou desabilitadas.
// 					QEI_INTFLAG_INX_Int: index pulse was detected interrupt
//					QEI_INTFLAG_TIM_Int: Velocity timer over flow interrupt
//					QEI_INTFLAG_VELC_Int: Capture velocity is less than compare interrupt
// 					QEI_INTFLAG_DIR_Int: Change of direction interrupt
//  				QEI_INTFLAG_ERR_Int: An encoder phase error interrupt
// 					QEI_INTFLAG_ENCLK_Int: An encoder clock pulse was detected interrupt
//					QEI_INTFLAG_POS0_Int: position 0 compare value is equal to the current position interrupt
//					QEI_INTFLAG_POS1_Int: position 1 compare value is equal to the
//												current position interrupt
//					QEI_INTFLAG_POS2_Int: position 2 compare value is equal to the
//														current position interrupt
//					QEI_INTFLAG_REV_Int: Index compare value is equal to the current
//														index count interrupt
//					QEI_INTFLAG_POS0REV_Int: Combined position 0 and revolution count interrupt
//					QEI_INTFLAG_POS1REV_Int: Combined position 1 and revolution count interrupt
//					QEI_INTFLAG_POS2REV_Int: Combined position 2 and revolution count interrupt
//				Se usar mais de uma por vez usar operador | >> Ex: (QEI_INTFLAG_INX_Int|QEI_INTFLAG_VELC_Int)
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetIntHandler(void (*intFunc) (int), uint intType) {
	qei_IntFunctions = intFunc;
	QEIIES = intType;
}

// Desabilita todas as interrupções
void qei_ClearIntHandler(void) {
	QEIIEC = QEI_IECLR_BITMASK; 	// Desliga todas as interrupções
	QEICLR = QEI_INTCLR_BITMASK; 	// Limpa as interrupções pendentes
	qei_IntFunctions = 0;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Checa se uma determinada interrupção foi gerada ou não
// Parametros: ints: valores de interrupções retirado de QEIINTSTAT
//					Quando a interrupção for executada a função será chamada com parametro sinalizando quais interrupções foram geradas
//			   intType: determina as interrupções a serem habilitadas ou desabilitadas.
// 					QEI_INTFLAG_INX_Int: index pulse was detected interrupt
//					QEI_INTFLAG_TIM_Int: Velocity timer over flow interrupt
//					QEI_INTFLAG_VELC_Int: Capture velocity is less than compare interrupt
// 					QEI_INTFLAG_DIR_Int: Change of direction interrupt
//  				QEI_INTFLAG_ERR_Int: An encoder phase error interrupt
// 					QEI_INTFLAG_ENCLK_Int: An encoder clock pulse was detected interrupt
//					QEI_INTFLAG_POS0_Int: position 0 compare value is equal to the current position interrupt
//					QEI_INTFLAG_POS1_Int: position 1 compare value is equal to the
//												current position interrupt
//					QEI_INTFLAG_POS2_Int: position 2 compare value is equal to the
//														current position interrupt
//					QEI_INTFLAG_REV_Int: Index compare value is equal to the current
//														index count interrupt
//					QEI_INTFLAG_POS0REV_Int: Combined position 0 and revolution count interrupt
//					QEI_INTFLAG_POS1REV_Int: Combined position 1 and revolution count interrupt
//					QEI_INTFLAG_POS2REV_Int: Combined position 2 and revolution count interrupt
// retorna: pdTRUE caso a interrupção foi gerada, senão retorna faso
// -------------------------------------------------------------------------------------------------------------------
int qei_GetIntStatus(uint ints, uint intType) {
	return((ints & intType) ? pdTRUE : pdFALSE);
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Limpa de forma orçada a interrupção em pedencia
// Parametro intType: determina as interrupções a ser limpa
// 					QEI_INTFLAG_INX_Int: index pulse was detected interrupt
//					QEI_INTFLAG_TIM_Int: Velocity timer over flow interrupt
//					QEI_INTFLAG_VELC_Int: Capture velocity is less than compare interrupt
// 					QEI_INTFLAG_DIR_Int: Change of direction interrupt
//  				QEI_INTFLAG_ERR_Int: An encoder phase error interrupt
// 					QEI_INTFLAG_ENCLK_Int: An encoder clock pulse was detected interrupt
//					QEI_INTFLAG_POS0_Int: position 0 compare value is equal to the current position interrupt
//					QEI_INTFLAG_POS1_Int: position 1 compare value is equal to the
//												current position interrupt
//					QEI_INTFLAG_POS2_Int: position 2 compare value is equal to the
//														current position interrupt
//					QEI_INTFLAG_REV_Int: Index compare value is equal to the current
//														index count interrupt
//					QEI_INTFLAG_POS0REV_Int: Combined position 0 and revolution count interrupt
//					QEI_INTFLAG_POS1REV_Int: Combined position 1 and revolution count interrupt
//					QEI_INTFLAG_POS2REV_Int: Combined position 2 and revolution count interrupt
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_IntClear(uint intType) {
	QEICLR = intType;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição: Liga de forma forçada uma interrupção
// Parametro intType: determina as interrupções a ser ligada
// 					QEI_INTFLAG_INX_Int: index pulse was detected interrupt
//					QEI_INTFLAG_TIM_Int: Velocity timer over flow interrupt
//					QEI_INTFLAG_VELC_Int: Capture velocity is less than compare interrupt
// 					QEI_INTFLAG_DIR_Int: Change of direction interrupt
//  				QEI_INTFLAG_ERR_Int: An encoder phase error interrupt
// 					QEI_INTFLAG_ENCLK_Int: An encoder clock pulse was detected interrupt
//					QEI_INTFLAG_POS0_Int: position 0 compare value is equal to the current position interrupt
//					QEI_INTFLAG_POS1_Int: position 1 compare value is equal to the
//												current position interrupt
//					QEI_INTFLAG_POS2_Int: position 2 compare value is equal to the
//														current position interrupt
//					QEI_INTFLAG_REV_Int: Index compare value is equal to the current
//														index count interrupt
//					QEI_INTFLAG_POS0REV_Int: Combined position 0 and revolution count interrupt
//					QEI_INTFLAG_POS1REV_Int: Combined position 1 and revolution count interrupt
//					QEI_INTFLAG_POS2REV_Int: Combined position 2 and revolution count interrupt
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_IntSet(uint intType) {
	QEISET = intType;
}


//###################################################################################################################
// SERVIÇO DE INTERRUPÇÕES
//###################################################################################################################
void qei_ISR (void) {
	ISR_ENTRY; // Procedimento para entrada da interrupção

	if (qei_IntFunctions) qei_IntFunctions(QEIINTSTAT);
	QEICLR = QEI_INTCLR_BITMASK; 	// Limpa as interrupções

	// Limpa interrupção
  	#if defined(arm7tdmi)
  	VICSoftIntClr = (1<<VIC_QEI);
	#endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_QEI);
	#endif

  	ISR_EXIT; // Procedimento para saida da interrupção
}
