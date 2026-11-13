#include "display.h"

extern bool         AutoStartUp;
extern Outputs_T    Outputs;
extern GSM_State_T  GSM_State;
extern AHT20_Data_T AHT20_Data;
extern bool         Sensor_Flt_Flag;
extern uint16_t     Key_Learn_ctr;
extern SMS_Authorized_Number_T SMS_Authorized_Number;
extern Config_Device_T         Config_Device;
extern Remote_Authorized_T     Remote_Authorized;
extern Remote_State_T          Remote_State;
char alcd_Buffer[16];

char* Pump_Display[]  = {"OFF", "ON"};
char* Motor_Display[] = {"OFF", "LOW", "HIGH"};
char* Learning_frames[] =
{
    "Learning   ",
    "Learning.  ",
    "Learning.. ",
    "Learning..."
};

uint8_t Display_Antenna[8] = {0x1F, 0x15, 0x0E, 0x04, 0x04, 0x04, 0x04, 0x04};
uint8_t Display_Time[8]    = {0x1F, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x1F, 0x00};
uint8_t Display_SQ3[8]     = {0x01, 0x01, 0x01, 0x05, 0x05, 0x05, 0x15, 0x15};
uint8_t Display_SQ2[8]     = {0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x14, 0x14};
uint8_t Display_SQ1[8]     = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10};
uint8_t Display_Sad[8]     = {0x00, 0x00, 0x00, 0x0A, 0x00, 0x0E, 0x11, 0x00};

alcd_customChar_T Display_customChar;
M66_SignalQ_T M66_SignalQ;
Dispay_Page_T Dispay_Page;
Display_BakckLight_T Display_BakckLight = Display_BL_ON;

uint8_t lFrame = 0;

void display_Init(void)
{
  alcd_init();
  //alcd_backLight(true);
  alcd_customChar(alcd_CustomChar_Antenna, Display_Antenna);
  alcd_customChar(alcd_CustumChar_Time, Display_Time);  
  alcd_customChar(alcd_CustumChar_SQ3, Display_SQ3);
  alcd_customChar(alcd_CustumChar_SQ2, Display_SQ2);
  alcd_customChar(alcd_CustumChar_SQ1, Display_SQ1);
  alcd_customChar(alcd_CustumChar_Sad, Display_Sad);  
};


void display_Update(void)
{
  static Dispay_Page_T Dispay_Page_Last;
  static uint8_t       _dispCounter = 0;
  static bool          _backLightState = true;
  static uint8_t       _backLightTimer = 0;
  switch (Display_BakckLight)
  {
    case Display_BL_ON:
      bitSet(__alcd_BL_Control, __alcd_BL_Pin);
    break;

    case Display_BL_OFF:
      bitClear(__alcd_BL_Control, __alcd_BL_Pin);
    break;

    case Display_BL_Blinky:
      _backLightTimer++;
      if(_backLightState == false)
      {
        if(_backLightTimer>=1)
        {
          _backLightTimer = 0;
          _backLightState = true;
        };
      }
      else
      {
        if(_backLightTimer>=6)
        {
          _backLightTimer = 0;
          _backLightState = false;
        };
      };
      bitChange(__alcd_BL_Control, __alcd_BL_Pin, _backLightState);
    break;
  };


  switch (Dispay_Page)
  {
    case Display_Page_Welcome:
      if(Dispay_Page_Last != Display_Page_Welcome)
      {
        alcd_clear();
        Dispay_Page_Last = Display_Page_Welcome;
      }
      display_PageWelcome();
    break;

    case Display_Page_Home:
      if(Dispay_Page_Last != Display_Page_Home)
      {
        alcd_clear();
        Dispay_Page_Last = Display_Page_Home;
      }    
      display_HomePage();
    break;

    case Display_Page_EV1527:
      if(Dispay_Page_Last != Display_Page_EV1527)
      {
        alcd_clear();
        Dispay_Page_Last = Display_Page_EV1527;
      };

      display_EV1527_Show();
    break;

    case Display_Page_Authorized:
      if(Dispay_Page_Last != Display_Page_Authorized)
      {
        _dispCounter = 0;
        alcd_clear();
        display_Authorized();
        Dispay_Page_Last = Display_Page_Authorized;
      };

      _dispCounter++;
      if(_dispCounter >= 20) /* 250ms *20 = 5000ms*/
      {
        _dispCounter = 0;
        Dispay_Page = Display_Page_Home;
      };      
    break;

    case Display_Page_Learning:
      if(Dispay_Page_Last != Display_Page_Learning)
      {
        _dispCounter = 0;
        alcd_clear();      
        Dispay_Page_Last = Display_Page_Learning;
      };

      _dispCounter++;
      if(_dispCounter >= 2) /* 250ms *2 = 500ms*/
      {
        _dispCounter = 0;
        lFrame++;
        if(lFrame > 3)
        {
          lFrame = 0;
        };
      };        
      

      display_Learning();  
    break;
  };
};


void display_Learning(void)
{
  if(Remote_State == Remote_Learning)
  {
    alcd_gotoxy(0, 0);
    alcd_puts(Learning_frames[lFrame]);
  }
  else if(Remote_State == Remote_Learn_Fail)
  {
    alcd_gotoxy(0, 0);
    alcd_puts(Learning_frames[0]);
    alcd_gotoxy(0, 1);
    alcd_puts("Failed!");
  }
  else if(Remote_State == Remote_Learn_Done)
  {
    alcd_gotoxy(0, 0);
    alcd_puts(Learning_frames[0]);    
    alcd_gotoxy(0, 1);
    sprintf(alcd_Buffer, "Done: 0x%05lx", Remote_Authorized.fields.Address);
    alcd_puts(alcd_Buffer);
  };
};


void display_Authorized(void)
{

  alcd_puts("AN:");
  alcd_puts(SMS_Authorized_Number.fields.Number);    
  sprintf(alcd_Buffer, "AR:0x%05lx [%02x]", Remote_Authorized.fields.Address, Config_Device.raw[0]);
  alcd_gotoxy(0, 1);
  alcd_puts(alcd_Buffer);  
};

void display_EV1527_Show(void)
{
  extern ev1527_T ev1527_Data_cpy;
  sprintf(alcd_Buffer, "Add: 0x%05lX", (uint32_t) ev1527_Data_cpy.Bits.Address);
  alcd_gotoxy(0, 0);
  alcd_puts(alcd_Buffer);

  sprintf(alcd_Buffer, "Key: 0x%01X [%c%c%c%c] ",
          ev1527_Data_cpy.Bits.Keys,
          (bitCheck(ev1527_Data_cpy.Bits.Keys, 3)) ? '1' : '0',
          (bitCheck(ev1527_Data_cpy.Bits.Keys, 2)) ? '1' : '0',
          (bitCheck(ev1527_Data_cpy.Bits.Keys, 1)) ? '1' : '0',
          (bitCheck(ev1527_Data_cpy.Bits.Keys, 0)) ? '1' : '0');
  alcd_gotoxy(0, 1);
  alcd_puts(alcd_Buffer);  
};

void display_PageWelcome(void)
{
  alcd_gotoxy(0, 0);
  sprintf(alcd_Buffer, "RCS AVR V%s", firmwareVer);
  alcd_puts(alcd_Buffer);

  alcd_gotoxy(0, 1);
  sprintf(alcd_Buffer, "aKaReZa[%s]", gitHash);
  alcd_puts(alcd_Buffer);
};


void display_HomePage(void)
{
  static uint8_t Counter = 0;
  alcd_gotoxy(0,0);
  if(Sensor_Flt_Flag == false)
  {
    sprintf(alcd_Buffer,"T:%2.1f H:%2.1f", AHT20_Data.Temp, AHT20_Data.Humidity);
  }
  else
  {
    sprintf(alcd_Buffer,"T:%c    H:%c", alcd_CustumChar_Sad, alcd_CustumChar_Sad);
  };
  alcd_puts(alcd_Buffer);

  Counter++;
  if(Counter > 4)
  {
    Counter = 0;
  };

  alcd_gotoxy(14, 0);
  if(GSM_State == GSM_StartUp)
  {
    if(Counter<2)
    {
      alcd_putc(' ');
      alcd_putc(' ');
    }
    else
    {
      alcd_putc(alcd_CustomChar_Antenna);
      alcd_putc(' ');
    };
  }
  else if(GSM_State == GSM_Init)
  {
    if(Counter<2)
    {
      alcd_putc(alcd_CustomChar_Antenna);
      alcd_putc(' ');
    }
    else
    {
      alcd_putc(alcd_CustomChar_Antenna);
      alcd_putc(alcd_CustumChar_Time);
    };
  }
  else
  {
    alcd_putc(alcd_CustomChar_Antenna);
    alcd_putc(Display_customChar);
  };

  if(Config_Device.fields.Status == 0x03)
  {
    alcd_gotoxy(0, 1);
    sprintf(alcd_Buffer,"P:%s ", Pump_Display[Outputs.Pump]);
    alcd_puts(alcd_Buffer);    

    alcd_gotoxy(7, 1);
    sprintf(alcd_Buffer,"M:%s ", Motor_Display[Outputs.Motor]);
    alcd_puts(alcd_Buffer);
  }
  else
  {
    alcd_gotoxy(0, 1);
    alcd_puts("Config Error!");
  }; 

  alcd_gotoxy(13, 1);
  if(Key_Learn_ctr>=99)
  {
    sprintf(alcd_Buffer, " %2u", (uint16_t)(Key_Learn_ctr*0.1));
    alcd_puts(alcd_Buffer);
  }
  else if(Key_Learn_ctr >= 3)
  {
    sprintf(alcd_Buffer, "%1.1f", (float)(Key_Learn_ctr*0.1f));
    alcd_puts(alcd_Buffer);
  }
  else
  {
    if(AutoStartUp == true)
    {
      if(Counter<2)
      {
        alcd_puts("ASU");
      }
      else
      {
        alcd_puts("   ");
      };      
    } 
    else
    {
      alcd_puts("   ");
    }
  };  
};