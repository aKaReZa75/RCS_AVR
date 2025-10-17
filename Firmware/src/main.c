#include "aKaReZa.h"



int main(void)
{
  outputs_Init();
  display_Init();
  millis_Init();
  usart_Init(Initialize);
  globalInt_Enable;

  while(1)
  {
    scheduler();
  };
};