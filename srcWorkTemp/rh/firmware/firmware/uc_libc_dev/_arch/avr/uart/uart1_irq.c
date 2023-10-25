/*
	SE NÃO USARMOS CONRTROLE DE FLUXO devemos levar em consideração o estouro da buffer
		de recepção para onde o ARM está trasmitindo
		se transmitirmos muitos dados em um baudrate elevado, o buffer do destino pode estourar e
		pode aperece que o problema seja nas nossas bibliotecas. então
		aumente o buffer rx do destino, ou aumente a prioridade de recepção
		ou diminuia o baudrate em uma taxa mais adequada
*/

#include <stdlib.h>
#include "uart1_irq.h"

// FLAGS DA UART
static volatile u8 uart1BufferedTx;			// Sinalizador de existencia de dados no buffer TX para transmissão

// ESTRUTURA PARA OS BUFFERS DE RECEPÇÃO E TRANSMISSÃO
static buffer_t uart1RxBuffer;
static buffer_t uart1TxBuffer;

// CONTADOR DE OVERFLOW DOS BUFFERS DE RECEPÇÃO
static volatile u16 uart1RxOverflow;

// CASO UTILIZAR MALLOC
#if (USE_MALLOC == pdON)
	u8 *uart1RxData;
	u8 *uart1TxData;
#else
	// ALOCANDO AUTOMATICAMENTE VARIAVEIS LOCAIS PARA CADA BUFFER QUANDO NÃO UTILIZANDO RAM EXTERNA
	static u8 uart1RxData[UART1_RX_BUFFER_SIZE];
	static u8 uart1TxData[UART1_TX_BUFFER_SIZE];
#endif

#if UART1_USE_INT_FUNC == pdON
typedef void (*voidPtrFunc)(u8);
static volatile voidPtrFunc Uart1RxFunc;
#endif

static void uart1_InitBuffers(void);

//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializo a UART
// Parametros:	BaudRate
// Retorna:		Nada
//------------------------------------------------------------------------------------------------------------------
void uart1_Init(u32 BaudRate) {
	uart1_InitBuffers(); 			// Inicializa os buffer da uart0
	#if UART1_USE_INT_FUNC == pdON
	uart1_ClearRxHandler();			// Retira qualquer função da interrupção de recepção de dados
	#endif
	
	uart1BufferedTx = pdFALSE;      	// Sinaliza que não existe dados no buffer TX para transmissão
	uart1RxOverflow = 0; 			// Limpa overflow do buffer de recepção

	UCSR1B = _BV(RXCIE1)|_BV(TXCIE1)|_BV(RXEN1)|_BV(TXEN1);	//RXCIE1 e TXCIE1 Habilitam ints, RXEN1 e TXEN1 habilitam TX e RX UArt
	
	uart1_SetBaudRate(BaudRate); 
	
	#if (UART1_USE_DOUBLESPEED == pdON)
		sbi(UCSR1A, U2X1);
	#endif
		
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializo os buffers para a uart
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
// Descrição: 	Ajusta baud rate da UART de acordo com o clock da CPU
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
// Descrição: 	Anexa a função ao handler de interrupçao da serial
// Parametros:	O ponteiro da função desejada
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart1_SetRxHandler(void (*rx_func)(u8 c)) {
	Uart1RxFunc = rx_func;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Retira as funções do Handler de interrupções
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart1_ClearRxHandler(void) {
	Uart1RxFunc = 0;
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
int uart1_GetChar(u8 *ch) {
	int ret;
	
	if(uart1RxBuffer.size == 0) 							// Checa se o buffer de recepção ainda não foi criado
		return errBUFFER_NO_EXIST;            				// Retorna indicando erro
	
	if(uart1RxBuffer.datalength) {							// Checo de o buffer de recepção contêm dados recebidos pela UART
		cli(); nop(); 										// Desabilita interrupções
		*ch = buffer_GetFromFront(&uart1RxBuffer);			// Captura os dados do inicio do buffer
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
u16 uart1_GetRxOverFlow(void) {
	return uart1RxOverflow;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa o buffer de recepção
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart1_ClearRxBuffer(void) {
	cli(); nop ();
	buffer_Clear(&uart1RxBuffer);
	sei();
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Testa se o buffer de recepção está vazia
// Parametros:	Nenhum
// Retorna:		Retorna pdTRUE se o buffer estiver vazio, senão retorna falso
// -----------------------------------------------------------------------------------------------------------------
int uart1_RxBufferIsEmpty(void) {
	if (uart1RxBuffer.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Testa se o buffer de recepção está vazia
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de recepção
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart1_GetRxBuffer(void) {
	return &uart1RxBuffer;
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
int uart1_PutCharBasic(n16 c) {
	while (uart1BufferedTx); 		// Espero que a transmissão do bufferTX termine
	while(!(UCSR1A & (1<<UDRE)));	// Espera que o registrador da UART1 para transmissão fique vazio
	UDR1 = (u8)c;
	return pdPASS;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Transmitir um byte na UART
// Parametros:	O byte a ser transmitido
// Retorna:		
// -----------------------------------------------------------------------------------------------------------------
int uart1_PutChar(n16 c) {
	int sts;

	if ( uart1TxBuffer.size == 0 )										// Checa se o buffer de transmissão ainda não foi criado
   		return errBUFFER_NO_EXIST;                      	    		// Retorna indicando erro

	// É obrigatório incluirmos o char no buffer, mesmo que vamos retirá-lo logo em seguida caso u1_tx_running for FALSE
	//	Se não for dessa forma o e usar THR = ch haverá erro de sincronismo entre os dados chegas e dados enviados.
	// Isto porque o manipulador de interrupção (ISR) pode atender a mais de uma interrupção simulataneas

	do {
		cli(); nop();													// Desabilita as interrupções
		sts = buffer_AddToEnd(&uart1TxBuffer, c);
		sei();															// Liga as interrupções

		CRITICAL_WAIT;													// Caso exista algum OS, pode trocar de contexto
	} while (sts == errBUFFER_FULL);

	cli(); nop();														// Desabilita as interrupções
	if (!uart1BufferedTx) {												// Checa se existe dado sendo transmitido
		uart1BufferedTx = pdTRUE;										// Sinaliza que existe dado sendo trasnmitido
		UDR1 = buffer_GetFromFront(&uart1TxBuffer);						// Atribui o dado do buffer ao registrador de transmissão
   	}
   	sei();																// Liga as interrupções

	return pdPASS;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Limpa o buffer de transmissão
// Parametros:	Nenhum
// Retorna:		Nada
// -----------------------------------------------------------------------------------------------------------------
void uart1_ClearTxBuffer(void) {
	cli(); nop();
	buffer_Clear(&uart1TxBuffer);
	sei();
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Testa se o buffer de transmissão está vazio
// Parametros:	Nenhum
// Retorna:		Retorna pdTRUE se o buffer estiver vazio
// -----------------------------------------------------------------------------------------------------------------
int uart1_TxBufferIsEmpty(void) {
	if (uart1TxBuffer.datalength == 0)
			return pdTRUE;
	else	return pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Testa se o buffer de recepção está vazia
// Parametros:	Nenhum
// Retorna:		Retorna o ponteiro da estrutura do buffer de transmissão
// -----------------------------------------------------------------------------------------------------------------
buffer_t *uart1_GetTxBuffer(void) {
	return &uart1TxBuffer; // return tx buffer pointer
}

//###################################################################################################################
// INTERRUPÇÔES
//###################################################################################################################
//###################################################################################################################

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Interrupção de transmissão completa da UART1
// -----------------------------------------------------------------------------------------------------------------
UART1_INTERRUPT_HANDLER (USART1_TX_vect) {
	if (uart1TxBuffer.datalength)						// Verifico se existe dados no buffer para transmissão
		UDR1 = buffer_GetFromFront(&uart1TxBuffer);
	else uart1BufferedTx = pdFALSE;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: 	Interrupção de recepção da UART0
// -----------------------------------------------------------------------------------------------------------------
UART1_INTERRUPT_HANDLER (USART1_RX_vect) {
	volatile u8 c;

	c = UDR1;

	if (buffer_AddToEnd(&uart1RxBuffer, c) == errBUFFER_FULL)
		uart1RxOverflow++;

	#if UART0_USE_INT_FUNC == pdON
	// Se existe uma função anexada a interrupção execute-a
	if (Uart1RxFunc)
		Uart1RxFunc(c);
	#endif
}
