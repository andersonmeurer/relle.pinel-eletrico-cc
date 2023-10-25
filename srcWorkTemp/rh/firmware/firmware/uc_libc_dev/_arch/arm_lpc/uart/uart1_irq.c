/*
	ESTA BIBLIOTECA NÃO TRATA OS ERROS DE RECEPÇÃO PE, OE, FE E BI NAS INTERRUPÇÕES
	Logo as interrupções rx line (U0IER_ELSI) e as interrupções responsável pelo modem estão desativadas
	
	1 - SE NÃO USARMOS CONRTROLE DE FLUXO devemos levar em consideração o estouro da buffer
		de recepção para onde o ARM está trasmitindo
		se transmitirmos muitos dados em um baudrate elevado, o buffer do destino pode estourar e
		pode aperece que o problema seja nas nossas bibliotecas. então
		aumente o buffer rx do destino, ou aumente a prioridade de recepção
		ou diminuia o baudrate em uma taxa mais adequada

	A interrupção de transmissão é gerada a cada 16 bytes transmitidos, ou seja, quando a FIFO TX estiver vazia.
	A interrupção de recepção é gerada, na pior das hipóteses, a cada byte recebido; isto porque podemos configurar
	o nível da FIFO RX (trigger) para que a interrupção seja gerada.

	Como a UART não é tão rápida assim e não há tanto fluxo de dados pela UART, as interrupções não são geradas
	subsequentemente dando tempo de sobra para outras funções da CPU.
*/

#include "uart1_irq.h"
#include "vic.h"	
#if defined(cortexm3)
#include "arm.h"
#endif

static volatile u8 u1_tx_running;						// Indicador se existe dados sendo transmitidos na UART
static volatile u16 u1_overflow_rx;						// Contador de estouro do buffer de recepção
	
// BUFFERS DE RECEPÇÃO E TRANSMISSÃO
static volatile buffer_t u1_buf_rx; 					// Cria uma estrutura do buffer de recepção
static volatile buffer_t u1_buf_tx;						// Cria uma estrutura do buffer de transmissão
	
#if (USE_MALLOC == pdON)
	static u8 *u1_data_rx;								// Ponteiro para alocação de memória para o buffer de recepção
	static u8 *u1_data_tx;								// Ponteiro para alocação de memória para o buffer de transmissão
#else
	static u8 u1_data_rx[UART1_RX_BUFFER_SIZE];			// Alocação de memória automática para o buffer de recepção
	static u8 u1_data_tx[UART1_TX_BUFFER_SIZE];			// Alocação de memória automática para o buffer de transmissão
#endif
		
void uart1_ISR (void);


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
int uart1_Init(u32 baudrate) {
	u32 f_div;
	
  	PCONP |= PCUART1;											// Ligar a uart no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões
  	
	#if defined(arm7tdmi)
  	U1_PINSEL = (U1_PINSEL & ~U1_PINMASK) | U1_PINS;			// Configura os pinos do uC para a função UART. 
  	#if defined(LPC2468)
  		U1_PINSEL1 = (U1_PINSEL1 & ~U1_PINMASK1) | U1_PINS1;	// Configura os pinos do uC para a função UART.
  	#endif
	#endif // arm7tdmi

  	#if defined(cortexm3)
	uart1_ConfigPort();
	#endif

	#if (UART1_USE_HANDSHAKING == pdON)
	// Por padrão esses pinos já são GPIO
	U1_CTS_DIR &= ~U1_CTS; 				// Pino CTS como entrada
	U1_RTS_DIR |= U1_RTS; 				// Pino RTS como saída
	uart1_RTSon(); 						// Sinaliza que posso receber dados
	#endif

  	U1IER = 0;   	                      						// Desabilita todas as fonte de interrupções da uart
  	U1IIR;                                						// Limpa os identificadores de pendencia interrupções da uart
  	U1FCR = (U1FCR_TX_FIFO_RESET | U1FCR_RX_FIFO_RESET);		// Limpa TX e RX FIFOS
  	
  	U1LCR = U1LCR_DLAB_ENABLE;             						// Seleciona os latches de devisões para ajusta do baudrate
  	f_div = ( PCLK / 16 ) / baudrate ;							// Calcular divisor para o baudrate
  	U1DLL = f_div%256;              							// Ajusta o baudrate byte baixo
  	U1DLM = f_div/256;       									// Ajusta o baudrate byte alto
  	U1LCR = (UART1_MODE & ~U1LCR_DLAB_ENABLE);					// Configura o modo de trabalho da uart e desativa o acesso aos registradores do baudrate
  	
  	U1FCR = UART1_FIFOMODE;										// Configura o modo de trabalho da FIFO

	U1RBR;                                						// Limpa o registrador de recepçao de dados
  	U1LSR;                                						// Limpa o registrador de estado de linha
	U1FCR = U1FCR_RX_FIFO_RESET;								// Limpa a FIFO de recepção
  	U1FCR = U1FCR_TX_FIFO_RESET;          						// Limpa a FIFO de transmissão

	#if defined(arm7tdmi)
  	if (!irq_Install(VIC_UART1, uart1_ISR, UART1_PRIO_LEVEL))
		return errIRQ_INSTALL;
	#endif

	#if defined (cortexm3)
	nvic_enableIRQ(VIC_UART1);
	nvic_setPriority(VIC_UART1, UART1_PRIO_LEVEL);
	#endif

	// INICIALIZO OS BUFFERS DE RECEPÇÃO E TRANSMISSÃO
	#if (USE_MALLOC == pdON)
	u1_data_rx = malloc(UART1_RX_BUFFER_SIZE*sizeof(u8));
	u1_data_tx = malloc(UART1_TX_BUFFER_SIZE*sizeof(u8));
	#endif

	buffer_Init(&u1_buf_rx, u1_data_rx, UART1_RX_BUFFER_SIZE);
	buffer_Init(&u1_buf_tx, u1_data_tx, UART1_TX_BUFFER_SIZE);
		
	u1_overflow_rx = 0; 										// Limpa overflow do buffer de recepção
	u1_tx_running = pdFALSE;
		
	U1IER |= U1IER_ERBFI;										// Habilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
	U1IER &= ~U1IER_ETBEI;               						// Desabilita a interrupção de buffer vazio de transmissão
	
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
int uart1_GetChar(u8 *ch) {
	int ret;

	if ( u1_buf_rx.size == 0 )								// Checa se o buffer de recepção ainda não foi criado
   		return errBUFFER_NO_EXIST;            				// Retorna indicando erro

	if ( u1_buf_rx.datalength ) {							// Checo de o buffer de recepção contêm dados recebidos pela UART
		U1IER &= ~U1IER_ERBFI;								// Desabilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
		nop();
		*ch = buffer_GetFromFront(&u1_buf_rx);				// Captura os dados do inicio do buffer
		U1IER |= U1IER_ERBFI;								// Habilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
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
void uart1_ClearBufferRx(void) {
	U1IER &= ~U1IER_ERBFI;				// Desabilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
	nop();
	u1_overflow_rx = 0; 				// Limpa overflow do buffer de recepção
	U1FCR = U1FCR_RX_FIFO_RESET;		// Limpa RX FIFO
	buffer_Clear(&u1_buf_rx);			// Limpa o buffer de recepção
	U1IER |= U1IER_ERBFI;				// Habilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna a quantidade de bytes do estouro do buffer de recepção
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
u32 uart1_CountRxOverflow (void) {
	u32 count = u1_overflow_rx;
	u1_overflow_rx = 0;
	return count;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Checa se buffer de recepção está vazio
// Parametros:	Nenhum
// Retorna:		pdTRUE se o buffer estiver vazio
//				pdFALSE se o buffer contiver dados recebidos
// -----------------------------------------------------------------------------------------------------------------
int uart1_BufferRxIsEmpty(void) {
	if (u1_buf_rx.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com a quantidade de bytes recebidos no buffer de recepção
// Parametros:	Nenhum
// Retorna:		Retorna com a quantidade de bytes recebidos no buffer de recepção
// -----------------------------------------------------------------------------------------------------------------
int uart1_BufferQtdRx(void) {
	return u1_buf_rx.datalength;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o ponteiro da estrutura do buffer de recepção
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de recepção
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart1_GetBufferRx(void) {
	return &u1_buf_rx;
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
int uart1_PutChar(n16 ch) {
	int sts;

	if ( u1_buf_tx.size == 0 )												// Checa se o buffer de transmissão ainda não foi criado
   		return errBUFFER_NO_EXIST;                          				// Retorna indicando erro

	// É obrigatório incluirmos o char no buffer, mesmo que vamos retirá-lo logo em seguida caso u3_tx_running for FALSE
	//	Se não for dessa forma o e usar U3THR = ch haverá erro de sincronismo entre os dados chegas e dados enviados.
	// Isto porque o manipulador de interrupção (uart3_ISR) pode atender a mais de uma interrupção simulataneas

	do {
		U1IER &= ~U1IER_ETBEI;                 								// Desativa a interrupção de transmissão
		nop();
		sts = buffer_AddToEnd(&u1_buf_tx, ch);
		U1IER |= U1IER_ETBEI;                  								// Habilita a interrupção de transmissão

		CRITICAL_WAIT;														// Caso exista algum OS, pode trocar de contexto
	} while (sts == errBUFFER_FULL);

	if (!u1_tx_running) {													// Checa se existe dado sendo transmitido
  		U1THR = buffer_GetFromFront(&u1_buf_tx);							// Atribui o dado do buffer ao registrador de transmissão
  		u1_tx_running = pdTRUE;												// Sinaliza que existe dado sendo trasnmitido
  		U1IER |= U1IER_ETBEI;                  								// Habilita a interrupção de transmissão
   	}

  	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Coloca um caractere na fila de transmissão. FUNÇÃO UTIL PARA DEBUG ONDE NÃO PRECISA INICIAR IRQ ou OS
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o código de erro
// -------------------------------------------------------------------------------------------------------------------
int uart1_BasicPutChar(n16 ch) {
  	while (!(U1LSR & U1LSR_THRE)) 								// Espero até que o buffer TX fique vazio
  		CRITICAL_WAIT;        									// Caso exista algum OS, pode trocar de contexto
  	U1THR = (u8)ch;												// Atualiza o registrador de TX uart com o dado
	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Captura o espaço disponivel para transmissão
// Parametros: 	Nenhum
// Retorna:		O tamanho em bytes do espaço disponivel para transmissão
// -------------------------------------------------------------------------------------------------------------------
u16 uart1_SpaceTx(void) {
	u16 qtd;
	
	U1IER &= ~U1IER_ETBEI;                 								// Desativa a interrupção de transmissão
	nop();
	qtd = (u16)(u1_buf_tx.size - u1_buf_tx.datalength);					// Retorna com o espaço disponivel
	U1IER |= U1IER_ETBEI;                  								// Habilita a interrupção de transmissão
	return qtd;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Remove todos os dados da fila de transmissão
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void uart1_FlushTx(void) {
	U1IER &= ~U1IER_ETBEI;                					// Desabilita a interrupção de transmissão
  	nop();
  	U1FCR = U1FCR_TX_FIFO_RESET;          					// Limpa a FIFO de transmissão
	buffer_Clear(&u1_buf_tx);								// Limpa os indeces do buffer TX da UART
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Retorna o status do registrador de transmissão de dados
// Parametros: 	Nenhum
// Retorna:		pdFALSE = Se o registradores U0THR ou U0TSR não estão vazios
//				pdTRUE = Se o registradores U0THR ou U0TSR estão vazios
// -------------------------------------------------------------------------------------------------------------------
int uart1_EmptyTx(void) {
  	if ((U1LSR & (U1LSR_THRE | U1LSR_TEMT)) == (U1LSR_THRE | U1LSR_TEMT))
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
int uart1_WriteTx(u8 *buffer, u16 count) {
	if (count > uart1_SpaceTx())									// Checa se exste espaço suficiente no buffer de TX de espaço insuficiente
   		return errBUFFER_FULL;										// Retorna cheio
  	
  	while (count && (uart1_PutChar(*buffer++) >= 0))				// Faça enquanto tem dados a serem transmitidos
    	count--;

  	return (count ? errBUFFER_FULL : pdPASS); 						// Se count = 0 retorna 0 (Sucesso) senão retorna -2 (erro)
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
u8 *uart1_PutString(u8 *str) {
   	register char ch;

  	while ((ch = *str) && (uart1_PutChar(ch) >= 0))				// Adiciona para transmissão enquanto tenha dados para transmitir
    	str++;

  	return str;													// Retorna o ponteiro atual da string
}
#endif // FREE_RTOS

//###################################################################################################################
// UART SERVIÇO DE INTERRUPÇÕES
// 		Descrição: 	Esta função implementa o ISR da UART1
//###################################################################################################################
//###################################################################################################################
void uart1_ISR (void) {
	ISR_ENTRY;																// Procedimento para entrada da interrupção

  	u8 int_flags;

  	while ( !((int_flags = U1IIR) & U1IIR_NO_INT) ) {							// Faça para todos os pedidos de interrupção pendentes emitidos pela UART
    	switch (int_flags & U1IIR_ID_MASK) {									// Identifica a fonte de interrupção
    	case U1IIR_RLS_INT:                									// Caso recebeu uma INT de erro na linha de recepção (receive line status)
        	U1LSR;                         									// Limpa registrador de status de linha
        	break;

        // ADICIONA NO BUFFER RX OS DADOS DA FIFO
        // No modo de trigger acima de 1 byte, se a quantidade de bytes da FIFO de recepção
        	// não alcançar o nível do trigger os dados da FIFO serão adicionados
        	// ao buffer de recepção pela interrupção de timerout da UART (U3IIR_CTI_INT)
      	case U1IIR_CTI_INT:                									// Character Timeout Indicator. Dados na FIFO RX que não foram lidos e está abaixo do nível de gatilho de int
      	case U1IIR_RDA_INT:                									// Caso que exista bytes disponivel recepção
     		do { 
      			if (buffer_AddToEnd(&u1_buf_rx, U1RBR) == errBUFFER_FULL)
					u1_overflow_rx++;
      		} while(U1LSR & U1LSR_RDR);										// Faça enquanto existe dados validos na FIFO de recepção

			#if (UART1_USE_HANDSHAKING == pdON) // AINDA NÃO TESTADO
     		if (UART1_RX_BUFFER_SIZE - u1_buf_rx.datalength > UART1_SIZE_FIFO)
     			 uart1_RTSon(); // Sinaliza que posso receber mais dados
     		else uart1_RTSoff(); // Sinaliza que não posso receber mais dados
  			#endif

			break;

     	// RETIRA OS DADOS DO BUFFER TX PARA A FIFO DE TRANSMISSÃO
     		// Essa interrupção é gerada quando a FIFO de transmissão de 16 bytes está vazia
			// O registrador UxTHR é o topo da FIFO de 16 bytes, ou seja, é o endereço da última posição a ser inserida da FIFO
      	case U1IIR_THRE_INT:              									// Interrupção da FIFO de 16 bytes quando está vazia
        	while (U1LSR & U1LSR_THRE) {									// Faça enquanto há espaço na FIFO
        		if (buffer_IsEmpty(&u1_buf_tx) ) {						 	// Checa se o buffer de transmissão já está vazio
        			u1_tx_running = pdFALSE;   								// Sinaliza que não existe dados para trasnmissao
        			U1IER &= ~U1IER_ETBEI;                					// Desativa a interrupção de transmissão
         			break;
        		} else {
					#if (UART1_USE_HANDSHAKING == pdON) // AINDA NÃO TESTADO
	    			if (uart1_CTS()) // Checa se o receptor esteja pronto para eviar dados
					#endif     			
        				U1THR = buffer_GetFromFront(&u1_buf_tx);			// Atribui o dado do buffer ao registrador de transmissão
        		}
        	}
 
        	break;

      	case U1IIR_MS_INT:                 									// MODEM Status
        	U1MSR;                          								// Limpa o registrador de status do modem
        	break;

      	default:															// Interrupção desconhecida
        	U1LSR;															// Limpa registrador de status de linha
        	U1RBR;															// Limpa o registrador de recepçao de dados
        	U1MSR;															// Limpa o registrador de status do modem
        	break;
      	}
	}
  	
  	// Limpa interrupção
  	#if defined(arm7tdmi)
  	VICSoftIntClr = (1<<VIC_UART1);
	#endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_UART1);
	#endif

  	ISR_EXIT;																// Procedimento para saida da interrupção
}

