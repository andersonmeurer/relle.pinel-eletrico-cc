#ifndef __CONFIG_LIB_CLI_H
#define __CONFIG_LIB_CLI_H

#include "_config_lib_.h"

// DEFINE O TAMANHO DO BUFFER PARA ENTRADA DE COMANDOS
#define CLI_CMD_BUFFER_SIZE 40

// DEFINE A QUANTIDADE DE ARGUMENTOS MÁXIMO PERMITIDO POR VEZ
#define CLI_N_ARGS 10

// MENSAGENS USADAS NA APLICAÇÃO
#define CLI_MSG_MEM_LOW_ARGS	"Insufficient memory to allocate %d arguments"CMD_TERMINATOR // Na mesagem tem que ter um %d para exibir a quantidades de argumentos
#define CLI_MSG_CMD_UNKNOW		"Unknown command"CMD_TERMINATOR
#define CLI_MSG_CMD_EXEC_ERROR	"Error in command execution"CMD_TERMINATOR

// MENSAGENS USADAS NA LIB CLI
#define CLI_MSG_FEW_ARGS 		"Missing arguments (requires %d)"CMD_TERMINATOR 				// Na mesagem tem que ter um %d para exibir a quantidades de argumentos necessários
#define CLI_MSG_MANY_ARGS 		"Many arguments (maximum %d)"CMD_TERMINATOR					// Na mesagem tem que ter um %d para exibir a quantidades de argumentos excedidos
#define CLI_MSG_SUB_LIST 		"SUBLIST"
#define CLI_MSG_HELP 			CMD_TERMINATOR"Use '<command> ?' to show its parameters"CMD_TERMINATOR

//// MENSAGENS USADAS NA APLICAÇÃO
//#define CLI_MSG_MEM_LOW_ARGS	"Memória insuficiente para alocar %d argumentos"CMD_TERMINATOR // Na mesagem tem que ter um %d para exibir a quantidades de argumentos
//#define CLI_MSG_CMD_UNKNOW		"Comando desconhecido"CMD_TERMINATOR
//#define CLI_MSG_CMD_EXEC_ERROR	"Erro na execução do comando"CMD_TERMINATOR
//
//// MENSAGENS USADAS NA LIB CLI
//#define CLI_MSG_FEW_ARGS 		"Falta argumentos (é necessário %d)"CMD_TERMINATOR 				// Na mesagem tem que ter um %d para exibir a quantidades de argumentos necessários
//#define CLI_MSG_MANY_ARGS 		"Muitos argumentos (máximo %d)"CMD_TERMINATOR					// Na mesagem tem que ter um %d para exibir a quantidades de argumentos excedidos
//#define CLI_MSG_SUB_LIST 		"SUBLISTA"
//#define CLI_MSG_HELP 			CMD_TERMINATOR"Utilize '<command> ?' para ver os parametros do comando"CMD_TERMINATOR


#endif
