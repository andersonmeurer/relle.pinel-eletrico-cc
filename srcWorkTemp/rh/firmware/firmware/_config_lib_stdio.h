#ifndef __CONFIG_LIB_STDIO_H
#define __CONFIG_LIB_STDIO_H

// ###################################################################################################################
// FORMATA��O DE IMPRESS�O
#define STDIO_USE_HEX			pdON			// %x, %X Hexadezimal output
#define STDIO_USE_FLOAT			pdON			// %e, %f float output
#define STDIO_USE_BIN			pdON			// %b binario output
#define STDIO_USE_TIME 			pdON			// %t time output e fun��es de convers�es
	#define STDIO_USE_WEEKDAY_PORTUGUESE pdOFF	// pdON o dia da semana ser� impresso em portugues, sen�o ser� em ingl�s
#define STDIO_USE_SUBINT 		pdON			// %k subdivi��es de inteiros K, M, G output

#endif
