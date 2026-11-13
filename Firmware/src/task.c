#include "task.h"

#define MSG_CMD_OK    "Command accepted."
#define MSG_CMD_NotOK "Invalid command!"

#define REMOTE_Idle_MS    10
#define REMOTE_Idle_TICKS 18
#define REMOTE_Sleep_MS   100
#define Remote_Key_A      0x08
#define Remote_Key_B      0x04
#define Remote_Key_C      0x02
#define Remote_Key_D      0x01

extern Outputs_T         Outputs;
extern Dispay_Page_T     Dispay_Page;
extern volatile uint32_t System_millis;
extern volatile bool     usart_RxFlag;   /**< True: Data has been received completely */   
extern char              usart_RxBuffer[__usart_RxBufferSize]; /* Buffer to store received data, with defined size */
extern alcd_customChar_T Display_customChar;
extern M66_SignalQ_T     M66_SignalQ;
extern volatile ev1527_T ev1527_Data;
extern ev1527_T ev1527_Data_cpy;
extern Remote_Authorized_T     Remote_Authorized;
extern SMS_Authorized_Number_T SMS_Authorized_Number;
extern Config_Device_T         Config_Device;
extern Display_BakckLight_T    Display_BakckLight;

bool         AutoStartUp     = false;
bool         System_PowerOn  = true;          
bool         Sensor_Flt_Flag = false;
AHT20_Data_T AHT20_Data = {.Humidity = 0.0f, .Temp = 0.0f};
uint16_t     Key_Learn_ctr = 0;

millis_T millisDisplay = {.Delta = 0, .Previous = 0, .Interval = 250};
millis_T millisOutputs = {.Delta = 0, .Previous = 0, .Interval = 50};
millis_T millisSystem  = {.Delta = 0, .Previous = 0, .Interval = 100};
millis_T millisGSM     = {.Delta = 0, .Previous = 0, .Interval = 100};
millis_T millisRemote  = {.Delta = 0, .Previous = 0, .Interval = REMOTE_Idle_MS}; 
millis_T millisSensor  = {.Delta = 0, .Previous = 0, .Interval = 500}; 
millis_T millisAStart  = {.Delta = 0, .Previous = 0, .Interval = 180000}; 
millis_T millisALCdBL  = {.Delta = 0, .Previous = 0, .Interval = 120000}; 

Sys_State_T Sys_State = Sys_StartUp;
GSM_State_T GSM_State = GSM_Reset;
GSM_StartUp_Flags_T GSM_StartUp_Flags;
GSM_Init_State_T    GSM_Init_State;
GSM_Init_Flags_T    GSM_Init_Flags;
SMS_CMD_Type_T      SMS_CMD_Type;
Remote_State_T      Remote_State;
Sensor_State_T      Sensor_State;

void scheduler(void)
{
    wdt_Reset();
    millisDisplay.Delta = System_millis - millisDisplay.Previous;
    millisOutputs.Delta = System_millis - millisOutputs.Previous;
    millisSystem.Delta  = System_millis - millisSystem.Previous;
    millisGSM.Delta     = System_millis - millisGSM.Previous;
    millisRemote.Delta  = System_millis - millisRemote.Previous;
    millisSensor.Delta  = System_millis - millisSensor.Previous;
    millisALCdBL.Delta  = System_millis - millisALCdBL.Previous;

    if(millisALCdBL.Delta > millisALCdBL.Interval)
    {
        millisALCdBL.Previous = System_millis;
        Display_BakckLight    = Display_BL_OFF;
    };

    if(millisSensor.Delta > millisSensor.Interval)
    {
        millisSensor.Previous = System_millis;
        Sensor_Task();
    };

    if(millisRemote.Delta > millisRemote.Interval)
    {
        millisRemote.Previous = System_millis;
        Remote_Task();
    };

    if(millisGSM.Delta > millisGSM.Interval)
    {
        millisGSM.Previous = System_millis;
        millisGSM.Interval = 50;
        GSM_Task();
    };

    if(millisSystem.Delta > millisSystem.Interval)
    {
        millisSystem.Previous = System_millis;
        System_Task();
    };

    if(millisOutputs.Delta > millisOutputs.Interval)
    {
        millisOutputs.Previous = System_millis;
        Outputs_Control();
    };


    if(millisDisplay.Delta > millisDisplay.Interval)
    {
        millisDisplay.Previous = System_millis;
        millisDisplay.Interval = 250;
        display_Update();
    };
};


void Sensor_Task(void)
{
    AHT20_Res_T Sensor_Res;
    switch (Sensor_State)
    {
        case Sensor_Init:
            millisSensor.Interval = 500;
            Sensor_Res = aht20_Init();
            if(Sensor_Res == AHT20_Res_OK)
            {
                Sensor_State = Sensor_Idle;
            }
            else
            {
                Sensor_State = Sensor_Fault;
            };
        break;

        case Sensor_Idle:
            Sensor_State = Sensor_Measure;
        break;
        
        case Sensor_Measure:
            Sensor_Res = aht20_getData(&AHT20_Data);
            if(Sensor_Res == AHT20_Res_OK)
            {
                Sensor_State = Sensor_Idle;
            }
            else
            {
                Sensor_State = Sensor_Fault;
            };           
        break;
        
        case Sensor_Fault:
            Sensor_State    = Sensor_Init;
            millisSensor.Interval = 10000;
            Sensor_Flt_Flag = true;
        break;        
    };
};


void Remote_Task(void)
{
    static uint8_t  _failCounter = 0;
    static uint16_t _learnCounter= 0;
    static uint16_t _listenTimer = 0;
    static Remote_State_T Remote_State_Last;
    static uint32_t _remoteAdd[5] = {0};
    static uint8_t  _rIndex = 0;
    switch (Remote_State)
    {
        case Remote_deInit:
            Remote_State_Last = Remote_deInit;
            ev1527_deInit();
            _failCounter = 0;
            millisRemote.Interval = REMOTE_Sleep_MS;
            Remote_State = Remote_Init;
        break;
        
        case Remote_Init:
            Remote_State_Last = Remote_Init;
            ev1527_Init();
            millisRemote.Interval = REMOTE_Idle_MS;
            Remote_State = Remote_Idle;
        break;

        case Remote_Idle:
            Remote_State_Last = Remote_Idle;
            if(ev1527_Data.Bits.Detect == true)
            {
                ev1527_Data.Bits.Detect = false;
                _failCounter = 0;
                Remote_State = Remote_Check;
            }
            else
            {
                _failCounter++;
                if(_failCounter >= REMOTE_Idle_TICKS)
                {
                    Remote_State = Remote_deInit;
                };
            };
        break;
        
        case Remote_Listening:
            if(Remote_State_Last != Remote_Listening)
            {
                ev1527_Data_cpy.rawValue = 0x0;
                _listenTimer = 0;
                Remote_State_Last = Remote_Listening;
                ev1527_Init();
                millisRemote.Interval = REMOTE_Idle_MS;
                Dispay_Page = Display_Page_EV1527;
            };

            if(ev1527_Data.Bits.Detect == true)
            {
                ev1527_Data_cpy.rawValue = ev1527_Data.rawValue;
                ev1527_Data.Bits.Detect = false;
                ev1527_Data.rawValue = 0x0;
                ev1527_Init();
                _listenTimer = 0;
                millisALCdBL.Previous = System_millis;              
            };

            _listenTimer++;
            if(_listenTimer > 1500)  /* 1500*10ms = 15S */
            {
                _listenTimer = 0;
                Remote_State = Remote_deInit;
                Dispay_Page  = Display_Page_Home;
            };

        break;

        case Remote_Check:
            Remote_State = Remote_Check;
            if(ev1527_Data.Bits.Address == Remote_Authorized.fields.Address)
            {              
                switch (ev1527_Data.Bits.Keys)
                {                 
                    case (Remote_Key_A | Remote_Key_B):
                        if(AutoStartUp == true)
                        {
                            AutoStartUp = false;
                            Display_BakckLight    = Display_BL_ON;
                            millisALCdBL.Previous = System_millis;
                            millisALCdBL.Interval = 5000;                                  
                        }
                        else
                        {
                            AutoStartUp = true;
                        };
                        millisAStart.Previous = System_millis;   
                    break;

                    case (Remote_Key_A | Remote_Key_C):
                        Display_BakckLight    = Display_BL_ON;
                        millisALCdBL.Previous = System_millis;
                        millisALCdBL.Interval = 60000;    
                    break;

                    case (Remote_Key_A | Remote_Key_D):
                        Outputs.Pump  = PUMP_ON;
                        Outputs.Motor = MOTOR_OFF;   
                    break;                    

                    case Remote_Key_A:
                        Outputs.Pump  = PUMP_OFF;
                        Outputs.Motor = MOTOR_OFF;
                    break;

                    case Remote_Key_B:
                        Outputs.Pump  = PUMP_ON;
                        Outputs.Motor = MOTOR_HIGH;
                    break;

                    case Remote_Key_C:
                        Outputs.Pump  = PUMP_ON;
                        Outputs.Motor = MOTOR_LOW;
                    break;  
                    
                    case Remote_Key_D:
                        Outputs.Pump  = PUMP_OFF;
                        Outputs.Motor = MOTOR_LOW;
                    break;                    
                };
                if(Config_Device.fields.RemoteSTS == true)
                {
                    SMS_CMD_Type = SMS_CMD_Status;                
                    GSM_State    = GSM_SMS_SendSMS;
                };
            };
            Remote_State = Remote_Idle;
        break;  
        
        case Remote_Learning:
            if(Remote_State_Last != Remote_Learning)
            {
                Display_BakckLight    = Display_BL_Blinky;
                millisALCdBL.Previous = System_millis;
                millisALCdBL.Interval = 18000;                
                Remote_State_Last = Remote_Learning;
                Dispay_Page = Display_Page_Learning;
                millisRemote.Interval = REMOTE_Idle_MS;
                _learnCounter = 0;
                _rIndex       = 0;
                memset(_remoteAdd, 0, sizeof(_remoteAdd));
                ev1527_Init();
            };


            if(ev1527_Data.Bits.Detect == true)
            {
                bool _res  = true;
                uint8_t _i = 0;
                _remoteAdd[_rIndex] = (uint32_t)ev1527_Data.Bits.Address;
                _rIndex++;
                if(_rIndex>5)
                {
                    for(_i=1;_i<5;_i++)
                    {
                        if(_remoteAdd[0] != _remoteAdd[_i])
                        {
                            _res = false;     
                        };
                    };

                    if(_res == true)
                    {
                        Config_Device.fields.Remote_CFG  = true;
                        Remote_Authorized.fields.Address = _remoteAdd[0];
                        Remote_State = Remote_Learn_Done;
                        millisRemote.Interval = 2500;
                    }
                    else
                    {
                        Remote_State = Remote_Learn_Fail;
                        millisRemote.Interval = 2500;
                    };
                };
                ev1527_Data.Bits.Detect = false;
                ev1527_Init();
            };

            
            _learnCounter++;
            if(_learnCounter >= 1500)  /* 1500*10ms=15Sec*/
            {
                _learnCounter=0;
                Remote_State = Remote_Learn_Fail;
                millisRemote.Interval = 2500;
            };
        break;   
        
        case Remote_Learn_Fail:
            if(GSM_State == GSM_Idle)
            {
                GSM_State    = GSM_SMS_SendSMS;
                SMS_CMD_Type = SMS_CMD_RemoteFail;
            };            
            Remote_State_Last = Remote_Learn_Fail;
            Dispay_Page  = Display_Page_Home;
            Remote_State = Remote_deInit;
        break;

        case Remote_Learn_Done:
            if(GSM_State == GSM_Idle)
            {
                GSM_State    = GSM_SMS_SendSMS;
                SMS_CMD_Type = SMS_CMD_RemoteDone;
            };        
            Remote_State_Last = Remote_Learn_Fail;
            Dispay_Page  = Display_Page_Home;
            Remote_State = Remote_Init;
            mem_Update_Remote();
        break;
    };

};

void System_Task(void)
{
    switch (Sys_State)
    {
        case Sys_StartUp:
            Dispay_Page = Display_Page_Home;
            millisDisplay.Interval = 2500;
            Sys_State = Sys_Idle;
        break;
        
        case Sys_Idle:
            if(AutoStartUp == true)
            {
                Display_BakckLight    = Display_BL_Blinky;
                millisALCdBL.Previous = System_millis;
                millisALCdBL.Interval = 18000;                   
                Outputs.Pump  = PUMP_ON;
                Outputs.Motor = MOTOR_OFF;               
                millisAStart.Delta = System_millis - millisAStart.Previous;
                if(millisAStart.Delta >= millisAStart.Interval)
                {
                    AutoStartUp = false;
                    millisAStart.Previous = System_millis;
                    Outputs.Pump  = PUMP_ON;
                    Outputs.Motor = MOTOR_HIGH;
                    SMS_CMD_Type  = SMS_CMD_Status;
                    GSM_State     = GSM_SMS_SendSMS;
                    Display_BakckLight    = Display_BL_OFF;
                    millisALCdBL.Previous = System_millis;
                    millisALCdBL.Interval = 5000;                         
                };
            };

            if(Config_Device.fields.Remote_CFG == false && Config_Device.fields.Phone_CFG == false)
            {
                Config_Device.fields.Status = 0x05;
            };

            if(bitCheckLow(Learn_Key_STS, Learn_Key_Pin))
            {
                Display_BakckLight = Display_BL_ON;
                millisALCdBL.Previous = System_millis;
                millisALCdBL.Interval = 120000;               
                Key_Learn_ctr++;
            }
            else
            {
                if(Key_Learn_ctr >= 100) /* 100*100ms = 10Sec */
                {
                    Key_Learn_ctr = 0;
                    mem_Wipe();
                }
                else if(Key_Learn_ctr >= 50) /* 50*100ms = 5Sec */
                {
                    Key_Learn_ctr = 0;
                    Remote_State   = Remote_Learning;
                }    
                else if(Key_Learn_ctr >= 25) /* 25*100ms = 2.5Sec */
                {
                    Key_Learn_ctr = 0;
                    Remote_State   = Remote_Listening;
                }                               
                else if(Key_Learn_ctr >= 15) /* 15*100ms = 1.5Sec */
                {
                    Key_Learn_ctr = 0;
                    Dispay_Page   = Display_Page_Authorized;
                }
                else if(Key_Learn_ctr >= 3) /* 3*100ms = 0.3Sec */
                {   
                    Key_Learn_ctr = 0;
                    Remote_State= Remote_deInit;
                    Dispay_Page = Display_Page_Home;
                }
                else
                {
                    Key_Learn_ctr = 0;
                };
            };
        break;
    };
};


void GSM_Task(void)
{
    static uint8_t  _csq_Timer   = 0;
    static uint8_t  _csq_TimeOut = 0;
    static uint16_t _Init_Timerout = 0;
    uint8_t        _rssi = 0;
    uint8_t        _ber  = 0;
    char          _Text[50] = {'\n'};
    switch (GSM_State)
    {
        case GSM_Reset:
            GSM_StartUp_Flags.raw = 0x00;
            usart_Write(__M66_CtrlZ);
            delay_ms(1);
            usart_Putsln(__M66_CMD_Restart);
            _Init_Timerout = 0;
            GSM_State = GSM_StartUp;
        break;
    
        case GSM_StartUp:
            if(usart_RxFlag == true)
            {
                M66_Startup();
                usart_RxFlag = false;
                usart_Flush();
            };

            if(GSM_StartUp_Flags.bits.Result == true)
            {
                GSM_Init_Flags.raw = 0x00;
                GSM_Init_State     = GSM_Init_Echo;
                _Init_Timerout     = 0;
                GSM_State = GSM_Init;
            };

            _Init_Timerout++;
            if(_Init_Timerout > 1200) /* 120 Sec*100ms = 1200*/
            {
                _Init_Timerout = 0;
                GSM_State = GSM_Fault;
            };
        break;

        case GSM_Init:
            M66_Config();
            if(usart_RxFlag == true)
            {
                usart_RxFlag = false;
                usart_Flush();
            };
            if(GSM_Init_State == GSM_Init_Done)
            {
                if(GSM_Init_Flags.bits.Result == true)
                {
                    Display_customChar = alcd_CustumChar_Time;
                    if(System_PowerOn == true)
                    {
                        if(Config_Device.fields.PowerOn == true)
                        {
                            SMS_CMD_Type = SMS_CMD_PowerOn;                          
                            GSM_State    = GSM_SMS_SendSMS;
                        }
                        else
                        {
                            GSM_State = GSM_Idle;
                        };
                    }
                    else
                    {
                        GSM_State = GSM_Idle;
                    }
                }
                else
                {
                    GSM_State = GSM_Fault;
                };
            };

            _Init_Timerout++;
            if(_Init_Timerout > 1200) /* 120 Sec*100ms = 1200*/
            {
                _Init_Timerout = 0;
                GSM_State = GSM_Fault;
            };            
        break;   

        case GSM_Idle:
            _csq_Timer++;
            if(usart_RxFlag == true)
            {
                if(strstr(usart_RxBuffer,"+CMTI") != NULL)
                {
                    GSM_State = GSM_SMS_ParseNumber;
                }
                else if(strstr(usart_RxBuffer,"+CLIP") != NULL)
                {
                    if(Config_Device.fields.Phone_CFG == true)
                    {
                        if(strstr(usart_RxBuffer, SMS_Authorized_Number.fields.Number) != NULL)
                        {                          
                            AutoStartUp = true;
                            SMS_CMD_Type  = SMS_CMD_autoStart;
                            GSM_State     = GSM_SMS_SendSMS;   
                            millisGSM.Interval = 2500;                         
                            millisAStart.Previous = System_millis;
                        };
                    };
                    usart_Putsln(__M66_CMD_Hangup);
                    usart_RxFlag = false;
                    usart_Flush();                  
                }
                else
                {
                    usart_RxFlag = false;
                    usart_Flush();
                };
            }; 
            
            if(_csq_Timer >= 25)
            {
                _csq_Timer   = 0;
                _csq_TimeOut = 0;
                usart_Putsln(__M66_CMD_CheckSQ);  
                GSM_State = GSM_CheckSQ;  
            };
        break;  
        
        case GSM_CheckSQ:
           if(usart_RxFlag == true)
           {
                if(strstr(usart_RxBuffer, "+CSQ"))
                {
                    sscanf(usart_RxBuffer, "\r\n+CSQ: %hhu,%hhu\r\n", &_rssi, &_ber);
                    if(_rssi >= 20)
                    {
                        M66_SignalQ = M66_SQ_High;
                    }
                    else if(_rssi >= 15)
                    {
                        M66_SignalQ = M66_SQ_Med;
                    }
                    else if(_rssi >= 10)
                    {
                        M66_SignalQ = M66_SQ_Low;
                    }
                    else if(_rssi < 10)
                    {
                        M66_SignalQ = M66_SQ_None;
                    };
                    Display_customChar = M66_SignalQ;
                };

                if(_rssi == 99)
                {
                    GSM_State = GSM_Fault;
                }
                else
                {
                    GSM_State = GSM_Idle;
                };
           }
           else
           {
            _csq_TimeOut++;
            if(_csq_TimeOut >= 5)
            {
                    _csq_TimeOut = 0;
                    GSM_State    = GSM_Idle;
            };            
           };          
        break;

        case GSM_SMS_ParseNumber:
            Display_customChar = alcd_CustumChar_Busy;
            M66_SMS_GetIndex();
            GSM_State = GSM_SMS_ReadContent;
        break;   
        
        case GSM_SMS_ReadContent:
            if(usart_RxFlag == true)
            {
                if(M66_CheckSMS() == M66_Res_OK)
                {
                    GSM_State = GSM_SMS_SendSMS;
                }
                else
                {
                    GSM_State = GSM_Fault;
                };
                usart_Putsln(__M66_CMD_SMSdellAll);
                millisGSM.Interval = 500;
                usart_RxFlag = false;
                usart_Flush();
            };
        break; 
        
        case GSM_SMS_SendSMS:
            if(Config_Device.fields.Phone_CFG == true)
            {
                Display_BakckLight    = Display_BL_ON;
                millisALCdBL.Previous = System_millis;
                millisALCdBL.Interval = 10000;                    
                if(SMS_CMD_Type == SMS_CMD_PowerOn)
                {
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, "RCS AVR powered on!");
                }
                else if(SMS_CMD_Type == SMS_CMD_autoStart)
                {
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, "Device is in auto startup mode.");
                }                 
                else if(SMS_CMD_Type == SMS_CMD_rListening)
                {
                    //M66_SendSMS(SMS_Authorized_Number.fields.Number, "Device is in remote listening mode.");
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, MSG_CMD_OK);
                }                  
                else if(SMS_CMD_Type == SMS_CMD_CfgStatus)
                {
                    sprintf(_Text,"Power-on SMS: %s\nRemote status SMS: %s",
                            Config_Device.fields.PowerOn   ? "ON" : "OFF",
                            Config_Device.fields.RemoteSTS ? "ON" : "OFF");
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, _Text);
                }                 
                else if(SMS_CMD_Type == SMS_CMD_RemoteAddress)
                {
                    sprintf(_Text, "Remote Address: 0x%05lX", (uint32_t)Remote_Authorized.fields.Address);
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, _Text);
                }  
                else if(SMS_CMD_Type == SMS_CMD_NumberDelete)
                {
                    //M66_SendSMS(SMS_Authorized_Number.fields.Number, "Number has been deleted successfully.");
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, MSG_CMD_OK);
                    memset(SMS_Authorized_Number.fields.Number, 0, sizeof(SMS_Authorized_Number.fields.Number));
                    Config_Device.fields.Phone_CFG = false;
                    mem_Update_Number();
                    mem_Update_Config();                    
                }                     
                else if(SMS_CMD_Type == SMS_CMD_RemoteDelete)
                {
                    //M66_SendSMS(SMS_Authorized_Number.fields.Number, "Remote has been deleted successfully.");
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, MSG_CMD_OK);
                }                              
                else if(SMS_CMD_Type == SMS_CMD_AddPhoneNumber)
                {
                    //M66_SendSMS(SMS_Authorized_Number.fields.Number, "Your number has been added as authorized.");
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, MSG_CMD_OK);
                }
                else if(SMS_CMD_Type == SMS_CMD_RemoteLearning)
                {
                    //M66_SendSMS(SMS_Authorized_Number.fields.Number, "Device is in remote learning mode...");
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, MSG_CMD_OK);
                }                 
                else if(SMS_CMD_Type == SMS_CMD_RemoteDone)
                {
                    //M66_SendSMS(SMS_Authorized_Number.fields.Number, "Remote has been set successfully.");
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, MSG_CMD_OK);
                } 
                else if(SMS_CMD_Type == SMS_CMD_RemoteFail)
                {
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, "Learning failed, Please try again.");
                }                           
                else if(SMS_CMD_Type == SMS_CMD_WipeAll)
                {
                    //M66_SendSMS(SMS_Authorized_Number.fields.Number, "Factory Reset Done!");
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, MSG_CMD_OK);
                    mem_Wipe();
                }    
                else if(SMS_CMD_Type == SMS_CMD_dConfig ) 
                {        
                    //M66_SendSMS(SMS_Authorized_Number.fields.Number, "System Config Done.");
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, MSG_CMD_OK);
                }            
                else if(SMS_CMD_Type != SMS_CMD_Unknown)
                {
                    M66_sendStatus();
                }
                else
                {
                    M66_SendSMS(SMS_Authorized_Number.fields.Number, MSG_CMD_NotOK);
                };
                GSM_State = GSM_Idle;                
            }
            else
            {
                GSM_State = GSM_Fault;
            };  
            millisDisplay.Interval = 2000;            
        break;   
        
        case GSM_Fault:
            Display_customChar = alcd_CustumChar_Sad;
            millisGSM.Interval = 2500;
            GSM_State          = GSM_Reset;
        break;          
    }
};