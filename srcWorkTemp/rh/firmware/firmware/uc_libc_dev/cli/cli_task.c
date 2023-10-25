#include "cli_task.h"
#include "stdio_uc.h"
#include "_config_lib_cli.h"
//#include "semphr.h"
//#include "queue.h"

//xSemaphoreHandle uart0_mutexTX = NULL;
//xSemaphoreHandle uart0_mutexRX = NULL;
//xQueueHandle uart0_queueTX = NULL;

static void vCliTask( void *pvParameters );
const cmd_list_t *cmd_list;
//###################################################################################################################
// UART CONFIGURAÇÔES
//###################################################################################################################
//###################################################################################################################

// -------------------------------------------------------------------------------------------------------------------
// Descrição: 	Incializa e configura o modo de trabalho da UART
// Parametros:	
// Retorna:		retorna pdPASS se iniciliazou sem problemas ou retorna código de erro
// -------------------------------------------------------------------------------------------------------------------

void vCliInit(const cmd_list_t *cl, unsigned portSHORT stack_size, unsigned int priority_task, xTaskHandle *task_handle) {
	taskENTER_CRITICAL();
	cmd_list = cl;
	xTaskCreate( vCliTask, ( signed portCHAR * ) "tu0", stack_size,( void * ) NULL, priority_task, task_handle );
	taskEXIT_CRITICAL();	
} 


// -------------------------------------------------------------------------------------------------------------------
// Descrição:	Tarefa que se encarrega de pegar conando do console e executa a função relacionado ao comando
// Parametros: 	Nenhum
// Retorna:		Nada
// -------------------------------------------------------------------------------------------------------------------
static void vCliTask( void *pvParameters __attribute__((unused)) ) {	
  	int l;
  	static u8 buffer_prompt [CLI_CMD_BUFFER_SIZE];
  	static portCHAR *argv [CLI_N_ARGS];
  	int argc;

	while(1) {
		if ((l = cli_GetLine (buffer_prompt, sizeof (buffer_prompt), PROMPT))) {						// Chamo o console para entrada de comandos
      		if (cli_ArgsParse ((char *) buffer_prompt, argv, sizeof (argv), &argc) == errCLI_EXED_ARGS) // Captura os endereços iniciais de cada argumento, contidos no comando, e grava nas posições do argv
        		lprintf(CLI_MSG_MEM_LOW_ARGS, arrsizeof (argv));
      		else if (argv [0])	l = cli_ArgsDispatch(cmd_list, argc, &argv [0]);							// Executa a função referente ao comando digitado

      		switch (l) {
	       	case errCLI_CMD_NOT_FOUND:
	       		lprintf(CLI_MSG_CMD_UNKNOW); break;
	       	case pdFAIL:
	       		lprintf(CLI_MSG_CMD_EXEC_ERROR); break;
	       	}
	  	}
	}
}

