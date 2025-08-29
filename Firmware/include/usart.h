/**
 * @file     usart.h
 * @brief    USART (Universal Synchronous and Asynchronous serial Receiver and Transmitter) Library with Idle Line Detection
 * @note     This USART library is designed specifically for the ATmega328 microcontroller.
 *           It provides full-duplex serial communication with interrupt-driven reception.
 * 
 *           A key feature of this library is the **Idle Line Detection** mechanism to identify 
 *           the end of a message on the RX line. Unlike protocols with end delimiters (e.g., '\n'), 
 *           this approach uses a time-based strategy:
 * 
 *           - Upon receiving the first byte, Timer2 is initialized.
 *           - Each received byte resets Timer2 to prevent timeout.
 *           - If no byte is received after a certain duration (measured by Timer2 overflow),
 *             the message is considered complete.
 * 
 *           This method enables flexible and delimiter-free message reception in asynchronous
 *           serial communication environments.
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     For detailed explanations of this library, along with additional notes and examples of usage, 
 *           please visit the following repository:
 *           https://github.com/aKaReZa75/AVR_USART
 */
#ifndef _usart_H_
#define _usart_H_

#include "aKaReZa.h"


/**
 * @def __usart_RxBufferSize
 * @brief Defines the buffer size for USART reception, which is 33 bytes (32 bytes data + 1 byte for null-terminator).
 */
#define __usart_RxBufferSize 320 


/**
 * @def uTimer_Reset
 * @brief Resets Timer2 counter value to 0.
 * 
 * This macro is used in the Idle Line Detection mechanism. It clears the Timer2 count register (TCNT2)
 * to start measuring the idle duration on the RX line from zero.
 */
#define uTimer_Reset TCNT2 = 0x00


/**
 * @def usart_frameError
 * @brief Checks if a framing error has occurred by reading the FE0 bit in the UCSR0A register.
 */
#define usart_frameError  bitCheck(UCSR0A, FE0 ) 


/**
 * @def usart_daraOverRun
 * @brief Checks if a data overrun error has occurred by reading the DOR0 bit in the UCSR0A register.
 */
#define usart_daraOverRun bitCheck(UCSR0A, DOR0) 


/**
 * @brief Initializes or deinitializes Timer2 for Idle Line Detection.
 * 
 * This function configures Timer2 to measure the time elapsed since the last received byte.
 * If no new data is received for a specified timeout period, it indicates the end of a message (idle line detected).
 * 
 * @param _initStatus If true, the timer is initialized and started; if false, the timer is stopped.
 */
void uTimer_Init(bool _initStatus);


/**
 * @brief Initializes the USART module.
 * 
 * This function must be called before any other USART function.
 * It configures the baud rate, frame format, and enables both the transmitter and receiver.
 * 
 * @param _initStatus If true, the USART module is initialized; if false, it is disabled.
 */
void usart_Init(bool _initStatus);


/**
 * @brief Transmits a single byte of data via USART.
 * 
 * This function writes a byte to the USART data register and waits for the USART to be ready for transmission.
 * 
 * @param _Data The byte of data to transmit.
 */
void usart_Write(uint8_t _Data);


/**
 * @brief Sends a null-terminated string via USART.
 * 
 * This function sends each byte of the string sequentially until the null terminator ('\0') is encountered.
 * 
 * @param _Data A pointer to the null-terminated string to be transmitted.
 */
void usart_Puts(char* _Data);


/**
 * @brief Sends a null-terminated string followed by a newline via USART.
 * 
 * This function sends the string, followed by a newline character, which is useful in many communication protocols.
 * 
 * @param _Data A pointer to the null-terminated string to be transmitted, followed by a newline.
 */
void usart_Putsln(char* _Data);


/**
 * @brief Receives a single byte of data from USART.
 * 
 * This function blocks until data is received. It waits for incoming data in the USART receive buffer and returns it.
 * 
 * @return The received byte of data from the USART receive buffer.
 */
uint8_t usart_getChar(void);


/**
 * @brief Clears the USART buffers.
 * 
 * This function flushes both the receive and transmit buffers, ensuring that no old or unprocessed data remains.
 */
void usart_Flush(void);

#endif