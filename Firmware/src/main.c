#include "aKaReZa.h"


int main(void)
{
  outputs_Init();
  display_Init();
  millis_Init();
  usart_Init(Initialize);
  globalInt_Enable;
  /*
  if(M66_Init() == M66_Res_OK)
  {
    Dispay_Page = Display_Page_Home;
    alcd_clear();
    display_Update();
  }
  else
  {
    alcd_clear();
    alcd_puts("M66_Res_ERR");
  };
  */
  while(1)
  {
    Scheduler();
  };
};