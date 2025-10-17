#ifndef _task_H_
#define _task_H_

#include "aKaReZa.h"

typedef enum
{
    Sys_StartUp = 0,
    Sys_Idle
} Sys_State_T;

void scheduler(void);
void System_Task(void);

#endif