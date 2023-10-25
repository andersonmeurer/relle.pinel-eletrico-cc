#ifndef __GKEY_H
#define __GKEY_H

#include "_config_cpu_.h"
#include "_config_lib_gkey.h"

typedef void (*pkeyFunc) (char key);
void gKey_Init(pkeyFunc onKdown, pkeyFunc onKpress, pkeyFunc onKup);
void gKey_Process(char key);

extern int gkey_FastInc; // sinaliza quando temos repetição rápida quando onkeypress é pressionado

#endif
