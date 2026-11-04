/**
 ******************************************************************************
 * @file     i2c.h
 * @brief    I2C (TWI) communication library header for AVR microcontrollers
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This library provides complete I2C master mode functionality for
 *           AVR microcontrollers using the hardware TWI peripheral.
 * 
 * @note     FUNCTION SUMMARY:
 *           - i2c_Init           : Initialize I2C peripheral with desired SCL frequency
 *           - i2c_Start          : Generate START condition to begin I2C transaction
 *           - i2c_Stop           : Generate STOP condition to end I2C transaction
 *           - i2c_Write          : Transmit single byte on I2C bus and wait for acknowledgment
 *           - i2c_Read           : Receive single byte from I2C bus with ACK/NACK control
 *           - i2c_writeAddress   : Write multiple bytes to slave device at specified address
 *           - i2c_readAdress     : Read multiple bytes from slave device at specified address
 *           - i2c_readSequential : Write-then-read operation for register-based devices (sensors, EEPROMs)
 * 
 * @note     Features:
 *           - Hardware TWI/I2C master mode operation
 *           - Support for standard (100kHz) and fast mode (400kHz) speeds
 *           - Multi-byte read/write transactions
 *           - Sequential read with register addressing (common for sensors)
 *           - Blocking operations with status checking
 *           - Compatible with all I2C slave devices
 * 
 * @note     Hardware Requirements:
 *           - AVR microcontroller with TWI peripheral (ATmega328P, etc.)
 *           - External pull-up resistors on SDA and SCL lines (typically 4.7kΩ)
 *           - Proper I2C bus voltage levels (5V or 3.3V depending on device)
 * 
 * @note     Usage:
 *           1. Call i2c_Init() once during system initialization
 *           2. Use i2c_writeAddress() for simple write operations
 *           3. Use i2c_readAdress() for simple read operations
 *           4. Use i2c_readSequential() for register-based read (write register address, then read data)
 * 
 * @note     Example - Read sensor register:
 *           uint8_t regAddr = 0x0F;  // Register address
 *           uint8_t data;
 *           i2c_readSequential(0x68, &regAddr, 1, &data, 1);
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_I2C
 ******************************************************************************
 */
#ifndef _i2c_H_
#define _i2c_H_

#include "aKaReZa.h"


/* ============================================================================
 *                    CRITICAL DEPENDENCY CHECK
 * ============================================================================
 *  This library requires the aKaReZa.h base library to compile correctly.
 *  If the file is missing, please download it or contact for support.
 * ============================================================================ */
#ifndef _aKaReZa_H_
    #warning "============================================================"
    #warning " [WARNING] Missing required dependency: aKaReZa.h"
    #warning "------------------------------------------------------------"
    #warning "  This module depends on the aKaReZa.h base library."
    #warning "  Please download it from: https://github.com/aKaReZa75/AVR_RawProject"
    #warning "  Or contact for support: akaReza75@gmail.com"
    #warning "------------------------------------------------------------"
    #error   "Compilation aborted: Required file 'aKaReZa.h' not found!"
    #warning "============================================================"
#endif


/* ============================================================================
 *                         I2C COMMAND DEFINITIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief I2C read/write direction control bits
 * @note These are appended to 7-bit slave address to form 8-bit address byte
 * ------------------------------------------------------- */
#define __i2c_WriteCmd 0x00              /**< Write operation - LSB = 0 in address byte */
#define __i2c_ReadCmd  0x01              /**< Read operation - LSB = 1 in address byte */

/* -------------------------------------------------------
 * @brief I2C acknowledge control bits
 * @note Used in i2c_Read() to control ACK/NACK after byte reception
 * ------------------------------------------------------- */
#define __i2c_ackCmd   0x01              /**< Send ACK after read - continue receiving more bytes */
#define __i2c_nackCmd  0x00              /**< Send NACK after read - signal end of reception to slave */


/* ============================================================================
 *                         FUNCTION PROTOTYPES
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize I2C (TWI) peripheral in master mode
 * @retval None
 * @note Configures SCL frequency based on CPU clock and TWBR register
 *       Enables TWI peripheral and sets pull-up resistors (if internal)
 *       Must be called before any I2C operations
 * ------------------------------------------------------- */
void i2c_Init(void);

/* -------------------------------------------------------
 * @brief Generate START condition on I2C bus
 * @retval None
 * @note START condition: SDA transitions from HIGH to LOW while SCL is HIGH
 *       Signals beginning of I2C transaction
 *       Must be called before addressing slave device
 * ------------------------------------------------------- */
void i2c_Start(void);

/* -------------------------------------------------------
 * @brief Generate STOP condition on I2C bus
 * @retval None
 * @note STOP condition: SDA transitions from LOW to HIGH while SCL is HIGH
 *       Signals end of I2C transaction and releases bus
 *       Must be called after completing read/write operations
 * ------------------------------------------------------- */
void i2c_Stop(void);

/* -------------------------------------------------------
 * @brief Write single byte to I2C bus
 * @param _Data: 8-bit data byte to transmit
 * @retval None
 * @note Transmits one byte and waits for slave acknowledgment
 *       Function blocks until transmission completes
 *       Used for both address and data transmission
 * ------------------------------------------------------- */
void i2c_Write(uint8_t _Data);

/* -------------------------------------------------------
 * @brief Write multiple bytes to I2C slave device
 * @param _address: 7-bit I2C slave address (without R/W bit)
 * @param _Data: Pointer to data buffer to transmit
 * @param _dataLength: Number of bytes to write
 * @retval None
 * @note Handles complete write transaction: START, address, data, STOP
 *       Automatically appends write bit to address
 * ------------------------------------------------------- */
void i2c_writeAddress(uint8_t _address, uint8_t* _Data, uint16_t _dataLength);

/* -------------------------------------------------------
 * @brief Read single byte from I2C bus
 * @param _ackNack: ACK/NACK control (__i2c_ackCmd or __i2c_nackCmd)
 * @retval uint8_t: Received data byte
 * @note Use __i2c_ackCmd (1) to continue reading more bytes
 *       Use __i2c_nackCmd (0) for last byte to signal end of read
 *       Function blocks until byte is received
 * ------------------------------------------------------- */
uint8_t i2c_Read(bool _ackNack);

/* -------------------------------------------------------
 * @brief Read multiple bytes from I2C slave device
 * @param _address: 7-bit I2C slave address (without R/W bit)
 * @param _Data: Pointer to buffer for received data
 * @param _dataLength: Number of bytes to read
 * @retval None
 * @note Handles complete read transaction: START, address, data, STOP
 *       Automatically sends ACK for all bytes except last (NACK)
 *       Automatically appends read bit to address
 * ------------------------------------------------------- */
void i2c_readAdress(uint8_t _address, uint8_t* _Data, uint16_t _dataLength);

/* -------------------------------------------------------
 * @brief Perform write-then-read transaction (sequential read)
 * @param _address: 7-bit I2C slave address (without R/W bit)
 * @param _txData: Pointer to write data buffer (typically register address)
 * @param _txDataLenght: Number of bytes to write before read
 * @param _rxData: Pointer to read data buffer
 * @param _rxDataLenght: Number of bytes to read
 * @retval None
 * @note Common operation for register-based I2C devices (sensors, EEPROMs)
 *       Sequence: START, Write(address+register), ReSTART, Read(data), STOP
 *       Example: Write register address 0x0F, then read 2 bytes of data
 * ------------------------------------------------------- */
void i2c_readSequential(uint8_t _address, uint8_t *_txData, uint16_t _txDataLenght, uint8_t *_rxData, uint16_t _rxDataLenght);

#endif /* _i2c_H_ */