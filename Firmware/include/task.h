#ifndef _task_H_
#define _task_H_

#include "aKaReZa.h"


typedef enum
{
    Sys_StartUp = 0,
    Sys_Idle
} Sys_State_T;


typedef enum
{
	GSM_Reset = 0,
	GSM_StartUp,
	GSM_Init,
	GSM_Idle,
    GSM_SMS_ParseNumber,    
    GSM_SMS_ReadContent,    
    GSM_SMS_SendSMS,        
	GSM_Fault
} GSM_State_T;



void scheduler(void);
void System_Task(void);
void GSM_Task(void);

#endif