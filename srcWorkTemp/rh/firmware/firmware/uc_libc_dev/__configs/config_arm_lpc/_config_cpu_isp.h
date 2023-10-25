#ifndef  __CONFIG_CPU_ISP_H
#define  __CONFIG_CPU_ISP_H

#include "_config_cpu_.h"

#define pinINT0_DIR			GPIO0_DIR
#define pinINT0_OFF			GPIO0_SET
#define pinINT0_ON			GPIO0_CLR
#define pinINT0				BIT_4

#define pinRESET_DIR		GPIO1_DIR
#define pinRESET_OFF		GPIO1_SET
#define pinRESET_ON			GPIO1_CLR
#define pinRESET			BIT_30

#endif
