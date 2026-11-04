/**
 ******************************************************************************
 * @file     aht20.h
 * @brief    AHT20 temperature and humidity sensor driver header for I2C interface
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75/AVR_AHT20
 * 
 * @note     This library provides complete interface for AHT20 digital
 *           temperature and humidity sensor with I2C communication.
 * 
 * @note     FUNCTION SUMMARY:
 *           - aht20_Init    : Initialize AHT20 sensor with calibration check and soft reset
 *           - aht20_getData : Trigger measurement and read temperature/humidity values
 * 
 * @note     Sensor Specifications:
 *           - Temperature range: -40°C to +85°C (±0.3°C accuracy)
 *           - Humidity range: 0% to 100% RH (±2% accuracy)
 *           - Resolution: 20-bit for both temperature and humidity
 *           - I2C address: 0x38 (fixed)
 *           - Supply voltage: 2.0V to 5.5V
 *           - Measurement time: ~80ms
 * 
 * @note     Communication Protocol:
 *           - Interface: I2C (up to 400kHz Fast Mode)
 *           - Initialization: Soft reset + calibration check
 *           - Measurement: Trigger command → Wait 80ms → Read 6 bytes
 *           - Data format: [Status, Humi[19:12], Humi[11:4], Humi[3:0]+Temp[19:16], Temp[15:8], Temp[7:0]]
 * 
 * @note     Timing Requirements:
 *           - Power-on delay: 40ms (sensor stabilization)
 *           - Command delay: 10ms (between commands)
 *           - Measurement time: 80ms (data acquisition)
 * 
 * @note     Usage Example:
 *           AHT20_Data_T sensor;
 *           if (aht20_Init() == AHT20_Res_OK) {
 *               if (aht20_getData(&sensor) == AHT20_Res_OK) {
 *                   printf("Temp: %.1f°C, Humidity: %.1f%%\n", sensor.Temp, sensor.Humidity);
 *               }
 *           }
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_AHT20
 ******************************************************************************
 */
#ifndef _aht20_H_
#define _aht20_H_

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
 *  This library requires the i2c.h base library to compile correctly.
 *  If the file is missing, please download it or contact for support.
 * ============================================================================ */
#ifndef _i2c_H_
    #warning "============================================================"
    #warning " [WARNING] Missing required dependency: i2c.h"
    #warning "------------------------------------------------------------"
    #warning "  This module depends on the i2c.h base library."
    #warning "  Please download it from: https://github.com/aKaReZa75/AVR_I2C"
    #warning "  Or contact for support: akaReza75@gmail.com"
    #warning "------------------------------------------------------------"
    #error   "Compilation aborted: Required file 'i2c.h' not found!"
    #warning "============================================================"
#endif
/* ============================================================================
 *  This library requires the err.h base library to compile correctly.
 *  If the file is missing, please download it or contact for support.
 * ============================================================================ */
#ifndef _err_H_
    #warning "============================================================"
    #warning " [WARNING] Missing required dependency: err.h"
    #warning "------------------------------------------------------------"
    #warning "  This module depends on the err.h base library."
    #warning "  Please download it from: https://github.com/aKaReZa75/Error_Detection"
    #warning "  Or contact for support: akaReza75@gmail.com"
    #warning "------------------------------------------------------------"
    #error   "Compilation aborted: Required file 'err.h' not found!"
    #warning "============================================================"
#endif


/* ============================================================================
 *                         AHT20 I2C ADDRESS
 * ============================================================================ */
#define __AHT20_Add       0x38           /**< AHT20 fixed I2C 7-bit address (no alternative address available) */


/* ============================================================================
 *                         AHT20 STATUS FLAGS
 * ============================================================================ */
#define __AHT20_Flag_CAL  3              /**< Calibration enable bit position in status byte (bit 3) */
#define __AHT20_Flag_BUSY 7              /**< Busy flag bit position in status byte (bit 7) - HIGH during measurement */


/* ============================================================================
 *                         AHT20 TIMING CONSTANTS
 * ============================================================================ */
#define __AHT20_AFTER_POWER_ON_DELAY 40  /**< Power-on stabilization delay in milliseconds (min 20ms per datasheet) */
#define __AHT20_DELAY                10  /**< Inter-command delay in milliseconds (min 5ms per datasheet) */
#define __AHT20_MEASURE_DELAY        80  /**< Measurement duration in milliseconds (typical 75-80ms) */


/* ============================================================================
 *                         AHT20 CONVERSION FACTORS
 * ============================================================================ */
/**
 * @brief Humidity calculation factor
 * @note Formula: Humidity(%) = (Raw_Value / 2^20) × 100
 *       Factor = 100 / 1048576 = 95.367e-6
 *       Raw value: 20-bit unsigned (0 to 1048575)
 */
#define __AHT20_Humi_factor 95.367e-6    /**< Humidity scaling factor: 100 / (2^20) */

/**
 * @brief Temperature calculation factor
 * @note Formula: Temperature(°C) = [(Raw_Value / 2^20) × 200] - 50
 *       Factor = 200 / 1048576 = 190.735e-6
 *       Raw value: 20-bit unsigned (0 to 1048575)
 */
#define __AHT20_Temp_factor 190.735e-6   /**< Temperature scaling factor: 200 / (2^20) */

/**
 * @brief Temperature offset constant
 * @note Applied after scaling: Temperature(°C) = (Raw × Factor) - 50
 *       Converts from 0-200°C range to -50 to +150°C range
 */
#define __AHT20_Temp_const  50.0         /**< Temperature offset constant in Celsius */


/* ============================================================================
 *                         TYPE DEFINITIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief AHT20 function return status codes
 * @note Used for error handling and status checking
 * ------------------------------------------------------- */
typedef enum
{
    AHT20_Res_OK,                        /**< Operation completed successfully */
    AHT20_Res_ERR,                       /**< General error (calibration failed, sensor not responding) */
    AHT20_Res_TimeOut                    /**< Timeout error (sensor busy too long, no response) */
} AHT20_Res_T;

/* -------------------------------------------------------
 * @brief AHT20 measurement data structure
 * @note Contains converted temperature and humidity values
 * ------------------------------------------------------- */
typedef struct 
{
    float Temp;                          /**< Temperature in degrees Celsius (°C), range: -40 to +85 */
    float Humidity;                      /**< Relative humidity in percentage (%), range: 0 to 100 */
} AHT20_Data_T;


/* ============================================================================
 *                         FUNCTION PROTOTYPES
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize AHT20 sensor with calibration check
 * @retval AHT20_Res_T: Status code
 *         - AHT20_Res_OK: Initialization successful, sensor ready
 *         - AHT20_Res_ERR: Initialization failed, sensor not calibrated
 * @note Initialization sequence:
 *       1. Wait 40ms for power-on stabilization
 *       2. Read status register
 *       3. Check calibration bit (bit 3)
 *       4. If not calibrated: send soft reset command
 *       5. Wait for calibration to complete
 * @note Must be called once before using aht20_getData()
 * ------------------------------------------------------- */
AHT20_Res_T aht20_Init(void);

/* -------------------------------------------------------
 * @brief Trigger measurement and read temperature/humidity data
 * @param _Data: Pointer to AHT20_Data_T structure to store results
 * @retval AHT20_Res_T: Status code
 *         - AHT20_Res_OK: Measurement successful, data valid
 *         - AHT20_Res_ERR: Measurement failed, sensor error
 *         - AHT20_Res_TimeOut: Measurement timeout, sensor busy too long
 * @note Measurement sequence:
 *       1. Send trigger measurement command (0xAC 0x33 0x00)
 *       2. Wait 80ms for measurement to complete
 *       3. Read 6 bytes of data (status + 5 data bytes)
 *       4. Extract 20-bit humidity and temperature values
 *       5. Convert raw values to physical units
 * @note Function blocks for ~90ms total (command + measurement + read)
 * ------------------------------------------------------- */
AHT20_Res_T aht20_getData(AHT20_Data_T* _Data);

#endif /* _aht20_H_ */