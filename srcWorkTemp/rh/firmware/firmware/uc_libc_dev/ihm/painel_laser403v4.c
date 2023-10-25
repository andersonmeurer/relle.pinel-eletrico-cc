#include "painel_laser403v4.h"

#define pn403_Listing()		PN403_DATAFLOW_CLR = PN403_DATAFLOW
#define pn403_Speaking()	PN403_DATAFLOW_SET = PN403_DATAFLOW

typedef struct {
	u8 buffer[17];			// Buffer de transmissor para linha do display de 16 char e mais o char sinalizador de fim de string
	u8 indexTX;				// Sinaliza qual posição do buffer está sendo transmitindo no bus rs485
	u8 maxTX;				// Sinaliza o máximo de char a ser enviado para bus rs485
}tLine, *pLine;

typedef struct {
	u8 key;					// Valor da tecla pressionada
	tLine line1, line2;		// Linhas dos display
} tPainel;

static tPainel painel;


// inicializa o controlador do painel
void pnl403_Init(void) {
	uart_Init(1250000);
	PN403_DATAFLOW_DIR |= PN403_DATAFLOW;
	pn403_Listing();
	painel.line1.maxTX = 0;
	painel.line2.maxTX = 0;
	painel.key = 0;
}

// emite um reset no display, colocando em condições iniciais
int pnl403_Reset(void) {
	// se não tiver comandos sendo enviado para o painel vamos eviar um comando dummy
	//	para qualquer linha para que o painel retorne com o valor da tecla pressionada
	if ( (painel.line1.maxTX == 0) && (painel.line2.maxTX == 0) ) {
		painel.line1.buffer[0] = 105;
		painel.line1.maxTX = 1;
		painel.line1.indexTX = 0;
		return pdPASS;
	}

	return pdFAIL;
}

// Imprime texto em uma linha do display
// Se a linha contiver algo sendo impresso retorna FAIL, senão retorna PASS
int pnl403_WriteLine(int line, pchar s) {
	pLine l;
	u8* b;

	// Se pelo menos uma linha sendo transmitida para o display cancela a operação.
	// Mesmo essa lib sendo desenhada para enviar duas linhas em sequencia, o display se tornou muito lento para
	// processar duas linhas em sequencia, por essa razão foi colocado o teste condicional abaixo e retirado if (l->maxTX) return pdFAIL;
	if ( (painel.line1.maxTX > 0) || (painel.line2.maxTX > 0)) return pdFAIL;
	if (line==1) l = &painel.line1; else l = &painel.line2; 	// Captura qual linha deve ser impresso a mensagem
	//if (l->maxTX) return pdFAIL;								// Se esta linha já tem dados sendo enviado para o painel retorna falso

	b = &l->buffer[0];	// Captura o endereço do buffer
	*b++ = 110+line; 	// comando 111 para imprimir na primeira linha, comando 112 para imprimir na segunda linha
	l->maxTX = 1;		// Sinaliza que pelo menos um char a ser transmitido
	l->indexTX = 0;		// Sinaliza que é o primneiro char a ser transmitido

	//
	while(*s) {
		*b++ =*s++;					// Captura o texto a ser transmitido para o buffer da linha do display
		l->maxTX++;					//
		if (l->maxTX == 17) break; 	// O máximo que pode imprimir por linha é 17 (1 comando e 16 de dados)
	}

	// Se não imprimiu 17 chars no buffer de impressão completar com espaço vazio
	//	Isto porque o painel caprtura 16 chars da serial e imprimi no LCD
	int x;
	if (l->maxTX < 17) {
		for (x=0; x < (17-l->maxTX); x++) *b++ = 32;
		l->maxTX=17;
	}

	return pdPASS;
}

// Limpa o determinada linha do display
int pnl403_Clear(u8 line) {
	 return pnl403_WriteLine(line, " ");
}


// Captura a tecla pressionada
// Retorna pdPASS se houve nova tecla pessioanda do teclado, senão retorna pdFALSE
int pnl403_GetKey(u8* key) {
	if (painel.key) { // Checa se há nova tecla pressionada
		// Se for o valor for KEY_NONE (200) é porque o teclado não foi pressioanado, então retornar 0
		if (painel.key == KEY_NONE) *key = 0; else *key = painel.key;
		painel.key = 0;
		return pdPASS;
	}

	// se não tiver comandos sendo enviado para o painel vamos eviar um comando dummy
	//	para qualquer linha para que o painel retorne com o valor da tecla pressionada
	if ( (painel.line1.maxTX == 0) && (painel.line2.maxTX == 0) ) {
		painel.line1.buffer[0] = 0x55;
		painel.line1.maxTX = 1;
		painel.line1.indexTX = 0;
	}

	return pdFAIL;
}


// Por o painel ser lento cada transmissão de bytes não pode ser inferior a 30uS
//	(funciona com 25us, mas é bom deixar uma margem)
//	Mesmo assim a cada byte transmitindo o painel retorna com um byte do valor do teclado
//	então não podemos ficar muito tempo contendo o barramento em falar com o painel
// 	pois o painel vai inverter o barramento para mandar o valor do teclado
// Primeiro transmite o buffer da linha 1 para depois transmitir o buffer da linha 2
void pnl403_Tick(void) {
	pLine l;

	// Vericar se houve resposta do painel do valor da tecla
	uart_GetChar(&painel.key);

	// Se há alguma transmissão sendo feita retorna sem fazer nada,
	//	senão pega o endereços da linha atual sendo transmitida
	if (painel.line1.maxTX)			l = &painel.line1;
	else if (painel.line2.maxTX)	l = &painel.line2;
	else							return;

	// transmitir comando e os chars da linha
	pn403_Speaking();						// Inverte  barramento rs485 para falar
	uart_PutChar(l->buffer[l->indexTX++]);	// Envia para transmissão o char atual do buffer
	while (!uart_EmptyTx()); 				// espera pelo fim da transmissão para inverter o barramento rs485 para escuta
	pn403_Listing();						// Inverte  barramento rs485 para escutar

	// Se transmitimos todos os dados do buffer sinaliza que não é mais para transmistir
	if (l->indexTX == l->maxTX) l->maxTX = 0;
}
