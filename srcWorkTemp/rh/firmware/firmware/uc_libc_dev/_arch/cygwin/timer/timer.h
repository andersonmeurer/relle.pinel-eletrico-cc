#ifndef __TIMER_H
#define __TIMER_H

#include "_config_cpu_.h"
#include <sys/time.h>

typedef unsigned long long tTime; // TODO Testar se n�o d� erro na compila��o

tTime now(void);

#endif
