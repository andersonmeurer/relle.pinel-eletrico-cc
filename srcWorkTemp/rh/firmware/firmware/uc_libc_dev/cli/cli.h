//  $Id: monitor.h 285 2008-11-06 01:23:29Z jcw $
//  $Revision: 285 $
//  $Author: jcw $
//  $Date: 2008-11-05 20:23:29 -0500 (Wed, 05 Nov 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/monitor/monitor.h $

#ifndef __CLI_H
#define __CLI_H

#include "_config_cpu_.h"
#include "_config_lib_cli.h"
#include "stdio_uc.h"

typedef enum {
  	CMD_TYPE_LIST,
  	CMD_TYPE_FUNCTION
} tCLIcmd;

typedef struct sCLI {
  	const portCHAR *command;
  	portCHAR min_args;
  	portCHAR max_args;
  	tCLIcmd cmd_type;
  	union {
    	const void *trick_gcc;
    	int (*handler) (int argc, portCHAR **argv);
    	struct sCLI *cmd_list;
  	};
  	const portCHAR *description;
  	const portCHAR *parameters;
} tCLI;


int cli_Help (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)));
int cli_GetLine(u8 *buffer, int bufferLength, const char *prompt);
int cli_ArgsParse(char *cmd, char **argv, int sizeofArgv, int *argc);
int cli_ArgsDispatch (const tCLI *cl, int argc, char **argv);


#endif
