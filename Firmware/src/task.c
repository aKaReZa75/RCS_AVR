#include "task.h"

extern Dispay_Page_T Dispay_Page;
extern volatile uint32_t System_millis;

millis_T millisDisplay = {.Delta = 0, .Previous = 0, .Interval = 250};
millis_T millisOutputs = {.Delta = 0, .Previous = 0, .Interval = 50};
millis_T millisSystem  = {.Delta = 0, .Previous = 0, .Interval = 50};

Sys_State_T Sys_State = Sys_StartUp;

void scheduler(void)
{
    millisDisplay.Delta = System_millis - millisDisplay.Previous;
    millisOutputs.Delta = System_millis - millisOutputs.Previous;
    millisSystem.Delta  = System_millis - millisSystem.Previous;

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
    }
};