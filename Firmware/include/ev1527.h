/**
 ******************************************************************************
 * @file     ev1527.h
 * @brief    EV1527 RF remote control decoder library header for AVR
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75/AVR_ev1527
 * 
 * @note     This header file provides definitions for:
 *           - Timer1 control macros for pulse width measurement
 *           - EV1527 timing thresholds and validation macros
 *           - Decoded data structure (20-bit address + 4-bit key)
 *           - Decoder initialization and deinitialization functions
 * 
 * @note     FUNCTION SUMMARY:
 *           - ev1527_Init   : Initialize Timer1 and INT0 for RF signal capture
 *           - ev1527_deInit : Disable Timer1 and INT0 to stop decoding
 * 
 * @note     EV1527 Protocol Specifications:
 *           - Encoding: Pulse width modulation
 *           - Data format: 24 bits (20-bit address + 4-bit key)
 *           - Bit encoding:
 *             * Logic '0': Short HIGH (1xT) + Long LOW (3xT)
 *             * Logic '1': Long HIGH (3xT) + Short LOW (1xT)
 *           - Preamble: Short HIGH (1xT) + Very long LOW (~31xT)
 *           - Base period (T): ~300-350us typical
 * 
 * @note     Timing Specifications (16MHz, Timer1 prescaler /8 = 0.5us per tick):
 *           - Valid HIGH+LOW pulse range: 450-8500 ticks (225us - 4.25ms)
 *           - Preamble: LOW is 25-40x longer than HIGH
 *           - Bit decision: HIGH >= 1.5x LOW -> '1', else -> '0'
 * 
 * @note     Hardware Requirements:
 *           - 433MHz/315MHz OOK receiver module on INT0 pin (PD2)
 *           - Receiver output: demodulated data (HIGH = carrier present)
 *           - Global interrupts must be enabled with sei()
 * 
 * @note     Usage Example:
 *           ev1527_Init();                             // Initialize decoder
 *           sei();                                     // Enable global interrupts
 *           
 *           if (ev1527_Data.Bits.Detect)               // Valid code received?
 *           {
 *               uint32_t address = ev1527_Data.Bits.Address;  // 20-bit address
 *               uint8_t  key     = ev1527_Data.Bits.Keys;     // 4-bit key (0-15)
 *               ev1527_Data.Bits.Detect = 0;           // Clear flag
 *               ev1527_Init();                         // Re-enable decoder
 *           }
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_ev1527
 ******************************************************************************
 */
#ifndef _ev1527_H_
#define _ev1527_H_

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
 *                         TIMER CONTROL MACROS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Reset Timer1 counter to zero
 * @note Used at the start of a pulse measurement to begin timing
 * ------------------------------------------------------- */
#define EV_Timer_Reset TCNT1 = 0x0000    /**< Clear Timer1 counter register */

/* -------------------------------------------------------
 * @brief Read current Timer1 counter value
 * @note Returns elapsed time since last reset in timer ticks
 * ------------------------------------------------------- */
#define EV_Timer_Value TCNT1             /**< Current Timer1 count (elapsed ticks) */


/* ============================================================================
 *                         PROTOCOL PARAMETERS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief EV1527 frame size definition
 * @note 24 bits total: 20-bit address + 4-bit key (index 0-23)
 * ------------------------------------------------------- */
#define EV_maxIndexData  23              /**< Maximum bit index of the data frame */


/* ============================================================================
 *                         TIMING THRESHOLDS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Valid HIGH+LOW pulse duration limits in timer ticks
 * @note At 16MHz with prescaler /8, one tick = 0.5us
 *       450 ticks = 225us, 8500 ticks = 4.25ms
 * ------------------------------------------------------- */
#define HPL_min          450             /**< Minimum HIGH+LOW duration (noise filter) */
#define HPL_Max          8500            /**< Maximum HIGH+LOW duration (timeout threshold) */


/* ============================================================================
 *                         VALIDATION MACROS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Validate that a pulse duration is within the acceptable range
 * @param _tickLow  LOW pulse duration in timer ticks
 * @param _tickHigh HIGH pulse duration in timer ticks
 * @retval true if (LOW + HIGH) is between HPL_min and HPL_Max, false otherwise
 * @note Filters out noise and invalid pulses
 * ------------------------------------------------------- */
#define EV_pulseIsValid(_tickLow, _tickHigh)  (((_tickLow + _tickHigh) > HPL_min) && ((_tickLow + _tickHigh) < HPL_Max))

/* -------------------------------------------------------
 * @brief Check if a pulse pair matches the EV1527 preamble
 * @param _tickLow  LOW pulse duration (should be very long)
 * @param _tickHigh HIGH pulse duration (should be short)
 * @retval true if LOW is 25-40x longer than HIGH, false otherwise
 * @note Example: LOW=10000us, HIGH=320us -> ratio=31.25 -> valid preamble
 *       A valid preamble marks the start of an EV1527 data frame
 * ------------------------------------------------------- */
#define EV_PrembleCheck(_tickLow, _tickHigh)  ((_tickLow >= 25*_tickHigh) && (_tickLow <= 40*_tickHigh))

/* -------------------------------------------------------
 * @brief Decode a bit value from a HIGH/LOW pulse pair
 * @param _tickLow  LOW pulse duration in timer ticks
 * @param _tickHigh HIGH pulse duration in timer ticks
 * @retval 1 if HIGH >= 1.5x LOW (logic '1'), otherwise 0 (logic '0')
 * @note Logic '0': HIGH=300us, LOW=900us -> returns 0
 *       Logic '1': HIGH=900us, LOW=300us -> returns 1
 * ------------------------------------------------------- */
#define EV_bitCheck(_tickLow, _tickHigh)      ((_tickHigh >= 1.5*_tickLow) ? 1 : 0)


/* ============================================================================
 *                         DATA STRUCTURE
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief EV1527 decoded data structure with bit-field access
 * @note Union allows access to the full 32-bit value or individual fields
 *       Total: 32 bits (20 address + 4 key + 1 detect flag + 7 reserved)
 * ------------------------------------------------------- */
typedef union 
{
    uint32_t rawValue;                   /**< Direct 32-bit access to the entire decoded value */
    
    struct
    {
        uint32_t Address : 20;           /**< 20-bit transmitter address (0 to 1,048,575) */
        uint32_t Keys    : 4;            /**< 4-bit key code (0 to 15) - identifies the pressed button */
        uint32_t Detect  : 1;            /**< Detection flag: 1 = valid code received, 0 = no detection */
        uint32_t Reserve : 7;            /**< Reserved bits */
    } Bits;                              /**< Bit-field access */
} ev1527_T;


/* ============================================================================
 *                         FUNCTION PROTOTYPES
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize EV1527 decoder hardware (Timer1 and INT0)
 * @retval None
 * @note Configures PD2 (INT0) as input, INT0 on rising edge, and
 *       Timer1 in normal mode with prescaler /8
 *       Must be called before decoding RF signals
 *       Global interrupts (sei()) must be enabled separately
 * ------------------------------------------------------- */
void ev1527_Init(void);

/* -------------------------------------------------------
 * @brief Disable EV1527 decoder and release hardware resources
 * @retval None
 * @note Disables INT0 and stops Timer1 (no clock source)
 *       Called automatically after a complete frame is received
 *       Call ev1527_Init() again to resume decoding
 * ------------------------------------------------------- */
void ev1527_deInit(void);

#endif /* _ev1527_H_ */
