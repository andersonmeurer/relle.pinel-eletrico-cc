/*
	SE N�O USARMOS CONRTROLE DE FLUXO devemos levar em considera��o o estouro da buffer
		de recep��o para onde o ARM est� trasmitindo
		se transmitirmos muitos dados em um baudrate elevado, o buffer do destino pode estourar e
		pode aperece que o problema seja nas nossas bibliotecas. ent�o
		aumente o buffer rx do destino, ou aumente a prioridade de recep��o
		ou diminuia o baudrate em uma taxa mais adequada
*/

#include <stdlib.h>
#include "uart0_irq.h"

// FLAGS DA UART
static volatile u8 uart0BufferedTx;			// Sinalizador de existencia de dados no buffer TX para transmiss�o

// ESTRUTURA PARA OS BUFFERS DE RECEP��O E TRANSMISS�O
static buffer_t uart0RxBuffer;
static buffer_t uart0TxBuffer;

// CONTADOR DE OVERFLOW DOS BUFFERS DE RECEP��O
static volatile u16 uart0RxOverflow;

// CASO UTILIZAR MALLOC
#if (USE_MALLOC == pdON)
	u8 *uart0RxData;
	u8 *uart0TxData;
#else
	// ALOCANDO AUTOMATICAMENTE VARIAVEIS LOCAIS PARA CADA BUFFER QUANDO N�O UTILIZANDO RAM EXTERNA
	static u8 uart0RxData[UART0_RX_BUFFER_SIZE];
	static u8 uart0TxData[UART0_TX_BUFFER_SIZE];
#endif

#if UART0_USE_INT_FUNC == pdON
typedef void (*voidPtrFunc)(u8);
static volatile voidPtrFunc Uart0RxFunc;
#endif

static void uart0_InitBuffers(void);

//###################################################################################################################
// UART CONFIGURA��ES
//###################################################################################################################
//###################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializo a UART
// Parametros:	BaudRate
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void uart0_Init(u32 BaudRate) {
	uart0_InitBuffers(); 			// Inicializa os buffer da uart0
	#if UART0_USE_INT_FUNC == pdON
	uart0_ClearRxHandler();			// Retira qualquer fun��o da interrup��o de recep��o de dados
	#endif

	uart0BufferedTx = pdFALSE;      	// Sinaliza que n�o existe dados no buffer TX para transmiss�o
	uart0RxOverflow = 0; 			// Limpa overflow do buffer de recep��o
	
	UCSR0B = _BV(RXCIE)|_BV(TXCIE)|_BV(RXEN)|_BV(TXEN); //RXCIE0 e TXCIE0 Habilitam ints, RXEN0 e TXEN0 habilitam TX e RX UArt
			
	uart0_SetBaudRate(BaudRate);

	#if defined(U2X) && (UART0_USE_DOUBLESPEED == pdON)
		sbi(UCSR0A, U2X);
	#endif
	
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Inicializo os buffers para a uart
// Parametros:	Nenhum
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
static void uart0_InitBuffers(void) {
	#if (USE_MALLOC == pdON)
		uart0RxData = malloc(UART0_RX_BUFFER_SIZE*sizeof(u8));
		uart0TxData = malloc(UART0_TX_BUFFER_SIZE*sizeof(u8));
		
		buffer_Init(&uart0RxBuffer, uart0RxData, UART0_RX_BUFFER_SIZE);
		buffer_Init(&uart0TxBuffer, uart0TxData, UART0_TX_BUFFER_SIZE);
	#else
		buffer_Init(&uart0RxBuffer, uart0RxData, UART0_RX_BUFFER_SIZE);
		buffer_Init(&uart0TxBuffer, uart0TxData, UART0_TX_BUFFER_SIZE);
	#endif
}

//------------------------------------------------------------------------------------------------------------------
// Descri��o: 	Ajusta baud rate da UART0 de acordo com o clock da CPU
// Parametros: 	Baudrate
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void uart0_SetBaudRate(u32 BaudRate) {
	u32 Br;
		
	// Calcular o fator de divisao de acordo com a frequencia da CPU
	#if defined(U2X) && (UART0_USE_DOUBLESPEED == pdON)
		Br = (u32)((F_CPU / (BaudRate * 8L)) - 1);
	#else
		Br = (u32)((F_CPU / (BaudRate * 16L)) - 1);	
	#endif
		
 	UBRR0L = (u8) (Br&0xFF);
	
	#ifdef UBRR0H
		UBRR0H = (u8) ((Br>>8)&0x0F);		
	#endif
}

#if UART0_USE_INT_FUNC == pdON
// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Anexa a fun��o ao handler de interrup�ao da serial
// Parametros:	O ponteiro da fun��o desejada
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart0_SetRxHandler(void (*rx_func)(u8 c)) {
	Uart0RxFunc = rx_func;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Retira as fun��es do Handler de interrup��es
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart0_ClearRxHandler(void) {
	Uart0RxFunc = 0;
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
int uart0_GetChar(u8 *ch) {
	int ret;
	
	if(uart0RxBuffer.size == 0) 							// Checa se o buffer de recep��o ainda n�o foi criado
		return errBUFFER_NO_EXIST;            				// Retorna indicando erro
	
	if(uart0RxBuffer.datalength) {							// Checo de o buffer de recep��o cont�m dados recebidos pela UART
		cli(); nop(); 										// Desabilita interrup��es
		*ch = buffer_GetFromFront(&uart0RxBuffer);			// Captura os dados do inicio do buffer
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
u16 uart0_GetRxOverFlow(void) {
	return uart0RxOverflow;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa o buffer de recep��o
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart0_ClearRxBuffer(void) {
	cli(); nop ();
	buffer_Clear(&uart0RxBuffer);
	sei();
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Testa se o buffer de recep��o est� vazia
// Parametros:	Nenhum
// Retorna:		Retorna pdTRUE se o buffer estiver vazio, sen�p retorna falso
// -----------------------------------------------------------------------------------------------------------------
int uart0_RxBufferIsEmpty(void) {
	if (uart0RxBuffer.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Testa se o buffer de recep��o est� vazia
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de recep��o
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart0_GetRxBuffer(void) {
	return &uart0RxBuffer;
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
int uart0_PutCharBasic(n16 c) {
	while (uart0BufferedTx); 		// Espero que a transmiss�o do bufferTX termine
	while(!(UCSR0A & (1<<UDRE)));  	// Espera que o registrador da UART0 para transmiss�o fique vazio
	UDR0 = (u8)c;
	return pdPASS;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Transmitir um byte na UART
// Parametros:	O byte a ser transmitido
// Retorna:		
// -----------------------------------------------------------------------------------------------------------------
int uart0_PutChar(n16 c) {
	int sts;

	if ( uart0TxBuffer.size == 0 )										// Checa se o buffer de transmiss�o ainda n�o foi criado
   		return errBUFFER_NO_EXIST;                          			// Retorna indicando erro

	// � obrigat�rio incluirmos o char no buffer, mesmo que vamos retir�-lo logo em seguida caso u1_tx_running for FALSE
	//	Se n�o for dessa forma o e usar THR = ch haver� erro de sincronismo entre os dados chegas e dados enviados.
	// Isto porque o manipulador de interrup��o (ISR) pode atender a mais de uma interrup��o simulataneas

	do {
		cli(); nop();													// Desabilita as interrup��es
		sts = buffer_AddToEnd(&uart0TxBuffer, c);
		sei();															// Liga as interrup��es

		CRITICAL_WAIT;													// Caso exista algum OS, pode trocar de contexto
	} while (sts == errBUFFER_FULL);

	cli(); nop();														// Desabilita as interrup��es
	if (!uart0BufferedTx) {												// Checa se existe dado sendo transmitido
		uart0BufferedTx = pdTRUE;										// Sinaliza que existe dado sendo trasnmitido
		UDR0 = buffer_GetFromFront(&uart0TxBuffer);						// Atribui o dado do buffer ao registrador de transmiss�o
   	}
   	sei();																// Liga as interrup��es

	return pdPASS;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Limpa o buffer de transmiss�o
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart0_ClearTxBuffer(void) {
	cli(); nop();
	buffer_Clear(&uart0TxBuffer);
	sei();
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Testa se o buffer de transmiss�o est� vazio
// Parametros:	Nenhum
// Retorna:		Retorna pdTRUE se o buffer estiver vazio
// -----------------------------------------------------------------------------------------------------------------
int uart0_TxBufferIsEmpty(void) {
	if (uart0TxBuffer.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Testa se o buffer de recep��o est� vazia
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de transmiss�o
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart0_GetTxBuffer(void) {
	return &uart0TxBuffer; // return tx buffer pointer
}

//###################################################################################################################
// INTERRUP��ES
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de transmiss�o completa da UART0. 
// -----------------------------------------------------------------------------------------------------------------
UART0_INTERRUPT_HANDLER (USART0_TX_vect) {
	if (uart0TxBuffer.datalength)						// Verifico se existe dados no buffer para transmiss�o
		UDR0 = buffer_GetFromFront(&uart0TxBuffer);
	else uart0BufferedTx = pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descri��o: 	Interrup��o de recep��o da UART0
// -----------------------------------------------------------------------------------------------------------------
UART0_INTERRUPT_HANDLER (USART0_RX_vect) {
	volatile u8 c;

	c = UDR0;

	if (buffer_AddToEnd(&uart0RxBuffer, c) == errBUFFER_FULL)
		uart0RxOverflow++;

	#if UART0_USE_INT_FUNC == pdON
	// Se existe uma fun��o anexada a interrup��o execute-a
	if (Uart0RxFunc)
		Uart0RxFunc(c);
	#endif
}
