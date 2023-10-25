#include "qei.h"
#include "vic.h"
#if defined(cortexm3)
#include "arm.h"
#endif

// Quadrature Encoder Interface (QEI)

// Biblioteca escrita em 2016-03-31

// Esta lib utiliza os pinos externos PHA e PHB para controle de um encoder
// As interrup��es s�o usadas para:
// - Sinalizar a ativa��o do pino externo index, este pino � usado para zerar os contadores
// - Sinalizar overflow na velocidade
// - Sinalizar mudan�as de dire��o
// - Sinalizar outras situa��es de compara��es e clocks

typedef void (*qei_pFunc_t)(int);
static volatile qei_pFunc_t qei_IntFunctions;
void qei_ISR (void);

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Inicializa o perif�rico qei com configura��es padr�es e sem interru��es. Configura��es padr�es:
//				QEI_DIRINV_NONE: A dire��o QEI � normal
//				QEI_SIGNALMODE_QUAD: Modo do sinal por fase
//				QEI_CAPMODE_4X: Modo de captura utilizando as bordas dos sinais PhA and PhB para contagem (4X)
//				QEI_INVINX_NONE: Sem invers�o do sinal INDEX
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_Init(void) {
	PCONP |= PCQEI; // Ligar hardware QEI
	qei_ConfigPort(); // Ajusta os pinos do ARM assunir fun��o de qei

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

	QEIIEC = QEI_IECLR_BITMASK; 	// Desliga todas as interrup��es
	QEICLR = QEI_INTCLR_BITMASK; 	// Limpa as interrup��es pendentes
	QEICONF = QEI_INVINX_NONE | QEI_CAPMODE_4X | QEI_SIGNALMODE_QUAD | QEI_DIRINV_NONE; // Set QEI configuration
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Reconfigura o QEI:
// Parametro: 	(QEI_DIRINV_CMPL ou QEI_DIRINV_NONE) |
//				(QEI_SIGNALMODE_CLKDIR ou QEI_SIGNALMODE_QUAD) |
//				(QEI_CAPMODE_2X ou QEI_CAPMODE_4X) |
//				(QEI_INVINX_NONE ou QEI_INVINX_EN)
//					QEI_DIRINV_CMPL: A dire��o QEI � complementado
//					QEI_DIRINV_NONE: A dire��o QEI � normal
//					QEI_SIGNALMODE_CLKDIR: Modo do sinal por clock/dire��o, onde PHA usado para determinar a dire��o e PHB para coletar o pulso
//					QEI_SIGNALMODE_QUAD: Modo do sinal por fase, usando PHA e PHB para detectar a fase
//					QEI_CAPMODE_2X: Modo de captura utilizando somente a brdas do sinal PhA para contagem (2X)
//					QEI_CAPMODE_4X: Modo de captura utilizando as bordas dos sinais PhA and PhB para contagem (4X)
//					QEI_INVINX_NONE: Sem invers�o do sinal INDEX
//					QEI_INVINX_EN: Com invers�o do sinal INDEX
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_Config(uint config) {
	QEICONF = config;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Reinicia os valores como velocidade, contadores, posi��o
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
// Descri��o: Aplica um filtro para coleta de amostras nas entradas PHA, PHB e INDEX.
//				O Filtro determina a quantidade	de clocks para requerer as amostras (debounce)
// Parametro: samplingPulse: Valor do filtro. Valor 0 n�o se utilza o filtro
//  		  reloadType: Determina o tipo de carregamento:
//					qeiVALUE_ABSOLUTE: Sinaliza que o registrador ser� atualizado com um valor absoluto
//					qeiVALUE_IN_US: Sinaliza que o registrador ser� atualizado com valor em micro segundos
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetFilter(uint samplingPulse, int reloadType) {
	if (reloadType)
			FILTER = (u32)( (PCLK) / (1000000UL/samplingPulse));
	else 	FILTER = samplingPulse;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Ajusta o valor de velocidade do timer. Quando haver um overflow no temporizador de velocidade,
//				o valor fixado no QEILOAD ser� carregado para o temporizador de velocidade
//				para o pr�ximo per�odo. A velocidade calculada em RPM, portanto, ser� afetado por esse valor.
// Parametro: reloadValue: Valor de carregamento para o registrador de velcodiade do timer (QEILOAD)
// retorna:  reloadType: Determina o tipo de carregamento:
//				qeiVALUE_ABSOLUTE: Sinaliza que o registrador ser� atualizado com um valor absoluto
//				qeiVALUE_IN_US: Sinaliza que o registrador ser� atualizado com valor em micro segundos
// -------------------------------------------------------------------------------------------------------------------
void qei_SetTimerReload(uint reloadValue, int reloadType) {
	if (reloadType)
			QEILOAD = (u32)((PCLK /(1000000/reloadValue)) - 1);
	else 	QEILOAD = reloadValue - 1;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Ajusta a posi��o m�xima
// Parametro: maxPos: M�xima posi��o
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetMaxPosition(uint maxPos) {
	QEIMAXPOS = maxPos;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Ajusta a compara��o de posi��o
// Parametro: posCompCh: Registrador de compara��o
// 				QEI_COMPPOS_CH_0: Registrador de compara��o de posi��o 0
// 				QEI_COMPPOS_CH_1: Registrador de compara��o de posi��o 1
// 			 	QEI_COMPPOS_CH_2: Registrador de compara��o de posi��o 2
//			  value: Valor para compara��o
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetPositionComp(u8 posCompCh, u32 value) {
	//u32 *tmp = (reg32 *) (&(CMPOS0) + bPosCompCh * 4);
	reg32 *tmp = (reg32 *) (QEI_BASE_ADDR + 0x14 + (posCompCh * 4));
	*tmp = value;
}


// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Valor de compara��o para o INDEX
// Parametro: value: Valor para compara��o
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetIndexComp(uint value) {
	INXCMP = value;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Ajusta o valor de compara��o para velocidade
// Parametro: value: Valor para compara��o
// retorna: nada
// -------------------------------------------------------------------------------------------------------------------
void qei_SetVelocityComp(uint value) {
	VELCOMP = value;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Retorna com a contagem de acionamento do pino externo INDEX quando habilitado
// -------------------------------------------------------------------------------------------------------------------
uint qei_GetIndex(void) {
	return INXCNT;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Pega o valor atual do contador timer
// -------------------------------------------------------------------------------------------------------------------
uint qei_GetTimer(void) {
	return QEITIME;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Pega o valor da velocidade de pulso do momento
// -------------------------------------------------------------------------------------------------------------------
uint qei_GetVelocity(void) {
	return QEIVEL;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Retorna com o mais recente medi��o de velocidade. Quando haver uma overflow na velocidade do timer
//				a velocidade corrente ser� lido do registrador QEICAP
// -------------------------------------------------------------------------------------------------------------------
uint qei_GetVelocityCap(void) {
	return QEICAP;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Retorna com a dire��o
// -------------------------------------------------------------------------------------------------------------------
int qei_GetDirection(void) {
	return ((QEISTAT & 0x1)?1:0);
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Retorna com a posi��o atual do encoder. Esta posi��o � incrementada ou decrementada quadno as contagens
//				ocorrerem dependendo da dire��o
// -------------------------------------------------------------------------------------------------------------------
uint qei_GetPosition(void) {
	return QEIPOS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Calcula a velocidade atual em RPM atrav�s do valor da velocidade e o ppr (Pulse Per Round) do encoder
// Parametros: 	velCapValue: Velocidade m�dia
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
// Descri��o: Habilita as interrup��es e aponta a fun��o que ser� chamada
// Parametros: intFunc: ponteiro da fun��o a ser chamada pelas interrup��es.
//					Quando a interrup��o for executada a fun��o ser� chamada com parametro sinalizando quais interrup��es foram geradas
//			   intType: determina as interrup��es a serem habilitadas ou desabilitadas.
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

// Desabilita todas as interrup��es
void qei_ClearIntHandler(void) {
	QEIIEC = QEI_IECLR_BITMASK; 	// Desliga todas as interrup��es
	QEICLR = QEI_INTCLR_BITMASK; 	// Limpa as interrup��es pendentes
	qei_IntFunctions = 0;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Checa se uma determinada interrup��o foi gerada ou n�o
// Parametros: ints: valores de interrup��es retirado de QEIINTSTAT
//					Quando a interrup��o for executada a fun��o ser� chamada com parametro sinalizando quais interrup��es foram geradas
//			   intType: determina as interrup��es a serem habilitadas ou desabilitadas.
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
// retorna: pdTRUE caso a interrup��o foi gerada, sen�o retorna faso
// -------------------------------------------------------------------------------------------------------------------
int qei_GetIntStatus(uint ints, uint intType) {
	return((ints & intType) ? pdTRUE : pdFALSE);
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: Limpa de forma or�ada a interrup��o em pedencia
// Parametro intType: determina as interrup��es a ser limpa
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
// Descri��o: Liga de forma for�ada uma interrup��o
// Parametro intType: determina as interrup��es a ser ligada
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
// SERVI�O DE INTERRUP��ES
//###################################################################################################################
void qei_ISR (void) {
	ISR_ENTRY; // Procedimento para entrada da interrup��o

	if (qei_IntFunctions) qei_IntFunctions(QEIINTSTAT);
	QEICLR = QEI_INTCLR_BITMASK; 	// Limpa as interrup��es

	// Limpa interrup��o
  	#if defined(arm7tdmi)
  	VICSoftIntClr = (1<<VIC_QEI);
	#endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_QEI);
	#endif

  	ISR_EXIT; // Procedimento para saida da interrup��o
}
