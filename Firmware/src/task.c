#include "task.h"

extern Dispay_Page_T Dispay_Page;
extern volatile uint32_t System_millis;
extern volatile bool usart_RxFlag;   /**< True: Data has been received completely */   
extern char usart_RxBuffer[__usart_RxBufferSize]; /* Buffer to store received data, with defined size */
extern alcd_customChar_T Display_customChar;

millis_T millisDisplay = {.Delta = 0, .Previous = 0, .Interval = 250};
millis_T millisOutputs = {.Delta = 0, .Previous = 0, .Interval = 50};
millis_T millisSystem  = {.Delta = 0, .Previous = 0, .Interval = 50};
millis_T millisGSM     = {.Delta = 0, .Previous = 0, .Interval = 100};


Sys_State_T Sys_State = Sys_StartUp;
GSM_State_T GSM_State = GSM_Reset;
GSM_StartUp_Flags_T GSM_StartUp_Flags;
GSM_Init_State_T GSM_Init_State;
GSM_Init_Flags_T GSM_Init_Flags;
SMS_CMD_Type_T   SMS_CMD_Type;

void scheduler(void)
{
    millisDisplay.Delta = System_millis - millisDisplay.Previous;
    millisOutputs.Delta = System_millis - millisOutputs.Previous;
    millisSystem.Delta  = System_millis - millisSystem.Previous;
    millisGSM.Delta     = System_millis - millisGSM.Previous;

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
        break;          
    }
};