#ifndef _mem_H
#define _mem_H

#include "aKaReZa.h"

typedef union
{
    uint8_t raw[3];
    struct
    {
        uint8_t  Status    :4;
        uint8_t  PowerOn   :1;
        uint8_t  RemoteSTS :1;
        uint8_t  Phone_CFG :1;
        uint8_t  Remote_CFG:1;
        uint16_t CheckSum  :16;
    }fields;
} Config_Device_T;


typedef union
{
    uint8_t raw[16];
    struct
    {
        char     Number[14];
        uint16_t CheckSum:16;
    }fields;
} SMS_Authorized_Number_T;

typedef union
{
    uint8_t raw[6];
    struct
    {
        uint32_t Address :32;
        uint16_t CheckSum:16;
    }fields;
} Remote_Authorized_T;

#define Config_Device_Address 0x00
#define Config_Device_Len     3

#define Config_Phone_Address  0x10
#define Config_Phone_Len      16

#define Config_Remote_Address 0x20
#define Config_Remote_Len     6

void mem_Read(void);
void mem_Update_Number(void);
void mem_Update_Remote(void);

#endif