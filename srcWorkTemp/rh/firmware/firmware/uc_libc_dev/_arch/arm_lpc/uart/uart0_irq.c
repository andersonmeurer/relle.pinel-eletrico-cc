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

#include "uart0_irq.h"
#include "vic.h"	
#if defined(cortexm3)
#include "arm.h"
#endif

static volatile u8 u0_tx_running;						// Indicador se existe dados sendo transmitidos na UART
static volatile u16 u0_overflow_rx;						// Contador de estouro do buffer de recepção
	
// BUFFERS DE RECEPÇÃO E TRANSMISSÃO
static volatile buffer_t u0_buf_rx; 					// Cria uma estrutura do buffer de recepção
static volatile buffer_t u0_buf_tx;						// Cria uma estrutura do buffer de transmissão
	
#if (USE_MALLOC == pdON)
	static u8 *u0_data_rx;								// Ponteiro para alocação de memória para o buffer de recepção
	static u8 *u0_data_tx;								// Ponteiro para alocação de memória para o buffer de transmissão
#else
	static u8 u0_data_rx[UART0_RX_BUFFER_SIZE];			// Alocação de memória automática para o buffer de recepção
	static u8 u0_data_tx[UART0_TX_BUFFER_SIZE];			// Alocação de memória automática para o buffer de transmissão
#endif

void uart0_ISR (void);

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa e configura o modo de trabalho da UART
// Parametros:	baudrate: 
//					1200, 2400, ... , 57600, 115200 ...
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna código do erro
// -------------------------------------------------------------------------------------------------------------------
int uart0_Init(u32 baudrate) {
  	u32 f_div;
  	
  	PCONP |= PCUART0;											// Ligar a uart no controle de potencia. Tem que ser antes da configuração, pois após ligar o periféricos, seus regs assumem valores padrões
  	
	#if defined(arm7tdmi)
  	U0_PINSEL = (U0_PINSEL & ~U0_PINMASK) | U0_PINS;			// Configura os pinos do uC para a função UART. Pinos TX=P0_2 e RX=P0_3
	#endif //arm7tdmi

	#if defined(cortexm3)
  	uart0_ConfigPort();
	#endif

	#if (UART0_USE_HANDSHAKING == pdON)
	// Por padrão esses pinos já são GPIO
	U0_CTS_DIR &= ~U0_CTS; 				// Pino CTS como entrada
	U0_RTS_DIR |= U0_RTS; 				// Pino RTS como saída
	uart0_RTSon(); 						// Sinaliza que posso receber dados
  	#endif

  	U0IER = 0;   	                      						// Desabilita todas as fonte de interrupções da uart
  	U0IIR;                                						// Limpa os identificadores de pendencia interrupções da uart
	U0FCR = (U0FCR_TX_FIFO_RESET | U0FCR_RX_FIFO_RESET);		// Limpa TX e RX FIFOS
  	
  	U0LCR = U0LCR_DLAB_ENABLE;             						// Seleciona os latches de devisões para ajusta do baudrate
  	f_div = ( PCLK / 16 ) / baudrate ;							// Calcular divisor para o baudrate
  	U0DLL = f_div%256;              							// Ajusta o baudrate byte baixo
  	U0DLM = f_div/256;       									// Ajusta o baudrate byte alto
  	U0LCR = (UART0_MODE & ~U0LCR_DLAB_ENABLE);					// Configura o modo de trabalho da uart e desativa o acesso aos registradores do baudrate
  	
  	U0FCR = UART0_FIFOMODE;										// Configura o modo de trabalho da FIFO
  	
  	U0RBR;                                						// Limpa o registrador de recepçao de dados
  	U0LSR;                                						// Limpa o registrador de estado de linha
	U0FCR = U0FCR_RX_FIFO_RESET;								// Limpa a FIFO de recepção
  	U0FCR = U0FCR_TX_FIFO_RESET;          						// Limpa a FIFO de transmissão
	
	#if defined(arm7tdmi)
  	if (!irq_Install(VIC_UART0, uart0_ISR, UART0_PRIO_LEVEL))
		return errIRQ_INSTALL;
	#endif

	#if defined (cortexm3)
	nvic_enableIRQ(VIC_UART0);
	nvic_setPriority(VIC_UART0, UART0_PRIO_LEVEL);
	#endif

	// INICIALIZO OS BUFFERS DE RECEPÇÃO E TRANSMISSÃO
	#if (USE_MALLOC == pdON)
	u0_data_rx = malloc(UART0_RX_BUFFER_SIZE*sizeof(u8));
	u0_data_tx = malloc(UART0_TX_BUFFER_SIZE*sizeof(u8));
	#endif

	buffer_Init(&u0_buf_rx, u0_data_rx, UART0_RX_BUFFER_SIZE);
	buffer_Init(&u0_buf_tx, u0_data_tx, UART0_TX_BUFFER_SIZE);
		
	u0_overflow_rx = 0; 										// Limpa overflow do buffer de recepção
	u0_tx_running = pdFALSE;
	
	U0IER |= U0IER_ERBFI;										// Habilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
	U0IER &= ~U0IER_ETBEI;                						// Desabilita a interrupção de buffer vazio de transmissão

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
int uart0_GetChar(u8 *ch) {
	int ret;

	if ( u0_buf_rx.size == 0 )								// Checa se o buffer de recepção ainda não foi criado
   		return errBUFFER_NO_EXIST;            				// Retorna indicando erro

	if ( u0_buf_rx.datalength ) {							// Checo de o buffer de recepção contêm dados recebidos pela UART
		U0IER &= ~U0IER_ERBFI;								// Desabilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
		nop();
		*ch = buffer_GetFromFront(&u0_buf_rx);				// Captura os dados do inicio do buffer
		U0IER |= U0IER_ERBFI;								// Habilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
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
void uart0_ClearBufferRx(void) {
	U0IER &= ~U0IER_ERBFI;				// Desabilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
	nop();
	u0_overflow_rx = 0; 				// Limpa overflow do buffer de recepção
	U0FCR = U0FCR_RX_FIFO_RESET;		// Limpa RX FIFO
	buffer_Clear(&u0_buf_rx);			// Limpa o buffer de recepção
	U0IER |= U0IER_ERBFI;				// Habilita interrupção de dados prontos na recepção e CTI (SIMULADOR PROTEUS NÃO FUNCIONA CTI)
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna a quantidade de bytes do estouro do buffer de recepção
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
u32 uart0_CountRxOverflow (void) {
	u32 count = u0_overflow_rx;
	u0_overflow_rx = 0;
	return count;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Checa se buffer de recepção está vazio
// Parametros:	Nenhum
// Retorna:		pdTRUE se o buffer estiver vazio
//				pdFALSE se o buffer contiver dados recebidos
// -----------------------------------------------------------------------------------------------------------------
int uart0_BufferRxIsEmpty(void) {
	if (u0_buf_rx.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com a quantidade de bytes recebidos no buffer de recepção
// Parametros:	Nenhum
// Retorna:		Retorna com a quantidade de bytes recebidos no buffer de recepção
// -----------------------------------------------------------------------------------------------------------------
int uart0_BufferQtdRx(void) {
	return u0_buf_rx.datalength;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna o ponteiro do buffer de recepção de dados
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de recepção
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart0_GetBufferRx(void) {
	return &u0_buf_rx;
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
int uart0_PutChar(n16 ch) {
	int sts;

	if ( u0_buf_tx.size == 0 )												// Checa se o buffer de transmissão ainda não foi criado
   		return errBUFFER_NO_EXIST;                          				// Retorna indicando erro

	// É obrigatório incluirmos o char no buffer, mesmo que vamos retirá-lo logo em seguida caso u3_tx_running for FALSE
	//	Se não for dessa forma o e usar U3THR = ch haverá erro de sincronismo entre os dados chegas e dados enviados.
	// Isto porque o manipulador de interrupção (uart3_ISR) pode atender a mais de uma interrupção simulataneas

	do {
		U0IER &= ~U0IER_ETBEI;                 								// Desabilita a interrupção de transmissão
		nop();
		sts = buffer_AddToEnd(&u0_buf_tx, ch);
		U0IER |= U0IER_ETBEI;                  								// Habilita a interrupção de transmissão

		CRITICAL_WAIT;														// Caso exista algum OS, pode trocar de contexto
	} while (sts == errBUFFER_FULL);

	if (!u0_tx_running) {													// Checa se existe dado sendo transmitido
  		U0THR = buffer_GetFromFront(&u0_buf_tx);							// Atribui o dado do buffer ao registrador de transmissão
  		u0_tx_running = pdTRUE;												// Sinaliza que existe dado sendo trasnmitido
  		U0IER |= U0IER_ETBEI;                  								// Habilita a interrupção de transmissão
   	}

  	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Coloca um caractere na fila de transmissão. FUNÇÃO UTIL PARA DEBUG ONDE NÃO PRECISA INICIAR IRQ ou OS
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o código de erro
// -------------------------------------------------------------------------------------------------------------------
int uart0_BasicPutChar(n16 ch) {
  	while (!(U0LSR & U0LSR_THRE))								// Espero até que o buffer TX fique vazio
  		CRITICAL_WAIT;        									// Caso exista algum OS, pode trocar de contexto
  	U0THR = (u8)ch;												// Atualiza o registrador de TX uart com o dado
	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Captura o espaço disponivel para transmissão
// Parametros: 	Nenhum
// Retorna:		O tamanho em bytes do espaço disponivel para transmissão
// -------------------------------------------------------------------------------------------------------------------
u16 uart0_SpaceTx(void) {
	u16 qtd;
	
	U0IER &= ~U0IER_ETBEI;                 								// Desativa a interrupção de transmissão
	nop();
  	qtd = (u16)(u0_buf_tx.size - u0_buf_tx.datalength);					// Retorna com o espaço disponivel
  	U0IER |= U0IER_ETBEI;                  								// Habilita a interrupção de transmissão
  	return qtd;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Remove todos os dados da fila de transmissão
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void uart0_FlushTx(void) {	
	U0IER &= ~U0IER_ETBEI;                					// Desabilita a interrupção de transmissão
	nop();
  	U0FCR = U0FCR_TX_FIFO_RESET;          					// Limpa a FIFO de transmissão
	buffer_Clear(&u0_buf_tx);								// Limpa os indeces do buffer TX da UART
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Retorna o status do registrador de transmissão de dados
// Parametros: 	Nenhum
// Retorna:		pdFALSE = Se o registradores U0THR ou U0TSR não estão vazios
//				pdTRUE = Se o registradores U0THR ou U0TSR estão vazios
// -------------------------------------------------------------------------------------------------------------------
int uart0_EmptyTx(void) {
  	if ((U0LSR & (U0LSR_THRE | U0LSR_TEMT)) == (U0LSR_THRE | U0LSR_TEMT))
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
int uart0_WriteTx(u8 *buffer, u16 count) {
	if (count > uart0_SpaceTx())									// Checa se exste espaço suficiente no buffer de TX de espaço insuficiente
   		return errBUFFER_FULL;										// Retorna cheio
  	
  	while (count && (uart0_PutChar(*buffer++) >= 0))				// Faça enquanto tem dados a serem transmitidos
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
u8 *uart0_PutString(u8 *str) {
  	register char ch;

	while ((ch = *str) && (uart0_PutChar(ch) >= 0))				// Adiciona para transmissão enquanto tenha dados para transmitir
    	str++;

	return str;													// Retorna o ponteiro atual da string
}
#endif // FREE_RTOS

//###################################################################################################################
// UART SERVIÇO DE INTERRUPÇÕES
// 		Descrição: 	Esta função implementa o ISR da UART0
//###################################################################################################################
//###################################################################################################################
void uart0_ISR (void) {
	ISR_ENTRY;																// Procedimento para entrada da interrupção

  	u8 int_flags;
 	 	
  	while ( !((int_flags = U0IIR) & U0IIR_NO_INT) ) {							// Faça para todos os pedidos de interrupção pendentes emitidos pela UART 	
    	switch (int_flags & U0IIR_ID_MASK) {									// Identifica a fonte de interrupção
      	case U0IIR_RLS_INT:                									// Caso recebeu uma INT de erro na linha de recepção (receive line status)
        	U0LSR;                          								// Limpa registrador de status de linha
        	break;

        // ADICIONA NO BUFFER RX OS DADOS DA FIFO
        // No modo de trigger acima de 1 byte, se a quantidade de bytes da FIFO de recepção
        	// não alcançar o nível do trigger os dados da FIFO serão adicionados
        	// ao buffer de recepção pela interrupção de timerout da UART (U3IIR_CTI_INT)
      	case U0IIR_CTI_INT:                									// Character Timeout Indicator. Dados na FIFO RX que não foram lidos e está abaixo do nível de gatilho de int
      	case U0IIR_RDA_INT:                									// Caso que exista bytes disponivel recepção
      		do { 
      			if (buffer_AddToEnd(&u0_buf_rx, U0RBR) == errBUFFER_FULL)
					u0_overflow_rx++;
      		} while(U0LSR & U0LSR_RDR);										// Faça enquanto existe dados validos na FIFO de recepção

			#if (UART0_USE_HANDSHAKING == pdON) // AINDA NÃO TESTADO
     		if (UART0_RX_BUFFER_SIZE - u0_buf_rx.datalength > UART0_SIZE_FIFO)
     			 uart0_RTSon(); // Sinaliza que posso receber mais dados
     		else uart0_RTSoff(); // Sinaliza que não posso receber mais dados
  			#endif

			break;

     	// RETIRA OS DADOS DO BUFFER TX PARA A FIFO DE TRANSMISSÃO
     		// Essa interrupção é gerada quando a FIFO de transmissão de 16 bytes está vazia
			// O registrador UxTHR é o topo da FIFO de 16 bytes, ou seja, é o endereço da última posição a ser inserida da FIFO
      	case U0IIR_THRE_INT:               									// Interrupção da FIFO de 16 bytes quando está vazia
        	while (U0LSR & U0LSR_THRE) {									// Faça enquanto há espaço na FIFO
        		if (buffer_IsEmpty(&u0_buf_tx) ) {							// Checa se o buffer de transmissão já está vazio
        			u0_tx_running = pdFALSE;     							// Sinaliza que não existe dados para trasnmissao
        			U0IER &= ~U0IER_ETBEI;                					// Desativa a interrupção de transmissão
            		break;
          		} else {
					#if (UART0_USE_HANDSHAKING == pdON) // AINDA NÃO TESTADO
	    			if (uart0_CTS()) // Checa se o receptor esteja pronto para eviar dados
					#endif
	          			U0THR = buffer_GetFromFront(&u0_buf_tx);			// Atribui o dado do buffer ao registrador de transmissão
	          	}
          	}
		    
		    break;

      	default:															// Interrupção desconhecida
        	U0LSR;															// Limpa registrador de status de linha
        	U0RBR;															// Limpa o registrador de recepçao de dados
	       	break;
      	}
	}
  	
	// Limpa interrupção
  	#if defined(arm7tdmi)
  	VICSoftIntClr = (1<<VIC_UART0);
	#endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_UART0);
	#endif

  	ISR_EXIT;																// Procedimento para saida da interrupção
}


#if defined(FREE_RTOS)

xSemaphoreHandle uart0_mutexTX = NULL; // semáforo para trasmissão de dados
xSemaphoreHandle uart0_mutexRX = NULL; // semáforo para recepção de dados
xQueueHandle uart0_queueTX = NULL;		// Fila de transmissão

static void vTaskUart0TX( void *pvParameters );

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa e configura o modo de trabalho da UART
// Parametros:	baudrate:
//					1200, 2400, ... , 57600, 115200 ...
//				stack_size. Tamanho do STACK das tarefas
//				priority_task. Priotidade da terafas
//					Deixar a prioridade da tarefa igual ou maior que as tarefas que a usam
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna código de erro
// -------------------------------------------------------------------------------------------------------------------

int xUart0Init(u32 baudrate, unsigned portSHORT stack_size, unsigned int priority_task, xTaskHandle *task_handle) {
	int ret;

	taskENTER_CRITICAL();

	if ( (uart0_mutexTX = xSemaphoreCreateMutex()) == NULL)
		return errSEMAPHORE_CREATE_ERROR;

	if ( (uart0_mutexRX = xSemaphoreCreateMutex()) == NULL)
		return errSEMAPHORE_CREATE_ERROR;

	if ( (uart0_queueTX = xQueueCreate(UART0_RX_BUFFER_SIZE, sizeof(u8))) == NULL)
		return errQUEUE_CREATE_ERROR;

	ret = uart0_Init(baudrate);	// Incializo UART
	xTaskCreate( vTaskUart0TX, "tu0", stack_size,( void* ) NULL, priority_task, task_handle);

	taskEXIT_CRITICAL();

	return ret;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa que se encarrega de pegar dados da FILA e jogar para o buffer de transmissão
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
static void vTaskUart0TX( void *pvParameters ) {
	u8 ch;

	( void ) pvParameters;												// para evitar avisos do compliador

	while(1) {
		if (xQueueReceive(uart0_queueTX, &ch, portMAX_DELAY) == pdPASS) // Checa se tem dados na fila. Caso não tenha dados na FILA coloca a tarefa no modo bloqueado
			uart0_PutChar(ch);											// Joga o dado no buffer de transmissão.
	}
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Espera para ter acesso a UART e coloca um caractere na fila de transmissão
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o código de erro
// -------------------------------------------------------------------------------------------------------------------
int xUart0PutChar(n16 ch) {
//	int ret;
//
//	// Tenta pegar o acesso ao recurso.
//	//	Mantenha a tarefa bloqueado até conseguir caso o recusro esteja ocupado
//	ret = xSemaphoreTake(uart0_mutexTX, portMAX_DELAY);
//
//	if (ret == pdPASS) {
//		xQueueSend(uart0_queueTX, &ch, portMAX_DELAY);
//	}
//
//	xSemaphoreGive(uart0_mutexTX); 									// Libera o recurso
//	return ret;

	xQueueSend(uart0_queueTX, &ch, portMAX_DELAY);
	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Espera para ter acesso a UART e poder colocar toda string a fila de transmissão
// Parametros: 	Ponteiro da string a ser transmitida
// Retorna:		Retorna pdPASS ou o código de erro
// -------------------------------------------------------------------------------------------------------------------
int xUart0PutString(const char *s) {
  	int ret;
  	u8 ch;

	ret = xSemaphoreTake(uart0_mutexTX, portMAX_DELAY); 				// Tenta pegar o acesso ao recurso. Mantenha a tarefa bloqueado até conseguir caso o recusro esteja ocupado
	if (ret == pdPASS) {
		while ((ch = *s) )		{
    		xUart0PutChar(ch);
    		//xQueueSend(uart0_queueTX, &ch, portMAX_DELAY);
    		s++;
    	}
	}
	xSemaphoreGive(uart0_mutexTX); 									// Libera o recurso
	return ret;															// Retorna o ponteiro atual da string
}

//int xUart0LockRX(void) {
//	return xSemaphoreTake(uart0_mutexRX, portMAX_DELAY); 				// Tenta pegar o acesso ao recurso. Mantenha a tarefa bloqueado até conseguir caso o recusro esteja ocupado
//}
//
//int xUart0UnLockRX(void) {
//	return xSemaphoreGive(uart0_mutexRX); 								// Libera o recurso
//}
#endif //defined(FREE_RTOS)

