// não ficou claro porque usar open, close ... ao invés de _open, _close ... ou  _open_r _close_r, ...
// como mencionado no reent.h do GNUARM. Se usar dá conflito de tipos
#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "_config_cpu_.h"


int     _open         _PARAMS ((const char *, int, ...));
void    syscalls_Init  _PARAMS ((void));

void syscalls_Init (void) {
}


int _open (const char *path, int flags, ...) {
  	IO0SET = 0x20;
  	return 0;
}


