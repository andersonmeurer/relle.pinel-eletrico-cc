#include "gkey.h"

// ###########################################################################################
// CONTROLE DO TECLADO
// ###########################################################################################

// COMO USAR
//	declarar as funções que deseja processar a tecla pressionada
//	EX:
//		onKeyDown	- Função a ser chamada quando a tecla for pressionada
//		onKeyPress	- Função a ser chamada quando a tecla for mantida pressionada
//		onKeyUp		- Função a ser chamada quando a tecla for solta

// anexar as funções as seus respectivos eventos ao inicar o sistema
// gKey_Init(&onKeyDown, &onKeyPress, &onKeyUp);

// chamar periodicamente a função de leitura do teclado e gerador de evento
//	A leitura do teclado pode ser de uma serial ou um painel, neste exemplo estamos lendo do painel pnl403
// ATENÇÃO: Tecla pressionada deve ter valor > 0. Nenhuma tecla pressionada deve retornar valor 0
//	void tickSystem (void) { // gerado a cada 1ms
//		pnl403_Tick(); // a chamada para esta função não pode ser menor que 30uS
//
//		static tTime timeRefresh = 0;
//		if (now() > timeRefresh) { // Checa se é tempo de atualia a tela
//			timeRefresh = now() + 50;
//
//			static u8 key = 0;
//			pnl403_GetKey(&key);
//			// Gera os eventos onKeyDown, onKeyPress e onKeyUp caso a tecla ou touch foi pressionados
//			key_Process(key); // OU COLOCAR NO LOOP PRINCIPAL DO SISTEMA
//		}
//	}

int gkey_FastInc; // sinaliza para o processos externos para incrementar ou decrementar de forma mais rápida quando a tecla ficar pressionada

#define keyREPEAT keyFASTINC_REPEAT*1000/keyKEYPRESS_REPEAT2;

extern tTime now(void);
static pkeyFunc onKeyDown = NULL;
static pkeyFunc onKeyPress= NULL;
static pkeyFunc onKeyUp = NULL;

// Inicializa que os eventos a serem chamados para onKeyDown, onKeyPress e onKeyUp
// Coloque valor NULL caso não queira que a função seja invocada
//	EX: gkey_Init(onKeyDown, &onKeyPress, &onKeyUp);
		// se desejar processar a tecla quando a mesma for pressionada, durante o pressionamento
		//		e quando soltar a tecla
//		gkey_Init(NULL, NULL, &onKeyUp);
		// se desejar processar a tecla somente a mesma for solta
void gKey_Init(pkeyFunc onKdown, pkeyFunc onKpress, pkeyFunc onKup) {
	onKeyDown = onKdown;
	onKeyPress = onKpress;
	onKeyUp = onKup;
	gkey_FastInc = 0; // sem repetição
}

// Gerador de enventos para tecla pressioada
// A função gKey_Process vai chamar as funções anexadas em onKdown, onKpress, onKup em gKey_Init
// passando o valor da tecla pressionada. São nessas funções que iremos processar a tecla
// ATENÇÃO: Tecla pressionada deve ter valor > 0. Nenhuma tecla pressionada deve retornar valor 0
void gKey_Process(char key) {
    static char keyold=0, keynow=0, estado=0;
    static tTime time=0;
    static uint repeat=0;

    //verificação da tecla
    keyold = keynow;
    keynow = key;

    //repetição rapida?
    //gkey_FastInc = repeat==0;					// Sinaliza se é para incrementar/decrementar mais rápido quando a tecle permanecer pressionada

    switch (estado) {
    case 0: 									// Estado de verificação se tecla foi pressionada
    	repeat = keyREPEAT;						// A cada X segundos incrementos o gkey_FastInc
		gkey_FastInc = 0;						// Sinaliza para não incrementar de modo rápido
    	if ((keyold==0)&&(keynow!=0)) {			// Checa se é a descida da tecla, se sim emitir o evento onKeyDown e onKeyPress
            if (onKeyDown) onKeyDown(key);		// Emitir o evento onKeyDown e onKeyPress
            if (onKeyPress) onKeyPress(key);	// Emitir o evento onKeyPress
            time = now() + keyKEYPRESS_REPEAT1;	// Determina em que tempo vamos repetir o evento onKeyPress após a primeira vez
    		estado = 1;							// Vá para o estado de verificação de tecla precionada
    	}
    	break;
    case 1: 									// Estado de verificação se a tecla esta sendo pressionada ou tecla foi solta
    	if ((keyold!=0)&&(keynow==0)) {			// Checa se é a subida da tecla, se sim emitir o evento onKeyUp
    		if (onKeyUp) onKeyUp(keyold);
    		estado = 0;							// Volta ao estado para esperar a próxima tecla a ser pressionada
    	} else if (now()>time) {				// Checa se está na hora de repetir o evento onKeyPress
            time = now() + keyKEYPRESS_REPEAT2; // Determina em que tempo vamos repetir o evento onKeyPress sucessivamente quando a tecla for mantida pressionada
            if (onKeyPress) onKeyPress(key);	// Chama o evento onKeyPress

            // checar o momento de incrementar a var FastInc
            if (repeat) repeat--;
            else {
            	repeat = keyREPEAT;
            	gkey_FastInc++;
            }
        }
    	break;
    }
}
