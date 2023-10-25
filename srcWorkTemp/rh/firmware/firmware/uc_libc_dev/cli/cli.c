//
//  $Id: args.c 56 2008-10-05 03:27:02Z jcw $
//  $Revision: 56 $
//  $Author: jcw $
//  $Date: 2008-10-04 23:27:02 -0400 (Sat, 04 Oct 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/monitor/args.c $
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cli.h"

static char *strtrim (char *s);
const tCLI *cmd_list_active = NULL;
#if defined(SIM_PROTEUS)
static int initGet = 1;  // Sinaliza se é incio da entrada de comandos
#else
static u8 initGet = 1;  // Sinaliza se é incio da entrada de comandos
#endif
static u8 *p;
static u8 c;

// -----------------------------------------------------------------------------------------------------------------
// Descrição:	Fica em loop para capturar dados vindo do console.
// Parametros: 	*buffer: Ponteiro do buffer
//				bufferLength: tamanho do buffer
//				*prompt: Prompt para ser exibido
// Retorna	:  A quantidade de dados processado no buffer
// -----------------------------------------------------------------------------------------------------------------
//  bufferLength includes the space reserved for the \0
int cli_GetLine (u8 *buffer, int bufferLength, const char *prompt) {
  	if (initGet) { // Checa se é incio da entrada de comandos
  		lprintf("%s", prompt);
  	  	p = buffer;
  	  	*p = '\0';
  	  	initGet = 0;
  	}

  	//while (1) { // FOI RETIRADO PARA QUE NÃO FIQUE NO LOOP ATÉ CHEGADA DO ENTER, PARA QUE OUTRAS FUNÇÕES SEJAM PROCESSADAS COMO UIP
    if (lgetchar(&c) == pdPASS) {
	  	switch (c) {
		case '\r' : // por causa da tecla enter do prompt no PROTEUS que envia \r
    	case '\n' : // estamos usando o finalizar de comandos "\n" ou "\r\n", logo  vamos podemos ignorar o "\r" e usar "\n"
    		lprintf(CMD_TERMINATOR);
    		initGet = 1;
    		return strlen ((char *) buffer);

    	case '\b' :
    		if (p > buffer) {
    			*--p = '\0';
        		lprintf("\b \b");
        	}
        	break;
        	break;

	    case 0x15 : // CTRL-U
    		while (p != buffer) {
    			lprintf("\b \b");
            	--p;
	        }
      		*p = '\0';
      		break;

	    case 0xfe :
    	case 0xff :
        	*buffer++ = c;
        	*buffer = '\0';
        	return 1;
	        		
	    default :
	    	if (p < buffer + bufferLength - 1 && c >= ' ' && c < 0x7f) {
        		*p++ = c;
            	*p = '\0';
            	lprintf("%c", c);
          	}

	    	//lprintf("%c", c);
	    	break;
      	}
    }
  	//}

  	return 0;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição:	Exibe a lista de comandos de help dos comandos
// Parametros:
// Retorna:
// -----------------------------------------------------------------------------------------------------------------
int cli_Help (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused))) {
  	unsigned int i;
  	int t;
  	int longestCmd;
  	portCHAR spaces [32];

  	memset (spaces, ' ', sizeof (spaces));

  	for (longestCmd = 0, i = 0; cmd_list_active[i].command; i++)
    	if ((t = strlen (cmd_list_active[i].command)) > longestCmd)
      		longestCmd = t;

  	spaces [longestCmd] = '\0';

  	for (i = 0; cmd_list_active[i].command; i++) {
    	const tCLI *cl = &cmd_list_active[i];

    	if (cl->cmd_type == CMD_TYPE_FUNCTION)
    		lprintf("%s%s -- %s"CMD_TERMINATOR, cl->command, &spaces [strlen (cl->command)], cl->description);
    	else
    		lprintf("%s%s -- " CLI_MSG_SUB_LIST " - %s"CMD_TERMINATOR, cl->command, &spaces [strlen (cl->command)], cl->description);
  	}

  	lprintf(CLI_MSG_HELP);

  	return pdPASS;
}


// -----------------------------------------------------------------------------------------------------------------
// Descrição: Captura os endereços do buffer do comando (cmd), que se referem a cada argumento, e grava os endereços das posições no vetor argv
//      Esta função encontra os argumentos contidos no buffer do comando (cmd), e separa-os com finalizador de string nulo (0)
//      argv[0] = primeiro argumento, que é o endereço inicial do comando do buffer cmd, e as demais posições sucessivas são dos argumentos
//		OBS: Os caracteres white-space são 9,10,11,12,13 e 32 = '\t' '\r\n' '\v' '\f' '\r' e ' '
// Exemplo:
//		cmd = set screen 720 480
//							addr cmd 	00 01 02 03 04 05 06 08 09 10 11 12 13 14 15 16 17 18
//			     			cmd	 	  	 s  e  t     s  c  r  e  e  n     7  2  0     4  8  0
//							addr argv 	00 01 02 03 04 05 06 08 09 10 11 12 13 14 15 16 17
//		argv aponta para:	argv     	00 04 12 16
// Parametros: 
//				*cmd:		Ponteiro da string do buffer do comando
//				**argv: 	Endereço do vetor de endereços para armazenar as posições dos argumentos contidos no cmd
// 				sizeofArgv: Sinaliza a qantidade de posições disponíveis no argv
//				*argc: 		Ponteiro para inteiro. Este guarda a quantidade de argumentos encontrados no comando
// Retorna:	
//				Retorna psPASS se a operação foi feita para todos os argumentos
//				Retorna errCLI_EXED_ARGS se a quantidade de argumetos superou a quantidade máxima
// -----------------------------------------------------------------------------------------------------------------
typedef enum {
  	P_WAIT_INIT_ARGS = 0, // Estado de espera do inicio do argumento
  	P_GET_INIT_ARGS,
  	P_SET_END_ARGS,
  	P_SET_END_QUOTE
} p_state_t;

int cli_ArgsParse (char *cmd, char **argv, int sizeofArgv, int *argc) {
  	int max_args = (sizeofArgv / sizeof (argv [0])) - 1;	// Determina a quantidade máximo de argumetos que podem ser gravados no argv
  	char *s = strtrim (cmd); 								// Retira espaços em branco ou caracteres de controle do lado direto ou esquerdo da string e captura o endereço do comando
  	
  	p_state_t pstate = P_WAIT_INIT_ARGS;					// Sinalizo para esperar pelo separado de argumentos que é o espaço em branco

  	*argc = 0;												// Sinaliza que ainda não capturou nenhum argumento
  	memset (argv, 0, sizeofArgv); 							// Preenche os endereços do argv com valores 0, ou seja, limpa todos os endereços
  	
  	while (*s) {											// Fazer para toda os caracteres do comando
    	switch (pstate) {
      		// PROCURA O INICIO DO ARGUMENTO
      		case P_WAIT_INIT_ARGS :  {						// Checa se estamos esperando pelo separador de argumentos, que é o espaço em branco
          		if (!isspace ((int)*s))  						// Checa se a posição atual não é um white-space, sinalizador de inicio de arqumento
            		pstate = P_GET_INIT_ARGS;				// Sinalizo para ir para o estado de captura do argumento atual
          		else
            		s++;									// Aponto para o próximo endereço do caractere do comando
        	}
        	break;

      		// CAPTURO A POSIÇÃO, ENDEREÇO, INICIAL DO ARGUMENTO NO VETOR DE COMANDOS
      		case P_GET_INIT_ARGS : {
          		*argv++ = s; 								// Captura o endereço S, posição X do buffer, e aponta para o próxima posição do ponteiro de argumentos

          		if (++*argc == max_args)					// Incremento a quantidade de argumetos encontrado e checa se está no limite máximo de argumentos
        	    	return errCLI_EXED_ARGS;					// Retorna erro
	          	if (*s == '"')								// Checa se encontramos o caractere aspa. Sinaliza inicio do parametro de string contend espaços em branco internos
    	        	pstate = P_SET_END_QUOTE;				// Vai ao estado de encontrar fim das aspas, isto porque nesta condição consideramos o espaço em branco como conteúdo do parametro 
        	  	else
            		pstate = P_SET_END_ARGS;				// Vai ao estado de de encontrar fim do argumento

	          	s++;										// Aponto para o próximo endereço do caractere do comando
    	    }
        	break;

      		// ENCONTRA FIM DO ARGUMENTO E SINALIZA NA POSIÇÃO, ENDEREÇO, DO COMANDO COM O CARACTERE NULO
      		case P_SET_END_ARGS : {
          		if (isspace ((int)*s)) {							// Checa se o caractere atual é um whitespace
            		pstate = P_WAIT_INIT_ARGS;				// Sinaliza para ir a estado para captura do prróximo argumento
            		*s = '\0';								// Sinaliza na posição atual que é o fim do argumento atual
          		}  else if (*s == '"')						// Checa se encontramos o caractere aspa. Sinaliza inicio do parametro de string contend espaços em branco internos
            		pstate = P_SET_END_QUOTE;				// Vai ao estado de encontrar fim do argumentos dentro das aspas, isto porque nesta condição consideramos o espaço em branco como conteúdo do parametor 

          		s++;										// Aponto para o próximo endereço do caractere do comando
       		}
        		break;

      		// ECONTRA O FIM DAS ASPAS PARA SINALIZAR FIM DE ARGUMENTO
      		case P_SET_END_QUOTE : {
          		if (*s == '"')	pstate = P_SET_END_ARGS;	// Checa se encontramos o caractere aspa. Sinaliza final do parametro e vá ao estado para procura do fim de argumento
          		
          		s++;										// Aponto para o próximo endereço do caractere do comando
        	}
        		break;
    	}
  	}

  	return pdPASS;												// Retorna que OK
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição:  Procura o comando digitado pelo usuário ,contindo no argv[0], na lista de comandos 
//                  Se encontrou o comando, chama a função anexado este comando passando:
//                      argc: Quantidade de argumentos a ser passado para a função
//                      argv: Os argumentos passado para a função menos o comando digitado
// Parametros:
//              cl: Ponteiro da lista de comandos
//              argc: Quantidade de argumentos encontrados no comando
//              argv: Lista de arqumentos encontrado no comando
// Retorna:    
//              Valor do retorno da função chamada
//              errCLI_CMD_NOT_FOUND: Se não encontrou o comando digitado na lista de comandos
//              errCLI_MANY_ARGS: Caso tenha muitos argumentos para o comando atual
//              errCLI_FEW_ARGS: Caso tenha pouco argumentos para o comando atual
// -----------------------------------------------------------------------------------------------------------------
int cli_ArgsDispatch (const tCLI *cl, int argc, char **argv) {
  	cmd_list_active = cl;

  	while (cl->command) { 								// Percorre toda lista de comandos
    	if (!strcmp (cl->command, argv [0])) {			// Checa se o comando atual é o mesmo digitado
      		int ret = 0;

      		if ((argc == 2) && !strcmp (argv [1], "?")) { // Checa se foi digitaldo ? para o comando 
        		lprintf("%s"CMD_TERMINATOR, cl->parameters);		// Exibe ajuda para os parametros
        		ret = pdPASS;
      		} else if ((argc - 1) < cl->min_args) {			// Checa se a quantidade de argumentos digitados é menor que o mínimo permitido
        		lprintf(CLI_MSG_FEW_ARGS, cl->min_args);
        		ret = errCLI_FEW_ARGS;
      		} else if (cl->cmd_type == CMD_TYPE_LIST) {		// Checa se é um comando ou uma nome de outra lista
        		ret = cli_ArgsDispatch (cl->cmd_list, argc - 1, &argv [1]); // Chama essa a sub lista
      		} else if ((argc - 1) > cl->max_args) {			// Checa se a quantidade de argumentos digitados é maior que o máximo permitido
        		lprintf(CLI_MSG_MANY_ARGS, cl->max_args);
        		ret = errCLI_MANY_ARGS;
      		} else {
        		ret = (*cl->handler) (argc - 1, &argv [1]); // Chama a função anexado a esse comando
        	}

      		return ret;										// Retorna o valor de retorno da função ou 0 se não chamou função alguma
    	}

    	cl++;											// aponta para o próximo comando na lista 
  	}

  	//if (!cl->command)
    	//lprintf(CLI_MSG_UNKNOWN_CMD " \"%sCMD_TERMINATOR, argv [0]);

  	return errCLI_CMD_NOT_FOUND;
}

// -----------------------------------------------------------------------------------------------------------------
// Descrição: Retorna com o endereço inicial do comando na string e colocar o caractere nulo no fim do comando
//				Ou seja, retira os espaços em branco do lado dirieto e esquerdo da string
// Parametros: ponteiro da string
// Retorna:		Ponteiro da strinf na posição onde começa o comando
// -----------------------------------------------------------------------------------------------------------------
static char *strtrim (char *s) {
  	char *t = s + strlen (s) - 1; 			// Aponta o endereço para o último caractere da string

  	// COLOCO O FINALIZAR LOGO APÓS O COMANDO. OU SEJA, RETIRA OS ESPAÇOS EM BRANCO DO LADO DIREITO DA STRING
  	while (t >= s && *t && isspace ((int)*t)) 	// Faça enquanto o endereço t seja maior ou igual ao endereço da string e se o caractere for um whitespace
  		*t-- = '\0'; 						// Posição atual da string recebe nulo e aponto para o endereço anterior na string

  	// ENCONTRA O INICIO DO COMANDO. OU SEJA, RETIRA OS ESPAÇOS EM BRANCO DO LADO ESQUERDO DA STRING
  	while (*s && isspace ((int)*s)) 		// Faça enquanto o endereço s seja menor que o endereço final da string e se o caractere for um whitespace
	    s++; 								// aponto para o próxinmo endereço da string

  	return s;								// Retorna com a posição inicial do char inicio do comando
}
