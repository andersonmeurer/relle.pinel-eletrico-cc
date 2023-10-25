#ifndef __EINT2_H
#define __EINT2_H

#include "_config_cpu_.h"
#include "_config_cpu_eint.h"
#include "vic.h"

#if !defined (EINT2_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO EXTERNA 2 NO _config_cpu_.h
#endif

int eint2_Init(u8 mode, u8 polar);
void eint2_SetIntHandler(void (*func)(void) );
void eint2_ClearIntHandler(void);

#if (EINT2_USE_FUNC_INLINE == pdON)
inline void eint2_SetMode(u8 mode);
#else
void eint2_SetMode(u8 mode);
#endif

#if (EINT2_USE_FUNC_INLINE == pdON)
inline void eint2_SetPolar(u8 polar);
#else
void eint2_SetPolar(u8 polar);
#endif

#endif
