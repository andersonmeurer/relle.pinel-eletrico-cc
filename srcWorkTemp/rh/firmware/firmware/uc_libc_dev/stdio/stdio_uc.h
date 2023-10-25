#ifndef __STDIO_UC_H
#define __STDIO_UC_H

#include "_config_cpu_.h"
#include <stdarg.h>

#if defined(LINUX)

#include "uc_libdefs.h"

// REGISTRO DE FUNÇÕES DE ENTRADA E SAÍDAS
void lgetchar_RegisterDev(int (*getc)(u8*));
void lputchar_RegisterDev(int (*putc)(n16 ));

// FUNÇÕES DE ENTRADA E SAÍDAS
int lgetchar(u8* ch);
int lputchar(n16 ch);
int rprintf(const char* fmt, ...) ;
#define lprintf(fmt, args...) 	rprintf(fmt, ##args)

// FUNÇÕES DE FORMATAÇÃO DE STRINGs
void sformat(pchar s, cpchar fmt, ...);
int sFormatArgs(pchar s, cpchar fmt, va_list args);
pchar date2str(u8 wday, u8 mday, u8 month, u16 year);
pchar time2str(u8 hour, u8 min, u8 sec);

#define STDIO_USE_SUBINT 	pdON
#define STDIO_USE_HEX		pdON
#define STDIO_USE_BIN		pdON
#define STDIO_USE_TIME		pdON
#define STDIO_USE_FLOAT		pdON

#else //#if defined(LINUX)

#include <string.h>
#include "_config_lib_stdio.h"

#if defined(FREE_RTOS)
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

// REGISTRO DE FUNÇÕES DE ENTRADA E SAÍDAS
void rprintf_RegisterDev(const u8 device, int(*putc)(n16), xQueueHandle queue, xSemaphoreHandle mutex);
void lputchar_RegisterDev(int (*func_putc)(n16), xQueueHandle queue, xSemaphoreHandle mutex);
void ldebug_RegisterDev(int (*putc)(n16), xQueueHandle queue, xSemaphoreHandle mutex);
void lerr_RegisterDev(int (*putc)(n16), xQueueHandle queue, xSemaphoreHandle mutex);

#define xPrintf rprintf

#else // #if defined(FREE_RTOS)

// REGISTRO DE FUNÇÕES DE ENTRADA E SAÍDAS
void rprintf_RegisterDev(const u8 device, int(*putc)(n16) );
void lputchar_RegisterDev(int (*putc)(n16 ));
void ldebug_RegisterDev(int (*putc)(n16 ));
void lerr_RegisterDev(int (*putc)(n16 ));

#endif //#if defined(FREE_RTOS)

void lgetchar_RegisterDev(int (*getc)(u8*));

// FUNÇÕES DE ENTRADA E SAÍDAS
int lgetchar(u8 *ch);
int lputchar(n16 ch);
int rprintf (const u8 device, cpchar format, ... );
#define lprintf(fmt, args...) 	rprintf(STDIO_LPRINTF, fmt, ##args)
#define perr(fmt, args...) 		rprintf(STDIO_ERROR, ":%s[%d]: " fmt CMD_TERMINATOR, __FILE__, __LINE__ , ##args)
#define plog(fmt, args...) 		rprintf(STDIO_DEBUG, PROMPT_LOG fmt, ##args)
#define plognp(fmt, args...) 	rprintf(STDIO_DEBUG, fmt, ##args)

// FUNÇÕES DE FORMATAÇÃO DE STRINGs
void sformat(pchar s, cpchar fmt, ...);
int sFormatArgs(pchar s, cpchar fmt, va_list args);
#if (STDIO_USE_TIME == pdON)
pchar date2str(u8 wday, u8 mday, u8 month, u16 year);
pchar time2str(u8 hour, u8 min, u8 sec);
#endif

#endif //#if defined(LINUX)
#endif

