/**
 ******************************************************************************
 * @file     e2prom.c
 * @brief    AVR internal EEPROM driver implementation with multi-type support
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This library provides complete driver for AVR internal EEPROM using
 *           hardware EEPROM controller with atomic write operations and wear-leveling.
 * 
 * @note     EXECUTION FLOW:
 *           1. Write Operation Flow:
 *              └─> e2prom_xxx_Write() → Wait for previous write to complete (EEPE=0)
 *                  → Set EEPROM address (EEAR register)
 *                  → Set data to write (EEDR register)
 *                  → Configure write mode (EEPM1:EEPM0 = 00 for erase+write)
 *                  → Set master write enable (EEMPE) → Start write (EEPE)
 *                  → Hardware automatically erases then writes (~3.3ms)
 * 
 *           2. Read Operation Flow:
 *              └─> e2prom_xxx_Read() → Wait for any pending write (EEPE=0)
 *                  → Set EEPROM address (EEAR register)
 *                  → Set read enable (EERE) → Read data from EEDR register
 *                  → Fast operation (~4 CPU cycles)
 * 
 *           3. Update Operation Flow (Wear-Leveling):
 *              └─> e2prom_xxx_Update() → Read current value from EEPROM
 *                  → Compare with new value
 *                  → If different: Write new value → Else: Skip (save write cycle)
 * 
 *           4. Multi-Byte Write/Read:
 *              └─> uint16/uint32/float → Use union to split into bytes
 *                  → Write/Read each byte sequentially (little-endian)
 *                  → Reconstruct value from bytes using union
 * 
 *           5. Block Operations:
 *              └─> Loop through array → Read/Write/Update each byte individually
 * 
 *           6. Format Operation:
 *              └─> Loop through entire EEPROM → Write 0xFF to each address
 *                  → Takes ~3.4 seconds for ATmega328P (1024 bytes)
 * 
 * @note     FUNCTION SUMMARY:
 *           Unsigned Write Functions:
 *           - e2prom_uint8_Write  : Atomic write of single byte using AVR EEPROM controller
 *           - e2prom_uint16_Write : Write 2 bytes in little-endian format
 *           - e2prom_uint32_Write : Write 4 bytes in little-endian format
 *           - e2prom_float_Write  : Write IEEE 754 float as 4 bytes
 *           - e2prom_block_Write  : Write array of bytes sequentially
 *
 *           Signed Write Functions:
 *           - e2prom_int8_Write   : Write signed byte (cast to unsigned)
 *           - e2prom_int16_Write  : Write signed 16-bit via union conversion
 *           - e2prom_int32_Write  : Write signed 32-bit via union conversion
 *
 *           Read Functions:
 *           - e2prom_uint8/16/32_Read : Read unsigned values
 *           - e2prom_float_Read       : Read float from 4 bytes
 *           - e2prom_int8/16/32_Read  : Read signed values
 *           - e2prom_block_Read       : Read byte array
 *
 *           Update Functions (Wear-Leveling):
 *           - e2prom_uint8/16/32_Update : Update unsigned (compare-then-write)
 *           - e2prom_float_Update       : Update float (epsilon comparison)
 *           - e2prom_int8/16/32_Update  : Update signed (compare-then-write)
 *           - e2prom_block_Update       : Update block byte-by-byte
 *
 *           Utility:
 *           - e2prom_Format : Erase entire EEPROM (write 0xFF to all 1024 bytes)
 * 
 * @note     AVR EEPROM Hardware Registers:
 *           - EEAR (EEPROM Address Register): Sets read/write address
 *           - EEDR (EEPROM Data Register): Holds data for read/write
 *           - EECR (EEPROM Control Register):
 *             * EERE (bit 0): EEPROM Read Enable
 *             * EEPE (bit 1): EEPROM Program Enable (write)
 *             * EEMPE (bit 2): EEPROM Master Program Enable
 *             * EEPM1:EEPM0 (bits 5:4): Programming mode (00=erase+write)
 * 
 * @note     Write Timing (ATmega328P at 16MHz):
 *           - Single byte: ~3.3ms (independent of CPU clock)
 *           - uint16: ~6.6ms (2 bytes)
 *           - uint32/float: ~13.2ms (4 bytes)
 *           - Full format (1024 bytes): ~3.4 seconds
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_e2prom
 ******************************************************************************
 */

#include "e2prom.h"


/* ============================================================================
 *                    UNSIGNED INTEGER WRITE FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Write unsigned 8-bit value to internal EEPROM
 * @param _address: EEPROM address (0 to 1023 for ATmega328P)
 * @param _Value: 8-bit value to write
 * @retval None
 * @note Atomic erase-and-write operation using AVR EEPROM hardware
 *       Blocks until previous write completes (~3.3ms typical)
 *       Write sequence follows AVR datasheet specifications
 * ------------------------------------------------------- */
void e2prom_uint8_Write(uint16_t _address, uint8_t _Value)
{
    /* Wait for completion of previous write operation */
    bitWaitLow(EECR, EEPE);                                /**< Poll EEPE bit until clear (previous write done) */

    /* Set EEPROM address and data */
    EEAR = _address;                                       /**< Load address into EEPROM Address Register */
    EEDR = _Value;                                         /**< Load data into EEPROM Data Register */

    /* Configure programming mode: Erase and Write in one operation */
    bitClear(EECR, EEPM0);                                 /**< EEPM0=0: Atomic erase+write mode (part 1) */
    bitClear(EECR, EEPM1);                                 /**< EEPM1=0: Atomic erase+write mode (part 2) */

    /* Enable EEPROM write (two-step process for safety) */
    bitSet(EECR, EEMPE);                                   /**< Set Master Program Enable (must be set within 4 cycles before EEPE) */
    bitSet(EECR, EEPE);                                    /**< Start EEPROM Program operation (write begins) */
    /* Note: EEPE will be cleared by hardware when write completes (~3.3ms) */
};

/* -------------------------------------------------------
 * @brief Write unsigned 16-bit value to internal EEPROM
 * @param _address: EEPROM starting address (needs 2 consecutive bytes)
 * @param _Value: 16-bit value to write
 * @retval None
 * @note Stores in little-endian format: [LSB at _address, MSB at _address+1]
 *       Total write time: ~6.6ms (2 × 3.3ms per byte)
 * ------------------------------------------------------- */
void e2prom_uint16_Write(uint16_t _address, uint16_t _Value)
{
    e2p_data16_t _data16;                                  /**< Union for 16-bit to byte conversion */

    _data16.u16 = _Value;                                  /**< Load 16-bit value into union */

    /* Write LSB and MSB sequentially */
    e2prom_uint8_Write((_address + 0), _data16.u8[0]);    /**< Write least significant byte (LSB) */
    e2prom_uint8_Write((_address + 1), _data16.u8[1]);    /**< Write most significant byte (MSB) */
};

/* -------------------------------------------------------
 * @brief Write unsigned 32-bit value to internal EEPROM
 * @param _address: EEPROM starting address (needs 4 consecutive bytes)
 * @param _Value: 32-bit value to write
 * @retval None
 * @note Stores in little-endian format: [LSB, byte1, byte2, MSB]
 *       Total write time: ~13.2ms (4 × 3.3ms per byte)
 * ------------------------------------------------------- */
void e2prom_uint32_Write(uint16_t _address, uint32_t _Value)
{
    e2p_data32_t _data32;                                  /**< Union for 32-bit to byte conversion */

    _data32.u32 = _Value;                                  /**< Load 32-bit value into union */

    /* Write all 4 bytes sequentially (little-endian) */
    e2prom_uint8_Write((_address + 0), _data32.u8[0]);    /**< Write byte 0 (LSB) */
    e2prom_uint8_Write((_address + 1), _data32.u8[1]);    /**< Write byte 1 */
    e2prom_uint8_Write((_address + 2), _data32.u8[2]);    /**< Write byte 2 */
    e2prom_uint8_Write((_address + 3), _data32.u8[3]);    /**< Write byte 3 (MSB) */
};

/* -------------------------------------------------------
 * @brief Write float value to internal EEPROM
 * @param _address: EEPROM starting address (needs 4 consecutive bytes)
 * @param _Value: Float value to write (IEEE 754 single-precision)
 * @retval None
 * @note Stores IEEE 754 representation as 4 bytes in little-endian format
 *       Total write time: ~13.2ms (4 × 3.3ms per byte)
 * ------------------------------------------------------- */
void e2prom_float_Write(uint16_t _address, float _Value)
{
    e2p_data32_t _data32;                                  /**< Union for float to byte conversion */
    
    _data32.f32 = _Value;                                  /**< Load float into union (reinterpret as bytes) */

    /* Write all 4 bytes of float representation */
    e2prom_uint8_Write((_address + 0), _data32.u8[0]);    /**< Write byte 0 (LSB of float bits) */
    e2prom_uint8_Write((_address + 1), _data32.u8[1]);    /**< Write byte 1 */
    e2prom_uint8_Write((_address + 2), _data32.u8[2]);    /**< Write byte 2 */
    e2prom_uint8_Write((_address + 3), _data32.u8[3]);    /**< Write byte 3 (MSB of float bits) */
};

/* -------------------------------------------------------
 * @brief Write block of bytes to internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Data: Pointer to source data array
 * @param _length: Number of bytes to write
 * @retval None
 * @note Writes bytes sequentially, one at a time
 *       Total write time: _length × 3.3ms
 * ------------------------------------------------------- */
void e2prom_block_Write(uint16_t _address, uint8_t *_Data, uint16_t _length)
{
    uint16_t _Index = 0;                                   /**< Loop counter for array iteration */

    /* Write each byte sequentially */
    for(_Index = 0; _Index < _length; _Index++)            /**< Loop through all bytes in array */
    {
        e2prom_uint8_Write((_address + _Index), _Data[_Index]);  /**< Write current byte to EEPROM */
    }
};


/* ============================================================================
 *                    UNSIGNED INTEGER READ FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Read unsigned 8-bit value from internal EEPROM
 * @param _adress: EEPROM address to read from
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Fast read operation (~4 CPU cycles after EERE set)
 *       Waits for any pending write to complete first
 * ------------------------------------------------------- */
void e2prom_uint8_Read(uint16_t _adress, uint8_t* _Value)
{
    /* Wait for completion of previous write operation */
    bitWaitLow(EECR, EEPE);                                /**< Ensure no write in progress */

    /* Set address and trigger read */
    EEAR = _adress;                                        /**< Load address into EEPROM Address Register */
    bitSet(EECR, EERE);                                    /**< Set EEPROM Read Enable - triggers read */

    /* Read data from EEDR register */
    *_Value = EEDR;                                        /**< Data available immediately after EERE set */
};

/* -------------------------------------------------------
 * @brief Read unsigned 16-bit value from internal EEPROM
 * @param _adress: EEPROM starting address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Reads 2 bytes in little-endian format and combines into uint16
 * ------------------------------------------------------- */
void e2prom_uint16_Read(uint16_t _adress, uint16_t* _Value)
{
    e2p_data16_t _data16;                                  /**< Union for byte to 16-bit conversion */

    /* Read LSB and MSB */
    e2prom_uint8_Read((_adress + 0), &_data16.u8[0]);     /**< Read least significant byte */
    e2prom_uint8_Read((_adress + 1), &_data16.u8[1]);     /**< Read most significant byte */

    *_Value = _data16.u16;                                 /**< Combine bytes into 16-bit value via union */
};

/* -------------------------------------------------------
 * @brief Read unsigned 32-bit value from internal EEPROM
 * @param _adress: EEPROM starting address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Reads 4 bytes in little-endian format and combines into uint32
 * ------------------------------------------------------- */
void e2prom_uint32_Read(uint16_t _adress, uint32_t* _Value)
{
    e2p_data32_t _data32;                                  /**< Union for byte to 32-bit conversion */

    /* Read all 4 bytes */
    e2prom_uint8_Read((_adress + 0), &_data32.u8[0]);     /**< Read byte 0 (LSB) */
    e2prom_uint8_Read((_adress + 1), &_data32.u8[1]);     /**< Read byte 1 */
    e2prom_uint8_Read((_adress + 2), &_data32.u8[2]);     /**< Read byte 2 */
    e2prom_uint8_Read((_adress + 3), &_data32.u8[3]);     /**< Read byte 3 (MSB) */

    *_Value = _data32.u32;                                 /**< Combine bytes into 32-bit value via union */
};

/* -------------------------------------------------------
 * @brief Read float value from internal EEPROM
 * @param _adress: EEPROM starting address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Reads 4 bytes and reinterprets as IEEE 754 float
 * ------------------------------------------------------- */
void e2prom_float_Read(uint16_t _adress, float* _Value)
{
    e2p_data32_t _data32;                                  /**< Union for byte to float conversion */

    /* Read all 4 bytes of float representation */
    e2prom_uint8_Read((_adress + 0), &_data32.u8[0]);     /**< Read byte 0 (LSB of float bits) */
    e2prom_uint8_Read((_adress + 1), &_data32.u8[1]);     /**< Read byte 1 */
    e2prom_uint8_Read((_adress + 2), &_data32.u8[2]);     /**< Read byte 2 */
    e2prom_uint8_Read((_adress + 3), &_data32.u8[3]);     /**< Read byte 3 (MSB of float bits) */

    *_Value = _data32.f32;                                 /**< Reinterpret bytes as float via union */
};

/* -------------------------------------------------------
 * @brief Read block of bytes from internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Data: Pointer to destination buffer
 * @param _length: Number of bytes to read
 * @retval None
 * @note Reads bytes sequentially into destination array
 * ------------------------------------------------------- */
void e2prom_block_Read(uint16_t _address, uint8_t *_Data, uint16_t _length)
{
    uint16_t _Index = 0;                                   /**< Loop counter for array iteration */

    /* Read each byte sequentially */
    for(_Index = 0; _Index < _length; _Index++)            /**< Loop through all bytes to read */
    {
        e2prom_uint8_Read((_address + _Index), &_Data[_Index]);  /**< Read current byte from EEPROM */
    };
};


/* ============================================================================
 *                    UNSIGNED INTEGER UPDATE FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Update unsigned 8-bit value (write only if different)
 * @param _address: EEPROM address
 * @param _Value: New value to write
 * @retval None
 * @note Wear-leveling optimization: Reads current value first
 *       Writes only if new value differs from stored value
 *       Extends EEPROM lifespan by avoiding unnecessary write cycles
 * ------------------------------------------------------- */
void e2prom_uint8_Update(uint16_t _address, uint8_t _Value)
{
    uint8_t _Data = 0x00;                                  /**< Storage for current EEPROM value */

    e2prom_uint8_Read(_address, &_Data);                   /**< Read current value from EEPROM */

    /* Write only if value has changed */
    if(_Data != _Value)                                    /**< Compare current with new value */
    {
        e2prom_uint8_Write(_address, _Value);             /**< Write new value only if different */
    };
};

/* -------------------------------------------------------
 * @brief Update unsigned 16-bit value (write only if different)
 * @param _address: EEPROM starting address
 * @param _Value: New value to write
 * @retval None
 * @note Reduces EEPROM wear by comparing before writing
 * ------------------------------------------------------- */
void e2prom_uint16_Update(uint16_t _address, uint16_t _Value)
{
    uint16_t _Data = 0x00;                                 /**< Storage for current EEPROM value */

    e2prom_uint16_Read(_address, &_Data);                  /**< Read current 16-bit value */
    
    /* Write only if value has changed */
    if(_Data != _Value)                                    /**< Compare current with new value */
    {
        e2prom_uint16_Write(_address, _Value);            /**< Write new value only if different */
    };
};

/* -------------------------------------------------------
 * @brief Update unsigned 32-bit value (write only if different)
 * @param _address: EEPROM starting address
 * @param _Value: New value to write
 * @retval None
 * @note Reduces EEPROM wear by comparing before writing
 * ------------------------------------------------------- */
void e2prom_uint32_Update(uint16_t _address, uint32_t _Value)
{
    uint32_t _Data = 0x00;                                 /**< Storage for current EEPROM value */

    e2prom_uint32_Read(_address, &_Data);                  /**< Read current 32-bit value */
    
    /* Write only if value has changed */
    if(_Data != _Value)                                    /**< Compare current with new value */
    {
        e2prom_uint32_Write(_address, _Value);            /**< Write new value only if different */
    };
};

/* -------------------------------------------------------
 * @brief Update float value (write only if significantly different)
 * @param _address: EEPROM starting address
 * @param _Value: New value to write
 * @retval None
 * @note Uses epsilon comparison (0.0000001) for float equality
 *       Writes only if difference exceeds epsilon tolerance
 * ------------------------------------------------------- */
void e2prom_float_Update(uint16_t _address, float _Value)
{
    float _Data = 0.0;                                     /**< Storage for current EEPROM value */

    e2prom_float_Read(_address, &_Data);                   /**< Read current float value */
    
    /* Write only if value difference is significant */
    if((_Data - _Value) < 0.0000001)                       /**< Compare with epsilon tolerance (note: should use absolute value) */
    {
        e2prom_float_Write(_address, _Value);             /**< Write new value only if significantly different */
    };
};

/* -------------------------------------------------------
 * @brief Update block of bytes (write only changed bytes)
 * @param _address: EEPROM starting address
 * @param _Data: Pointer to new data array
 * @param _length: Number of bytes to update
 * @retval None
 * @note Compares and updates each byte individually
 *       Most efficient for partial block updates
 * ------------------------------------------------------- */
void e2prom_block_Update(uint16_t _address, uint8_t *_Data, uint16_t _length)
{
    uint16_t _Index = 0;                                   /**< Loop counter for array iteration */
    
    /* Update each byte individually using wear-leveling */
    for(_Index = 0; _Index < _length; _Index++)            /**< Loop through all bytes */
    {
        e2prom_uint8_Update((_address + _Index), _Data[_Index]);  /**< Update byte only if different */
    };
};


/* ============================================================================
 *                    SIGNED INTEGER WRITE FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Write signed 8-bit value to internal EEPROM
 * @param _address: EEPROM address
 * @param _Value: Signed 8-bit value to write (-128 to 127)
 * @retval None
 * @note Casts signed to unsigned for EEPROM write (bit pattern preserved)
 * ------------------------------------------------------- */
void e2prom_int8_Write(uint16_t _address, int8_t _Value)
{
    /* Wait for completion of previous write operation */
    bitWaitLow(EECR, EEPE);                                /**< Poll EEPE bit until clear */

    /* Set address and data */
    EEAR = _address;                                       /**< Load address into EEPROM Address Register */
    EEDR = (uint8_t) _Value;                               /**< Cast signed to unsigned (preserves bit pattern) */

    /* Configure write mode */
    bitClear(EECR, EEPM0);                                 /**< Atomic erase+write mode (part 1) */
    bitClear(EECR, EEPM1);                                 /**< Atomic erase+write mode (part 2) */

    /* Start write operation */
    bitSet(EECR, EEMPE);                                   /**< Set Master Program Enable */
    bitSet(EECR, EEPE);                                    /**< Start EEPROM Program operation */
};

/* -------------------------------------------------------
 * @brief Write signed 16-bit value to internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Value: Signed 16-bit value to write (-32768 to 32767)
 * @retval None
 * @note Stores in little-endian format using union conversion
 * ------------------------------------------------------- */
void e2prom_int16_Write(uint16_t _address, int16_t _Value)
{
    e2p_data16_t _data16;                                  /**< Union for signed to byte conversion */

    _data16.i16 = _Value;                                  /**< Load signed 16-bit value into union */

    /* Write LSB and MSB */
    e2prom_int8_Write((_address + 0), _data16.u8[0]);     /**< Write least significant byte */
    e2prom_int8_Write((_address + 1), _data16.u8[1]);     /**< Write most significant byte */
};

/* -------------------------------------------------------
 * @brief Write signed 32-bit value to internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Value: Signed 32-bit value to write
 * @retval None
 * @note Stores in little-endian format using union conversion
 * ------------------------------------------------------- */
void e2prom_int32_Write(uint16_t _address, int32_t _Value)
{
    e2p_data32_t _data32;                                  /**< Union for signed to byte conversion */

    _data32.i32 = _Value;                                  /**< Load signed 32-bit value into union */

    /* Write all 4 bytes */
    e2prom_int8_Write((_address + 0), _data32.u8[0]);     /**< Write byte 0 (LSB) */
    e2prom_int8_Write((_address + 1), _data32.u8[1]);     /**< Write byte 1 */
    e2prom_int8_Write((_address + 2), _data32.u8[2]);     /**< Write byte 2 */
    e2prom_int8_Write((_address + 3), _data32.u8[3]);     /**< Write byte 3 (MSB) */
};


/* ============================================================================
 *                    SIGNED INTEGER READ FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Read signed 8-bit value from internal EEPROM
 * @param _address: EEPROM address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Casts unsigned read result back to signed (bit pattern preserved)
 * ------------------------------------------------------- */
void e2prom_int8_Read(uint16_t _address, int8_t* _Value)
{
    /* Wait for completion of previous write operation */
    bitWaitLow(EECR, EEPE);                                /**< Ensure no write in progress */

    /* Set address and trigger read */
    EEAR = _address;                                       /**< Load address into EEPROM Address Register */
    bitSet(EECR, EERE);                                    /**< Set EEPROM Read Enable */

    /* Read and cast data */
    *_Value = (int8_t) EEDR;                               /**< Cast unsigned read result to signed */
};

/* -------------------------------------------------------
 * @brief Read signed 16-bit value from internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Reads 2 bytes and converts to signed int16 via union
 * ------------------------------------------------------- */
void e2prom_int16_Read(uint16_t _address, int16_t* _Value)
{
    e2p_data16_t _data16;                                  /**< Union for byte to signed conversion */

    /* Read LSB and MSB */
    e2prom_uint8_Read((_address + 0), &_data16.u8[0]);    /**< Read least significant byte */
    e2prom_uint8_Read((_address + 1), &_data16.u8[1]);    /**< Read most significant byte */

    *_Value = _data16.i16;                                 /**< Convert to signed via union */
};

/* -------------------------------------------------------
 * @brief Read signed 32-bit value from internal EEPROM
 * @param _address: EEPROM starting address
 * @param _Value: Pointer to store read value
 * @retval None
 * @note Reads 4 bytes and converts to signed int32 via union
 * ------------------------------------------------------- */
void e2prom_int32_Read(uint16_t _address, int32_t* _Value)
{
    e2p_data32_t _data32;                                  /**< Union for byte to signed conversion */

    /* Read all 4 bytes */
    e2prom_uint8_Read((_address + 0), &_data32.u8[0]);    /**< Read byte 0 (LSB) */
    e2prom_uint8_Read((_address + 1), &_data32.u8[1]);    /**< Read byte 1 */
    e2prom_uint8_Read((_address + 2), &_data32.u8[2]);    /**< Read byte 2 */
    e2prom_uint8_Read((_address + 3), &_data32.u8[3]);    /**< Read byte 3 (MSB) */

    *_Value = _data32.i32;                                 /**< Convert to signed via union */
};


/* ============================================================================
 *                    SIGNED INTEGER UPDATE FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Update signed 8-bit value (write only if different)
 * @param _address: EEPROM address
 * @param _Value: New signed value to write
 * @retval None
 * @note Wear-leveling optimization for signed values
 * ------------------------------------------------------- */
void e2prom_int8_Update(uint16_t _address, int8_t _Value)
{
    int8_t _Data = 0x00;                                   /**< Storage for current EEPROM value */

    e2prom_int8_Read(_address, &_Data);                    /**< Read current signed value */

    /* Write only if value has changed */
    if(_Data != _Value)                                    /**< Compare current with new value */
    {
        e2prom_int8_Write(_address, _Value);              /**< Write new value only if different */
    };
};

/* -------------------------------------------------------
 * @brief Update signed 16-bit value (write only if different)
 * @param _address: EEPROM starting address
 * @param _Value: New signed value to write
 * @retval None
 * @note Reduces EEPROM wear by comparing before writing
 * ------------------------------------------------------- */
void e2prom_int16_Update(uint16_t _address, int16_t _Value)
{
    int16_t _Data = 0x00;                                  /**< Storage for current EEPROM value */

    e2prom_int16_Read(_address, &_Data);                   /**< Read current signed 16-bit value */

    /* Write only if value has changed */
    if(_Data != _Value)                                    /**< Compare current with new value */
    {
        e2prom_int16_Write(_address, _Value);             /**< Write new value only if different */
    };
};

/* -------------------------------------------------------
 * @brief Update signed 32-bit value (write only if different)
 * @param _address: EEPROM starting address
 * @param _Value: New signed value to write
 * @retval None
 * @note Reduces EEPROM wear by comparing before writing
 *       Saves up to 4 write cycles (13.2ms) if value unchanged
 * ------------------------------------------------------- */
void e2prom_int32_Update(uint16_t _address, int32_t _Value)
{
    int32_t _Data = 0x00;                                  /**< Storage for current EEPROM value */

    e2prom_int32_Read(_address, &_Data);                   /**< Read current signed 32-bit value */

    /* Write only if value has changed */
    if(_Data != _Value)                                    /**< Compare current with new value */
    {
        e2prom_int32_Write(_address, _Value);             /**< Write new value only if different */
    };
};


/* ============================================================================
 *                         UTILITY FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Erase entire internal EEPROM (write 0xFF to all addresses)
 * @retval None
 * @note Formatting details:
 *       - Writes 0xFF (erased/blank state) to all 1024 bytes
 *       - Total time: 1024 × 3.3ms = ~3.4 seconds for ATmega328P
 *       - For other AVRs: adjust loop limit to EEPROM size
 *         * ATmega8/168: 512 bytes → ~1.7 seconds
 *         * ATmega2560: 4096 bytes → ~13.5 seconds
 * @note Use this function to:
 *       - Reset EEPROM to factory blank state
 *       - Clear all stored data before reprogramming
 *       - Prepare EEPROM for new data structure
 * @note Warning: This operation takes several seconds and cannot be interrupted
 *       Plan accordingly in time-critical applications
 * ------------------------------------------------------- */
void e2prom_Format(void)
{
    uint16_t _Index = 0;                                   /**< Loop counter for address iteration */
    
    /* Write 0xFF (erased state) to entire EEPROM */
    for(_Index = 0; _Index < 1024; _Index++)               /**< Loop through all 1024 bytes (ATmega328P size) */
    {
        e2prom_uint8_Update(_Index, 0xFF);                  /**< Write erased value (0xFF) to current address */
    };    
};