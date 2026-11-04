/**
 ******************************************************************************
 * @file     err.c
 * @brief    Error detection library (Checksum & CRC) implementation for AVR
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This file provides:
 *           - Simple additive checksum (8/16/32-bit)
 *           - Configurable bitwise CRC (8/16/32-bit)
 *           - Internal bit reflection helper used by the CRC functions
 * 
 * @note     FUNCTION SUMMARY:
 *           - checkSum8_Calc  : 8-bit additive checksum
 *           - checkSum16_Calc : 16-bit additive checksum
 *           - checkSum32_Calc : 32-bit additive checksum
 *           - bitReflected    : Reverse the bit order of a value (internal helper)
 *           - CRC8_Calc       : 8-bit CRC with configurable parameters
 *           - CRC16_Calc      : 16-bit CRC with configurable parameters
 *           - CRC32_Calc      : 32-bit CRC with configurable parameters
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/Error_Detection
 ******************************************************************************
 */

#include "err.h"


/* ============================================================================
 *                         CHECKSUM FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Calculate 8-bit additive checksum
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 8-bit checksum
 * @note Simple and fast, but it cannot detect all errors
 *       (e.g. swapped bytes or errors that cancel each other out)
 * ------------------------------------------------------- */
uint8_t checkSum8_Calc(uint8_t *_data, uint16_t _dataLength)
{
    uint8_t _Sum = 0x00;
    uint16_t _index = 0x00;

    for(_index = 0; _index < _dataLength; _index++)
    {
        _Sum += _data[_index];
    };

    return _Sum;
};


/* -------------------------------------------------------
 * @brief Calculate 16-bit additive checksum
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 16-bit checksum
 * @note Better detection than the 8-bit version with the same simplicity
 * ------------------------------------------------------- */
uint16_t checkSum16_Calc(uint8_t *_data, uint16_t _dataLength)
{
    uint16_t _Sum = 0x00;
    uint16_t _index = 0x00;

    for(_index = 0; _index < _dataLength; _index++)
    {
        _Sum += _data[_index];
    };

    return _Sum;
};


/* -------------------------------------------------------
 * @brief Calculate 32-bit additive checksum
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 32-bit checksum
 * @note Highest detection level among the additive checksums
 * ------------------------------------------------------- */
uint32_t checkSum32_Calc(uint8_t *_data, uint16_t _dataLength)
{
    uint32_t _Sum = 0x00;
    uint16_t _index = 0x00;

    for(_index = 0; _index < _dataLength; _index++)
    {
        _Sum += _data[_index];
    };

    return _Sum;
};


/* ============================================================================
 *                         HELPER FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Reflect (reverse) the bit order of a value
 * @param _data     Input value to reflect
 * @param _dataBits Number of bits to reflect (8, 16 or 32)
 * @retval Bit-reflected value
 * @note Used internally by the CRC functions when the CRC standard
 *       requires input or output reflection
 * ------------------------------------------------------- */
uint32_t bitReflected(uint32_t _data, uint8_t _dataBits)
{
    uint32_t _temp = 0x00;
    uint8_t _index = 0x00;
    _dataBits--;

    for(_index = 0; _index <= _dataBits; _index++)
    {
        bitChange(_temp, (_dataBits-_index), bitCheck(_data, _index));
    }

    return _temp;
};


/* ============================================================================
 *                         CRC FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Calculate 8-bit CRC
 * @param hcrc        Pointer to CRC8 configuration structure
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 8-bit CRC
 * @note Bitwise (table-less) implementation with configurable:
 *       - Polynomial
 *       - Initial value
 *       - Input/output reflection
 *       - Final XOR value
 * ------------------------------------------------------- */
uint8_t CRC8_Calc(hcrc8_T *hcrc, uint8_t *_data, uint16_t _dataLength)
{
    uint8_t _CRC = hcrc->Init;
    uint8_t _bitIndex  = 0x00;
    uint16_t _dataIndex = 0x00;
    uint8_t _dataTemp  = 0x00;

    for(_dataIndex = 0; _dataIndex < _dataLength; _dataIndex++)
    {
        _dataTemp = _data[_dataIndex];
        if(hcrc->refIn)
        {
           _dataTemp = (uint8_t) bitReflected(_dataTemp, 8);
        }

        _CRC ^= _dataTemp;

        for(_bitIndex = 0; _bitIndex < 8; _bitIndex++)
        {
            if(bitCheckHigh(_CRC, 7))
            {
                bitShiftLeft(_CRC, 1);
                _CRC ^= hcrc->Poly;
            }
            else
            {
                bitShiftLeft(_CRC, 1);
            };
        };
    };

    _CRC ^= hcrc->xorOut;  
    if(hcrc->refOut)
    {
        _CRC = (uint8_t) bitReflected(_CRC, 8);
    }

    return _CRC;
};


/* -------------------------------------------------------
 * @brief Calculate 16-bit CRC
 * @param hcrc        Pointer to CRC16 configuration structure
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 16-bit CRC
 * @note Bitwise (table-less) implementation with configurable:
 *       - Polynomial
 *       - Initial value
 *       - Input/output reflection
 *       - Final XOR value
 * ------------------------------------------------------- */
uint16_t CRC16_Calc(hcrc16_T *hcrc, uint8_t *_data, uint16_t _dataLength)
{
    uint16_t _CRC = hcrc->Init;
    uint16_t _bitIndex  = 0x00;
    uint16_t _dataIndex = 0x00;
    uint8_t _dataTemp  = 0x00;

    for(_dataIndex = 0; _dataIndex < _dataLength; _dataIndex++)
    {
        _dataTemp = _data[_dataIndex];

        if(hcrc->refIn)
        {
            _dataTemp = bitReflected(_dataTemp, 8);
        };

        _CRC ^= ((uint16_t)_dataTemp<<8);

        for(_bitIndex = 0; _bitIndex < 8; _bitIndex++)
        {
            if(bitCheckHigh(_CRC, 15))
            {
                bitShiftLeft(_CRC, 1);
                _CRC ^= hcrc->Poly;
            }
            else
            {
                _CRC <<= 1;
            };
        };
    };

    _CRC ^= hcrc->xorOut; 

    if(hcrc->refOut)
    {
        _CRC = (uint16_t) bitReflected(_CRC, 16);
    };

    return _CRC;
};


/* -------------------------------------------------------
 * @brief Calculate 32-bit CRC
 * @param hcrc        Pointer to CRC32 configuration structure
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 32-bit CRC
 * @note Bitwise (table-less) implementation with configurable:
 *       - Polynomial
 *       - Initial value
 *       - Input/output reflection
 *       - Final XOR value
 * ------------------------------------------------------- */
uint32_t CRC32_Calc(hcrc32_T *hcrc, uint8_t *_data, uint16_t _dataLength)
{
    uint32_t _CRC = hcrc->Init;
    uint32_t _bitIndex  = 0x00;
    uint32_t _dataIndex = 0x00;
    uint8_t _dataTemp  = 0x00;

    for(_dataIndex = 0; _dataIndex < _dataLength; _dataIndex++)
    {
        _dataTemp = _data[_dataIndex];

        if(hcrc->refIn)
        {
            _dataTemp = bitReflected(_dataTemp, 8);
        };

        _CRC ^= ((uint32_t)_dataTemp)<<24;

        for(_bitIndex = 0; _bitIndex < 8; _bitIndex++)
        {
            if(bitCheckHigh(_CRC, 31))
            {
                bitShiftLeft(_CRC, 1);
                _CRC ^= hcrc->Poly;
            }
            else
            {
                _CRC <<= 1;
            };
        };
    };

    _CRC ^= hcrc->xorOut; 

    if(hcrc->refOut)
    {
        _CRC = (uint32_t) bitReflected(_CRC, 32);
    };

    return _CRC;
};
