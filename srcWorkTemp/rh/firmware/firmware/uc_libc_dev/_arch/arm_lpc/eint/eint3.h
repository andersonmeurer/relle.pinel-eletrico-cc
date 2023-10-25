#ifndef __EINT3_H
#define __EINT3_H

#include "_config_cpu_.h"
#include "_config_cpu_eint.h"
#include "vic.h"

#if !defined (EINT3_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO EXTERNA 3 NO _config_cpu_.h
#endif

int eint3_Init(u8 mode, u8 polar);
void eint3_SetIntHandler(void (*func)(void) );
void eint3_ClearIntHandler(void);

#if (EINT3_USE_FUNC_INLINE == pdON)
inline void eint3_SetMode(u8 mode);
#else
void eint3_SetMode(u8 mode);
#endif

#if (EINT3_USE_FUNC_INLINE == pdON)
inline void eint3_SetPolar(u8 polar);
#else
void eint3_SetPolar(u8 polar);
#endif

#endif
