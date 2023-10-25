#ifndef __CONFIG_LIB_STDIO_H
#define __CONFIG_LIB_STDIO_H

// ###################################################################################################################
// FORMATAÇÂO DE IMPRESSÃO
#define STDIO_USE_HEX			pdON			// %x, %X Hexadezimal output
#define STDIO_USE_FLOAT			pdON			// %e, %f float output
#define STDIO_USE_BIN			pdON			// %b binario output
#define STDIO_USE_TIME 			pdON			// %t time output e funções de conversões
	#define STDIO_USE_WEEKDAY_PORTUGUESE pdOFF	// pdON o dia da semana será impresso em portugues, senão será em inglês
#define STDIO_USE_SUBINT 		pdON			// %k subdivições de inteiros K, M, G output

#endif
