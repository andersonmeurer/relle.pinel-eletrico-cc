#ifndef __CONFIG_LIB_UTILS_H
#define __CONFIG_LIB_UTILS_H

// UTILIZE pdOFF PARA OS GRUPOS DE FUNÇÕES QUE NÃO DESEJA USAR
#define UTIL_USE_DUMP 				pdOFF		// pdON disponibiliza a impressão de bytes em formato HEX e ASCII
#define UTIL_USE_XATOI 				pdON		// pdON disponibiliza o conversor de string para inteiro
#define UTIL_USE_XITOA  			pdOFF
#define UTIL_USE_DATETIME 			pdOFF
#define UTIL_USE_POT 				pdOFF
#define UTIL_USE_GET_NUMBER	 		pdOFF
#define UTIL_USE_ASCII2BYTE	 		pdOFF		// pdON disponibiliza os conversores de chars para inteiro
#define UTIL_USE_HEX2BYTE 			pdOFF		// pdON disponibiliza os conversores de hex para inteiro
#define UTIL_USE_PERCENT_INT		pdOFF		// pdON disponibiliza função de porcentagem com parametros tipo inteiros
#define	UTIL_USE_ACCESS_BUF_LE		pdOFF		// pdON disponibiliza funções de leitura e escrita de tipos u16 e u32 em buffers u8
												//	  em processsadores little endian
#define	UTIL_USE_ACCESS_BUF_BE		pdOFF		// pdON disponibiliza funções de leitura e escrita de tipos u16 e u32 em buffers u8
												//	  em processsadores big endian

#endif
