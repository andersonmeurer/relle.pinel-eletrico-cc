#include "gkey.h"

// ###########################################################################################
// CONTROLE DO TECLADO
// ###########################################################################################

// COMO USAR
//	declarar as fun��es que deseja processar a tecla pressionada
//	EX:
//		onKeyDown	- Fun��o a ser chamada quando a tecla for pressionada
//		onKeyPress	- Fun��o a ser chamada quando a tecla for mantida pressionada
//		onKeyUp		- Fun��o a ser chamada quando a tecla for solta

// anexar as fun��es as seus respectivos eventos ao inicar o sistema
// gKey_Init(&onKeyDown, &onKeyPress, &onKeyUp);

// chamar periodicamente a fun��o de leitura do teclado e gerador de evento
//	A leitura do teclado pode ser de uma serial ou um painel, neste exemplo estamos lendo do painel pnl403
// ATEN��O: Tecla pressionada deve ter valor > 0. Nenhuma tecla pressionada deve retornar valor 0
//	void tickSystem (void) { // gerado a cada 1ms
//		pnl403_Tick(); // a chamada para esta fun��o n�o pode ser menor que 30uS
//
//		static tTime timeRefresh = 0;
//		if (now() > timeRefresh) { // Checa se � tempo de atualia a tela
//			timeRefresh = now() + 50;
//
//			static u8 key = 0;
//			pnl403_GetKey(&key);
//			// Gera os eventos onKeyDown, onKeyPress e onKeyUp caso a tecla ou touch foi pressionados
//			key_Process(key); // OU COLOCAR NO LOOP PRINCIPAL DO SISTEMA
//		}
//	}

int gkey_FastInc; // sinaliza para o processos externos para incrementar ou decrementar de forma mais r�pida quando a tecla ficar pressionada

#define keyREPEAT keyFASTINC_REPEAT*1000/keyKEYPRESS_REPEAT2;

extern tTime now(void);
static pkeyFunc onKeyDown = NULL;
static pkeyFunc onKeyPress= NULL;
static pkeyFunc onKeyUp = NULL;

// Inicializa que os eventos a serem chamados para onKeyDown, onKeyPress e onKeyUp
// Coloque valor NULL caso n�o queira que a fun��o seja invocada
//	EX: gkey_Init(onKeyDown, &onKeyPress, &onKeyUp);
		// se desejar processar a tecla quando a mesma for pressionada, durante o pressionamento
		//		e quando soltar a tecla
//		gkey_Init(NULL, NULL, &onKeyUp);
		// se desejar processar a tecla somente a mesma for solta
void gKey_Init(pkeyFunc onKdown, pkeyFunc onKpress, pkeyFunc onKup) {
	onKeyDown = onKdown;
	onKeyPress = onKpress;
	onKeyUp = onKup;
	gkey_FastInc = 0; // sem repeti��o
}

// Gerador de enventos para tecla pressioada
// A fun��o gKey_Process vai chamar as fun��es anexadas em onKdown, onKpress, onKup em gKey_Init
// passando o valor da tecla pressionada. S�o nessas fun��es que iremos processar a tecla
// ATEN��O: Tecla pressionada deve ter valor > 0. Nenhuma tecla pressionada deve retornar valor 0
void gKey_Process(char key) {
    static char keyold=0, keynow=0, estado=0;
    static tTime time=0;
    static uint repeat=0;

    //verifica��o da tecla
    keyold = keynow;
    keynow = key;

    //repeti��o rapida?
    //gkey_FastInc = repeat==0;					// Sinaliza se � para incrementar/decrementar mais r�pido quando a tecle permanecer pressionada

    switch (estado) {
    case 0: 									// Estado de verifica��o se tecla foi pressionada
    	repeat = keyREPEAT;						// A cada X segundos incrementos o gkey_FastInc
		gkey_FastInc = 0;						// Sinaliza para n�o incrementar de modo r�pido
    	if ((keyold==0)&&(keynow!=0)) {			// Checa se � a descida da tecla, se sim emitir o evento onKeyDown e onKeyPress
            if (onKeyDown) onKeyDown(key);		// Emitir o evento onKeyDown e onKeyPress
            if (onKeyPress) onKeyPress(key);	// Emitir o evento onKeyPress
            time = now() + keyKEYPRESS_REPEAT1;	// Determina em que tempo vamos repetir o evento onKeyPress ap�s a primeira vez
    		estado = 1;							// V� para o estado de verifica��o de tecla precionada
    	}
    	break;
    case 1: 									// Estado de verifica��o se a tecla esta sendo pressionada ou tecla foi solta
    	if ((keyold!=0)&&(keynow==0)) {			// Checa se � a subida da tecla, se sim emitir o evento onKeyUp
    		if (onKeyUp) onKeyUp(keyold);
    		estado = 0;							// Volta ao estado para esperar a pr�xima tecla a ser pressionada
    	} else if (now()>time) {				// Checa se est� na hora de repetir o evento onKeyPress
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
