#include "aKaReZa.h"

Outputs_T Outputs;

int main(void)
{
  //outputs_Init();
  display_Init();
  display_Update();
  while(1)
  {
    Outputs.Pump = PUMP_ON;
    Outputs.Motor = MOTOR_OFF;
    Outputs_Control(Outputs);
    display_Update();
    delay_ms(2500);
 
    Outputs.Pump = PUMP_ON;
    Outputs.Motor = MOTOR_LOW;
    Outputs_Control(Outputs);
    display_Update();
    delay_ms(2500); 
    
    Outputs.Pump = PUMP_ON;
    Outputs.Motor = MOTOR_HIGH;
    Outputs_Control(Outputs);
    display_Update();
    delay_ms(2500); 

    Outputs.Pump = PUMP_OFF;
    Outputs.Motor = MOTOR_HIGH;
    Outputs_Control(Outputs);
    display_Update();
    delay_ms(2500);   
    
    Outputs.Pump = PUMP_OFF;
    Outputs.Motor = MOTOR_LOW;
    Outputs_Control(Outputs);
    display_Update();
    delay_ms(2500); 
    
    Outputs.Pump = PUMP_OFF;
    Outputs.Motor = MOTOR_OFF;
    Outputs_Control(Outputs);
    display_Update();
    delay_ms(2500);  

  };
};