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

void mem_Update_Config(void)
{
    Config_Device.fields.CheckSum  = CRC16_Calc(&crc16_modbus, Config_Device.raw, (Config_Device_Len-2));
    e2prom_block_Write(Config_Device_Address, Config_Device.raw, Config_Device_Len);
};

void mem_Wipe(void)
{
    Config_Device.fields.Phone_CFG = false;
    Config_Device.fields.PowerOn   = false;
    Config_Device.fields.Remote_CFG= false;
    Config_Device.fields.RemoteSTS = false;
    Config_Device.fields.Status    = 0x0F;
    Config_Device.fields.CheckSum  = CRC16_Calc(&crc16_modbus, Config_Device.raw, (Config_Device_Len-2));
    e2prom_block_Write(Config_Device_Address, Config_Device.raw, Config_Device_Len);

    memset(SMS_Authorized_Number.fields.Number, 0, sizeof(SMS_Authorized_Number.fields.Number));
    SMS_Authorized_Number.fields.CheckSum = CRC16_Calc(&crc16_modbus, SMS_Authorized_Number.raw, (Config_Phone_Len-2));
    e2prom_block_Write(Config_Phone_Address, SMS_Authorized_Number.raw, Config_Phone_Len);

    Remote_Authorized.fields.Address = 0x0;
    Remote_Authorized.fields.CheckSum = CRC16_Calc(&crc16_modbus, Remote_Authorized.raw, (Config_Remote_Len-2));
    e2prom_block_Write(Config_Remote_Address, Remote_Authorized.raw, Config_Remote_Len);    
};

void mem_Update_Remote(void)
{
    Config_Device.fields.Remote_CFG= true;
    Config_Device.fields.Status    = 0x03;
    Config_Device.fields.CheckSum  = CRC16_Calc(&crc16_modbus, Config_Device.raw, (Config_Device_Len-2));
    e2prom_block_Write(Config_Device_Address, Config_Device.raw, Config_Device_Len);

    Remote_Authorized.fields.CheckSum = CRC16_Calc(&crc16_modbus, Remote_Authorized.raw, (Config_Remote_Len-2));
    e2prom_block_Write(Config_Remote_Address, Remote_Authorized.raw, Config_Remote_Len);
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

    if(_checkSum_Err == true)
    {
        Config_Device.fields.Status = 0x05;
        mem_Wipe();
    };
};
