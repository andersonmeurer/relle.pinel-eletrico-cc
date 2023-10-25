#ifndef __EINT0_H
#define __EINT0_H

#include "_config_cpu_.h"
#include "_config_cpu_eint.h"
#include "vic.h"

#if !defined (EINT0_PRIO_LEVEL)
#error NÃO FOI DEFINIDO O NÍVEL DE INTERRUPÇÃO EXTERNA 0 NO _config_cpu_.h
#endif

int eint0_Init(u8 mode, u8 polar);
void eint0_SetIntHandler(void (*func)(void) );
void eint0_ClearIntHandler(void);

#if (EINT0_USE_FUNC_INLINE == pdON)
inline void eint0_SetMode(u8 mode);
#else
void eint0_SetMode(u8 mode);
#endif

#if (EINT0_USE_FUNC_INLINE == pdON)
inline void eint0_SetPolar(u8 polar);
#else
void eint0_SetPolar(u8 polar);
#endif

#endif
