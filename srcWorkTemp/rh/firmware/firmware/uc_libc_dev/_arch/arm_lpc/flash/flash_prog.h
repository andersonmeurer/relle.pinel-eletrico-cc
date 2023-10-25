#ifndef __FLASH_PROG_H
#define __FLASH_PROG_H

#include "_config_cpu_.h"

#if defined(arm7tdmi)
#define VECTORTABLE_SIZE 64
// Requisitos do tamanho da cache
//	1 = Tem que ser na mesma quantidade de dados que se possa gravar na FLASH
//		, ou seja, 256, 512, 1024 ou 4096
//	2 = Deve conter o ser maior ou igual a 2*VECTORTABLE_SIZE para caber os vetores de ints
#define CACHE_SIZE 256
#endif

#if defined(cortexm3)
#define VECTORTABLE_SIZE 1024
// Requisitos do tamanho da cache
//	1 = Tem que ser na mesma quantidade de dados que se possa gravar na FLASH
//		, ou seja, 256, 512, 1024 ou 4096
//	2 = Deve conter o ser maior ou igual a 2*VECTORTABLE_SIZE para caber os vetores de ints
#define CACHE_SIZE 4096
#endif

int flashProg_Init (void);
int flashProg_SaveVectorBoot(void);
u32 *flashProg_DataStoreAddr(void);
int flashProg_Format(void);
int flashProg_Record(u8 *buff);
#endif
