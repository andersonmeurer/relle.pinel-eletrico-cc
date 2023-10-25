#ifndef __VIC_H 
#define __VIC_H
#if defined(arm7tdmi)
#include "vic_arm7tdmi.h"
#else
#include "vic_cortexm3.h"
#endif

#endif

