#ifndef _task_H_
#define _task_H_

#include "aKaReZa.h"


typedef enum
{
    Display_State_Welcome = 0,
    Display_State_Home
} Dispay_State_T;


typedef enum
{
	Sys_Booting,
	Sys_Idle
} Sys_State_T;


typedef enum
{
	GSM_Idle
} GSM_State_T;

void Scheduler(void);
void System_Task(void);
void Display_Task(void);
void GSM_Task(void);

#endif