/**
 ******************************************************************************
 * @file     aht20.c
 * @brief    AHT20 temperature and humidity sensor driver implementation for I2C
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75/AVR_AHT20
 * 
 * @note     This library provides complete driver for AHT20 digital sensor
 *           with I2C communication, CRC validation, and data conversion.
 * 
 * @note     EXECUTION FLOW:
 *           1. Initialization Flow:
 *              └─> aht20_Init() → Power-on delay (40ms) → Send soft reset (0xBA)
 *                  → Wait for reset → Read status register (0x71)
 *                  → Check calibration bit → If not calibrated: send init command (0xBE)
 *                  → Re-read status → Verify calibration success → Return status
 * 
 *           2. Data Acquisition Flow:
 *              └─> aht20_getData() → Send trigger command (0xAC 0x33 0x00)
 *                  → Wait 80ms for measurement → Read 7 bytes (status + 5 data + CRC)
 *                  → Check BUSY and CAL flags → Validate CRC-8
 *                  → Extract 20-bit temperature (bits from bytes 3,4,5)
 *                  → Extract 20-bit humidity (bits from bytes 1,2,3)
 *                  → Convert to physical units → Return data
 * 
 *           3. Data Extraction (from 7-byte response):
 *              Byte 0: [Status] - BUSY (bit7), CAL (bit3)
 *              Byte 1: [Humi_19:12] - Humidity MSB
 *              Byte 2: [Humi_11:4]  - Humidity middle byte
 *              Byte 3: [Humi_3:0 | Temp_19:16] - Shared nibbles
 *              Byte 4: [Temp_15:8]  - Temperature middle byte
 *              Byte 5: [Temp_7:0]   - Temperature LSB
 *              Byte 6: [CRC-8]      - CRC checksum
 * 
 *           4. Conversion Formulas:
 *              Temperature(°C) = (Raw_Temp × 200 / 2^20) - 50
 *              Humidity(%)     = Raw_Humidity × 100 / 2^20
 * 
 * @note     FUNCTION SUMMARY:
 *           - aht20_Init    : Initialize sensor with soft reset and calibration verification
 *           - aht20_getData : Trigger measurement, read data, validate CRC, convert to physical units
 * 
 * @note     CRC-8 Configuration (AHT20 specific):
 *           - Polynomial: 0x31 (x^8 + x^5 + x^4 + 1)
 *           - Initial value: 0xFF
 *           - Input reflection: No
 *           - Output reflection: No
 *           - Final XOR: 0x00
 *           - Validates all 7 bytes (status + data + CRC itself = 0x00)
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_AHT20
 ******************************************************************************
 */

#include "aht20.h"


/* ============================================================================
 *                       INITIALIZATION FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize AHT20 sensor with calibration check
 * @retval AHT20_Res_T: Initialization status
 *         - AHT20_Res_OK: Sensor initialized and calibrated successfully
 *         - AHT20_Res_ERR: Initialization failed, sensor not calibrated
 * @note Initialization sequence (per AHT20 datasheet):
 *       1. Wait 40ms after power-on for sensor stabilization
 *       2. Send soft reset command (0xBA) to reset sensor
 *       3. Wait 40ms for reset to complete
 *       4. Read status register (command 0x71)
 *       5. Check calibration bit (bit 3) - should be HIGH if calibrated
 *       6. If not calibrated: send initialization command (0xBE 0x08 0x00)
 *       7. Wait 10ms for calibration
 *       8. Re-read status to verify calibration success
 * ------------------------------------------------------- */
AHT20_Res_T aht20_Init(void)
{
    /* AHT20 command definitions */
    uint8_t _AHT20_CMD_Init[3] = {0xBE, 0x08, 0x00};       /**< Initialization/calibration command sequence */
    uint8_t _AHT20_CMD_Reset[1] = {0xBA};                  /**< Soft reset command */
    uint8_t _AHT20_CMD_Status[1] = {0x71};                 /**< Status register read command */
    uint8_t _Status = 0x00;                                /**< Status register value storage */
    
    /* Wait for sensor power-on stabilization */
    delay_ms(__AHT20_AFTER_POWER_ON_DELAY);                /**< 40ms delay for sensor internal initialization */
    
    /* Perform soft reset to ensure clean state */
    i2c_writeAddress(__AHT20_Add, _AHT20_CMD_Reset, 1);    /**< Send reset command to sensor */
    delay_ms(__AHT20_AFTER_POWER_ON_DELAY);                /**< Wait 40ms for reset to complete */
    
    /* Read initial status register */
    i2c_readSequential(__AHT20_Add, _AHT20_CMD_Status, 1, &_Status, 1);  /**< Send status command and read 1 byte */
    
    /* Check if sensor needs calibration */
    if(bitCheckLow(_Status, __AHT20_Flag_CAL))             /**< If calibration bit (bit 3) is LOW */
    {
        /* Send calibration command sequence */
        i2c_writeAddress(__AHT20_Add, _AHT20_CMD_Init, sizeof(_AHT20_CMD_Init));  /**< Write 3-byte init command */
    };
    
    /* Wait for calibration to complete */
    delay_ms(__AHT20_DELAY);                               /**< 10ms delay for calibration process */
 
    /* Verify calibration success */
    i2c_readSequential(__AHT20_Add, _AHT20_CMD_Status, 1, &_Status, 1);  /**< Re-read status register */
    
    /* Check if calibration was successful */
    if(bitCheckLow(_Status, __AHT20_Flag_CAL))             /**< If calibration bit still LOW */
    {
        return AHT20_Res_ERR;                              /**< Calibration failed - sensor not ready */
    };   
    
    return AHT20_Res_OK;                                   /**< Initialization successful - sensor ready */
};


/* ============================================================================
 *                       DATA ACQUISITION FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Trigger measurement and read temperature/humidity data
 * @param _Data: Pointer to AHT20_Data_T structure to store converted values
 * @retval AHT20_Res_T: Measurement status
 *         - AHT20_Res_OK: Data acquired and validated successfully
 *         - AHT20_Res_ERR: Measurement failed (sensor busy, not calibrated, or CRC error)
 * @note Measurement sequence:
 *       1. Send trigger command (0xAC 0x33 0x00)
 *       2. Wait 80ms for sensor to complete measurement
 *       3. Read 7 bytes: [Status | Humi_H | Humi_M | Humi_L/Temp_H | Temp_M | Temp_L | CRC]
 *       4. Validate status flags (BUSY=0, CAL=1)
 *       5. Validate CRC-8 checksum
 *       6. Extract and convert 20-bit raw values to physical units
 * @note Data format in response bytes:
 *       Humidity: Bits [Byte1:Byte2:Byte3[7:4]] = 20-bit value
 *       Temperature: Bits [Byte3[3:0]:Byte4:Byte5] = 20-bit value
 * ------------------------------------------------------- */
AHT20_Res_T aht20_getData(AHT20_Data_T* _Data)
{
    uint32_t _Temp_I = 0x0;                                /**< Temporary storage for raw temperature value */
    uint32_t _Humi_I = 0x0;                                /**< Temporary storage for raw humidity value */
    
    /* AHT20 measurement trigger command */
    uint8_t _AHT20_CMD_Trigger[3] = {0xAC, 0x33, 0x00};    /**< Trigger measurement command sequence */
    
    /* Buffer for sensor response (7 bytes total) */
    uint8_t _rxBuffer[7] = {0};                            /**< [Status, Humi[19:12], Humi[11:4], Humi[3:0]+Temp[19:16], Temp[15:8], Temp[7:0], CRC] */
    
    /* CRC-8 configuration for AHT20 (per datasheet) */
    hcrc8_T crc8_aht20 = 
    {
        .Poly = 0x31,                                      /**< Polynomial: x^8 + x^5 + x^4 + 1 (0x31 = 0b00110001) */
        .Init = 0xFF,                                      /**< Initial CRC value: 0xFF */
        .refIn = false,                                    /**< No input bit reflection */
        .refOut = false,                                   /**< No output bit reflection */
        .xorOut = 0x00                                     /**< No final XOR operation */
    };
    
    /* Trigger measurement */
    i2c_writeAddress(__AHT20_Add, _AHT20_CMD_Trigger, 3);  /**< Send 3-byte trigger command */
    delay_ms(__AHT20_MEASURE_DELAY);                       /**< Wait 80ms for measurement to complete */
    
    /* Read measurement result */
    i2c_readAdress(__AHT20_Add, _rxBuffer, 7);             /**< Read 7 bytes (status + 5 data + CRC) */
    
    /* Validate status flags */
    if((bitCheckHigh(_rxBuffer[0], __AHT20_Flag_BUSY)) || (bitCheckLow(_rxBuffer[0], __AHT20_Flag_CAL)))  /**< Check if busy (bit7=1) or not calibrated (bit3=0) */
    {
        return AHT20_Res_ERR;                              /**< Sensor not ready or measurement failed */
    };
    
    /* Validate CRC-8 checksum */
    if(CRC8_Calc(&crc8_aht20, _rxBuffer, sizeof(_rxBuffer)) != 0x00)  /**< CRC calculation on all 7 bytes should equal 0x00 */
    {
        return AHT20_Res_ERR;                              /**< CRC validation failed - data corrupted */
    };
    
    /* ===== Extract and convert TEMPERATURE data ===== */
    /* Temperature bits: Byte3[3:0] (MSB) + Byte4[7:0] + Byte5[7:0] (LSB) = 20 bits */
    _Temp_I = ((uint32_t) _rxBuffer[5] + ((uint32_t)_rxBuffer[4] << 8) + ((uint32_t)_rxBuffer[3] << 16));  /**< Combine bytes into 32-bit value */
    _Temp_I &= 0x000FFFFF;                                 /**< Mask to keep only lower 20 bits (ignore humidity bits) */
    
    /* Convert to Celsius: (Raw × 200 / 2^20) - 50 */
    _Data->Temp = ((_Temp_I * __AHT20_Temp_factor) - __AHT20_Temp_const);  /**< Apply scaling factor and offset */
    
    /* ===== Extract and convert HUMIDITY data ===== */
    /* Humidity bits: Byte1[7:0] (MSB) + Byte2[7:0] + Byte3[7:4] (LSB) = 20 bits */
    _Humi_I = ((uint32_t) _rxBuffer[1] << 16) + ((uint32_t) _rxBuffer[2] << 8)  + ((uint32_t) _rxBuffer[3]);  /**< Combine bytes into 32-bit value */
    _Humi_I = _Humi_I >> 4;                                /**< Shift right by 4 bits to extract upper 20 bits (remove temperature bits) */
    
    /* Convert to percentage: Raw × 100 / 2^20 */
    _Data->Humidity = _Humi_I * __AHT20_Humi_factor;       /**< Apply scaling factor */
    
    return AHT20_Res_OK;                                   /**< Measurement successful - data valid */
};