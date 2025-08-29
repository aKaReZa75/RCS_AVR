/**
 * @file     M66.h
 * @brief    M66 GSM Module Communication Library
 * @note     This header provides macros and function declarations for interfacing with the M66 GSM module.
 *           It includes AT command definitions and utility functions for initialization, configuration,
 *           and SMS communication.
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This library uses the USART peripheral for communication. See USART library at:
 *           https://github.com/aKaReZa75/AVR_USART
 */

#ifndef _M66_H_
#define _M66_H_

#include "aKaReZa.h"

/*  AT Command Definitions */
#define __M66_CMD_EchoOff      "ATE0"               ///< Disable command echo
#define __M66_CMD_AT           "AT"                 ///< Basic AT test command
#define __M66_CMD_fullFunc     "AT+CFUN=1"          ///< Set full functionality
#define __M66_CMD_VError       "AT+CMEE=2"          ///< Enable verbose error reporting
#define __M66_CMD_textMode     "AT+CMGF=1"          ///< Set SMS to text mode
#define __M66_CMD_Settings     "AT+CSMP=17,167,0,0" ///< Set SMS parameters
#define __M66_CMD_charSetting  "AT+CSCS=\"GSM\""    ///< Set character set to GSM
#define __M66_CMD_SMSdellAll   "AT+CMGD=1,4"        ///< Delete all SMS messages
#define __M66_CMD_SignalQua    "AT+CSQ"             ///< Check signal quality
#define __M66_CMD_NetReg       "AT+CREG?"           ///< Check network registration
#define __M66_CMD_Operator     "AT+COPS?"           ///< Query current operator
#define __M66_CMD_Restart      "AT+QPOWD=0"         ///< Restart the module

/* Control Characters & Defaults */
#define __M66_CtrlZ            0x1A                 ///< End-of-message character for SMS
#define __M66_Okey             "OK"                 ///< Expected success response
#define __M66_ERR              "ERROR"              ///< Expected error response
#define __M66_Default_TimeOut  300                  ///< Default timeout in milliseconds
#define __M66_DelSMS_TimeOut   3000                 ///< Default timeout in milliseconds
#define __M66_StartUp_TimeOut  20000                ///< Startup timeout in milliseconds


/**
 * @brief Response status from M66 operations.
 */
typedef enum
{
    M66_Res_OK,        ///< Operation successful
    M66_Res_ERR,       ///< Operation failed
    M66_Res_TimeOut    ///< Operation timed out
} M66_Res_T;


/**
 * @brief Initializes the M66 module.
 * 
 * @return M66_Res_T Result of the initialization process.
 */
M66_Res_T M66_Init(void);

/**
 * @brief Performs startup sequence for the M66 module.
 * 
 * @return M66_Res_T Result of the startup process.
 */
M66_Res_T M66_Startup(void);

/**
 * @brief Configures the M66 module with required settings.
 * 
 * @return M66_Res_T Result of the configuration process.
 */
M66_Res_T M66_Config(void);

/**
 * @brief Sends an SMS message to a specified phone number.
 * 
 * @param _PhoneNumber Pointer to the recipient's phone number string.
 * @param _Text Pointer to the message text string.
 * @return M66_Res_T Result of the SMS sending process.
 */
M66_Res_T M66_SendSMS(char* _PhoneNumber, char* _Text);

/**
 * @brief Sends a custom AT command to the M66 module.
 * 
 * @param _CMD Pointer to the AT command string.
 * @param _Reponse Pointer to the expected response string.
 * @param _TimeOut Timeout duration in milliseconds.
 * @return M66_Res_T Result of the command execution.
 */
M66_Res_T M66_SendAtCmd(char* _CMD, char* _Reponse, int16_t _TimeOut);

/**
 * @brief Checks for new SMS messages.
 * 
 * @return M66_Res_T Result of the SMS check operation.
 */
M66_Res_T M66_CheckSMS(void);

#endif