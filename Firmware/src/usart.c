/**
 * @file     usart.c
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
#include "usart.h"


/* USART Flags */
volatile bool usart_RxFlag  = false; /**< True: Data has been received completely */     
bool usart_RxBUfferOVF      = false; /**< True: Receive buffer overflow occurred */
bool usart_errFlag          = false; /**< True: An error occurred during communication */

/* Index to keep track of the next available space in the receive buffer */
uint8_t bufferIndex    = 0; 
/* Buffer to store received data, with defined size */
char usart_RxBuffer[__usart_RxBufferSize]; 


/**
 * @brief Timer2 Overflow Interrupt Service Routine.
 *
 * Triggered when Timer2 overflows. This ISR is used to detect the end of a USART message
 * based on a timeout (i.e., no data received for a certain period).
 * 
 * Appends a null-terminator to the receive buffer and sets a flag to indicate that
 * a complete message has been received. Then it deinitializes the timer.
 */
ISR(TIMER2_OVF_vect)
{
    /* End of string: Add null-terminator and set flag */
    usart_RxBuffer[bufferIndex++] = '\0';
    usart_RxFlag = true;
    uTimer_Init(deInitialize);
};


/**
 * @brief USART Receive Interrupt Service Routine.
 * 
 * This interrupt service routine handles data reception, checks for errors (buffer overflow or framing errors), 
 * and stores the received data in the USART receive buffer.
 *
 * @note **Important**: For the interrupt to work, `globalInt_Enable` must be called to enable global interrupts. 
 *       Alternatively, you can set the I-bit (Interrupt Enable) in the `SREG` (Status Register) manually by writing `1` to the I-bit.
 */
ISR(USART_RX_vect)
{
    uint8_t _RxData;
    usart_errFlag = false;

    /* Check for reception errors (Buffer Overflow, Framing Errors, Data Overrun) */
    usart_errFlag = usart_RxBUfferOVF | usart_frameError | usart_daraOverRun;

    _RxData = UDR0; /**< Read received data from the USART data register */

    if(bufferIndex == 0)
    {
        uTimer_Init(Initialize);
    };

    if(!usart_errFlag)
    {
        uTimer_Reset;
        /* Store the received byte in buffer */
        usart_RxBuffer[bufferIndex++] = _RxData;
        if(bufferIndex > __usart_RxBufferSize) usart_RxBUfferOVF = true;
    };
};


/**
 * @brief Initializes or deinitializes Timer2 for Idle Line Detection.
 * 
 * This function configures Timer2 to measure the time elapsed since the last received byte.
 * If no new data is received for a specified timeout period, it indicates the end of a message (idle line detected).
 * 
 * @param _initStatus If true, the timer is initialized and started; if false, the timer is stopped.
 */
void uTimer_Init(bool _initStatus)
{
    if(_initStatus)
    {
        /* Mode: Normal */
        bitClear(TCCR2A, WGM20);
        bitClear(TCCR2A, WGM21);
        bitClear(TCCR2B, WGM22);

        /* Reset Timer2 int Flag */
        intFlag_clear(TIFR2, TOV2);

        /* Enbale Timer2 OVF */
        bitSet(TIMSK2, TOIE2);

        TCNT2 = 0x00;

        /* Prescaler: 64 => 16Mhz/64= 1024uS */
        bitSet(TCCR2B, CS20);   
        bitSet(TCCR2B, CS21); 
        bitClear(TCCR2B, CS22);
    }
    else
    {
        bitClear(TCCR2A, WGM20);
        bitClear(TCCR2A, WGM21);
        bitClear(TCCR2B, WGM22);
        bitClear(TIMSK2, TOIE2);
        bitClear(TCCR2B, CS20);   
        bitClear(TCCR2B, CS21); 
        bitClear(TCCR2B, CS22);  
        TCNT2 = 0x00;
    };
};


/**
 * @brief Initializes the USART module.
 * 
 * This function configures the USART for asynchronous communication with the following settings:
 * - Baud Rate: 115200
 * - 8 data bits
 * - No parity
 * - 1 stop bit
 * - Double speed mode
 * - Enables the receiver and transmitter, along with interrupts for receiving data.
 * 
 * @param _initStatus If true, the USART module is initialized; if false, it is disabled.
 */
void usart_Init(bool _initStatus)
{
    if(_initStatus)
    {       
        /* Configure USART for Asynchronous mode */
        bitClear(UCSR0C, UMSEL01);
        bitClear(UCSR0C, UMSEL00);
        /* Disable Parity */
        bitClear(UCSR0C, UPM01);
        bitClear(UCSR0C, UPM00);
        bitClear(UCSR0C, USBS0);  /**< Stop bit = 1 */
        /* Set Data Bits to 8 */
        bitClear(UCSR0B, UCSZ02);
        bitSet  (UCSR0C, UCSZ01);
        bitSet  (UCSR0C, UCSZ00);
        bitSet  (UCSR0B, TXEN0);  /**< Enable Transmit Mode */
        bitSet  (UCSR0B, RXEN0);  /**< Enable Receive Mode */
        bitSet  (UCSR0B, RXCIE0); /**< Enable Receive Intrrupts */
        bitSet  (UCSR0A, U2X0);   /**< Double Speed Mode */
        /* Set Baud Rate to 115200 */
        UBRR0H = 0;
        UBRR0L = 16; 
        usart_Flush();
    }
    else
    {    
        /* Disable USART and reset configuration */   
        bitClear(UCSR0C, UMSEL01);
        bitClear(UCSR0C, UMSEL00);
        bitClear(UCSR0C, UPM01);
        bitClear(UCSR0C, UPM00);
        bitClear(UCSR0C, USBS0);  
        bitClear(UCSR0B, UCSZ02);
        bitClear(UCSR0C, UCSZ01);
        bitClear(UCSR0C, UCSZ00);
        bitClear(UCSR0B, TXEN0);  
        bitClear(UCSR0B, RXEN0); 
        bitClear(UCSR0B, RXCIE0); 
        bitClear(UCSR0A, U2X0);  
        UBRR0H = 0;
        UBRR0L = 0; 
    };
};


/**
 * @brief Transmits a byte of data via USART.
 * 
 * This function waits for the USART data register to be ready and then sends a byte of data.
 * 
 * @param _Data The byte of data to transmit.
 */
void usart_Write(uint8_t _Data)
{
    /* Wait until the data register is ready for new data */
    bitWaitHigh(UCSR0A, UDRE0);
    UDR0 = _Data;
    /* Wait until transmission is complete */
    bitWaitHigh(UCSR0A, TXC0);
};


/**
 * @brief Sends a null-terminated string via USART.
 * 
 * This function transmits each byte of the string sequentially until the null terminator ('\0') is encountered.
 * 
 * @param _Data A pointer to the null-terminated string to be transmitted.
 */
void usart_Puts(char* _Data)
{
    while(*_Data != '\0')  usart_Write(*_Data++);
};


/**
 * @brief Sends a null-terminated string followed by a newline via USART.
 * 
 * This function transmits the string, followed by carriage return (`\r`) and newline (`\n`) characters.
 * 
 * @param _Data A pointer to the null-terminated string to be transmitted, followed by newline.
 */
void usart_Putsln(char* _Data)
{
    usart_Puts(_Data);
    usart_Write('\r');  /**< Carraige Return (CR) */
    usart_Write('\n');  /**< NewLine (NL) */
};


/**
 * @brief Receives a byte of data from USART.
 * 
 * This function blocks until data is received. It waits for the data to be available in the USART receive buffer.
 * 
 * @return The byte of received data.
 */
uint8_t usart_getChar(void)
{
    /* Wait for data to be received */
    bitWaitHigh(UCSR0A, RXC0);    
    return UDR0;
};


/**
 * @brief Clears the USART receive and transmit buffers.
 * 
 * This function flushes the receive buffer, clears the buffer index, and resets error flags.
 */
void usart_Flush(void)
{
    /* Clear the receive buffer */
    memset(usart_RxBuffer, '\0', __usart_RxBufferSize);

    /* Reset timer module */
    uTimer_Reset;
    uTimer_Init(deInitialize);

    /* Reset flags and buffer index */
    bufferIndex = 0;
    usart_RxBUfferOVF = false;
    usart_RxFlag      = false;
    usart_errFlag     = false;
};
