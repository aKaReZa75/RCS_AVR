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


typedef union 
{
    struct
    {
        uint8_t Ready:1;
        uint8_t CFUN :1;
        uint8_t CPIN :1;
        uint8_t Call :1;
        uint8_t SMS  :1;
        uint8_t Result:1;
        uint8_t reserve:2;
    }bits;
    uint8_t raw;
} GSM_StartUp_Flags_T;


typedef enum
{
    GSM_Init_Echo,
    GSM_Init_Func,
    GSM_Init_Text,
    GSM_Init_Char,
    GSM_Init_SMS,
    GSM_Init_Del,
    GSM_Init_Done
} GSM_Init_State_T;



typedef union 
{
    struct
    {
        uint8_t Echo :1;
        uint8_t Func :1;
        uint8_t Text :1;
        uint8_t Char :1;
        uint8_t SMS  :1;
        uint8_t Del  :1;
        uint8_t Result:1;
        uint8_t reserve:1;
    }bits;
    uint8_t raw;
} GSM_Init_Flags_T;


void scheduler(void);
void System_Task(void);
void GSM_Task(void);

#endif