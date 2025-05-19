#include "display.h"

char alcd_Buffer[16];

void display_HomePage(void)
{
  alcd_clear();

  alcd_gotoxy(0,0);
  sprintf(alcd_Buffer,"T:%2.1f H:%d", 25.1, 30);
  alcd_puts(alcd_Buffer);

  alcd_gotoxy(0, 1);
  sprintf(alcd_Buffer,"P:%s", "OFF");
  alcd_puts(alcd_Buffer);    

  alcd_gotoxy(7, 1);
  sprintf(alcd_Buffer,"M:%s", "High");
  alcd_puts(alcd_Buffer);     
};