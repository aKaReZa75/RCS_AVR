/**
 ******************************************************************************
 * @file     e2prom.h
 * @brief    AVR internal EEPROM driver header with multi-type support
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This library provides complete interface for AVR internal EEPROM
 *           with support for multiple data types and wear-leveling updates.
 * 
 * @note     FUNCTION SUMMARY:
 *           Write Functions (uint8, uint16, uint32, float, int8, int16, int32, block):
 *           - e2prom_uint8_Write    : Write single unsigned 8-bit value to EEPROM
 *           - e2prom_uint16_Write   : Write unsigned 16-bit value (2 bytes)
 *           - e2prom_uint32_Write   : Write unsigned 32-bit value (4 bytes)
 *           - e2prom_float_Write    : Write float value (4 bytes, IEEE 754)
 *           - e2prom_int8_Write     : Write single signed 8-bit value
 *           - e2prom_int16_Write    : Write signed 16-bit value (2 bytes)
 *           - e2prom_int32_Write    : Write signed 32-bit value (4 bytes)
 *           - e2prom_block_Write    : Write array of bytes (multiple bytes)
 *
 *           Read Functions (uint8, uint16, uint32, float, int8, int16, int32, block):
 *           - e2prom_uint8_Read     : Read single unsigned 8-bit value from EEPROM
 *           - e2prom_uint16_Read    : Read unsigned 16-bit value (2 bytes)
 *           - e2prom_uint32_Read    : Read unsigned 32-bit value (4 bytes)
 *           - e2prom_float_Read     : Read float value (4 bytes, IEEE 754)
 *           - e2prom_int8_Read      : Read single signed 8-bit value
 *           - e2prom_int16_Read     : Read signed 16-bit value (2 bytes)
 *           - e2prom_int32_Read     : Read signed 32-bit value (4 bytes)
 *           - e2prom_block_Read     : Read array of bytes (multiple bytes)
 *
 *           Update Functions (uint8, uint16, uint32, float, int8, int16, int32, block):
 *           - e2prom_uint8_Update    : Update 8-bit value (read-compare-write if different)
 *           - e2prom_uint16_Update   : Update 16-bit value (wear-leveling)
 *           - e2prom_uint32_Update   : Update 32-bit value (wear-leveling)
 *           - e2prom_float_Update    : Update float value (with epsilon comparison)
 *           - e2prom_int8_Update     : Update signed 8-bit value (wear-leveling)
 *           - e2prom_int16_Update    : Update signed 16-bit value (wear-leveling)
 *           - e2prom_int32_Update    : Update signed 32-bit value (wear-leveling)
 *           - e2prom_block_Update    : Update block of bytes (byte-by-byte comparison)
 *
 *           Utility Functions:
 *           - e2prom_Format         : Erase entire EEPROM (write 0xFF to all addresses)
 * 
 * @note     AVR Internal EEPROM Specifications:
 *           - ATmega328P: 1024 bytes (1 KB)
 *           - ATmega2560: 4096 bytes (4 KB)
 *           - ATmega168: 512 bytes
 *           - ATmega8: 512 bytes
 *           - Address range: 0x0000 to (EEPROM_SIZE - 1)
 *           - Write time: ~3.3ms typical (independent of CPU frequency)
 *           - Endurance: 100,000 write/erase cycles minimum
 *           - Data retention: 20 years at 85°C, 100 years at 25°C
 *           - Supply voltage: Operates same as MCU voltage
 * 
 * @note     Write Modes:
 *           1. Atomic Write: Uses AVR hardware EEPROM controller
 *              - Automatic erase-before-write
 *              - Write only mode (EEPM1:EEPM0 = 00)
 *           2. Update Mode: Read-modify-write only if data changed
 *              - Reduces EEPROM wear significantly
 *              - Extends lifespan by avoiding unnecessary cycles
 * 
 * @note     Advantages over External EEPROM (AT24C):
 *           - No external components required (integrated on-chip)
 *           - No I2C communication overhead (faster access)
 *           - No additional pins required
 *           - Lower power consumption
 *           - Simpler code (direct register access)
 * 
 * @note     Usage Example:
 *           // Write operations
 *           e2prom_uint16_Write(0x0000, 1234);    // Write uint16 at address 0
 *           e2prom_float_Write(0x0100, 3.14159);  // Write float at address 256
 *           
 *           // Read operations
 *           uint16_t value;
 *           e2prom_uint16_Read(0x0000, &value);   // Read uint16 from address 0
 *           
 *           // Update (wear-leveling)
 *           e2prom_uint16_Update(0x0000, 5678);    // Only writes if different
 *           
 *           // Format entire EEPROM
 *           e2prom_Format();                      // Write 0xFF to all locations
 * 
 * @note     Important Considerations:
 *           - Always use update functions when possible (reduces wear)
 *           - Multi-byte values stored in little-endian format
 *           - EEPROM write takes ~3.3ms - plan accordingly in time-critical code
 *           - Write operations automatically wait for completion (blocking)
 *           - Power loss during write may corrupt single byte only
 * 
 * @note     Comparison with AT24C02 External EEPROM:
 *           Internal EEPROM (this library):
 *           + No external components, faster, simpler
 *           - Limited size (512B-4KB depending on MCU)
 *           
 *           AT24C02 External (at24c02.h):
 *           + Expandable (multiple devices on I2C)
 *           + Larger capacity options available
 *           - Requires I2C, external component, slower
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_e2prom
 ******************************************************************************
 */
#ifndef _e2prom_H_
#define _e2prom_H_

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
 *                         DATA TYPE UNIONS
 * ============================================================================ */

/**
 * @brief 16-bit data type union for EEPROM byte-level access
 * @note Allows access to 16-bit value as individual bytes for storage
 *       Little-endian byte order: u8[0]=LSB, u8[1]=MSB
 */
typedef union
{
    uint16_t u16;                        /**< Unsigned 16-bit integer (0 to 65535) */
    int16_t  i16;                        /**< Signed 16-bit integer (-32768 to 32767) */
    uint8_t  u8[2];                      /**< Byte array representation [LSB, MSB] */
} e2p_data16_t;

/**
 * @brief 32-bit data type union for EEPROM byte-level access
 * @note Allows access to 32-bit value as individual bytes for storage
 *       Little-endian byte order: u8[0]=LSB ... u8[3]=MSB
 */
typedef union
{
    uint32_t u32;                        /**< Unsigned 32-bit integer (0 to 4,294,967,295) */
    int32_t  i32;                        /**< Signed 32-bit integer (-2,147,483,648 to 2,147,483,647) */
    float    f32;                        /**< IEEE 754 single-precision float */
    uint8_t  u8[4];                      /**< Byte array representation [LSB, byte1, byte2, MSB] */
} e2p_data32_t;


/* ============================================================================
 *                    UNSIGNED INTEGER WRITE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Write unsigned 8-bit value to internal EEPROM
 * @param _address: EEPROM address (0 to EEPROM_SIZE-1)
 * @param _Value: 8-bit value to write
 * @retval None
 * @note Blocks for ~3.3ms during write operation
 */
void e2prom_uint8_Write(uint16_t _address, uint8_t _Value);

/**
 * @brief Write unsigned 16-bit value to internal EEPROM
 * @param _address: EEPROM starting address (needs 2 consecutive bytes)
 * @param _Value: 16-bit value to write
 * @retval None
 * @note Stores in little-endian format (LSB first)
 *       Total write time: ~6.6ms (2 bytes × 3.3ms)
 */
void e2prom_uint16_Write(uint16_t _address, uint16_t _Value);

/**
 * @brief Write unsigned 32-bit value to internal EEPROM
 * @param _address: EEPROM starting address (needs 4 consecutive bytes)
 * @param _Value: 32-bit value to write
 * @retval None
 * @note Stores in little-endian format (LSB first)
 *       Total write time: ~13.2ms (4 bytes × 3.3ms)
 */
void e2prom_uint32_Write(uint16_t _address, uint32_t _Value);

/**
 * @brief Write float value to internal EEPROM
 * @param _address: EEPROM starting address (needs 4 consecutive bytes)
 * @param _Value: Float value to write (IEEE 754 format)
 * @retval None
 * @note Stores as 4 bytes in little-endian format
 *       Total write time: ~13.2ms (4 bytes × 3.3ms)
 */
void e2prom_float_Write(uint16_t _address, float _Value);

/**
 * @brief Write block of bytes to internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Data: Pointer to data buffer
 * @param _length: Number of bytes to write
 * @retval None
 * @note Write time: _length × 3.3ms
 */
void e2prom_block_Write(uint16_t _address, uint8_t *_Data, uint16_t _length);


/* ============================================================================
 *                    UNSIGNED INTEGER READ FUNCTIONS
 * ============================================================================ */

/**
 * @brief Read unsigned 8-bit value from internal EEPROM
 * @param _adress: EEPROM address (0 to EEPROM_SIZE-1)
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Read operation is fast (~4 CPU cycles)
 */
void e2prom_uint8_Read(uint16_t _adress, uint8_t* _Value);

/**
 * @brief Read unsigned 16-bit value from internal EEPROM
 * @param _adress: EEPROM starting address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Reads in little-endian format
 */
void e2prom_uint16_Read(uint16_t _adress, uint16_t* _Value);

/**
 * @brief Read unsigned 32-bit value from internal EEPROM
 * @param _adress: EEPROM starting address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Reads in little-endian format
 */
void e2prom_uint32_Read(uint16_t _adress, uint32_t* _Value);

/**
 * @brief Read float value from internal EEPROM
 * @param _adress: EEPROM starting address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Reads 4 bytes as IEEE 754 float
 */
void e2prom_float_Read(uint16_t _adress, float* _Value);

/**
 * @brief Read block of bytes from internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Data: Pointer to buffer for read data
 * @param _length: Number of bytes to read
 * @retval None
 */
void e2prom_block_Read(uint16_t _address, uint8_t *_Data, uint16_t _length);


/* ============================================================================
 *                    UNSIGNED INTEGER UPDATE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Update unsigned 8-bit value (write only if different)
 * @param _address: EEPROM address
 * @param _Value: New value to write
 * @retval None
 * @note Reads current value first, writes only if different
 *       Extends EEPROM lifespan by reducing write cycles
 */
void e2prom_uint8_Update(uint16_t _address, uint8_t _Value);

/**
 * @brief Update unsigned 16-bit value (write only if different)
 * @param _address: EEPROM starting address
 * @param _Value: New value to write
 * @retval None
 * @note Wear-leveling optimization
 */
void e2prom_uint16_Update(uint16_t _address, uint16_t _Value);

/**
 * @brief Update unsigned 32-bit value (write only if different)
 * @param _address: EEPROM starting address
 * @param _Value: New value to write
 * @retval None
 * @note Wear-leveling optimization
 */
void e2prom_uint32_Update(uint16_t _address, uint32_t _Value);

/**
 * @brief Update float value (write only if significantly different)
 * @param _address: EEPROM starting address
 * @param _Value: New value to write
 * @retval None
 * @note Uses epsilon comparison for float equality check
 */
void e2prom_float_Update(uint16_t _address, float _Value);

/**
 * @brief Update block of bytes (write only changed bytes)
 * @param _address: EEPROM starting address
 * @param _Data: Pointer to new data buffer
 * @param _length: Number of bytes to update
 * @retval None
 * @note Compares each byte individually, writes only differences
 */
void e2prom_block_Update(uint16_t _address, uint8_t *_Data, uint16_t _length);


/* ============================================================================
 *                    SIGNED INTEGER WRITE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Write signed 8-bit value to internal EEPROM
 * @param _address: EEPROM address
 * @param _Value: Signed 8-bit value to write (-128 to 127)
 * @retval None
 */
void e2prom_int8_Write(uint16_t _address, int8_t _Value);

/**
 * @brief Write signed 16-bit value to internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Value: Signed 16-bit value to write (-32768 to 32767)
 * @retval None
 * @note Stores in little-endian format
 */
void e2prom_int16_Write(uint16_t _address, int16_t _Value);

/**
 * @brief Write signed 32-bit value to internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Value: Signed 32-bit value to write
 * @retval None
 * @note Stores in little-endian format
 */
void e2prom_int32_Write(uint16_t _address, int32_t _Value);


/* ============================================================================
 *                    SIGNED INTEGER READ FUNCTIONS
 * ============================================================================ */

/**
 * @brief Read signed 8-bit value from internal EEPROM
 * @param _address: EEPROM address
 * @param _Value: Pointer to store read value
 * @retval None
 */
void e2prom_int8_Read(uint16_t _address, int8_t* _Value);

/**
 * @brief Read signed 16-bit value from internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Reads in little-endian format
 */
void e2prom_int16_Read(uint16_t _address, int16_t* _Value);

/**
 * @brief Read signed 32-bit value from internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Reads in little-endian format
 */
void e2prom_int32_Read(uint16_t _address, int32_t* _Value);


/* ============================================================================
 *                    SIGNED INTEGER UPDATE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Update signed 8-bit value (write only if different)
 * @param _address: EEPROM address
 * @param _Value: New signed value to write
 * @retval None
 * @note Wear-leveling optimization
 */
void e2prom_int8_Update(uint16_t _address, int8_t _Value);

/**
 * @brief Update signed 16-bit value (write only if different)
 * @param _address: EEPROM starting address
 * @param _Value: New signed value to write
 * @retval None
 * @note Wear-leveling optimization
 */
void e2prom_int16_Update(uint16_t _address, int16_t _Value);

/**
 * @brief Update signed 32-bit value (write only if different)
 * @param _address: EEPROM starting address
 * @param _Value: New signed value to write
 * @retval None
 * @note Wear-leveling optimization
 */
void e2prom_int32_Update(uint16_t _address, int32_t _Value);


/* ============================================================================
 *                         UTILITY FUNCTIONS
 * ============================================================================ */

/**
 * @brief Erase entire internal EEPROM (write 0xFF to all addresses)
 * @retval None
 * @note Time required: EEPROM_SIZE × 3.3ms
 *       - ATmega328P (1024 bytes): ~3.4 seconds
 *       - ATmega2560 (4096 bytes): ~13.5 seconds
 *       - ATmega168 (512 bytes): ~1.7 seconds
 * @note 0xFF is standard erased/blank state for EEPROM
 *       Use this to reset EEPROM to factory state
 */
void e2prom_Format(void);

#endif /* _e2prom_H_ */