#include "task.h"

extern Dispay_Page_T Dispay_Page;
extern volatile uint32_t System_millis;

millis_T millisDisplay = {.Delta = 0, .Previous = 0, .Interval = 250};
millis_T millisOutputs = {.Delta = 0, .Previous = 0, .Interval = 50};
millis_T millisSystem  = {.Delta = 0, .Previous = 0, .Interval = 50};
millis_T millisGSM     = {.Delta = 0, .Previous = 0, .Interval = 100};


Sys_State_T Sys_State = Sys_StartUp;
GSM_State_T GSM_State = GSM_Reset;


void scheduler(void)
{
    millisDisplay.Delta = System_millis - millisDisplay.Previous;
    millisOutputs.Delta = System_millis - millisOutputs.Previous;
    millisSystem.Delta  = System_millis - millisSystem.Previous;
    millisGSM.Delta     = System_millis - millisGSM.Previous;

    if(millisGSM.Delta > millisGSM.Interval)
    {
        millisGSM.Previous = System_millis;
        GSM_Task();
    };

    if(millisSystem.Delta > millisSystem.Interval)
    {
        millisSystem.Previous = System_millis;
        System_Task();
    };

    if(millisOutputs.Delta > millisOutputs.Interval)
    {
        millisOutputs.Previous = System_millis;
        Outputs_Control();
    };


    if(millisDisplay.Delta > millisDisplay.Interval)
    {
        millisDisplay.Previous = System_millis;
        millisDisplay.Interval = 250;
        display_Update();
    };
};



void System_Task(void)
{
    switch (Sys_State)
    {
        case Sys_StartUp:
            Dispay_Page = Display_Page_Home;
            millisDisplay.Interval = 2500;
        break;
        
        case Sys_Idle:

        break;
    };
};


void GSM_Task(void)
{
    switch (GSM_State)
    {
        case GSM_Reset:

        break;
    
        case GSM_StartUp:

        break;

        case GSM_Init:

        break;   

        case GSM_Idle:

        break;  
        
        case GSM_SMS_ParseNumber:

        break;   
        
        case GSM_SMS_ReadContent:

        break; 
        
        case GSM_SMS_SendSMS:

        break;   
        
        case GSM_Fault:

        break;          
    }
};