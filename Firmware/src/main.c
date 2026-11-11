#include "aKaReZa.h"

Reset_Source_t Reset_Source = RESET_SOURCE_UNKNOWN;
extern bool    System_PowerOn;
extern Display_BakckLight_T    Display_BakckLight;

int main(void)
{
  wdt_Init(WDT_TIMEOUT_8S);
  get_Reset_Srouce(&Reset_Source);
  if(Reset_Source == RESET_SOURCE_POWER_ON)
  {
    System_PowerOn = true;
    Display_BakckLight = Display_BL_ON;
  }
  else
  {
    System_PowerOn = false;
    Display_BakckLight = Display_BL_OFF;
  };
  outputs_Init();
  display_Init();
  display_Update();
  millis_Init();
  usart_Init(Initialize);
  ev1527_Init();
  i2c_Init();
  mem_Read();
  globalInt_Enable;

  while(1)
  {
    scheduler();
  };
};