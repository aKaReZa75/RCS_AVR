#include "task.h"

#define REMOTE_Idle_MS    10
#define REMOTE_Idle_TICKS 18
#define REMOTE_Sleep_MS   100
#define Remote_Authorized 0x59647
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
extern volatile ev1527_T ev1527_Data;

bool         Sensor_Flt_Flag = false;
AHT20_Data_T AHT20_Data = {.Humidity = 0.0f, .Temp = 0.0f};

millis_T millisDisplay = {.Delta = 0, .Previous = 0, .Interval = 250};
millis_T millisOutputs = {.Delta = 0, .Previous = 0, .Interval = 50};
millis_T millisSystem  = {.Delta = 0, .Previous = 0, .Interval = 50};
millis_T millisGSM     = {.Delta = 0, .Previous = 0, .Interval = 100};
millis_T millisRemote  = {.Delta = 0, .Previous = 0, .Interval = REMOTE_Idle_MS}; 
millis_T millisSensor  = {.Delta = 0, .Previous = 0, .Interval = 500}; 

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
    millisDisplay.Delta = System_millis - millisDisplay.Previous;
    millisOutputs.Delta = System_millis - millisOutputs.Previous;
    millisSystem.Delta  = System_millis - millisSystem.Previous;
    millisGSM.Delta     = System_millis - millisGSM.Previous;
    millisRemote.Delta  = System_millis - millisRemote.Previous;
    millisSensor.Delta  = System_millis - millisSensor.Previous;

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
            Sensor_Flt_Flag = true;
        break;        
    };
};


void Remote_Task(void)
{
    static uint8_t _failCounter = 0;
    switch (Remote_State)
    {
        case Remote_deInit:
            ev1527_deInit();
            _failCounter = 0;
            millisRemote.Interval = REMOTE_Sleep_MS;
            Remote_State = Remote_Init;
        break;
        
        case Remote_Init:
            ev1527_Init();
            millisRemote.Interval = REMOTE_Idle_MS;
            Remote_State = Remote_Idle;
        break;

        case Remote_Idle:
            if(ev1527_Data.Bits.Detect == true)
            {
                ev1527_Data.Bits.Detect = false;
                _failCounter = 0;
                if(bitCheckLow(Learn_Key_STS, Learn_Key_Pin))
                {
                    Dispay_Page  = Display_Page_EV1527;
                };
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
        
        case Remote_Check:
            if(ev1527_Data.Bits.Address == Remote_Authorized)
            {
                switch (ev1527_Data.Bits.Keys)
                {
                    case (Remote_Key_A | Remote_Key_B):
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
            };
            Remote_State = Remote_Idle;
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

        break;
    };
};


void GSM_Task(void)
{
    switch (GSM_State)
    {
        case GSM_Reset:
            GSM_StartUp_Flags.raw = 0x00;
            usart_Putsln(__M66_CMD_Restart);
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
                GSM_State = GSM_Init;
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
                    GSM_State = GSM_Idle;
                }
                else
                {
                    GSM_State = GSM_Fault;
                };
            };
        break;   

        case GSM_Idle:
            Display_customChar = alcd_CustumChar_SQ2;
            if(usart_RxFlag == true)
            {
                if(strstr(usart_RxBuffer,"+CMTI") != NULL)
                {
                    GSM_State = GSM_SMS_ParseNumber;
                }
                else
                {
                    usart_RxFlag = false;
                    usart_Flush();
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
            if(SMS_CMD_Type != SMS_CMD_Unknown)
            {
                M66_sendStatus();
            }
            else
            {
                M66_sendWarning();
            };
            millisDisplay.Interval = 2000;            
            GSM_State = GSM_Idle;
        break;   
        
        case GSM_Fault:
            Display_customChar = alcd_CustumChar_Sad;
            millisGSM.Interval = 2500;
            GSM_State          = GSM_Reset;
        break;          
    }
};