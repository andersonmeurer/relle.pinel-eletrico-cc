/*
	ESTA BIBLIOTECA NÃO TRATA OS ERROS DE RECEPÇÃO PE, OE, FE E BI NAS INTERRUPÇÕES
	Logo as interrupções rx line (U0IER_ELSI) e as interrupções responsável pelo modem estão desativadas
	
	1 - SE NÃO USARMOS CONRTROLE DE FLUXO devemos levar em consideração o estouro da buffer
		de recepção para onde o ARM está trasmitindo
		se transmitirmos muitos dados em um baudrate elevado, o buffer do destino pode estourar e
		pode aperece que o problema seja nas nossas bibliotecas. então
		aumente o buffer rx do destino, ou aumente a prioridade de recepção
		ou diminuia o baudrate em uma taxa mais adequada.

	A interrupção de transmissão é gerada a cada 16 bytes transmitidos, ou seja, quando a FIFO TX estiver vazia.
	A interrupção de recepção é gerada, na pior das hipóteses, a cada byte recebido; isto porque podemos configurar
	o nível da FIFO RX (trigger) para que a interrupção seja gerada.

	Como a UART não é tão rápida assim e não há tanto fluxo de dados pela UART, as interrupções não são geradas
	subsequentemente dando tempo de sobra para outras funções da CPU.
*/

#include "uart2_irq.h"
#include "vic.h"	
#if defined(cortexm3)
#include "arm.h"
#endif

static volatile u8 u2_tx_running;					// Indicador se existe dados sendo transmitidos na UART
static volatile u16 u2_overflow_rx;					// Contador de estouro do buffer de recepção
	
// BUFFERS DE RECEPÇÃO E TRANSMISSÃO
static volatile buffer_t u2_buf_rx; 				// Cria uma estrutura do buffer de recepção
static volatile buffer_t u2_buf_tx;					// Cria uma estrutura do buffer de transmissão
	
#if (USE_MALLOC == pdON)
	static u8 *u2_data_rx;							// Ponteiro para alocação de memória para o buffer de recepção
	static u8 *u2_data_tx;							// Ponteiro para alocação de memória para o buffer de transmissão
#else
	static u8 u2_data_rx[UART2_RX_BUFFER_SIZE];		// Alocação de memória automática para o buffer de recepção
	static u8 u2_data_tx[UART2_TX_BUFFER_SIZE];		// Alocação de memória automática para o buffer de transmissão
#endif
		
void uart2_ISR(void);

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa e configura o modo de trabalho da UART
// Parametros:	baudrate: 
//					1200, 2400, ... , 57600, 115200 ...
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna código de erro
// -------------------------------------------------------------------------------------------------------------------
int uart2_Init(u32 baudrate) {
	u32 f_div;
	
  	PCONP |= PCUART2;											// Ligar a uart2 no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões
  	
	#if defined(arm7tdmi)
  	U2_PINSEL = (U2_PINSEL & ~U2_PINMASK) | U2_PINS;			// Configura os pinos do uC para a função UART. 
	#endif // arm7tdmi

  	#if defined(cortexm3)
  	uart2_ConfigPort();
	#endif

	#if (UART2_USE_HANDSHAKING == pdON)
	// Por padrão esses pinos já são GPIO
	U2_CTS_DIR &= ~U2_CTS; // Pino CTS como entrada
  	U2_RTS_DIR |= U2_RTS; // Pino RTS como saída
  	uart2_RTSon(); // Sinaliza que posso receber dados
  	#endif

  	U2IER = 0;   	                      						// Desabilita todas as fonte de interrupções da uart
  	U2IIR;                                						// Limpa os identificadores de pendencia interrupções da uart
  	U2FCR = (U2FCR_TX_FIFO_RESET | U2FCR_RX_FIFO_RESET);		// Limpa TX e RX FIFOS
  	
  	U2LCR = U2LCR_DLAB_ENABLE;             						// Seleciona os latches de devisões para ajusta do baudrate
  	f_div = ( PCLK / 16 ) / baudrate ;							// Calcular divisor para o baudrate
  	U2DLL = f_div%256;              							// Ajusta o baudrate byte baixo
  	U2DLM = f_div/256;       									// Ajusta o baudrate byte alto
  	U2LCR = (UART2_MODE & ~U2LCR_DLAB_ENABLE);					// Configura o modo de trabalho da uart e desativa o acesso aos registradores do baudrate
  	
  	U2FCR = UART2_FIFOMODE;										// Configura o modo de trabalho da FIFO

	U2RBR;                                						// Limpa o registrador de recepçao de dados
  	U2LSR;                                						// Limpa o registrador de estado de linha
	U2FCR = U2FCR_RX_FIFO_RESET;								// Limpa a FIFO de recepção
  	U2FCR = U2FCR_TX_FIFO_RESET;          						// Limpa a FIFO de transmissão

	#if defined(arm7tdmi)
  	if (!irq_Install(VIC_UART2, uart2_ISR, UART2_PRIO_LEVEL))
		return errIRQ_INSTALL;
	#endif

	#if defined (cortexm3)
	nvic_enableIRQ(VIC_UART2);
	nvic_setPriority(VIC_UART2, UART2_PRIO_LEVEL);
	#endif

	// INICIALIZO OS BUFFERS DE RECEPÇÃO E TRANSMISSÃO
	#if (USE_MALLOC == pdON)
	u2_data_rx = malloc(UART2_RX_BUFFER_SIZE*sizeof(u8));
	u2_data_tx = malloc(UART2_TX_BUFFER_SIZE*sizeof(u8));
	#endif

	buffer_Init(&u2_buf_rx, u2_data_rx, UART2_RX_BUFFER_SIZE);
	buffer_Init(&u2_buf_tx, u2_data_tx, UART2_TX_BUFFER_SIZE);
		
	u2_overflow_rx = 0; 										// Limpa overflow do buffer de recepção
	u2_tx_running = pdFALSE;
		
	U2IER |= U2IER_ERBFI;										// Habilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
	U2IER &= ~U2IER_ETBEI;                						// Desabilita a interrupção de buffer vazio de transmissão
		
	return pdPASS;
}

//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Captura o próximo byte recebido na UART
// Parametros: 	Ponteiro para da variavel char para o retorno do byte recebido
// Retorna:		pdPASS indicando que existe caractere recebido ou o código do erro
// -------------------------------------------------------------------------------------------------------------------
int uart2_GetChar(u8 *ch) {
	int ret;

	if ( u2_buf_rx.size == 0 )								// Checa se o buffer de recepção ainda não foi criado
   		return errBUFFER_NO_EXIST;            				// Retorna indicando erro

	if ( u2_buf_rx.datalength ) {							// Checo de o buffer de recepção contêm dados recebidos pela UART
		U2IER &= ~U2IER_ERBFI;								// Desabilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
		nop();
		*ch = buffer_GetFromFront(&u2_buf_rx);				// Captura os dados do inicio do buffer
		U2IER |= U2IER_ERBFI;								// Habilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
		ret = pdPASS;
	} else
		ret = errBUFFER_EMPTY;

	return ret;
}


// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa o buffer e FIFO de recepção
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart2_ClearBufferRx(void) {
	U2IER &= ~U2IER_ERBFI;				// Desabilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
	nop();
	u2_overflow_rx = 0; 				// Limpa overflow do buffer de recepção
	U2FCR = U2FCR_RX_FIFO_RESET;		// Limpa RX FIFO
	buffer_Clear(&u2_buf_rx);			// Limpa o buffer de recepção
	U2IER |= U2IER_ERBFI;				// Habilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna a quantidade de bytes do estouro do buffer de recepção
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
u32 uart2_CountRxOverflow (void) {
	u32 count = u2_overflow_rx;
	u2_overflow_rx = 0;
	return count;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Checa se buffer de recepção está vazio
// Parametros:	Nenhum
// Retorna:		pdTRUE se o buffer estiver vazio
//				pdFALSE se o buffer contiver dados recebidos
// -----------------------------------------------------------------------------------------------------------------
int uart2_BufferRxIsEmpty(void) {
	if (u2_buf_rx.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com a quantidade de bytes recebidos no buffer de recepção
// Parametros:	Nenhum
// Retorna:		Retorna com a quantidade de bytes recebidos no buffer de recepção
// -----------------------------------------------------------------------------------------------------------------
int uart2_BufferQtdRx(void) {
	return u2_buf_rx.datalength;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o ponteiro da estrutura do buffer de recepção
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de recepção
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart2_GetBufferRx(void) {
	return &u2_buf_rx;
}

//###################################################################################################################
// UART TRANSMISSAO
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Coloca um caractere na fila de transmissão
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o código de erro
// -------------------------------------------------------------------------------------------------------------------
int uart2_PutChar(n16 ch) {
	int sts;

	if ( u2_buf_tx.size == 0 )												// Checa se o buffer de transmissão ainda não foi criado
   		return errBUFFER_NO_EXIST;                          				// Retorna indicando erro

	// É obrigatório incluirmos o char no buffer, mesmo que vamos retirá-lo logo em seguida caso u3_tx_running for FALSE
	//	Se não for dessa forma o e usar U3THR = ch haverá erro de sincronismo entre os dados chegas e dados enviados.
	// Isto porque o manipulador de interrupção (uart3_ISR) pode atender a mais de uma interrupção simulataneas

	do {
		U2IER &= ~U2IER_ETBEI;                 								// Desativa a interrupção de transmissão
		nop();
		sts = buffer_AddToEnd(&u2_buf_tx, ch);
		U2IER |= U2IER_ETBEI;                  								// Habilita a interrupção de transmissão

		CRITICAL_WAIT;														// Caso exista algum OS, pode trocar de contexto
	} while (sts == errBUFFER_FULL);

	if (!u2_tx_running) {													// Checa se existe dado sendo transmitido
  		U2THR = buffer_GetFromFront(&u2_buf_tx);							// Atribui o dado do buffer ao registrador de transmissão
  		u2_tx_running = pdTRUE;												// Sinaliza que existe dado sendo trasnmitido
  		U2IER |= U2IER_ETBEI;                  								// Habilita a interrupção de transmissão
   	}

  	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Coloca um caractere na fila de transmissão. FUNÇÃO UTIL PARA DEBUG ONDE NÃO PRECISA INICIAR IRQ ou OS
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o código de erro
// -------------------------------------------------------------------------------------------------------------------
int uart2_BasicPutChar(n16 ch) {
  	while (!(U2LSR & U2LSR_THRE)) 								// Espero até que o buffer TX fique vazio  	 		
  		CRITICAL_WAIT;        									// Caso exista algum OS, pode trocar de contexto
  	U2THR = (u8)ch;												// Atualiza o registrador de TX uart com o dado
	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Captura o espaço disponivel para transmissão
// Parametros: 	Nenhum
// Retorna:		O tamanho em bytes do espaço disponivel para transmissão
// -------------------------------------------------------------------------------------------------------------------
u16 uart2_SpaceTx(void) {
	u16 qtd;
	
	U2IER &= ~U2IER_ETBEI;                 								// Desativa a interrupção de transmissão
	nop();
	qtd = (u16)(u2_buf_tx.size - u2_buf_tx.datalength);		// Retorna com o espaço disponivel	
	U2IER |= U2IER_ETBEI;                  								// Habilita a interrupção de transmissão
	return qtd;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Remove todos os dados da fila de transmissão
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void uart2_FlushTx(void) {
	U2IER &= ~U2IER_ETBEI;                					// Desabilita a interrupção de transmissão
	nop();
  	U2FCR = U0FCR_TX_FIFO_RESET;          					// Limpa a FIFO de transmissão
	buffer_Clear(&u2_buf_tx);								// Limpa os indeces do buffer TX da UART
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Retorna o status do registrador de transmissão de dados
// Parametros: 	Nenhum
// Retorna:		pdFALSE = Se o registradores U0THR ou U0TSR não estão vazios
//				pdTRUE = Se o registradores U0THR ou U0TSR estão vazios
// -------------------------------------------------------------------------------------------------------------------
int uart2_EmptyTx(void) {
  	if ((U2LSR & (U2LSR_THRE | U2LSR_TEMT)) == (U2LSR_THRE | U2LSR_TEMT))
  			return pdTRUE;
  	else	return pdFALSE;
}

#if !defined(FREE_RTOS)
// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Adiociona N caracteres no buffer de transmissão
// Parametros: 	Ponteiro do buffer 
//				Quantidade de caracteres as serem transmitidos
// Retorna:		pdPASS = Transmitido com sucesso ou o código do erro
// -------------------------------------------------------------------------------------------------------------------
int uart2_WriteTx(u8 *buffer, u16 count) {
	if (count > uart2_SpaceTx())									// Checa se exste espaço suficiente no buffer de TX de espaço insuficiente
   		return errBUFFER_FULL;										// Retorna cheio
  	
  	while (count && (uart2_PutChar(*buffer++) >= 0))				// Faça enquanto tem dados a serem transmitidos
    	count--;

  	return (count ? errBUFFER_FULL : pdPASS);						// Se count = 0 retorna 0 (Sucesso) senão retorna -2 (erro)
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Transmite uma string até que encontre o caractere NULO '\0' ou que o buffer estoura
//				ATENÇÂO: o Compilador já adiciona \0 na string do tipo "xyz" EX: "\r\r\nHello World!\r\r\n" 
// Parametros: 	Ponteiro da string
// Retorna:		O ponteiro da string. 
//					Retorna NULO se a string foi completamente transmitindo 
//					Ou retorna o ponteiro do próximo caractere a ser transmitido
// OBS: Se a string for maior que o espaço disponivel no buffer de TX pode ocorrer que a string não seja toda transmitida
// -------------------------------------------------------------------------------------------------------------------
u8 *uart2_PutString(u8 *str) {
   	register char ch;

  	while ((ch = *str) && (uart2_PutChar(ch) >= 0))				// Adiciona para transmissão enquanto tenha dados para transmitir
    	str++;

  	return str;													// Retorna o ponteiro atual da string
}
#endif // FREE_RTOS

//###################################################################################################################
// UART SERVIÇO DE INTERRUPÇÕES
// 		Descrição: 	Esta função implementa o ISR da UART2
//###################################################################################################################
//###################################################################################################################
void uart2_ISR (void) {
	ISR_ENTRY;																// Procedimento para entrada da interrupção

  	u8 int_flags;

  	while ( !((int_flags = U2IIR) & U2IIR_NO_INT) ) {							// Faça para todos os pedidos de interrupção pendentes emitidos pela UART
    	switch (int_flags & U2IIR_ID_MASK) {									// Identifica a fonte de interrupção
      	case U2IIR_RLS_INT:                									// Caso recebeu uma INT de erro na linha de recepção (receive line status)
        	U2LSR;                         									// Limpa registrador de status de linha
        	break;

        // ADICIONA NO BUFFER RX OS DADOS DA FIFO
        // No modo de trigger acima de 1 byte, se a quantidade de bytes da FIFO de recepção
        	// não alcançar o nível do trigger os dados da FIFO serão adicionados
        	// ao buffer de recepção pela interrupção de timerout da UART (U3IIR_CTI_INT)
      	case U2IIR_CTI_INT:                									// Character Timeout Indicator. Dados na FIFO RX que não foram lidos e está abaixo do nível de gatilho de int
      	case U2IIR_RDA_INT:                									// Caso que exista bytes disponivel recepção
     		do { 
      			if (buffer_AddToEnd(&u2_buf_rx, U2RBR) == errBUFFER_FULL)
					u2_overflow_rx++;
      		} while(U2LSR & U2LSR_RDR);										// Faça enquanto existe dados validos na FIFO de recepção
			
			#if (UART2_USE_HANDSHAKING == pdON) // AINDA NÃO TESTADO
     		if (UART2_RX_BUFFER_SIZE - u2_buf_rx.datalength > UART2_SIZE_FIFO)
     			 uart2_RTSon(); // Sinaliza que posso receber mais dados
     		else uart2_RTSoff(); // Sinaliza que não posso receber mais dados
  			#endif

			break;

     	// RETIRA OS DADOS DO BUFFER TX PARA A FIFO DE TRANSMISSÃO
     		// Essa interrupção é gerada quando a FIFO de transmissão de 16 bytes está vazia
			// O registrador UxTHR é o topo da FIFO de 16 bytes, ou seja, é o endereço da última posição a ser inserida da FIFO
   		case U2IIR_THRE_INT:              									// Interrupção da FIFO de 16 bytes quando está vazia
      		while (U2LSR & U2LSR_THRE) {									// Faça enquanto há espaço na FIFO
       			if (buffer_IsEmpty(&u2_buf_tx) ) {						 	// Checa se o buffer de transmissão já está vazio
       				u2_tx_running = pdFALSE;     							// Sinaliza que não existe dados para trasnmissao
       				U2IER &= ~U2IER_ETBEI;                					// Desativa a interrupção de transmissão
          			break;
       			} else {
					#if (UART2_USE_HANDSHAKING == pdON) // AINDA NÃO TESTADO
	    			if (uart2_CTS()) // Checa se o receptor esteja pronto para eviar dados
					#endif
	       				U2THR = buffer_GetFromFront(&u2_buf_tx);			// Atribui o dado do buffer ao registrador de transmissão
	       		}
       		}
       		
       		break;

  		default:															// Interrupção desconhecida
       		U2LSR;															// Limpa registrador de status de linha
       		U2RBR;															// Limpa o registrador de recepçao de dados
       		break;
      	}
	}
  	
	// Limpa interrupção
  	#if defined(arm7tdmi)
  	VICSoftIntClr = (1<<VIC_UART2);
	#endif
	#if defined (cortexm3)
  	nvic_clearPendingIRQ(VIC_UART2);
	#endif

  	ISR_EXIT;																// Procedimento para saida da interrupção
}

