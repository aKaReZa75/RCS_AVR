#include "aKaReZa.h"

int main(void)
{
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