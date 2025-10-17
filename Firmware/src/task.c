#include "task.h"

extern volatile uint32_t System_millis;

millis_T millisDisplay = {.Delta = 0, .Previous = 0, .Interval = 250};
millis_T millisOutputs = {.Delta = 0, .Previous = 0, .Interval = 50};

void scheduler(void)
{
    millisDisplay.Delta = System_millis - millisDisplay.Previous;
    millisOutputs.Delta = System_millis - millisOutputs.Previous;


    if(millisOutputs.Delta > millisOutputs.Interval)
    {
        millisOutputs.Previous = System_millis;
        Outputs_Control();
    };


    if(millisDisplay.Delta > millisDisplay.Interval)
    {
        millisDisplay.Previous = System_millis;
        display_Update();
    };
};