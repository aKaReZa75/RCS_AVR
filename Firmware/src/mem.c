#include "mem.h"

Config_Device_T         Config_Device = {.raw = 0};
Remote_Authorized_T     Remote_Authorized = {.raw = {0}};
SMS_Authorized_Number_T SMS_Authorized_Number = {.raw = {0}};

hcrc16_T crc16_modbus = 
{
  .Poly = 0x8005, // x¹⁶ + x¹⁵ + x² + 1
  .Init = 0xFFFF,
  .refIn = true,
  .refOut = true,
  .xorOut = 0x0000
};


void mem_Update_Number(void)
{
    Config_Device.fields.Phone_CFG = true;
    Config_Device.fields.Status    = 0x03;
    Config_Device.fields.CheckSum  = CRC16_Calc(&crc16_modbus, Config_Device.raw, (Config_Device_Len-2));
    e2prom_block_Write(Config_Device_Address, Config_Device.raw, Config_Device_Len);

    SMS_Authorized_Number.fields.CheckSum = CRC16_Calc(&crc16_modbus, SMS_Authorized_Number.raw, (Config_Phone_Len-2));
    e2prom_block_Write(Config_Phone_Address, SMS_Authorized_Number.raw, Config_Phone_Len);
};


void mem_Read(void)
{
    bool _checkSum_Err = false;

    char alcdBuf[17];
    e2prom_block_Read(Config_Device_Address, Config_Device.raw, Config_Device_Len);
    if(CRC16_Calc(&crc16_modbus, Config_Device.raw, Config_Device_Len) != 0)
    {
        _checkSum_Err = true;
    };

    e2prom_block_Read(Config_Phone_Address, SMS_Authorized_Number.raw, Config_Phone_Len);
    if(CRC16_Calc(&crc16_modbus, SMS_Authorized_Number.raw, Config_Phone_Len) != 0)
    {
        _checkSum_Err = true;
    };

    e2prom_block_Read(Config_Remote_Address, Remote_Authorized.raw, Config_Remote_Len);
    if(CRC16_Calc(&crc16_modbus, Remote_Authorized.raw, Config_Remote_Len) != 0)
    {
        _checkSum_Err = true;
    };

    alcd_clear();
    if(_checkSum_Err == true)
    {
        alcd_puts("Config Error!");
    }
    else
    {
        
        alcd_puts("AN:");
        alcd_puts(SMS_Authorized_Number.fields.Number);    
        sprintf(alcdBuf, "AR:0x%05lx [%02x]", Remote_Authorized.fields.Address, Config_Device.raw[0]);
        alcd_gotoxy(0, 1);
        alcd_puts(alcdBuf);   
    }
    delay_ms(5000);
};
