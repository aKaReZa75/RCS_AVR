/**
 * @file     M66.c
 * @brief    M66 GSM Module Communication Implementation
 * @note     This source file contains the implementation of functions used to initialize,
 *           configure, and communicate with the M66 GSM module, including SMS handling
 *           and AT command transmission.
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     For more information and examples, visit the project repository:
 *           https://github.com/aKaReZa75/AVR_M66
 * 
 * @note     This library uses the USART peripheral for communication. See USART library at:
 *           https://github.com/aKaReZa75/AVR_USART
 */
#include "M66.h"


extern volatile bool usart_RxFlag;   /**< True: Data has been received completely */   
extern char usart_RxBuffer[__usart_RxBufferSize]; /* Buffer to store received data, with defined size */
extern GSM_StartUp_Flags_T GSM_StartUp_Flags;
extern GSM_Init_State_T GSM_Init_State;
extern GSM_Init_Flags_T GSM_Init_Flags;
extern SMS_CMD_Type_T   SMS_CMD_Type;
extern Outputs_T Outputs;

#define SMS_Authorized_Number "+989355282124"
#define SMS_scanSet_Index     "\r\n+CMTI: \"SM\",%hhu\r\n"
#define SMS_scanSet_Number    "\r\n+CMGR: \"%*[^\"]\",\"%[^\"]\""
#define SMS_scanSet_Content   "\r\n+CMGR: \"%*[^\"]\",\"%*[^\"]\",\"\",\"%*[^\"]\"\r\n%[^\r]\r\n"
#define SMS_CMD_Pump_OFF_Motor_OFF   "RCS+OC=0,0"
#define SMS_CMD_Pump_ON_Motor_OFF    "RCS+OC=1,0"
#define SMS_CMD_Pump_ON_Motor_LOW    "RCS+OC=1,1"
#define SMS_CMD_Pump_ON_Motor_HIGH   "RCS+OC=1,2"
#define SMS_CMD_Pump_OFF_Motor_LOW   "RCS+OC=0,1"
#define SMS_CMD_Pump_OFF_Motor_HIGH  "RCS+OC=0,2"

/**
 * @brief Initializes the M66 GSM module
 * @details Performs module startup and configuration sequence
 * @return M66_Res_T Status of initialization (OK, ERR, or TimeOut)
 */
M66_Res_T M66_Init(void)
{
    M66_Res_T _Res = M66_Res_ERR;

    // First attempt to start up the module
    _Res = M66_Startup();
    if(_Res != M66_Res_OK)
    {
        return _Res;
    };

    // If startup successful, proceed with configuration
    _Res = M66_Config();
    if(_Res != M66_Res_OK)
    {
        return _Res;
    };

    return M66_Res_OK;
};


/**
 * @brief Performs the M66 module startup sequence
 * @details Sends restart command and waits for all readiness indicators
 * @return M66_Res_T Status of startup process
 */
M66_Res_T M66_Startup(void)
{
    // Check for basic readiness
    if(strstr(usart_RxBuffer, "RDY") != NULL)
    {
        GSM_StartUp_Flags.bits.Ready = true;
    };

    // Check for full functionality
    if(strstr(usart_RxBuffer, "+CFUN: 1") != NULL)
    {
        GSM_StartUp_Flags.bits.CFUN = true;
    };

    // Check SIM card status
    if(strstr(usart_RxBuffer, "+CPIN: READY") != NULL)
    {
        GSM_StartUp_Flags.bits.CPIN = true;
    };    

    // Check call readiness
    if(strstr(usart_RxBuffer, "Call Ready") != NULL)
    {
        GSM_StartUp_Flags.bits.Call = true;
    };    

    // Check SMS readiness
    if(strstr(usart_RxBuffer, "SMS Ready") != NULL)
    {
        GSM_StartUp_Flags.bits.SMS = true;
    }; 

    GSM_StartUp_Flags.bits.Result = GSM_StartUp_Flags.bits.Ready
                                  & GSM_StartUp_Flags.bits.CFUN
                                  & GSM_StartUp_Flags.bits.CPIN
                                  & GSM_StartUp_Flags.bits.Call
                                  & GSM_StartUp_Flags.bits.SMS;

    return M66_Res_OK;
};


/**
 * @brief Configures the M66 module with default settings
 * @details Sets up echo, functionality mode, SMS format, and other parameters
 * @return M66_Res_T Status of configuration process
 */
M66_Res_T M66_Config(void)
{
    static GSM_Init_State_T GSM_Init_State_Last = GSM_Init_Func;
    switch (GSM_Init_State)
    {
        case GSM_Init_Echo: // Disable command echo
            if(GSM_Init_State_Last != GSM_Init_Echo)
            {
                usart_Putsln(__M66_CMD_EchoOff);
                GSM_Init_State_Last = GSM_Init_Echo;
            }
            else
            {
                if(usart_RxFlag == true)
                {
                    if(strstr(usart_RxBuffer, "ATE0") != NULL)
                    {
                        GSM_Init_Flags.bits.Echo = true;
                    }
                    else if(strstr(usart_RxBuffer, __M66_Okey) != NULL)
                    {
                        GSM_Init_Flags.bits.Echo = true;
                    }
                    else
                    {
                        GSM_Init_Flags.bits.Echo = false;
                    };
                    GSM_Init_State = GSM_Init_Func;
                };
            };
        break;

        case GSM_Init_Func: // Enable full functionality
            if(GSM_Init_State_Last != GSM_Init_Func)
            {
                usart_Putsln(__M66_CMD_fullFunc);
                GSM_Init_State_Last = GSM_Init_Func;
            }
            else
            {
                if(usart_RxFlag == true)
                {                
                    if(strstr(usart_RxBuffer, __M66_Okey) != NULL)
                    {
                        GSM_Init_Flags.bits.Func = true;
                    }
                    else
                    {
                        GSM_Init_Flags.bits.Func = false;
                    };
                    GSM_Init_State = GSM_Init_Text;
                };
            };
        break;

        case GSM_Init_Text: // Set SMS text mode
            if(GSM_Init_State_Last != GSM_Init_Text)
            {
                usart_Putsln(__M66_CMD_textMode);
                GSM_Init_State_Last = GSM_Init_Text;
            }
            else
            {
                if(usart_RxFlag == true)
                {                            
                    if(strstr(usart_RxBuffer, __M66_Okey) != NULL)
                    {
                        GSM_Init_Flags.bits.Text = true;
                    }
                    else
                    {
                        GSM_Init_Flags.bits.Text = false;
                    };
                    GSM_Init_State = GSM_Init_Char;
                };
            };
        break;
        
        case GSM_Init_Char: // Set character encoding to GSM
            if(GSM_Init_State_Last != GSM_Init_Char)
            {
                usart_Putsln(__M66_CMD_charSetting);
                GSM_Init_State_Last = GSM_Init_Char;
            }
            else
            {
                if(usart_RxFlag == true)
                {                            
                    if(strstr(usart_RxBuffer, __M66_Okey) != NULL)
                    {
                        GSM_Init_Flags.bits.Char = true;
                    }
                    else
                    {
                        GSM_Init_Flags.bits.Char = false;
                    };
                    GSM_Init_State = GSM_Init_SMS;
                };
            };
        break;
        
        case GSM_Init_SMS:  // Configure SMS parameters
            if(GSM_Init_State_Last != GSM_Init_SMS)
            {
                usart_Putsln(__M66_CMD_Settings);
                GSM_Init_State_Last = GSM_Init_SMS;
            }
            else
            {
                if(usart_RxFlag == true)
                {                            
                    if(strstr(usart_RxBuffer, __M66_Okey) != NULL)
                    {
                        GSM_Init_Flags.bits.SMS = true;
                    }
                    else
                    {
                        GSM_Init_Flags.bits.SMS = false;
                    };
                    GSM_Init_State = GSM_Init_Del;
                };
            };
        break;
        
        case GSM_Init_Del:
            if(GSM_Init_State_Last != GSM_Init_Del)
            {
                usart_Putsln(__M66_CMD_SMSdellAll);
                GSM_Init_State_Last = GSM_Init_Del;
            }
            else
            {
                if(usart_RxFlag == true)
                {                            
                    if(strstr(usart_RxBuffer, __M66_Okey) != NULL)
                    {
                        GSM_Init_Flags.bits.Del = true;
                    }
                    else
                    {
                        GSM_Init_Flags.bits.Del = false;
                    };
                    GSM_Init_State = GSM_Init_Done;
                    GSM_Init_Flags.bits.Result = GSM_Init_Flags.bits.Echo
                                            & GSM_Init_Flags.bits.Func
                                            & GSM_Init_Flags.bits.Text
                                            & GSM_Init_Flags.bits.Char
                                            & GSM_Init_Flags.bits.SMS
                                            & GSM_Init_Flags.bits.Del;                    
                };
            };
        break;   
        
        case GSM_Init_Done:
            
        break;           
    };

    return M66_Res_OK;
};



/**
 * @brief Sends an SMS message to a specified phone number.
 *
 * @param _PhoneNumber Destination phone number (null-terminated string).
 * @param _Text        Pointer to the SMS text (null-terminated string).
 * @return M66_Res_T   Result of the SMS sending operation.
 *
 * @note Why `Ctrl+Z` is sent on error:
 *       The command `AT+CMGS="..."` switches the modem into **SMS text entry mode** and the modem
 *       indicates readiness with the `'>'` prompt. If the prompt is not detected (timeout,
 *       partial/late response, or any non-OK status), the modem might still be in text-entry state
 *       and will interpret any subsequent bytes as SMS payload rather than AT commands.
 *       To **force the modem out of text-entry mode** and restore normal command parsing, the code
 *       transmits **Ctrl+Z (0x1A)** even when `_Res != M66_Res_OK`. This acts as a universal
 *       end-of-message terminator, ensuring the modem exits the input state and future AT commands
 *       are processed correctly. 
 */
M66_Res_T M66_SendSMS(char* _PhoneNumber, char* _Text)
{
    M66_Res_T _Res = M66_Res_ERR;
    char _CMD[25];
    
    /* Prepare and send SMS command */
    sprintf(_CMD, "AT+CMGS=\"%s\"", _PhoneNumber);
    _Res = M66_SendAtCmd(_CMD, ">", __M66_Default_TimeOut); 

    if(_Res != M66_Res_OK)
    {
        /* Send Ctrl+Z to terminate any pending text-entry state */
        usart_Write(__M66_CtrlZ);        
        return _Res;
    };

    /* Send message content and termination character */
    usart_Puts(_Text);
    usart_Write(__M66_CtrlZ);

    return M66_Res_OK;
};


/**
 * @brief Sends an AT command to the M66 module and waits for a response.
 * 
 * @param _CMD The AT command to send (empty string for listen-only)
 * @param _Reponse Expected response string to check for
 * @param _TimeOut Timeout duration in milliseconds.
 * @return M66_Res_T Result of the command execution.
 */
M66_Res_T M66_SendAtCmd(char* _CMD, char* _Reponse, int16_t _TimeOut)
{
    M66_Res_T _Res = M66_Res_TimeOut;
    uint16_t _Timer = 0;

    usart_Flush();

    // Send command if not empty
    if(*_CMD != '\0')
    {
        usart_Putsln(_CMD);
    };

    // Wait for response with timeout
    while(!usart_RxFlag)
    {
        _Timer++;
        if(_Timer > _TimeOut)
        {
            break;
        };
        delay_ms(1);
    };

    // Check response content
    if(strstr(usart_RxBuffer, _Reponse))
    {
        _Res = M66_Res_OK;
    }
    else if(strstr(usart_RxBuffer, __M66_ERR))
    {
        _Res = M66_Res_ERR;
    };
    
    usart_Flush();

    return _Res;
};


void str_toUpper(char *_str, uint16_t _Length)
{
    uint16_t _Index = 0;
    for(_Index = 0; _Index < _Length; _Index++)
    {
        _str[_Index] = (char)toupper((int) _str[_Index]);
    };
};



void M66_SMS_GetIndex(void)
{
    char _gsmCmd[20];    
    uint8_t _SMSnum = 0;   /**< Number of the SMS message in the memory */

    /* Extract the SMS number from the usart_RxBuffer */
    sscanf(usart_RxBuffer, SMS_scanSet_Index, &_SMSnum);
    if(_SMSnum == 0)
    {
        _SMSnum = 1;
    };

    usart_Flush();

    memset(_gsmCmd, '\0', sizeof(_gsmCmd));
    sprintf(_gsmCmd, "AT+CMGR=%d", _SMSnum);
    usart_Putsln(_gsmCmd);
};




/**
 * @brief Checks for incoming SMS messages and parses the content.
 * 
 * @details 
 * This function reads the first SMS stored in the GSM module using the `AT+CMGR=1` command.
 * It then parses the sender's phone number and the message content from the response using `sscanf`.
 * 
 * The expected format of the GSM response is:
 * @code
 * \r\n+CMGR: "REC READ","+989123456789","","24/08/14,17:30:00"\r\nHello World!\r\n
 * @endcode
 * 
 * Parsing logic:
 * - `\"%*[^\"]\"` skips the SMS status field (e.g., "REC READ").
 * - `\"%[^\"]\"` captures the sender's phone number into `PhoneNumber`.
 * - `\"\"` matches the empty third field.
 * - `\"%*[^\"]\"` skips the timestamp.
 * - `%[^\r]` captures the actual SMS message into `IncomeTex`.
 * 
 * @note 
 * - The function only checks SMS at index 1.
 * - If parsing fails (i.e., `Status != 2`), it returns `M66_Res_ERR`.
 * - On success, it returns `M66_Res_OK`.
 * 
 * @return M66_Res_T Result of the SMS check operation:
 * - `M66_Res_OK` if SMS was successfully parsed.
 * - `M66_Res_ERR` if parsing failed.
 */
M66_Res_T M66_CheckSMS(void)
{
    char _phoneNumber[14];
    char _incomeText[20];
    uint8_t _Status = 0;
    SMS_CMD_Type = SMS_CMD_Phone_Incorrect;

    // Parse SMS content
    memset(_phoneNumber, '\0', sizeof(_phoneNumber));
    memset(_incomeText, '\0', sizeof(_incomeText));

    _Status = sscanf(usart_RxBuffer, SMS_scanSet_Number, _phoneNumber);
    if(_Status != 1)
    {                   
        return M66_Res_ERR;
    };

    if(strstr(_phoneNumber, SMS_Authorized_Number))
    {

        _Status = sscanf(usart_RxBuffer, SMS_scanSet_Content, _incomeText);
        if(_Status != 1)
        {                   
            return M66_Res_ERR;
        };

        str_toUpper(_incomeText, strlen(_incomeText));

        if(strstr(_incomeText, SMS_CMD_Pump_OFF_Motor_OFF) != NULL)
        {
            Outputs.Pump  = PUMP_OFF;
            Outputs.Motor = MOTOR_OFF;
            SMS_CMD_Type  = SMS_CMD_POFF_MOFF;
        }
        else if(strstr(_incomeText, SMS_CMD_Pump_ON_Motor_OFF) != NULL)
        {
            Outputs.Pump  = PUMP_ON;
            Outputs.Motor = MOTOR_OFF;
            SMS_CMD_Type  = SMS_CMD_PON_MOFF;
        }       
        else if(strstr(_incomeText, SMS_CMD_Pump_ON_Motor_LOW) != NULL)
        {
            Outputs.Pump  = PUMP_ON;
            Outputs.Motor = MOTOR_LOW;
            SMS_CMD_Type  = SMS_CMD_PON_MLOW;

        }
        else if(strstr(_incomeText, SMS_CMD_Pump_ON_Motor_HIGH) != NULL)
        {
            Outputs.Pump  = PUMP_ON;
            Outputs.Motor = MOTOR_HIGH;
            SMS_CMD_Type  = SMS_CMD_PON_MHIGH;            
        }
        else if(strstr(_incomeText, SMS_CMD_Pump_OFF_Motor_LOW) != NULL)
        {
            Outputs.Pump  = PUMP_OFF;
            Outputs.Motor = MOTOR_LOW;
            SMS_CMD_Type  = SMS_CMD_POFF_MLOW;                
        }
        else if(strstr(_incomeText, SMS_CMD_Pump_OFF_Motor_HIGH) != NULL)
        {
            Outputs.Pump  = PUMP_OFF;
            Outputs.Motor = MOTOR_HIGH;
            SMS_CMD_Type  = SMS_CMD_POFF_MHIGH;                
        }
        else
        {
            SMS_CMD_Type  = SMS_CMD_Unknown;                
        };
    };

    return M66_Res_OK;
};


void M66_sendStatus(void)
{
    char _cmd[30];
    extern char* Pump_Display[];
    extern char* Motor_Display[]; 

    sprintf(_cmd, "Pump: %s\nMotor: %s", 
                Pump_Display[Outputs.Pump],
                Motor_Display[Outputs.Motor]);

    M66_SendSMS(SMS_Authorized_Number, _cmd);
};


void M66_sendWarning(void)
{
    M66_SendSMS(SMS_Authorized_Number, "Unknown Command!");
};