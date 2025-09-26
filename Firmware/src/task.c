#include "aKaReZa.h"

extern volatile uint32_t System_millis;
extern volatile bool usart_RxFlag;   /**< True: Data has been received completely */   
extern char usart_RxBuffer[__usart_RxBufferSize]; /* Buffer to store received data, with defined size */

Outputs_T Outputs;

millis_T millisSystem = {.Delta = 0, .Previous = 0, .Interval = 10};
millis_T millisDisplay= {.Delta = 0, .Previous = 0, .Interval = 250};
millis_T millisGsm    = {.Delta = 0, .Previous = 0, .Interval = 100};

Dispay_State_T Display_State;
Sys_State_T    System_Satate;
GSM_State_T    GSM_State;

void Scheduler(void)
{
    millisSystem.Delta  = System_millis - millisSystem.Previous;
    millisDisplay.Delta = System_millis - millisDisplay.Previous;
    millisGsm.Delta     = System_millis - millisGsm.Previous;

    if(millisSystem.Delta > millisSystem.Interval)
    {   
        millisSystem.Previous = System_millis;
        System_Task();
    };


    if(millisDisplay.Delta > millisDisplay.Interval)
    {   
        millisDisplay.Previous = System_millis;
        Display_Task();
    };  
    
    if(millisGsm.Delta > millisGsm.Interval)
    {   
        millisGsm.Previous = System_millis;
        GSM_Task();
    };        
};


void System_Task(void)
{
   switch (System_Satate)
   {
    case Sys_Booting:

    break;

    case Sys_Idle:

    break;
   };
};


void Display_Task(void)
{
  switch (Display_State)
  {
    case Display_State_Welcome:
      display_PageWelcome();
    break;

    case Display_State_Home:
      display_HomePage();
    break;
  };
};


void GSM_Task(void)
{
  switch (GSM_State)
  {
    case GSM_Idle:
        if(usart_RxFlag)
        {
            if(strstr(usart_RxBuffer,"+CMTI") != NULL)
            {
                M66_CheckSMS();
            }
            else
            {
                usart_Flush();
            };
        };        
    break;
  };
};