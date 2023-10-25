/*
	ESTA BIBLIOTECA N�O TRATA OS ERROS DE RECEP��O PE, OE, FE E BI NAS INTERRUP��ES
	Logo as interrup��es rx line (U0IER_ELSI) e as interrup��es respons�vel pelo modem est�o desativadas
	
	1 - SE N�O USARMOS CONRTROLE DE FLUXO devemos levar em considera��o o estouro da buffer
		de recep��o para onde o ARM est� trasmitindo
		se transmitirmos muitos dados em um baudrate elevado, o buffer do destino pode estourar e
		pode aperece que o problema seja nas nossas bibliotecas. ent�o
		aumente o buffer rx do destino, ou aumente a prioridade de recep��o
		ou diminuia o baudrate em uma taxa mais adequada.

	A interrup��o de transmiss�o � gerada a cada 16 bytes transmitidos, ou seja, quando a FIFO TX estiver vazia.
	A interrup��o de recep��o � gerada, na pior das hip�teses, a cada byte recebido; isto porque podemos configurar
	o n�vel da FIFO RX (trigger) para que a interrup��o seja gerada.

	Como a UART n�o � t�o r�pida assim e n�o h� tanto fluxo de dados pela UART, as interrup��es n�o s�o geradas
	subsequentemente dando tempo de sobra para outras fun��es da CPU.
*/

#include "uart2_irq.h"
#include "vic.h"	
#if defined(cortexm3)
#include "arm.h"
#endif

static volatile u8 u2_tx_running;					// Indicador se existe dados sendo transmitidos na UART
static volatile u16 u2_overflow_rx;					// Contador de estouro do buffer de recep��o
	
// BUFFERS DE RECEP��O E TRANSMISS�O
static volatile buffer_t u2_buf_rx; 				// Cria uma estrutura do buffer de recep��o
static volatile buffer_t u2_buf_tx;					// Cria uma estrutura do buffer de transmiss�o
	
#if (USE_MALLOC == pdON)
	static u8 *u2_data_rx;							// Ponteiro para aloca��o de mem�ria para o buffer de recep��o
	static u8 *u2_data_tx;							// Ponteiro para aloca��o de mem�ria para o buffer de transmiss�o
#else
	static u8 u2_data_rx[UART2_RX_BUFFER_SIZE];		// Aloca��o de mem�ria autom�tica para o buffer de recep��o
	static u8 u2_data_tx[UART2_TX_BUFFER_SIZE];		// Aloca��o de mem�ria autom�tica para o buffer de transmiss�o
#endif
		
void uart2_ISR(void);

//###################################################################################################################
// UART CONFIGURA��ES
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Incializa e configura o modo de trabalho da UART
// Parametros:	baudrate: 
//					1200, 2400, ... , 57600, 115200 ...
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna c�digo de erro
// -------------------------------------------------------------------------------------------------------------------
int uart2_Init(u32 baudrate) {
	u32 f_div;
	
  	PCONP |= PCUART2;											// Ligar a uart2 no controle de potencia. Tem que ser antes da configura��o, pois ap�s ligar o perif�ricos, seus regs assumem valores padr�es
  	
	#if defined(arm7tdmi)
  	U2_PINSEL = (U2_PINSEL & ~U2_PINMASK) | U2_PINS;			// Configura os pinos do uC para a fun��o UART. 
	#endif // arm7tdmi

  	#if defined(cortexm3)
  	uart2_ConfigPort();
	#endif

	#if (UART2_USE_HANDSHAKING == pdON)
	// Por padr�o esses pinos j� s�o GPIO
	U2_CTS_DIR &= ~U2_CTS; // Pino CTS como entrada
  	U2_RTS_DIR |= U2_RTS; // Pino RTS como sa�da
  	uart2_RTSon(); // Sinaliza que posso receber dados
  	#endif

  	U2IER = 0;   	                      						// Desabilita todas as fonte de interrup��es da uart
  	U2IIR;                                						// Limpa os identificadores de pendencia interrup��es da uart
  	U2FCR = (U2FCR_TX_FIFO_RESET | U2FCR_RX_FIFO_RESET);		// Limpa TX e RX FIFOS
  	
  	U2LCR = U2LCR_DLAB_ENABLE;             						// Seleciona os latches de devis�es para ajusta do baudrate
  	f_div = ( PCLK / 16 ) / baudrate ;							// Calcular divisor para o baudrate
  	U2DLL = f_div%256;              							// Ajusta o baudrate byte baixo
  	U2DLM = f_div/256;       									// Ajusta o baudrate byte alto
  	U2LCR = (UART2_MODE & ~U2LCR_DLAB_ENABLE);					// Configura o modo de trabalho da uart e desativa o acesso aos registradores do baudrate
  	
  	U2FCR = UART2_FIFOMODE;										// Configura o modo de trabalho da FIFO

	U2RBR;                                						// Limpa o registrador de recep�ao de dados
  	U2LSR;                                						// Limpa o registrador de estado de linha
	U2FCR = U2FCR_RX_FIFO_RESET;								// Limpa a FIFO de recep��o
  	U2FCR = U2FCR_TX_FIFO_RESET;          						// Limpa a FIFO de transmiss�o

	#if defined(arm7tdmi)
  	if (!irq_Install(VIC_UART2, uart2_ISR, UART2_PRIO_LEVEL))
		return errIRQ_INSTALL;
	#endif

	#if defined (cortexm3)
	nvic_enableIRQ(VIC_UART2);
	nvic_setPriority(VIC_UART2, UART2_PRIO_LEVEL);
	#endif

	// INICIALIZO OS BUFFERS DE RECEP��O E TRANSMISS�O
	#if (USE_MALLOC == pdON)
	u2_data_rx = malloc(UART2_RX_BUFFER_SIZE*sizeof(u8));
	u2_data_tx = malloc(UART2_TX_BUFFER_SIZE*sizeof(u8));
	#endif

	buffer_Init(&u2_buf_rx, u2_data_rx, UART2_RX_BUFFER_SIZE);
	buffer_Init(&u2_buf_tx, u2_data_tx, UART2_TX_BUFFER_SIZE);
		
	u2_overflow_rx = 0; 										// Limpa overflow do buffer de recep��o
	u2_tx_running = pdFALSE;
		
	U2IER |= U2IER_ERBFI;										// Habilita interrup��o de dados prontos na recep��o e CTI (SIMULADOR PROTEUS N�O FUNCIONA CTI)
	U2IER &= ~U2IER_ETBEI;                						// Desabilita a interrup��o de buffer vazio de transmiss�o
		
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
int uart2_GetChar(u8 *ch) {
	int ret;

	if ( u2_buf_rx.size == 0 )								// Checa se o buffer de recep��o ainda n�o foi criado
   		return errBUFFER_NO_EXIST;            				// Retorna indicando erro

	if ( u2_buf_rx.datalength ) {							// Checo de o buffer de recep��o cont�m dados recebidos pela UART
		U2IER &= ~U2IER_ERBFI;								// Desabilita interrup��o de dados prontos na recep��o e CTI (SIMULADOR PROTEUS N�O FUNCIONA CTI)
		nop();
		*ch = buffer_GetFromFront(&u2_buf_rx);				// Captura os dados do inicio do buffer
		U2IER |= U2IER_ERBFI;								// Habilita interrup��o de dados prontos na recep��o e CTI (SIMULADOR PROTEUS N�O FUNCIONA CTI)
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
void uart2_ClearBufferRx(void) {
	U2IER &= ~U2IER_ERBFI;				// Desabilita interrup��o de dados prontos na recep��o e CTI (SIMULADOR PROTEUS N�O FUNCIONA CTI)
	nop();
	u2_overflow_rx = 0; 				// Limpa overflow do buffer de recep��o
	U2FCR = U2FCR_RX_FIFO_RESET;		// Limpa RX FIFO
	buffer_Clear(&u2_buf_rx);			// Limpa o buffer de recep��o
	U2IER |= U2IER_ERBFI;				// Habilita interrup��o de dados prontos na recep��o e CTI (SIMULADOR PROTEUS N�O FUNCIONA CTI)
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna a quantidade de bytes do estouro do buffer de recep��o
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
u32 uart2_CountRxOverflow (void) {
	u32 count = u2_overflow_rx;
	u2_overflow_rx = 0;
	return count;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Checa se buffer de recep��o est� vazio
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
// Descri��o: 	Retorna com a quantidade de bytes recebidos no buffer de recep��o
// Parametros:	Nenhum
// Retorna:		Retorna com a quantidade de bytes recebidos no buffer de recep��o
// -----------------------------------------------------------------------------------------------------------------
int uart2_BufferQtdRx(void) {
	return u2_buf_rx.datalength;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna o ponteiro da estrutura do buffer de recep��o
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de recep��o
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart2_GetBufferRx(void) {
	return &u2_buf_rx;
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
int uart2_PutChar(n16 ch) {
	int sts;

	if ( u2_buf_tx.size == 0 )												// Checa se o buffer de transmiss�o ainda n�o foi criado
   		return errBUFFER_NO_EXIST;                          				// Retorna indicando erro

	// � obrigat�rio incluirmos o char no buffer, mesmo que vamos retir�-lo logo em seguida caso u3_tx_running for FALSE
	//	Se n�o for dessa forma o e usar U3THR = ch haver� erro de sincronismo entre os dados chegas e dados enviados.
	// Isto porque o manipulador de interrup��o (uart3_ISR) pode atender a mais de uma interrup��o simulataneas

	do {
		U2IER &= ~U2IER_ETBEI;                 								// Desativa a interrup��o de transmiss�o
		nop();
		sts = buffer_AddToEnd(&u2_buf_tx, ch);
		U2IER |= U2IER_ETBEI;                  								// Habilita a interrup��o de transmiss�o

		CRITICAL_WAIT;														// Caso exista algum OS, pode trocar de contexto
	} while (sts == errBUFFER_FULL);

	if (!u2_tx_running) {													// Checa se existe dado sendo transmitido
  		U2THR = buffer_GetFromFront(&u2_buf_tx);							// Atribui o dado do buffer ao registrador de transmiss�o
  		u2_tx_running = pdTRUE;												// Sinaliza que existe dado sendo trasnmitido
  		U2IER |= U2IER_ETBEI;                  								// Habilita a interrup��o de transmiss�o
   	}

  	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Coloca um caractere na fila de transmiss�o. FUN��O UTIL PARA DEBUG ONDE N�O PRECISA INICIAR IRQ ou OS
// Parametros: 	Caractere a ser transmitindo
// Retorna:		Retorna pdPASS ou o c�digo de erro
// -------------------------------------------------------------------------------------------------------------------
int uart2_BasicPutChar(n16 ch) {
  	while (!(U2LSR & U2LSR_THRE)) 								// Espero at� que o buffer TX fique vazio  	 		
  		CRITICAL_WAIT;        									// Caso exista algum OS, pode trocar de contexto
  	U2THR = (u8)ch;												// Atualiza o registrador de TX uart com o dado
	return pdPASS;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Captura o espa�o disponivel para transmiss�o
// Parametros: 	Nenhum
// Retorna:		O tamanho em bytes do espa�o disponivel para transmiss�o
// -------------------------------------------------------------------------------------------------------------------
u16 uart2_SpaceTx(void) {
	u16 qtd;
	
	U2IER &= ~U2IER_ETBEI;                 								// Desativa a interrup��o de transmiss�o
	nop();
	qtd = (u16)(u2_buf_tx.size - u2_buf_tx.datalength);		// Retorna com o espa�o disponivel	
	U2IER |= U2IER_ETBEI;                  								// Habilita a interrup��o de transmiss�o
	return qtd;
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Remove todos os dados da fila de transmiss�o
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
void uart2_FlushTx(void) {
	U2IER &= ~U2IER_ETBEI;                					// Desabilita a interrup��o de transmiss�o
	nop();
  	U2FCR = U0FCR_TX_FIFO_RESET;          					// Limpa a FIFO de transmiss�o
	buffer_Clear(&u2_buf_tx);								// Limpa os indeces do buffer TX da UART
}

// -------------------------------------------------------------------------------------------------------------------
// Descri��o:	Retorna o status do registrador de transmiss�o de dados
// Parametros: 	Nenhum
// Retorna:		pdFALSE = Se o registradores U0THR ou U0TSR n�o est�o vazios
//				pdTRUE = Se o registradores U0THR ou U0TSR est�o vazios
// -------------------------------------------------------------------------------------------------------------------
int uart2_EmptyTx(void) {
  	if ((U2LSR & (U2LSR_THRE | U2LSR_TEMT)) == (U2LSR_THRE | U2LSR_TEMT))
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
int uart2_WriteTx(u8 *buffer, u16 count) {
	if (count > uart2_SpaceTx())									// Checa se exste espa�o suficiente no buffer de TX de espa�o insuficiente
   		return errBUFFER_FULL;										// Retorna cheio
  	
  	while (count && (uart2_PutChar(*buffer++) >= 0))				// Fa�a enquanto tem dados a serem transmitidos
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
u8 *uart2_PutString(u8 *str) {
   	register char ch;

  	while ((ch = *str) && (uart2_PutChar(ch) >= 0))				// Adiciona para transmiss�o enquanto tenha dados para transmitir
    	str++;

  	return str;													// Retorna o ponteiro atual da string
}
#endif // FREE_RTOS

//###################################################################################################################
// UART SERVI�O DE INTERRUP��ES
// 		Descri��o: 	Esta fun��o implementa o ISR da UART2
//###################################################################################################################
//###################################################################################################################
void uart2_ISR (void) {
	ISR_ENTRY;																// Procedimento para entrada da interrup��o

  	u8 int_flags;

  	while ( !((int_flags = U2IIR) & U2IIR_NO_INT) ) {							// Fa�a para todos os pedidos de interrup��o pendentes emitidos pela UART
    	switch (int_flags & U2IIR_ID_MASK) {									// Identifica a fonte de interrup��o
      	case U2IIR_RLS_INT:                									// Caso recebeu uma INT de erro na linha de recep��o (receive line status)
        	U2LSR;                         									// Limpa registrador de status de linha
        	break;

        // ADICIONA NO BUFFER RX OS DADOS DA FIFO
        // No modo de trigger acima de 1 byte, se a quantidade de bytes da FIFO de recep��o
        	// n�o alcan�ar o n�vel do trigger os dados da FIFO ser�o adicionados
        	// ao buffer de recep��o pela interrup��o de timerout da UART (U3IIR_CTI_INT)
      	case U2IIR_CTI_INT:                									// Character Timeout Indicator. Dados na FIFO RX que n�o foram lidos e est� abaixo do n�vel de gatilho de int
      	case U2IIR_RDA_INT:                									// Caso que exista bytes disponivel recep��o
     		do { 
      			if (buffer_AddToEnd(&u2_buf_rx, U2RBR) == errBUFFER_FULL)
					u2_overflow_rx++;
      		} while(U2LSR & U2LSR_RDR);										// Fa�a enquanto existe dados validos na FIFO de recep��o
			
			#if (UART2_USE_HANDSHAKING == pdON) // AINDA N�O TESTADO
     		if (UART2_RX_BUFFER_SIZE - u2_buf_rx.datalength > UART2_SIZE_FIFO)
     			 uart2_RTSon(); // Sinaliza que posso receber mais dados
     		else uart2_RTSoff(); // Sinaliza que n�o posso receber mais dados
  			#endif

			break;

     	// RETIRA OS DADOS DO BUFFER TX PARA A FIFO DE TRANSMISS�O
     		// Essa interrup��o � gerada quando a FIFO de transmiss�o de 16 bytes est� vazia
			// O registrador UxTHR � o topo da FIFO de 16 bytes, ou seja, � o endere�o da �ltima posi��o a ser inserida da FIFO
   		case U2IIR_THRE_INT:              									// Interrup��o da FIFO de 16 bytes quando est� vazia
      		while (U2LSR & U2LSR_THRE) {									// Fa�a enquanto h� espa�o na FIFO
       			if (buffer_IsEmpty(&u2_buf_tx) ) {						 	// Checa se o buffer de transmiss�o j� est� vazio
       				u2_tx_running = pdFALSE;     							// Sinaliza que n�o existe dados para trasnmissao
       				U2IER &= ~U2IER_ETBEI;                					// Desativa a interrup��o de transmiss�o
          			break;
       			} else {
					#if (UART2_USE_HANDSHAKING == pdON) // AINDA N�O TESTADO
	    			if (uart2_CTS()) // Checa se o receptor esteja pronto para eviar dados
					#endif
	       				U2THR = buffer_GetFromFront(&u2_buf_tx);			// Atribui o dado do buffer ao registrador de transmiss�o
	       		}
       		}
       		
       		break;

  		default:															// Interrup��o desconhecida
       		U2LSR;															// Limpa registrador de status de linha
       		U2RBR;															// Limpa o registrador de recep�ao de dados
       		break;
      	}
	}
  	
	// Limpa interrup��o
  	#if defined(arm7tdmi)
  	VICSoftIntClr = (1<<VIC_UART2);
	#endif
	#if defined (cortexm3)
  	nvic_clearPendingIRQ(VIC_UART2);
	#endif

  	ISR_EXIT;																// Procedimento para saida da interrup��o
}

