#include "app.h"
#include "utils.h"
#include "cli.h"
#include <math.h>
#include <stdlib.h>

#if (MODE_DEVELOPMENT == pdOFF)
#error ESTE MÓDULO NÃO PODE ESTAR ADICIONADO NA VERSÃO FINAL DO FIRMWARE
#error no makefile retire os arquivos fontes "uart0_irq.c", "prompt.c" e "cli.c"
#endif

static int prompt_ProcessCommand(void);
static int prompt_Relay (int argc, portCHAR **argv);
static int prompt_STS(int argc, portCHAR **argv);

// ###############################################################################################
// LISTA DOS COMANDOS A SER CHAMADOS PELO CLI

static const tCLI cli_list [] = {
	{ "help",
  			0,  0,
  			CMD_TYPE_FUNCTION, { cli_Help },
  			"Displays this help list", NULL },

	{ "relay",
  			2,  2,
  			CMD_TYPE_FUNCTION,  { prompt_Relay },
  			"relay on or off", 	"relay <1..8> <on/off>" },

	{ "sts",
  			0,  0,
  			CMD_TYPE_FUNCTION,  { prompt_STS },
  			"Shows all status of the ", 	NULL },


	{ NULL, 0,  0, CMD_TYPE_FUNCTION,  	{ NULL },
			NULL,
			NULL },
};

void prompt_Process(void) {
	static int resuslt;
	resuslt = prompt_ProcessCommand();
	switch (resuslt) {
	case errCLI_EXED_ARGS:
		lprintf("System error"CMD_TERMINATOR);
		break;
	case errCLI_CMD_NOT_FOUND:
		lprintf("Unknown command"CMD_TERMINATOR);
    	break;
    case pdFAIL:
    	lprintf("Error in executing the command"CMD_TERMINATOR);
    	break;
    }
}

static int result_prompt;
static u8 buffer_prompt[CLI_CMD_BUFFER_SIZE];
static portCHAR *argv_prompt [CLI_N_ARGS];
static int argc_prompt;

static int prompt_ProcessCommand(void) {
	// CAPTURA COMANDOS DO TERMINAL. SE TECLOU ENTER A FUNÇÃO RETORNA COM A QUANTIDADE DE CHAR DO COMANDO
	result_prompt = cli_GetLine (buffer_prompt, sizeof (buffer_prompt), PROMPT); // Chamo o console para entrada de comandos

	// CHECA FOI ENTRADO COM O COMANDO
	if (result_prompt) {	// checa se há entrada de comandos
   		if (cli_ArgsParse ((char *) buffer_prompt, argv_prompt, sizeof (argv_prompt), &argc_prompt) == errCLI_EXED_ARGS) // Captura os endereços iniciais de cada argumento, contidos no comando, e grava nas posições do argv
   			result_prompt = errCLI_EXED_ARGS;
   		else if (argv_prompt[0])
   			result_prompt = cli_ArgsDispatch(cli_list, argc_prompt, &argv_prompt[0]);							// Executa a função referente ao comando digitado
  	} else result_prompt = pdPASS;

	return result_prompt;
}


static int prompt_Relay (int argc __attribute__ ((unused)), portCHAR **argv) {
	long p1;
	uint st;
	uint act = control.relays;

	if (!xatoi(&argv[0], &p1))	{ return pdFAIL; }
	if ( (p1<=0) || (p1>8) ) 	{ return pdFAIL; }

	if (strcmp(argv[1], "on")) st = 0; else st = 1;

	if (st == 0) act &= ~(1<<(p1-1)); else act |= (1<<(p1-1));
	app_SetRelay(act);
	plognp("set relay %d = %s (act = 0x%x)"CMD_TERMINATOR, p1, (st == 0)?"off":"on");
	return pdPASS;
}

static int prompt_STS(int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused))) {
	lprintf(CMD_TERMINATOR);
	lprintf(CMD_TERMINATOR);
	lprintf(PROGAM_NAME);
	lprintf("ucLibc version "__VERSION CMD_TERMINATOR);

	lprintf(CMD_TERMINATOR);
	lprintf("MODBUS"CMD_TERMINATOR);
	lprintf("   Baudrate SBC = %d"CMD_TERMINATOR, BAUDRATE_SBC);
	lprintf("   Baudrate Multimeter = %d"CMD_TERMINATOR, BAUDRATE_MULTIMETER);
	lprintf("   Slave ID = %d"CMD_TERMINATOR, control.modbusID);

	lprintf(CMD_TERMINATOR);
	lprintf("MULTIMETERS"CMD_TERMINATOR);
	uint x; for(x=0;x<control.nMultimetersGeren;x++) {
		lprintf("   MULTIMETER[%d]: stsCom=0x%x sts=0x%x  value=%d"CMD_TERMINATOR,
			x+1,
			control.multimeter[x].stsCom,
			control.multimeter[x].sts,
			control.multimeter[x].valueMSW << 16 | control.multimeter[x].valueLSW
		);
	}

	return pdPASS;
}
