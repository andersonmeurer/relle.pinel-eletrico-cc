/*
Não foi usado controle de tempo para o clock do sinal porque o mais rápido
que seja o uC que termos, não obteremos mais do que 20Mhz de clock de acesso,
onde o máximo permitido no CI é de 25Mhz

Para mais informações consute o documento tcl5917 - divers leds.docx
    D:\meus_documentos\_meu_saber\_devices_misc__\drivers
*/


#include <math.h>
#include "tlc5917.h"
#if defined(FREE_RTOS)
#include "systime.h"
#else
#include "delay.h"
#endif

static u8 FstOE; // Guarda o estado do OE
static void tlc5917_ModeNormal ( void );
static void tlc5917_ModeSpecial ( void );

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Inicializa as portas IO do tlc5917 com os pinos desligados
// Parametros: 	Nenhum 
// Retorna: 	Nada
//------------------------------------------------------------------------------------------------------------------
void tlc5917_Init ( void ) {
	// Configura os pinos CLK, SDI, OE e LE como saída e o
	//	e o pino SDO como entrada
	tlc5917_SetupPorts();

	tlc5917_clk(pdFALSE);
	tlc5917_sdi(pdFALSE);
	tlc5917_le(pdFALSE);
	tlc5917_SetOutput(pdTRUE);
}

// -----------------------------------------------------------------------------
// Habilita o driver de saída para os LEDs. Permitindo ligar ou desligar os LEDs
//  de acordo com as configurações no LATCH
// -----------------------------------------------------------------------------
void tlc5917_EnableOutput( void ) {
	tlc5917_oe(pdFALSE);
	FstOE = pdFALSE;
}

// -----------------------------------------------------------------------------
// Desabilita o driver de saída para os LEDs desligando todos os LEDs
//  independente das as configurações no LATCH
// -----------------------------------------------------------------------------
void tlc5917_DesableOutput( void ) {
	tlc5917_oe(pdTRUE);
	FstOE = pdTRUE;
}


// -----------------------------------------------------------------------------
// Habilita ou desabilita o driver de saída para os LEDs.
// Parametro:	pdTRUE para colocar o pino #OE em alta
//				pdFALSE para colocar o pino #OE em baixa
// -----------------------------------------------------------------------------
void tlc5917_SetOutput( u8 st ) {
	tlc5917_oe(st);
	FstOE = st;
}

// -----------------------------------------------------------------------------
// Retorna com estado do driver de saída dos LEDs
// -----------------------------------------------------------------------------
u8 tlc5917_StateOutput ( void ) {
	return FstOE;
}

// #############################################################################
// TROCA DE MODO DE OPERAÇÃO DO TCL5917
// #############################################################################
/*
Sempre que acessar o CI seja no modo normal ou de configuração envie o pedido
de troca de modo, mesmo que o pedido seja para o mesmo modo que está o CI.
Isto garante a sincronia entre os dados enviados pelo HOST com os estados do CIs.
Pode ocorrer que uma transmissão do HOST é cancelada antes de completar os
envios de todos os bits, e o CI fica no estado de espera de mais bits para
completar o seu quadro de bits a receber, e se o HOST enviar o pedido de troca
de modo na próxima comunicação o CI se reinicia para essa nova comunicação.
*/

// -----------------------------------------------------------------------------
// Coloca o TCL5917 no modo de trabalho normal, ou seja, para HOST acessar o
// LATCH do CI para determinar quais LEDs serão acionados ou não
// -----------------------------------------------------------------------------
//                   1       2       3       4       5
//           CLK ____HHHH____HHHH____HHHH____HHHH____HHHH
//           #OE HHHHHHHH________HHHHHHHHHHHHHHHHHHHHHHHH
//           LE  ________________________________________
//   modo antigo | chama atenção     | troca de      | Modo normal
//   normal ou   | do CI             | modo          |
//   de especial
void tlc5917_ModeNormal ( void ) {
    u8 i;
    u8 stOE;

    stOE = tlc5917_StateOutput(); // Captura o estado anterior do pino OE
    tlc5917_le(pdFALSE);

    for(i=1; i<=5; i++) {
        // CHAMA ATENÇÃO DO TCL NO PEDIDO DE TROCA DO MODO (101)
        if (i==2) tlc5917_oe(pdFALSE);
        else 	  tlc5917_oe(pdTRUE);

        // GERAR UM PULSO DE CLOCK
        tlc5917_clk(pdTRUE);
        tlc5917_clk(pdFALSE);
    }

    tlc5917_SetOutput(stOE); // Retorna na condição original do pino OE
}


// -----------------------------------------------------------------------------
// Coloca o TCL5917 no modo de trabalho escpecial, ou seja, para HOST acessar o
// os registradores de controle de corrente e leitura de status dos LEDs
// -----------------------------------------------------------------------------
//
//                   1       2       3       4       5
//           CLK ____HHHH____HHHH____HHHH____HHHH____HHHH
//           #OE HHHHHHHH________HHHHHHHHHHHHHHHHHHHHHHHH
//           LE  ________________________HHHHHHHH________
//   modo antigo | chama atenção     | troca de      | Modo especial
//   normal ou   | do CI             | modo          |
//   de especial
void tlc5917_ModeSpecial ( void ) {
    u8 i;
    u8 stOE;

    stOE = tlc5917_StateOutput(); // Captura o estado anterior do pino OE

    for (i=1; i<=5;i++) {
        // CHAMA ATENÇÃO DO TCL NO PEDIDO DE TROCA DO MODO (101)
        if (i==2)	tlc5917_oe(pdFALSE);
        else 		tlc5917_oe(pdTRUE);

        // SINALIZA AO TCL PARA TRANSITAR DO MODO NORMAL PARA O MODO ESPECIAL
        if (i==4)	tlc5917_le(pdTRUE);
        else 		tlc5917_le(pdFALSE);

        // GERAR UM PULSO DE CLOCK
        tlc5917_clk(pdTRUE);
        tlc5917_clk(pdFALSE);
    }

    tlc5917_SetOutput(stOE); // Retorna na condição original do pino OE
}

// -----------------------------------------------------------------------------
// Esta função encontra os valores dos bits do registrador de conrtole de
//  corrente nos CIs de acordo com a amperagem desejada e o resistor externo
//  a ser usado
// Parametro 1: Valor em ohms do resistor externo.
//                  Valores válidos entre 500 a 4000
// Parametro 2: O valor da amperagem em mA a ser encontrada
// Parametros 3a5: Variáveis dos bits dos registradores. Se a função encontrar
//                     a corrente desejada, essas variáveis retornarão com os
//                     valores para ser escrito no registrador
// Retorna: A função retorna TRUE se a função encontrou os valores para a
//              amperagem desejada.
//          A função retorna FALSE se não foi encontrada a amperagem desejada,
//              ou os parametros 1 e 2 estão fora dos limites permitidos
//  REGISTRADOR DE AJUSTE DE CORRENTE
//  D7  D6  D5  D4  D3  D2  D1  D0
//  CM  HC  CC0 CC1 CC2 CC3 CC4 CC5
// -----------------------------------------------------------------------------
int tlc5917_RegCalcBits(u16 r_ext, u8 iout, u8 *cmBit, u8 *hcBit, u8 *ccBits) {
	s8 i,j,k;
    double vg;
    double Vref;
    double Iref;
    double IoutComp;

	#if (TLC5917_USE_DEBUG == pdON)
    plog("TLC: r_ext=%u iout=%u"CMD_TERMINATOR, r_ext, iout);
	#endif

    if ( (r_ext < 500) || (r_ext > 4000) || (iout < 2) || (iout > 120) )
        return pdFALSE;

    for (i=1; i>=0; i--) {
        *cmBit = i;
        for (j=1; j>=0; j--) {
            *hcBit = j;
            for(k=63; k>=0; k--) {
                *ccBits = k;

                vg = ((1.0+*hcBit) * (1.0+(*ccBits/64.0)))/4.0;
                Vref = 1.26 * vg;
                Iref = Vref/r_ext;
                IoutComp = Iref * 15 * pow(3, (*cmBit-1));

                // Multiplicamos por 1000, porque o resultado acima é em Amperes e vamos
                //  comparar em mA
                IoutComp *= 1000;

                if ( IoutComp <= (double)iout ) {
                	return pdTRUE;
                }
            }
        }
    }

    return pdFALSE;
}

// #############################################################################
// COMUNICAÇÃO COM CI TCL5917
// #############################################################################
// -----------------------------------------------------------------------------
// Procedimento para acessar o LATCH do CI para determinar quais os LEDs serão
//      ligados ou não.
//      O CI DEVE ESTAR NO MODO DE TRABALHO NORMAL
//      O primeiro byte enviado será atribuído ao último CI da cascata, logo
//          o último byte enviado será atribuído ao primeiro CI da cascata

// Parametro 1: Vetor de bits do estado de ligado ou desligado dos LEDs
//                  Para cada grupo de 8 bits corresponde a um CI
// Parametro 2: Quantidade de bytes contido do parametro 1, ou seja,
//                  a quantidade de CIs em cascata que vamos acessar
// -----------------------------------------------------------------------------

//            1            2           3       ...        X (multiplo de 8)
//  CLK ______HHHHHH_______HHHHHH______HHHHHH       ______HHHHHH________________
//  SDI   T   vC1L1    T   vC1L2   T   vC1L3   ...    T   vCxLy
//  LE  _____________________________________  ... __________________HHHHHH_____
//                                                                     HL
//      T = Troca valor do bit no barramento SDI
//      vCxLy = Valor do bit para o CI x led y
//      HL  = Habilita Latch
void tlc5917_WriteLed(u8 *stBits, u8 nBytes) {

    u8 i,j;
    u8 *pBits;

    pBits = stBits;

    // AJUSTANDO O CI A TRABALHAR NO MODO NORMAL
    tlc5917_ModeNormal();

    // FAZER PARA TODOS OS CIs TLC5917
    for (j=0; j<nBytes; j++){
        // FAZER PARA TODOS O BITS DO CI ATUAL
        for(i=0; i<8; i++) {
            // AJUSTAR O PINO SDI
            if (*pBits & (1 << i) ) // Checa se o bits do CI atual está ativado
            		tlc5917_sdi(pdTRUE);
            else  	tlc5917_sdi(pdFALSE);
            // EMITIR UM PULSO DE CLOCK
            tlc5917_clk(pdTRUE);
            tlc5917_clk(pdFALSE);
        }

        pBits++;   // Aponta para o próximo CI (byte) ne stBist
    }

    // HABILITA O LATCH DOS CIS PARA CAPTURAR OS BITS DO SHIFT REGISTER
    //     O shift register converte o dado serial para paralelo dentro do tlc5917
    tlc5917_le(pdTRUE);
    tlc5917_le(pdFALSE);
}

// -----------------------------------------------------------------------------
// Procedimento para ajustar o nível de corrente no CI
//      O CI DEVE ESTAR NO MODO DE TRABALHO ESPECIAL
//      O primeiro byte enviado será atribuído ao último CI da cascata, logo
//          o último byte enviado será atribuído ao primeiro CI da cascata

// Parametro 1: Vetor de valores da corrente de cada CI
// Parametro 2: Quantidade de bytes contido do parametro 1, ou seja,
//                  a quantidade de CIs em cascata que vamos acessar
// -----------------------------------------------------------------------------

//         1    ...     7    ...     1   ...    7
//  CLK ___HHH  ...  ___HHH  ...  ___HHH ... ___HHH__________
//  SDI T  vC1curr           ...  T  vCXcurr
//  #OE HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH__________
//  LE  _____________________________________________HHH_____
//                                                   HL
//      T = Troca valor do bit no barramento SDI
//      vCXcurr= Valor da corrente do CIx
//      HL  = Habilita Latch de configuração da corrente
void tlc5917_WriteCurrent(u8 *stBits, u8 nBytes) {
    u8 i,j;
    u8 *pBits;
    u8 stOE;

    stOE= tlc5917_StateOutput(); // Captura o estado anterior do pino OE
    tlc5917_SetOutput(pdTRUE); // Desativa #OE

    pBits = stBits;

    // AJUSTANDO O CI A TRABALHAR NO MODO ESPECIAL
    tlc5917_ModeSpecial();

    // FAZER PARA TODOS OS CIs TLC5917
    for (j=0; j<nBytes; j++) {
        // FAZER PARA TODOS O BITS DO CI ATUAL
        for(i=0; i<8; i++) {
            // AJUSTAR O PINO SDI
            if ( *pBits & (1 << i)) // Checa se o bits do CI atual está ativado
        			tlc5917_sdi(pdTRUE);
            else  	tlc5917_sdi(pdFALSE);

            // EMITIR UM PULSO DE CLOCK
            tlc5917_clk(pdTRUE);
            tlc5917_clk(pdFALSE);
        }

        pBits++;   // Aponta para o próximo CI (byte) ne stBist
    }

    // HABILITA O LATCH DOS CIS PARA CAPTURAR OS BITS DO SHIFT REGISTER
    //     O shift register converte o dado serial para paralelo dentro do tlc5917
    tlc5917_le(pdTRUE);
    tlc5917_le(pdFALSE);

    tlc5917_SetOutput(stOE); // Restaura o estado anterior do pino OE
}

// -----------------------------------------------------------------------------
// Procedimento para ler o registrador de status, onde cada bit representa
//      um LED no CI. Se o bit for lido 1 sinaliza que está tudo bem com o LED.
//      Caso for lido 0 sinaliza que há problemas com o LED tal como:
//          - Curto circuito
//          - Circuito aberto
//          - Aquecimento demasiado no driver e o mesmo se desligou

//      O CI DEVE ESTAR NO MODO DE TRABALHO ESPECIAL E COM SINAL #OE ATIVADO
//      Para medir todos os LEDs devem estar ligados para forncer a corrente
//          ao circuito para medição. O sinal #OE a função controla para poder
//          mensurar
//      O primeiro byte recebido é primeiro CI da cascata

// Parametro 1: Vetor de status para ser atualizados
// Parametro 2: Quantidade de bytes contido do parametro 1, ou seja,
//                  a quantidade de CIs em cascata que vamos acessar
// -----------------------------------------------------------------------------

//
//         1     2     3        Wait       4     5     6     7     8     9     10    11
//  CLK ___HHH___HHH___HHH_________________HHH___HHH___HHH___HHH___HHH___HHH___HHH___HHH___
//  #OE HH_______________________________HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
//  SDO ---------------------------------| B7  | B6  | B5  | B4  | B3  | B2  | B1  | B0  |-
//  LE  ___________________________________________________________________________________
//
//      Wait: Esperar 2uS entre o clock 3 e 4, para que o status seja carregado
//          para o registrador
//      B[7:0]: Bits no registrador de status dos respectivos LEDs;

void tlc5917_ReadStatus(u8 *stBits, u8 nBytes) {
	u8 i,j;
    u8 *pBits;
    u8 stOE;

    stOE = tlc5917_StateOutput(); 	// Captura o estado anterior do pino OE
    tlc5917_SetOutput(pdFALSE); 		// Habilita #OE

    pBits = stBits;

    // AJUSTANDO O CI A TRABALHAR NO MODO ESPECIAL
    tlc5917_ModeSpecial();

    // EMITE TRES CLOCKS
    for (j=0; j<3; j++) {
        tlc5917_clk(pdTRUE);
        delay_us(1); // Foi colocado este delay, porque no ARM os pulsos são gerados de forma rápida sem que o sinal cai a zero
        tlc5917_clk(pdFALSE);
        delay_us(1);
    }

    // Datasheet especifica esperar o mínimo de 2uS entre o primeiro clock até a
    //	leitura do registrador de status
    //delay_us(2);

    tlc5917_SetOutput(pdTRUE); 		// Desabilita #OE

    // FAZER PARA TODOS OS CIs TLC5917
    for (j=0; j<nBytes; j++) {
        *pBits = 0;

        // FAZER PARA TODOS O BITS DO CI ATUAL
        for(i=0; i<8; i++) {
            // AJUSTAR O VETOR DE ACORDO COM O PINO SDO
            if (tlc5917_sdo())
                *pBits = *pBits | (1 << i);

            // EMITIR UM PULSO DE CLOCK
            tlc5917_clk(pdTRUE);
            tlc5917_clk(pdFALSE);
        }
        pBits++;   // Aponta para o próximo CI (byte) ne stBist
    }

    tlc5917_SetOutput(stOE); // Restaura o estado anterior do pino OE
}
