#include "display.h"

extern Outputs_T Outputs;

char alcd_Buffer[16];

char* Pump_Display[]  = {"OFF", "ON"};
char* Motor_Display[] = {"OFF", "LOW", "HIGH"};

uint8_t Display_Antenna[8] = {0x1F, 0x15, 0x0E, 0x04, 0x04, 0x04, 0x04, 0x04};
uint8_t Display_Time[8]    = {0x1F, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x1F, 0x00};
uint8_t Display_SQ3[8]     = {0x01, 0x01, 0x01, 0x05, 0x05, 0x05, 0x15, 0x15};
uint8_t Display_SQ2[8]     = {0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x14, 0x14};
uint8_t Display_SQ1[8]     = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10};
uint8_t Display_Sad[8]     = {0x00, 0x00, 0x00, 0x0A, 0x00, 0x0E, 0x11, 0x00};

alcd_customChar_T Display_customChar;
M66_SignalQ_T M66_SignalQ;
Dispay_Page_T Dispay_Page;

void display_Init(void)
{
  alcd_init();
  alcd_backLight(true);
  alcd_customChar(alcd_CustomChar_Antenna, Display_Antenna);
  alcd_customChar(alcd_CustumChar_Time, Display_Time);  
  alcd_customChar(alcd_CustumChar_SQ3, Display_SQ3);
  alcd_customChar(alcd_CustumChar_SQ2, Display_SQ2);
  alcd_customChar(alcd_CustumChar_SQ1, Display_SQ1);
  alcd_customChar(alcd_CustumChar_Sad, Display_Sad);  
};


void display_Update(void)
{
  switch (Dispay_Page)
  {
    case Display_Page_Welcome:
      display_PageWelcome();
    break;

    case Display_Page_Home:
      display_HomePage();
    break;
  }
}


void display_PageWelcome(void)
{
  alcd_gotoxy(0, 0);
  alcd_puts("RCS AVR");

  alcd_gotoxy(0, 1);
  alcd_puts("aKaReZa");
};


void display_HomePage(void)
{
  alcd_gotoxy(0,0);
  sprintf(alcd_Buffer,"T:%2.1f H:%d", 25.1, 30);
  alcd_puts(alcd_Buffer);

  alcd_gotoxy(14, 0);
  alcd_putc(alcd_CustomChar_Antenna);
  M66_SignalQ = M66_SQ_Med;
  alcd_putc(M66_SignalQ);

  alcd_gotoxy(0, 1);
  sprintf(alcd_Buffer,"P:%s ", Pump_Display[Outputs.Pump]);
  alcd_puts(alcd_Buffer);    

  alcd_gotoxy(7, 1);
  sprintf(alcd_Buffer,"M:%s ", Motor_Display[Outputs.Motor]);
  alcd_puts(alcd_Buffer);     
};