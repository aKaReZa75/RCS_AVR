/**
 ******************************************************************************
 * @file     err.h
 * @brief    Error detection library (Checksum & CRC) header for AVR
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This header file provides definitions for:
 *           - CRC configuration structures (8/16/32-bit)
 *           - Simple additive checksum functions (8/16/32-bit)
 *           - Configurable CRC functions (8/16/32-bit)
 *           Suitable for communication protocols and data integrity verification.
 * 
 * @note     FUNCTION SUMMARY:
 *           - checkSum8_Calc  / checkSum16_Calc / checkSum32_Calc : Additive checksum
 *           - CRC8_Calc       / CRC16_Calc      / CRC32_Calc      : Table-less bitwise CRC
 * 
 * @note     CRC parameters (Poly, Init, refIn, refOut, xorOut) follow the
 *           standard "Rocksoft" model, so any standard CRC can be configured.
 * 
 * @note     Usage Example:
 *           // CRC-16/CCITT-FALSE
 *           hcrc16_T crc16 = {.Poly = 0x1021, .Init = 0xFFFF, .refIn = false, .refOut = false, .xorOut = 0x0000};
 *           uint16_t result = CRC16_Calc(&crc16, data, sizeof(data));   // "123456789" -> 0x29B1
 * 
 *           // CRC-32 (Ethernet / ZIP)
 *           hcrc32_T crc32 = {.Poly = 0x04C11DB7, .Init = 0xFFFFFFFF, .refIn = true, .refOut = true, .xorOut = 0xFFFFFFFF};
 *           uint32_t result32 = CRC32_Calc(&crc32, data, sizeof(data)); // "123456789" -> 0xCBF43926
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/Error_Detection
 ******************************************************************************
 */
#ifndef _err_H_
#define _err_H_

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
 *                         CRC CONFIGURATION STRUCTURES
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief CRC8 configuration structure
 * @note Holds all parameters needed to configure a CRC-8 calculation
 * ------------------------------------------------------- */
typedef struct 
{
  uint8_t Poly;    /**< Polynomial value */
  uint8_t Init;    /**< Initial CRC value */
  bool refIn;      /**< Reflect input bytes (true/false) */
  bool refOut;     /**< Reflect final CRC (true/false) */
  uint8_t xorOut;  /**< Final XOR value */
} hcrc8_T;

/* -------------------------------------------------------
 * @brief CRC16 configuration structure
 * @note Holds all parameters needed to configure a CRC-16 calculation
 * ------------------------------------------------------- */
typedef struct 
{
  uint16_t Poly;    /**< Polynomial value */
  uint16_t Init;    /**< Initial CRC value */
  bool refIn;       /**< Reflect input bytes (true/false) */
  bool refOut;      /**< Reflect final CRC (true/false) */
  uint16_t xorOut;  /**< Final XOR value */
} hcrc16_T;

/* -------------------------------------------------------
 * @brief CRC32 configuration structure
 * @note Holds all parameters needed to configure a CRC-32 calculation
 * ------------------------------------------------------- */
typedef struct 
{
  uint32_t Poly;    /**< Polynomial value */
  uint32_t Init;    /**< Initial CRC value */
  bool refIn;       /**< Reflect input bytes (true/false) */
  bool refOut;      /**< Reflect final CRC (true/false) */
  uint32_t xorOut;  /**< Final XOR value */
} hcrc32_T;


/* ============================================================================
 *                         CHECKSUM FUNCTION PROTOTYPES
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Calculate 8-bit additive checksum
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 8-bit checksum
 * @note Sum of all bytes modulo 256
 * ------------------------------------------------------- */
uint8_t checkSum8_Calc(uint8_t *_data, uint16_t _dataLength);

/* -------------------------------------------------------
 * @brief Calculate 16-bit additive checksum
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 16-bit checksum
 * @note Sum of all bytes modulo 65536
 * ------------------------------------------------------- */
uint16_t checkSum16_Calc(uint8_t *_data, uint16_t _dataLength);

/* -------------------------------------------------------
 * @brief Calculate 32-bit additive checksum
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 32-bit checksum
 * @note Sum of all bytes modulo 2^32
 * ------------------------------------------------------- */
uint32_t checkSum32_Calc(uint8_t *_data, uint16_t _dataLength);


/* ============================================================================
 *                         CRC FUNCTION PROTOTYPES
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Calculate 8-bit CRC
 * @param hcrc        Pointer to CRC8 configuration structure
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 8-bit CRC
 * ------------------------------------------------------- */
uint8_t CRC8_Calc(hcrc8_T *hcrc, uint8_t *_data, uint16_t _dataLength);

/* -------------------------------------------------------
 * @brief Calculate 16-bit CRC
 * @param hcrc        Pointer to CRC16 configuration structure
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 16-bit CRC
 * ------------------------------------------------------- */
uint16_t CRC16_Calc(hcrc16_T *hcrc, uint8_t *_data, uint16_t _dataLength);

/* -------------------------------------------------------
 * @brief Calculate 32-bit CRC
 * @param hcrc        Pointer to CRC32 configuration structure
 * @param _data       Pointer to input data array
 * @param _dataLength Length of data in bytes
 * @retval Calculated 32-bit CRC
 * ------------------------------------------------------- */
uint32_t CRC32_Calc(hcrc32_T *hcrc, uint8_t *_data, uint16_t _dataLength);

#endif /* _err_H_ */
