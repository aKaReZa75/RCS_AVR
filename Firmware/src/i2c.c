/**
 ******************************************************************************
 * @file     i2c.c
 * @brief    I2C (TWI) communication library implementation for AVR microcontrollers
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This library provides complete I2C master mode functionality using
 *           the hardware TWI peripheral on AVR microcontrollers.
 * 
 * @note     FUNCTION SUMMARY:
 *           - i2c_Init           : Initialize TWI peripheral for 100kHz operation at 16MHz CPU
 *           - i2c_Start          : Generate START or repeated START condition
 *           - i2c_Stop           : Generate STOP condition and release I2C bus
 *           - i2c_Write          : Transmit single byte and wait for acknowledgment
 *           - i2c_Read           : Receive single byte with ACK or NACK response
 *           - i2c_writeAddress   : Complete write transaction to slave device
 *           - i2c_readAdress     : Complete read transaction from slave device
 *           - i2c_readSequential : Write-then-read for register-based devices
 * 
 * @note     Hardware Configuration (16MHz CPU):
 *           - SCL Frequency: 100kHz (standard mode)
 *           - Formula: SCL_freq = F_CPU / (16 + 2*TWBR*Prescaler)
 *           - With Prescaler=1: 100000 = 16000000 / (16 + 2*TWBR*1)
 *           - TWBR = 72
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_I2C
 ******************************************************************************
 */

#include "i2c.h"


/* ============================================================================
 *                       INITIALIZATION FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize I2C (TWI) peripheral in master mode
 * @retval None
 * @note Configuration for 100kHz SCL frequency at 16MHz CPU clock:
 *       - TWI Prescaler: 1 (TWPS1:TWPS0 = 00)
 *       - TWBR: 72
 *       - Formula: SCL = F_CPU / (16 + 2*TWBR*Prescaler)
 *       - Calculation: 100kHz = 16MHz / (16 + 2*72*1) = 16MHz / 160
 * @note Must be called once before any I2C operations
 * ------------------------------------------------------- */
void i2c_Init(void)
{
    /* Set TWI prescaler to 1 (fastest prescaler) */
    bitClear(TWSR, TWPS0);               /**< Clear TWPS0 bit for prescaler = 1 */
    bitClear(TWSR, TWPS1);               /**< Clear TWPS1 bit for prescaler = 1 */
    
    /* Configure bit rate register for 100kHz SCL at 16MHz CPU */
    TWBR = 72;                           /**< Set bit rate register: (16MHz / 100kHz - 16) / 2 = 72 */
    
    /* Enable TWI peripheral */
    bitSet(TWCR, TWEN);                  /**< Set TWI Enable bit to activate I2C hardware */
};


/* ============================================================================
 *                       START & STOP CONDITIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Generate START condition on I2C bus
 * @retval None
 * @note START condition occurs when SDA transitions HIGH→LOW while SCL is HIGH
 *       This function can also generate repeated START for sequential operations
 *       Blocks until START condition is transmitted (TWINT flag set)
 * ------------------------------------------------------- */
void i2c_Start(void)
{
    /* Send START condition: Set TWINT, TWEN, and TWSTA bits */
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);  /**< TWINT clears flag, TWSTA requests START */
    
    /* Wait for START condition to complete */
    bitWaitHigh(TWCR, TWINT);            /**< Poll TWINT flag until hardware sets it (operation complete) */
};

/* -------------------------------------------------------
 * @brief Generate STOP condition on I2C bus
 * @retval None
 * @note STOP condition occurs when SDA transitions LOW→HIGH while SCL is HIGH
 *       Releases I2C bus for other masters
 *       Non-blocking - STOP is executed asynchronously
 * ------------------------------------------------------- */
void i2c_Stop(void)
{
    /* Send STOP condition: Set TWINT, TWEN, and TWSTO bits */
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);  /**< TWSTO requests STOP condition */
    /* Note: STOP executes asynchronously, no need to wait for TWINT */
};


/* ============================================================================
 *                       LOW-LEVEL WRITE FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Write single byte to I2C bus
 * @param _Data: 8-bit data to transmit (can be address or data)
 * @retval None
 * @note Used for both slave address transmission and data transmission
 *       Waits for slave acknowledgment before returning
 *       Blocks until transmission completes (TWINT flag set)
 * ------------------------------------------------------- */
void i2c_Write(uint8_t _Data)
{
    /* Load data into TWI Data Register */
    TWDR = _Data;                        /**< Write byte to be transmitted into TWDR */
    
    /* Initiate transmission: Clear TWINT flag and enable TWI */
    TWCR = (1 << TWINT) | (1 << TWEN);   /**< Clearing TWINT starts transmission */
    
    /* Wait for transmission to complete */
    bitWaitHigh(TWCR, TWINT);            /**< Block until TWINT flag is set by hardware (byte transmitted + ACK received) */
};


/* ============================================================================
 *                       HIGH-LEVEL WRITE FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Write multiple bytes to I2C slave device
 * @param _address: 7-bit I2C slave address (0x00 to 0x7F)
 * @param _Data: Pointer to data buffer to transmit
 * @param _dataLength: Number of bytes to write
 * @retval None
 * @note Complete I2C write transaction sequence:
 *       1. Generate START condition
 *       2. Send slave address with write bit (LSB = 0)
 *       3. Send all data bytes sequentially
 *       4. Generate STOP condition
 * ------------------------------------------------------- */
void i2c_writeAddress(uint8_t _address, uint8_t* _Data, uint16_t _dataLength)
{
    /* Start I2C transaction */
    i2c_Start();                         /**< Generate START condition to begin communication */
    
    /* Send slave address with write command */
    i2c_Write((_address << 1) | __i2c_WriteCmd);  /**< Shift 7-bit address left, append write bit (0) */
    
    /* Transmit all data bytes */
    while(_dataLength)                   /**< Loop until all bytes transmitted */
    {
        i2c_Write(*_Data++);             /**< Send current byte and increment pointer */
        _dataLength--;                   /**< Decrement remaining byte count */
    };
    
    /* End I2C transaction */
    i2c_Stop();                          /**< Generate STOP condition to release bus */
};


/* ============================================================================
 *                       LOW-LEVEL READ FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Read single byte from I2C bus
 * @param _ackNack: Acknowledgment control (__i2c_ackCmd or __i2c_nackCmd)
 * @retval uint8_t: Received data byte from slave
 * @note Master sends ACK to request more data, or NACK to signal end of read
 *       Use __i2c_ackCmd (1) for all bytes except the last one
 *       Use __i2c_nackCmd (0) for the last byte to signal slave to stop sending
 * ------------------------------------------------------- */
uint8_t i2c_Read(bool _ackNack)
{
    /* Initiate reception with ACK/NACK control */
    TWCR = (1 << TWINT) | (_ackNack << TWEA) | (1 << TWEN);  /**< TWEA=1 sends ACK, TWEA=0 sends NACK */
    
    /* Wait for byte reception to complete */
    bitWaitHigh(TWCR, TWINT);            /**< Block until TWINT flag set (byte received) */
    
    /* Return received data */
    return TWDR;                         /**< Read received byte from TWI Data Register */
};


/* ============================================================================
 *                       HIGH-LEVEL READ FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Read multiple bytes from I2C slave device
 * @param _address: 7-bit I2C slave address (0x00 to 0x7F)
 * @param _Data: Pointer to buffer for storing received data
 * @param _dataLength: Number of bytes to read
 * @retval None
 * @note Complete I2C read transaction sequence:
 *       1. Generate START condition
 *       2. Send slave address with read bit (LSB = 1)
 *       3. Read all bytes with ACK, except last byte with NACK
 *       4. Generate STOP condition
 * ------------------------------------------------------- */
void i2c_readAdress(uint8_t _address, uint8_t* _Data, uint16_t _dataLength)
{
    /* Start I2C transaction */
    i2c_Start();                         /**< Generate START condition to begin communication */
    
    /* Send slave address with read command */
    i2c_Write((_address << 1) | __i2c_ReadCmd);  /**< Shift 7-bit address left, append read bit (1) */
    
    /* Receive all data bytes */
    while(_dataLength)                   /**< Loop until all bytes received */
    {
        /* Read byte with ACK for all except last byte (NACK for last) */
        *_Data++ = i2c_Read((_dataLength == 1) ? __i2c_nackCmd : __i2c_ackCmd);  /**< NACK on last byte signals end */
        _dataLength--;                   /**< Decrement remaining byte count */
    };
    
    /* End I2C transaction */
    i2c_Stop();                          /**< Generate STOP condition to release bus */
};


/* ============================================================================
 *                       SEQUENTIAL READ FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Perform write-then-read transaction with repeated START
 * @param _address: 7-bit I2C slave address
 * @param _txData: Pointer to write data buffer (typically register address)
 * @param _txDataLenght: Number of bytes to write
 * @param _rxData: Pointer to read data buffer
 * @param _rxDataLenght: Number of bytes to read
 * @retval None
 * @note Common pattern for register-based I2C devices (sensors, EEPROMs):
 *       1. START condition
 *       2. Write slave address + write bit
 *       3. Write register address (or multiple bytes)
 *       4. Repeated START condition (no STOP between write and read)
 *       5. Write slave address + read bit
 *       6. Read data bytes with ACK/NACK control
 *       7. STOP condition
 * @note Example: Read 2 bytes from register 0x0F on device 0x68
 *       uint8_t reg = 0x0F;
 *       uint8_t data[2];
 *       i2c_readSequential(0x68, &reg, 1, data, 2);
 * ------------------------------------------------------- */
void i2c_readSequential(uint8_t _address, uint8_t *_txData, uint16_t _txDataLenght, uint8_t *_rxData, uint16_t _rxDataLenght)
{
    /* ===== WRITE PHASE (typically register address) ===== */
    i2c_Start();                         /**< Generate START condition to begin transaction */
    
    /* Send slave address with write command */
    i2c_Write((_address << 1) | __i2c_WriteCmd);  /**< Address slave device in write mode */
    
    /* Write all transmit bytes (usually register address) */
    while(_txDataLenght)                 /**< Loop through all bytes to write */
    {
        i2c_Write(*_txData++);           /**< Send current byte and increment pointer */
        _txDataLenght--;                 /**< Decrement remaining write byte count */
    };
    
    /* ===== READ PHASE ===== */
    i2c_Start();                         /**< Generate repeated START (no STOP between write and read) */
    
    /* Send slave address with read command */
    i2c_Write((_address << 1) | __i2c_ReadCmd);  /**< Address same slave device in read mode */
    
    /* Receive all data bytes */
    while(_rxDataLenght)                 /**< Loop until all bytes received */
    {
        /* Read byte with ACK for all except last byte (NACK for last) */
        *_rxData++ = i2c_Read((_rxDataLenght == 1) ? __i2c_nackCmd : __i2c_ackCmd);  /**< NACK signals end of read */
        _rxDataLenght--;                 /**< Decrement remaining read byte count */
    };
    
    /* End I2C transaction */
    i2c_Stop();                          /**< Generate STOP condition to complete transaction and release bus */
};