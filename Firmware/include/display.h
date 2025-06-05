#ifndef _display_H_
#define _display_H_

#include "aKaReZa.h"

typedef enum
{
    alcd_CustomChar_Antenna = 0,
    alcd_CustumChar_SQ1,
    alcd_CustumChar_SQ2,
    alcd_CustumChar_SQ3,
    alcd_CustumChar_Sad,
    alcd_CustumChar_Time
}alcd_customChar_T;


typedef enum
{
    M66_SQ_None = 0x20,
    M66_SQ_Low  = alcd_CustumChar_SQ1,
    M66_SQ_Med  = alcd_CustumChar_SQ2,
    M66_SQ_High = alcd_CustumChar_SQ3   
}M66_SignalQ_T;


typedef enum
{
    Display_Page_Welcome = 0,
    Display_Page_Home
} Dispay_Page_T;

void display_HomePage(void);

void display_Init(void);

void display_Update(void);

void display_PageWelcome(void);

#endif