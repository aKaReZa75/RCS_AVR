#include "task.h"

extern volatile uint32_t System_millis;

millis_T millisDisplay = {.Delta = 0, .Previous = 0, .Interval = 250};

void scheduler(void)
{
    millisDisplay.Delta = System_millis - millisDisplay.Previous;

    if(millisDisplay.Delta > millisDisplay.Interval)
    {
        millisDisplay.Previous = System_millis;
        display_Update();
    };
};