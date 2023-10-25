#ifndef __EINT1_H
#define __EINT1_H

#include "_config_cpu_.h"
#include "_config_cpu_eint.h"
#include "vic.h"

#if !defined (EINT1_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO EXTERNA 1 NO _config_cpu_.h
#endif

int eint1_Init(u8 mode, u8 polar);
void eint1_SetIntHandler(void (*func)(void) );
void eint1_ClearIntHandler(void);

#if (EINT1_USE_FUNC_INLINE == pdON)
inline void eint1_SetMode(u8 mode);
#else
void eint1_SetMode(u8 mode);
#endif

#if (EINT1_USE_FUNC_INLINE == pdON)
inline void eint1_SetPolar(u8 polar);
#else
void eint1_SetPolar(u8 polar);
#endif

#endif
