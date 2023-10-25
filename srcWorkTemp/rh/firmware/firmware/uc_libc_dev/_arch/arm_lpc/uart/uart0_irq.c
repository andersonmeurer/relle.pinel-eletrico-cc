/*
	ESTA BIBLIOTECA N�O TRATA OS ERROS DE RECEP��O PE, OE, FE E BI NAS INTERRUP��ES
	Logo as interrup��es rx line (U0IER_ELSI) e as interrup��es respons�vel pelo modem est�o desativadas
	
	1 - SE N�O USARMOS CONRTROLE DE FLUXO devemos levar em considera��o o estouro da buffer
		de recep��o para onde o ARM est� trasmitindo
		se transmitirmos muitos dados em um baudrate elevado, o buffer do destino pode estourar e
		pode aperece que o problema seja nas nossas bibliotecas. ent�o
		aumente o buffer rx do destino, ou aumente a prioridade de recep��o
		ou diminuia o baudrate em uma taxa mais adequada

	A interrup��o de transmiss�o � gerada a cada 16 bytes transmitidos, ou seja, quando a FIFO TX estiver vazia.
	A interrup��o de recep��o � gerada, na pior das hip�teses, a cada byte recebido; isto porque podemos configurar
	o n�vel da FIFO RX (trigger) para que a interrup��o seja gerada.

	Como a UART n�o � t�o r�pida assim e n�o h� tanto fluxo de dados pela UART, as interrup��es n�o s�o geradas
	subsequentemente dando tempo de sobra para outras fun��es da CPU.
*/

#include "uart0_irq.h"
#include "vic.h"	
#if defined(cortexm3)
#include "arm.h"
#endif

static volatile u8 u0_tx_running;						// Indicador se existe dados sendo transmitidos na UART
static volatile u16 u0_overflow_rx;						// Contador de estouro do buffer de recep��o
	
// BUFFERS DE RECEP��O E TRANSMISS�O
static volatile buffer_t u0_buf_rx; 					// Cria uma estrutura do buffer de recep��o
static volatile buffer_t u0_buf_tx;						// Cria uma estrutura do buffer de transmiss�o
	
#if (USE_MALLOC == pdON)
	static u8 *u0_data_rx;								// Ponteiro para aloca��o de mem�ria para o buffer de recep��o
	static u8 *u0_data_tx;								// Ponteiro para aloca��o de mem�ria para o buffer de transmiss�o
#else
	static u8 u0_data_rx[UART0_RX_BUFFER_SIZE];			// Aloca��o de mem�ria autom�tica para o buffer de recep��o
	static u8 u0_data_tx[UART0_TX_BUFFER_SIZE];			// Aloca��o de mem�ria autom�tica para o buffer de transmiss�o
#endif

void uart0_ISR (void);

//###################################################################################################################
// UART CONFIGURA��ES
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incializa e configura o modo de trabalho da UART
// Parametros:	baudrate: 
//					1200, 2400, ... , 57600, 115200 ...
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna c�digo do erro
// -------------------------------------------------------------------------------------------------------------------
int uart0_Init(u32 baudrate) {
  	u32 f_div;
  	
  	PCONP |= PCUART0;											// Ligar a uart no controle de potencia. Tem que ser antes da configura��o, pois ap�s ligar o perif�ricos, seus regs assumem valores padr�es
  	
	#if defined(arm7tdmi)
  	U0_PINSEL = (U0_PINSEL & ~U0_PINMASK) | U0_PINS;			// Configura os pinos do uC para a fun��o UART. Pinos TX=P0_2 e RX=P0_3
	#endif //arm7tdmi

	#if defined(cortexm3)
  	uart0_ConfigPort();
	#endif

	#if (UART0_USE_HANDSHAKING == pdON)
	// Por padr�o esses pinos j� s�o GPIO
	U0_CTS_DIR &= ~U0_CTS; 				// Pino CTS como entrada
	U0_RTS_DIR |= U0_RTS; 				// Pino RTS como sa�da
	uart0_RTSon(); 						// Sinaliza que posso receber dados
  	#endif

  	U0IER = 0;   	                      						// Desabilita todas as fonte de interrup��es da uart
  	U0IIR;                                						// Limpa os identificadores de pendencia interrup��es da uart
	U0FCR = (U0FCR_TX_FIFO_RESET | U0FCR_RX_FIFO_RESET);		// Limpa TX e RX FIFOS
  	
  	U0LCR = U0LCR_DLAB_ENABLE;             						// Seleciona os latches de devis�es para ajusta do baudrate
  	f_div = ( PCLK / 16 ) / baudrate ;							// Calcular divisor para o baudrate
  	U0DLL = f_div%256;              							// Ajusta o baudrate byte baixo
  	U0DLM = f_div/256;       									// Ajusta o baudrate byte alto
  	U0LCR = (UART0_MODE & ~U0LCR_DLAB_ENABLE);					// Configura o modo de trabalho da uart e desativa o acesso aos registradores do baudrate
  	
  	U0FCR = UART0_FIFOMODE;										// Configura o modo de trabalho da FIFO
  	
  	U0RBR;                                						// Limpa o registrador de recep�ao de dados
  	U0LSR;                                						// Limpa o registrador de estado de linha
	U0FCR = U0FCR_RX_FIFO_RESET;								// Limpa a FIFO de recep��o
  	U0FCR = U0FCR_TX_FIFO_RESET;          						// Limpa a FIFO de transmiss�o
	
	#if defined(arm7tdmi)
  	if (!irq_Install(VIC_UART0, uart0_ISR, UART0_PRIO_LEVEL))
		return errIRQ_INSTALL;
	#endif

	#if defined (cortexm3)
	nvic_enableIRQ(VIC_UART0);
	nvic_setPriority(VIC_UART0, UART0_PRIO_LEVEL);
	#endif

	// INICIALIZO OS BUFFERS DE RECEP��O E TRANSMISS�O
	#if (USE_MALLOC == pdON)
	u0_data_rx = malloc(UART0_RX_BUFFER_SIZE*sizeof(u8));
	u0_data_tx = malloc(UART0_TX_BUFFER_SIZE*sizeof(u8));
	#endif

	buffer_Init(&u0_buf_rx, u0_data_rx, UART0_RX_BUFFER_SIZE);
	buffer_Init(&u0_buf_tx, u0_data_tx, UART0_TX_BUFFER_SIZE);
		
	u0_overflow_rx = 0; 										// Limpa overflow do buffer de recep��o
	u0_tx_running = pdFALSE;
	
	U0IER |= U0IER_ERBFI;										// Habilita interrup��o de dados prontos na recep��o e CTI (SIMULADOR PROTEUS N�O FUNCIONA CTI)
	U0IER &= ~U0IER_ETBEI;                						// Desabilita a interrup��o de buffer vazio de transmiss�o

	return pdPASS;
}

//###################################################################################################################
// UART RECEP��O
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Captura o pr�ximo byte recebido na UART
// Parametros: 	Ponteiro para da variavel char para o retorno do byte recebido
// Retorna:		pdPASS indicando que existe caractere recebido ou o c�digo do erro
// -------------------------------------------------------------------------------------------------------------------
int uart0_GetChar(u8 *ch) {
	int ret;

	if ( u0_buf_rx.size == 0 )								// Checa se o buffer de recep��o ainda n�o foi criado
   		return errBUFFER_NO_EXIST;            				// Retorna indicando erro

	if ( u0_buf_rx.datalength ) {							// Checo de o buffer de recep��o cont�m dados recebidos pela UART
		U0IER &= ~U0IER_ERBFI;								// Desabilita interrup��o de dados prontos na recep��o e CTI (SIMULADOR PROTEUS N�O FUNCIONA CTI)
		nop();
		*ch = buffer_GetFromFront(&u0_buf_rx);				// Captura os dados do inicio do buffer
		U0IER |= U0IER_ERBFI;								// Habilita interrup��o de dados prontos na recep��o e CTI (SIMULADOR PROTEUS N�O FUNCIONA CTI)
		ret = pdPASS;
	} else
		ret = errBUFFER_EMPTY;

	return ret;
}


// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa o buffer e FIFO de recep��o
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart0_ClearBufferRx(void) {
	U0IER &= ~U0IER_ERBFI;				// Desabilita interrup��o de dados prontos na recep��o e CTI (SIMULADOR PROTEUS N�O FUNCIONA CTI)
	nop();
	u0_overflow_rx = 0; 				// Limpa overflow do buffer de recep��o
	U0FCR = U0FCR_RX_FIFO_RESET;		// Limpa RX FIFO
	buffer_Clear(&u0_buf_rx);			// Limpa o buffer de recep��o
	U0IER |= U0IER_ERBFI;				// Habilita interrup��o de dados prontos na recep��o e CTI (SIMULADOR PROTEUS N�O FUNCIONA CTI)
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna a quantidade de bytes do estouro do buffer de recep��o
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
u32 uart0_CountRxOverflow (void) {
	u32 count = u0_overflow_rx;
	u0_overflow_rx = 0;
	return count;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Checa se buffer de recep��o est� vazio
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
// Descri��o: 	Retorna com a quantidade de bytes recebidos no buffer de recep��o
// Parametros:	Nenhum
// Retorna:		Retorna com a quantidade de bytes recebidos no buffer de recep��o
// -----------------------------------------------------------------------------------------------------------------
int uart0_BufferQtdRx(void) {
	return u0_buf_rx.datalength;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o ponteiro do buffer de recep��o de dados
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de recep��o
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart0_GetBufferRx(void) {
	return &u0_buf_rx;
}

//###################################################################################################################
// UART TRANSMISSAO
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Coloca um caractere na fila de transmiss�o
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o c�digo de erro
// -------------------------------------------------------------------------------------------------------------------
int uart0_PutChar(n16 ch) {
	int sts;

	if ( u0_buf_tx.size == 0 )												// Checa se o buffer de transmiss�o ainda n�o foi criado
   		return errBUFFER_NO_EXIST;                          				// Retorna indicando erro

	// � obrigat�rio incluirmos o char no buffer, mesmo que vamos retir�-lo logo em seguida caso u3_tx_running for FALSE
	//	Se n�o for dessa forma o e usar U3THR = ch haver� erro de sincronismo entre os dados chegas e dados enviados.
	// Isto porque o manipulador de interrup��o (uart3_ISR) pode atender a mais de uma interrup��o simulataneas

	do {
		U0IER &= ~U0IER_ETBEI;                 								// Desabilita a interrup��o de transmiss�o
		nop();
		sts = buffer_AddToEnd(&u0_buf_tx, ch);
		U0IER |= U0IER_ETBEI;                  								// Habilita a interrup��o de transmiss�o

		CRITICAL_WAIT;														// Caso exista algum OS, pode trocar de contexto
	} while (sts == errBUFFER_FULL);

	if (!u0_tx_running) {													// Checa se existe dado sendo transmitido
  		U0THR = buffer_GetFromFront(&u0_buf_tx);							// Atribui o dado do buffer ao registrador de transmiss�o
  		u0_tx_running = pdTRUE;												// Sinaliza que existe dado sendo trasnmitido
  		U0IER |= U0IER_ETBEI;                  								// Habilita a interrup��o de transmiss�o
   	}

  	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Coloca um caractere na fila de transmiss�o. FUN��O UTIL PARA DEBUG ONDE N�O PRECISA INICIAR IRQ ou OS
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o c�digo de erro
// -------------------------------------------------------------------------------------------------------------------
int uart0_BasicPutChar(n16 ch) {
  	while (!(U0LSR & U0LSR_THRE))								// Espero at� que o buffer TX fique vazio
  		CRITICAL_WAIT;        									// Caso exista algum OS, pode trocar de contexto
  	U0THR = (u8)ch;												// Atualiza o registrador de TX uart com o dado
	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Captura o espa�o disponivel para transmiss�o
// Parametros: 	Nenhum
// Retorna:		O tamanho em bytes do espa�o disponivel para transmiss�o
// -------------------------------------------------------------------------------------------------------------------
u16 uart0_SpaceTx(void) {
	u16 qtd;
	
	U0IER &= ~U0IER_ETBEI;                 								// Desativa a interrup��o de transmiss�o
	nop();
  	qtd = (u16)(u0_buf_tx.size - u0_buf_tx.datalength);					// Retorna com o espa�o disponivel
  	U0IER |= U0IER_ETBEI;                  								// Habilita a interrup��o de transmiss�o
  	return qtd;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Remove todos os dados da fila de transmiss�o
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void uart0_FlushTx(void) {	
	U0IER &= ~U0IER_ETBEI;                					// Desabilita a interrup��o de transmiss�o
	nop();
  	U0FCR = U0FCR_TX_FIFO_RESET;          					// Limpa a FIFO de transmiss�o
	buffer_Clear(&u0_buf_tx);								// Limpa os indeces do buffer TX da UART
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Retorna o status do registrador de transmiss�o de dados
// Parametros: 	Nenhum
// Retorna:		pdFALSE = Se o registradores U0THR ou U0TSR n�o est�o vazios
//				pdTRUE = Se o registradores U0THR ou U0TSR est�o vazios
// -------------------------------------------------------------------------------------------------------------------
int uart0_EmptyTx(void) {
  	if ((U0LSR & (U0LSR_THRE | U0LSR_TEMT)) == (U0LSR_THRE | U0LSR_TEMT))
  			return pdTRUE;
  	else	return pdFALSE;  		
}

#if !defined(FREE_RTOS)
// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Adiociona N caracteres no buffer de transmiss�o
// Parametros: 	Ponteiro do buffer 
//				Quantidade de caracteres as serem transmitidos
// Retorna:		pdPASS = Transmitido com sucesso ou o c�digo do erro
// -------------------------------------------------------------------------------------------------------------------
int uart0_WriteTx(u8 *buffer, u16 count) {
	if (count > uart0_SpaceTx())									// Checa se exste espa�o suficiente no buffer de TX de espa�o insuficiente
   		return errBUFFER_FULL;										// Retorna cheio
  	
  	while (count && (uart0_PutChar(*buffer++) >= 0))				// Fa�a enquanto tem dados a serem transmitidos
    	count--;

  	return (count ? errBUFFER_FULL : pdPASS);						// Se count = 0 retorna 0 (Sucesso) sen�o retorna -2 (erro)
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Transmite uma string at� que encontre o caractere NULO '\0' ou que o buffer estoura
//				ATEN��O: o Compilador j� adiciona \0 na string do tipo "xyz" EX: "\r\r\nHello World!\r\r\n" 
// Parametros: 	Ponteiro da string
// Retorna:		O ponteiro da string. 
//					Retorna NULO se a string foi completamente transmitindo 
//					Ou retorna o ponteiro do pr�ximo caractere a ser transmitido
// OBS: Se a string for maior que o espa�o disponivel no buffer de TX pode ocorrer que a string n�o seja toda transmitida
// -------------------------------------------------------------------------------------------------------------------
u8 *uart0_PutString(u8 *str) {
  	register char ch;

	while ((ch = *str) && (uart0_PutChar(ch) >= 0))				// Adiciona para transmiss�o enquanto tenha dados para transmitir
    	str++;

	return str;													// Retorna o ponteiro atual da string
}
#endif // FREE_RTOS

//###################################################################################################################
// UART SERVI�O DE INTERRUP��ES
// 		Descri��o: 	Esta fun��o implementa o ISR da UART0
//###################################################################################################################
//###################################################################################################################
void uart0_ISR (void) {
	ISR_ENTRY;																// Procedimento para entrada da interrup��o

  	u8 int_flags;
 	 	
  	while ( !((int_flags = U0IIR) & U0IIR_NO_INT) ) {							// Fa�a para todos os pedidos de interrup��o pendentes emitidos pela UART 	
    	switch (int_flags & U0IIR_ID_MASK) {									// Identifica a fonte de interrup��o
      	case U0IIR_RLS_INT:                									// Caso recebeu uma INT de erro na linha de recep��o (receive line status)
        	U0LSR;                          								// Limpa registrador de status de linha
        	break;

        // ADICIONA NO BUFFER RX OS DADOS DA FIFO
        // No modo de trigger acima de 1 byte, se a quantidade de bytes da FIFO de recep��o
        	// n�o alcan�ar o n�vel do trigger os dados da FIFO ser�o adicionados
        	// ao buffer de recep��o pela interrup��o de timerout da UART (U3IIR_CTI_INT)
      	case U0IIR_CTI_INT:                									// Character Timeout Indicator. Dados na FIFO RX que n�o foram lidos e est� abaixo do n�vel de gatilho de int
      	case U0IIR_RDA_INT:                									// Caso que exista bytes disponivel recep��o
      		do { 
      			if (buffer_AddToEnd(&u0_buf_rx, U0RBR) == errBUFFER_FULL)
					u0_overflow_rx++;
      		} while(U0LSR & U0LSR_RDR);										// Fa�a enquanto existe dados validos na FIFO de recep��o

			#if (UART0_USE_HANDSHAKING == pdON) // AINDA N�O TESTADO
     		if (UART0_RX_BUFFER_SIZE - u0_buf_rx.datalength > UART0_SIZE_FIFO)
     			 uart0_RTSon(); // Sinaliza que posso receber mais dados
     		else uart0_RTSoff(); // Sinaliza que n�o posso receber mais dados
  			#endif

			break;

     	// RETIRA OS DADOS DO BUFFER TX PARA A FIFO DE TRANSMISS�O
     		// Essa interrup��o � gerada quando a FIFO de transmiss�o de 16 bytes est� vazia
			// O registrador UxTHR � o topo da FIFO de 16 bytes, ou seja, � o endere�o da �ltima posi��o a ser inserida da FIFO
      	case U0IIR_THRE_INT:               									// Interrup��o da FIFO de 16 bytes quando est� vazia
        	while (U0LSR & U0LSR_THRE) {									// Fa�a enquanto h� espa�o na FIFO
        		if (buffer_IsEmpty(&u0_buf_tx) ) {							// Checa se o buffer de transmiss�o j� est� vazio
        			u0_tx_running = pdFALSE;     							// Sinaliza que n�o existe dados para trasnmissao
        			U0IER &= ~U0IER_ETBEI;                					// Desativa a interrup��o de transmiss�o
            		break;
          		} else {
					#if (UART0_USE_HANDSHAKING == pdON) // AINDA N�O TESTADO
	    			if (uart0_CTS()) // Checa se o receptor esteja pronto para eviar dados
					#endif
	          			U0THR = buffer_GetFromFront(&u0_buf_tx);			// Atribui o dado do buffer ao registrador de transmiss�o
	          	}
          	}
		    
		    break;

      	default:															// Interrup��o desconhecida
        	U0LSR;															// Limpa registrador de status de linha
        	U0RBR;															// Limpa o registrador de recep�ao de dados
	       	break;
      	}
	}
  	
	// Limpa interrup��o
  	#if defined(arm7tdmi)
  	VICSoftIntClr = (1<<VIC_UART0);
	#endif
	#if defined (cortexm3)
	nvic_clearPendingIRQ(VIC_UART0);
	#endif

  	ISR_EXIT;																// Procedimento para saida da interrup��o
}


#if defined(FREE_RTOS)

xSemaphoreHandle uart0_mutexTX = NULL; // sem�foro para trasmiss�o de dados
xSemaphoreHandle uart0_mutexRX = NULL; // sem�foro para recep��o de dados
xQueueHandle uart0_queueTX = NULL;		// Fila de transmiss�o

static void vTaskUart0TX( void *pvParameters );

//###################################################################################################################
// UART CONFIGURA��ES
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incializa e configura o modo de trabalho da UART
// Parametros:	baudrate:
//					1200, 2400, ... , 57600, 115200 ...
//				stack_size. Tamanho do STACK das tarefas
//				priority_task. Priotidade da terafas
//					Deixar a prioridade da tarefa igual ou maior que as tarefas que a usam
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna c�digo de erro
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
// Descri��o:	Tarefa que se encarrega de pegar dados da FILA e jogar para o buffer de transmiss�o
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
static void vTaskUart0TX( void *pvParameters ) {
	u8 ch;

	( void ) pvParameters;												// para evitar avisos do compliador

	while(1) {
		if (xQueueReceive(uart0_queueTX, &ch, portMAX_DELAY) == pdPASS) // Checa se tem dados na fila. Caso n�o tenha dados na FILA coloca a tarefa no modo bloqueado
			uart0_PutChar(ch);											// Joga o dado no buffer de transmiss�o.
	}
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Espera para ter acesso a UART e coloca um caractere na fila de transmiss�o
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o c�digo de erro
// -------------------------------------------------------------------------------------------------------------------
int xUart0PutChar(n16 ch) {
//	int ret;
//
//	// Tenta pegar o acesso ao recurso.
//	//	Mantenha a tarefa bloqueado at� conseguir caso o recusro esteja ocupado
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
// Descri��o:	Espera para ter acesso a UART e poder colocar toda string a fila de transmiss�o
// Parametros: 	Ponteiro da string a ser transmitida
// Retorna:		Retorna pdPASS ou o c�digo de erro
// -------------------------------------------------------------------------------------------------------------------
int xUart0PutString(const char *s) {
  	int ret;
  	u8 ch;

	ret = xSemaphoreTake(uart0_mutexTX, portMAX_DELAY); 				// Tenta pegar o acesso ao recurso. Mantenha a tarefa bloqueado at� conseguir caso o recusro esteja ocupado
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
//	return xSemaphoreTake(uart0_mutexRX, portMAX_DELAY); 				// Tenta pegar o acesso ao recurso. Mantenha a tarefa bloqueado at� conseguir caso o recusro esteja ocupado
//}
//
//int xUart0UnLockRX(void) {
//	return xSemaphoreGive(uart0_mutexRX); 								// Libera o recurso
//}
#endif //defined(FREE_RTOS)

