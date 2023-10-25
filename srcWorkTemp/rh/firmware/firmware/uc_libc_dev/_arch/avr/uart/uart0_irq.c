/*
	SE NÃO USARMOS CONRTROLE DE FLUXO devemos levar em consideração o estouro da buffer
		de recepção para onde o ARM está trasmitindo
		se transmitirmos muitos dados em um baudrate elevado, o buffer do destino pode estourar e
		pode aperece que o problema seja nas nossas bibliotecas. então
		aumente o buffer rx do destino, ou aumente a prioridade de recepção
		ou diminuia o baudrate em uma taxa mais adequada
*/

#include <stdlib.h>
#include "uart0_irq.h"

// FLAGS DA UART
static volatile u8 uart0BufferedTx;			// Sinalizador de existencia de dados no buffer TX para transmissão

// ESTRUTURA PARA OS BUFFERS DE RECEPÇÃO E TRANSMISSÃO
static buffer_t uart0RxBuffer;
static buffer_t uart0TxBuffer;

// CONTADOR DE OVERFLOW DOS BUFFERS DE RECEPÇÃO
static volatile u16 uart0RxOverflow;

// CASO UTILIZAR MALLOC
#if (USE_MALLOC == pdON)
	u8 *uart0RxData;
	u8 *uart0TxData;
#else
	// ALOCANDO AUTOMATICAMENTE VARIAVEIS LOCAIS PARA CADA BUFFER QUANDO NÃO UTILIZANDO RAM EXTERNA
	static u8 uart0RxData[UART0_RX_BUFFER_SIZE];
	static u8 uart0TxData[UART0_TX_BUFFER_SIZE];
#endif

#if UART0_USE_INT_FUNC == pdON
typedef void (*voidPtrFunc)(u8);
static volatile voidPtrFunc Uart0RxFunc;
#endif

static void uart0_InitBuffers(void);

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializo a UART
// Parametros:	BaudRate
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void uart0_Init(u32 BaudRate) {
	uart0_InitBuffers(); 			// Inicializa os buffer da uart0
	#if UART0_USE_INT_FUNC == pdON
	uart0_ClearRxHandler();			// Retira qualquer função da interrupção de recepção de dados
	#endif

	uart0BufferedTx = pdFALSE;      	// Sinaliza que não existe dados no buffer TX para transmissão
	uart0RxOverflow = 0; 			// Limpa overflow do buffer de recepção
	
	UCSR0B = _BV(RXCIE)|_BV(TXCIE)|_BV(RXEN)|_BV(TXEN); //RXCIE0 e TXCIE0 Habilitam ints, RXEN0 e TXEN0 habilitam TX e RX UArt
			
	uart0_SetBaudRate(BaudRate);

	#if defined(U2X) && (UART0_USE_DOUBLESPEED == pdON)
		sbi(UCSR0A, U2X);
	#endif
	
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializo os buffers para a uart
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
// Descrição: 	Ajusta baud rate da UART0 de acordo com o clock da CPU
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
// Descrição: 	Anexa a função ao handler de interrupçao da serial
// Parametros:	O ponteiro da função desejada
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart0_SetRxHandler(void (*rx_func)(u8 c)) {
	Uart0RxFunc = rx_func;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retira as funções do Handler de interrupções
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart0_ClearRxHandler(void) {
	Uart0RxFunc = 0;
}
#endif

//###################################################################################################################
// UART RECEPÇÂO
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descrição:	Captura o próximo byte recebido na UART
// Parametros: 	Ponteiro para da variavel char para o retorno do byte recebido
// Retorna:		pdPASS indicando que existe caractere recebido ou o código do erro
// -----------------------------------------------------------------------------------------------------------------
int uart0_GetChar(u8 *ch) {
	int ret;
	
	if(uart0RxBuffer.size == 0) 							// Checa se o buffer de recepção ainda não foi criado
		return errBUFFER_NO_EXIST;            				// Retorna indicando erro
	
	if(uart0RxBuffer.datalength) {							// Checo de o buffer de recepção contêm dados recebidos pela UART
		cli(); nop(); 										// Desabilita interrupções
		*ch = buffer_GetFromFront(&uart0RxBuffer);			// Captura os dados do inicio do buffer
		sei();												//  Habilita interrupções
		ret = pdPASS;
	} else
		ret = errBUFFER_EMPTY;

	return ret;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com a quantidade de byte perdidos na recepção por causa do buffer cheio
// Parametros:	Nenhum
// Retorna:		Retorna com a quantidade de byte perdidos na recepção por causa do buffer cheio
// -----------------------------------------------------------------------------------------------------------------
u16 uart0_GetRxOverFlow(void) {
	return uart0RxOverflow;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa o buffer de recepção
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart0_ClearRxBuffer(void) {
	cli(); nop ();
	buffer_Clear(&uart0RxBuffer);
	sei();
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Testa se o buffer de recepção está vazia
// Parametros:	Nenhum
// Retorna:		Retorna pdTRUE se o buffer estiver vazio, senãp retorna falso
// -----------------------------------------------------------------------------------------------------------------
int uart0_RxBufferIsEmpty(void) {
	if (uart0RxBuffer.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Testa se o buffer de recepção está vazia
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de recepção
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart0_GetRxBuffer(void) {
	return &uart0RxBuffer;
}

//###################################################################################################################
// UART TRANSMISSÂO
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Transmitir um byte na UART de forma baisca. FUNÇÃO UTIL PARA DEBUG ONDE NÃO PRECISA INICIAR IRQ ou OS
// Parametros:	O byte a ser transmitido
// Retorna:		
// -----------------------------------------------------------------------------------------------------------------
int uart0_PutCharBasic(n16 c) {
	while (uart0BufferedTx); 		// Espero que a transmissão do bufferTX termine
	while(!(UCSR0A & (1<<UDRE)));  	// Espera que o registrador da UART0 para transmissão fique vazio
	UDR0 = (u8)c;
	return pdPASS;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Transmitir um byte na UART
// Parametros:	O byte a ser transmitido
// Retorna:		
// -----------------------------------------------------------------------------------------------------------------
int uart0_PutChar(n16 c) {
	int sts;

	if ( uart0TxBuffer.size == 0 )										// Checa se o buffer de transmissão ainda não foi criado
   		return errBUFFER_NO_EXIST;                          			// Retorna indicando erro

	// É obrigatório incluirmos o char no buffer, mesmo que vamos retirá-lo logo em seguida caso u1_tx_running for FALSE
	//	Se não for dessa forma o e usar THR = ch haverá erro de sincronismo entre os dados chegas e dados enviados.
	// Isto porque o manipulador de interrupção (ISR) pode atender a mais de uma interrupção simulataneas

	do {
		cli(); nop();													// Desabilita as interrupções
		sts = buffer_AddToEnd(&uart0TxBuffer, c);
		sei();															// Liga as interrupções

		CRITICAL_WAIT;													// Caso exista algum OS, pode trocar de contexto
	} while (sts == errBUFFER_FULL);

	cli(); nop();														// Desabilita as interrupções
	if (!uart0BufferedTx) {												// Checa se existe dado sendo transmitido
		uart0BufferedTx = pdTRUE;										// Sinaliza que existe dado sendo trasnmitido
		UDR0 = buffer_GetFromFront(&uart0TxBuffer);						// Atribui o dado do buffer ao registrador de transmissão
   	}
   	sei();																// Liga as interrupções

	return pdPASS;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa o buffer de transmissão
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart0_ClearTxBuffer(void) {
	cli(); nop();
	buffer_Clear(&uart0TxBuffer);
	sei();
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Testa se o buffer de transmissão está vazio
// Parametros:	Nenhum
// Retorna:		Retorna pdTRUE se o buffer estiver vazio
// -----------------------------------------------------------------------------------------------------------------
int uart0_TxBufferIsEmpty(void) {
	if (uart0TxBuffer.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Testa se o buffer de recepção está vazia
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de transmissão
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart0_GetTxBuffer(void) {
	return &uart0TxBuffer; // return tx buffer pointer
}

//###################################################################################################################
// INTERRUPÇÔES
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Interrupção de transmissão completa da UART0. 
// -----------------------------------------------------------------------------------------------------------------
UART0_INTERRUPT_HANDLER (USART0_TX_vect) {
	if (uart0TxBuffer.datalength)						// Verifico se existe dados no buffer para transmissão
		UDR0 = buffer_GetFromFront(&uart0TxBuffer);
	else uart0BufferedTx = pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Interrupção de recepção da UART0
// -----------------------------------------------------------------------------------------------------------------
UART0_INTERRUPT_HANDLER (USART0_RX_vect) {
	volatile u8 c;

	c = UDR0;

	if (buffer_AddToEnd(&uart0RxBuffer, c) == errBUFFER_FULL)
		uart0RxOverflow++;

	#if UART0_USE_INT_FUNC == pdON
	// Se existe uma função anexada a interrupção execute-a
	if (Uart0RxFunc)
		Uart0RxFunc(c);
	#endif
}
