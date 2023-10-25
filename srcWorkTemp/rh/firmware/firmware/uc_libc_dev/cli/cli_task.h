#ifndef __CLI_TASK_H
#define __CLI_TASK_H

#include "cli.h"
#include "FreeRTOS.h"
#include "queue.h"

void vCliInit(const cmd_list_t *cl, unsigned portSHORT stack_size, unsigned int priority_task, xTaskHandle *task_handle);

#endif
