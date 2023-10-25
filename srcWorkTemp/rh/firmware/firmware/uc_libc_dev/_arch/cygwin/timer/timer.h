#ifndef __TIMER_H
#define __TIMER_H

#include "_config_cpu_.h"
#include <sys/time.h>

typedef unsigned long long tTime; // TODO Testar se não dá erro na compilação

tTime now(void);

#endif
