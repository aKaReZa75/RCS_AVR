#include "aKaReZa.h"

extern volatile bool usart_RxFlag;   /**< True: Data has been received completely */   
extern char usart_RxBuffer[__usart_RxBufferSize]; /* Buffer to store received data, with defined size */
extern Dispay_Page_T Dispay_Page;

Outputs_T Outputs;

int main(void)
{
  outputs_Init();
  display_Init();
  
  Dispay_Page = Display_Page_Welcome;
  display_Update();

  usart_Init(Initialize);
  globalInt_Enable;
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

  while(1)
  {
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
  };
};