/*
	SE N�O USARMOS CONRTROLE DE FLUXO devemos levar em considera��o o estouro da buffer
		de recep��o para onde o ARM est� trasmitindo
		se transmitirmos muitos dados em um baudrate elevado, o buffer do destino pode estourar e
		pode aperece que o problema seja nas nossas bibliotecas. ent�o
		aumente o buffer rx do destino, ou aumente a prioridade de recep��o
		ou diminuia o baudrate em uma taxa mais adequada
*/

#include <stdlib.h>
#include "uart1_irq.h"

// FLAGS DA UART
static volatile u8 uart1BufferedTx;			// Sinalizador de existencia de dados no buffer TX para transmiss�o

// ESTRUTURA PARA OS BUFFERS DE RECEP��O E TRANSMISS�O
static buffer_t uart1RxBuffer;
static buffer_t uart1TxBuffer;

// CONTADOR DE OVERFLOW DOS BUFFERS DE RECEP��O
static volatile u16 uart1RxOverflow;

// CASO UTILIZAR MALLOC
#if (USE_MALLOC == pdON)
	u8 *uart1RxData;
	u8 *uart1TxData;
#else
	// ALOCANDO AUTOMATICAMENTE VARIAVEIS LOCAIS PARA CADA BUFFER QUANDO N�O UTILIZANDO RAM EXTERNA
	static u8 uart1RxData[UART1_RX_BUFFER_SIZE];
	static u8 uart1TxData[UART1_TX_BUFFER_SIZE];
#endif

#if UART1_USE_INT_FUNC == pdON
typedef void (*voidPtrFunc)(u8);
static volatile voidPtrFunc Uart1RxFunc;
#endif

static void uart1_InitBuffers(void);

//###################################################################################################################
// UART CONFIGURA��ES
//###################################################################################################################
//###################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializo a UART
// Parametros:	BaudRate
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void uart1_Init(u32 BaudRate) {
	uart1_InitBuffers(); 			// Inicializa os buffer da uart0
	#if UART1_USE_INT_FUNC == pdON
	uart1_ClearRxHandler();			// Retira qualquer fun��o da interrup��o de recep��o de dados
	#endif
	
	uart1BufferedTx = pdFALSE;      	// Sinaliza que n�o existe dados no buffer TX para transmiss�o
	uart1RxOverflow = 0; 			// Limpa overflow do buffer de recep��o

	UCSR1B = _BV(RXCIE1)|_BV(TXCIE1)|_BV(RXEN1)|_BV(TXEN1);	//RXCIE1 e TXCIE1 Habilitam ints, RXEN1 e TXEN1 habilitam TX e RX UArt
	
	uart1_SetBaudRate(BaudRate); 
	
	#if (UART1_USE_DOUBLESPEED == pdON)
		sbi(UCSR1A, U2X1);
	#endif
		
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializo os buffers para a uart
// Parametros:	Nenhum
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
static void uart1_InitBuffers(void) {
	#if (USE_MALLOC == pdON)
		uart1RxData = malloc(UART1_RX_BUFFER_SIZE*sizeof(u8));
		uart1TxData = malloc(UART1_TX_BUFFER_SIZE*sizeof(u8));
		
		buffer_Init(&uart1RxBuffer, uart1RxData, UART1_RX_BUFFER_SIZE);
		buffer_Init(&uart1TxBuffer, uart1TxData, UART1_TX_BUFFER_SIZE);
	#else
		buffer_Init(&uart1RxBuffer, uart1RxData, UART1_RX_BUFFER_SIZE);
		buffer_Init(&uart1TxBuffer, uart1TxData, UART1_TX_BUFFER_SIZE);	
	#endif
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta baud rate da UART de acordo com o clock da CPU
// Parametros: 	Baudrate
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void uart1_SetBaudRate(u32 BaudRate) {
	u32 Br;
		
	// Calcular o fator de divisao de acordo com a frequencia da CPU
	#if (UART1_USE_DOUBLESPEED == pdON)
		Br = (u32)((F_CPU / (BaudRate * 8L)) - 1);
	#else
		Br = (u32)((F_CPU / (BaudRate * 16L)) - 1);	
	#endif
		
 	UBRR1L = (u8) (Br&0xFF);
	UBRR1H = (u8) ((Br>>8)&0x0F);
}

#if UART1_USE_INT_FUNC == pdON
// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Anexa a fun��o ao handler de interrup�ao da serial
// Parametros:	O ponteiro da fun��o desejada
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart1_SetRxHandler(void (*rx_func)(u8 c)) {
	Uart1RxFunc = rx_func;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retira as fun��es do Handler de interrup��es
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart1_ClearRxHandler(void) {
	Uart1RxFunc = 0;
}
#endif

//###################################################################################################################
// UART RECEP��O
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descri��o:	Captura o pr�ximo byte recebido na UART
// Parametros: 	Ponteiro para da variavel char para o retorno do byte recebido
// Retorna:		pdPASS indicando que existe caractere recebido ou o c�digo do erro
// -----------------------------------------------------------------------------------------------------------------
int uart1_GetChar(u8 *ch) {
	int ret;
	
	if(uart1RxBuffer.size == 0) 							// Checa se o buffer de recep��o ainda n�o foi criado
		return errBUFFER_NO_EXIST;            				// Retorna indicando erro
	
	if(uart1RxBuffer.datalength) {							// Checo de o buffer de recep��o cont�m dados recebidos pela UART
		cli(); nop(); 										// Desabilita interrup��es
		*ch = buffer_GetFromFront(&uart1RxBuffer);			// Captura os dados do inicio do buffer
		sei();												//  Habilita interrup��es
		ret = pdPASS;
	} else
		ret = errBUFFER_EMPTY;

	return ret;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retorna com a quantidade de byte perdidos na recep��o por causa do buffer cheio
// Parametros:	Nenhum
// Retorna:		Retorna com a quantidade de byte perdidos na recep��o por causa do buffer cheio
// -----------------------------------------------------------------------------------------------------------------
u16 uart1_GetRxOverFlow(void) {
	return uart1RxOverflow;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa o buffer de recep��o
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart1_ClearRxBuffer(void) {
	cli(); nop ();
	buffer_Clear(&uart1RxBuffer);
	sei();
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Testa se o buffer de recep��o est� vazia
// Parametros:	Nenhum
// Retorna:		Retorna pdTRUE se o buffer estiver vazio, sen�o retorna falso
// -----------------------------------------------------------------------------------------------------------------
int uart1_RxBufferIsEmpty(void) {
	if (uart1RxBuffer.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Testa se o buffer de recep��o est� vazia
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de recep��o
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart1_GetRxBuffer(void) {
	return &uart1RxBuffer;
}

//###################################################################################################################
// UART TRANSMISS�O
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Transmitir um byte na UART de forma baisca. FUN��O UTIL PARA DEBUG ONDE N�O PRECISA INICIAR IRQ ou OS
// Parametros:	O byte a ser transmitido
// Retorna:		
// -----------------------------------------------------------------------------------------------------------------
int uart1_PutCharBasic(n16 c) {
	while (uart1BufferedTx); 		// Espero que a transmiss�o do bufferTX termine
	while(!(UCSR1A & (1<<UDRE)));	// Espera que o registrador da UART1 para transmiss�o fique vazio
	UDR1 = (u8)c;
	return pdPASS;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Transmitir um byte na UART
// Parametros:	O byte a ser transmitido
// Retorna:		
// -----------------------------------------------------------------------------------------------------------------
int uart1_PutChar(n16 c) {
	int sts;

	if ( uart1TxBuffer.size == 0 )										// Checa se o buffer de transmiss�o ainda n�o foi criado
   		return errBUFFER_NO_EXIST;                      	    		// Retorna indicando erro

	// � obrigat�rio incluirmos o char no buffer, mesmo que vamos retir�-lo logo em seguida caso u1_tx_running for FALSE
	//	Se n�o for dessa forma o e usar THR = ch haver� erro de sincronismo entre os dados chegas e dados enviados.
	// Isto porque o manipulador de interrup��o (ISR) pode atender a mais de uma interrup��o simulataneas

	do {
		cli(); nop();													// Desabilita as interrup��es
		sts = buffer_AddToEnd(&uart1TxBuffer, c);
		sei();															// Liga as interrup��es

		CRITICAL_WAIT;													// Caso exista algum OS, pode trocar de contexto
	} while (sts == errBUFFER_FULL);

	cli(); nop();														// Desabilita as interrup��es
	if (!uart1BufferedTx) {												// Checa se existe dado sendo transmitido
		uart1BufferedTx = pdTRUE;										// Sinaliza que existe dado sendo trasnmitido
		UDR1 = buffer_GetFromFront(&uart1TxBuffer);						// Atribui o dado do buffer ao registrador de transmiss�o
   	}
   	sei();																// Liga as interrup��es

	return pdPASS;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa o buffer de transmiss�o
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart1_ClearTxBuffer(void) {
	cli(); nop();
	buffer_Clear(&uart1TxBuffer);
	sei();
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Testa se o buffer de transmiss�o est� vazio
// Parametros:	Nenhum
// Retorna:		Retorna pdTRUE se o buffer estiver vazio
// -----------------------------------------------------------------------------------------------------------------
int uart1_TxBufferIsEmpty(void) {
	if (uart1TxBuffer.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Testa se o buffer de recep��o est� vazia
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de transmiss�o
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart1_GetTxBuffer(void) {
	return &uart1TxBuffer; // return tx buffer pointer
}

//###################################################################################################################
// INTERRUP��ES
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de transmiss�o completa da UART1
// -----------------------------------------------------------------------------------------------------------------
UART1_INTERRUPT_HANDLER (USART1_TX_vect) {
	if (uart1TxBuffer.datalength)						// Verifico se existe dados no buffer para transmiss�o
		UDR1 = buffer_GetFromFront(&uart1TxBuffer);
	else uart1BufferedTx = pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de recep��o da UART0
// -----------------------------------------------------------------------------------------------------------------
UART1_INTERRUPT_HANDLER (USART1_RX_vect) {
	volatile u8 c;

	c = UDR1;

	if (buffer_AddToEnd(&uart1RxBuffer, c) == errBUFFER_FULL)
		uart1RxOverflow++;

	#if UART0_USE_INT_FUNC == pdON
	// Se existe uma fun��o anexada a interrup��o execute-a
	if (Uart1RxFunc)
		Uart1RxFunc(c);
	#endif
}
