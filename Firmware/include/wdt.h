/**
 ******************************************************************************
 * @file     wdt.h
 * @brief    Watchdog Timer (WDT) library header for AVR microcontrollers
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This library provides watchdog timer management for AVR
 *           microcontrollers using the hardware WDT peripheral, including
 *           reset-source detection after every restart.
 * 
 * @note     FUNCTION SUMMARY:
 *           - wdt_Init           : Enable WDT in interrupt + system reset mode with selected timeout
 *           - wdt_deInit         : Safely disable the watchdog timer
 *           - get_Reset_Srouce   : Detect the cause of the last MCU reset and clear reset flags
 *           - wdt_Reset          : (macro) Restart the watchdog counter ("wdr" instruction)
 * 
 * @note     Features:
 *           - Interrupt + System Reset mode (WDIE = 1, WDE = 1)
 *           - Ten selectable timeouts from 16ms up to 8s
 *           - Timed-sequence handling (WDCE) performed safely with interrupts disabled
 *           - Global interrupt flag (SREG.I) is saved and restored automatically
 *           - Reset source detection: Power-On, External, Brown-Out, Watchdog
 * 
 * @note     Hardware Requirements:
 *           - AVR microcontroller with WDT peripheral (ATmega328P, etc.)
 *           - Timeouts are nominal values (typical at VCC = 5V, 128kHz internal WDT oscillator)
 *           - ISR(WDT_vect) in wdt.c uses the USART library (usart_Putsln) for debug output
 * 
 * @note     Usage:
 *           1. Call get_Reset_Srouce() as early as possible after startup (clears MCUSR)
 *           2. Call wdt_Init() with one of the WDT_TIMEOUT_xxx values
 *           3. Call wdt_Reset() periodically from the main loop before the timeout expires
 *           4. Call wdt_deInit() if the watchdog must be disabled again
 * 
 * @note     Example - Enable 1 second watchdog and detect reset source:
 *           Reset_Source_t resetSource;
 *           get_Reset_Srouce(&resetSource);
 *           wdt_Init(WDT_TIMEOUT_1S);
 *           while(1)
 *           {
 *               wdt_Reset();      // Feed the watchdog
 *           };
 ******************************************************************************
 */
#ifndef _wdt_H_
#define _wdt_H_

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
 *                         WATCHDOG COMMAND MACRO
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Reset (feed) the watchdog timer counter
 * @note Executes the AVR "wdr" instruction
 *       Must be called periodically before the configured timeout expires,
 *       otherwise the watchdog fires (interrupt first, then system reset)
 * ------------------------------------------------------- */
#define wdt_Reset() asm volatile ("wdr")


/* ============================================================================
 *                         WDT TIMEOUT DEFINITIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Watchdog timeout selection values (WDP3:0 prescaler bits)
 * @note Values are OR-ed directly into WDTCSR by wdt_Init()
 *       Bits 2:0 map to WDP2:WDP0, bit 5 maps to WDP3
 *       Timeouts are nominal (VCC = 5V)
 * ------------------------------------------------------- */
#define WDT_TIMEOUT_16MS    0x00         /**< Timeout ~16ms  (WDP3:0 = 0000) */
#define WDT_TIMEOUT_32MS    0x01         /**< Timeout ~32ms  (WDP3:0 = 0001) */
#define WDT_TIMEOUT_64MS    0x02         /**< Timeout ~64ms  (WDP3:0 = 0010) */
#define WDT_TIMEOUT_125MS   0x03         /**< Timeout ~125ms (WDP3:0 = 0011) */
#define WDT_TIMEOUT_250MS   0x04         /**< Timeout ~250ms (WDP3:0 = 0100) */
#define WDT_TIMEOUT_500MS   0x05         /**< Timeout ~500ms (WDP3:0 = 0101) */
#define WDT_TIMEOUT_1S      0x06         /**< Timeout ~1s    (WDP3:0 = 0110) */
#define WDT_TIMEOUT_2S      0x07         /**< Timeout ~2s    (WDP3:0 = 0111) */
#define WDT_TIMEOUT_4S      0x20         /**< Timeout ~4s    (WDP3:0 = 1000) */
#define WDT_TIMEOUT_8S      0x21         /**< Timeout ~8s    (WDP3:0 = 1001) */


/* ============================================================================
 *                         RESET SOURCE DEFINITIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief MCU reset source identifiers
 * @note Returned by get_Reset_Srouce() based on MCUSR flags
 *       Detection priority: Power-On > External > Brown-Out > Watchdog
 * ------------------------------------------------------- */
typedef enum
{
    RESET_SOURCE_UNKNOWN = 0,            /**< No reset flag is set / source could not be determined */
    RESET_SOURCE_POWER_ON,               /**< Power-On Reset (PORF) */
    RESET_SOURCE_EXTERNAL,               /**< External reset via RESET pin (EXTRF) */
    RESET_SOURCE_BROWN_OUT,              /**< Brown-Out Detector reset (BORF) */
    RESET_SOURCE_WATCHDOG                /**< Watchdog system reset (WDRF) */
} Reset_Source_t;


/* ============================================================================
 *                         FUNCTION PROTOTYPES
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize and enable the watchdog timer
 * @param _timeOut: Timeout selection (WDT_TIMEOUT_16MS ... WDT_TIMEOUT_8S)
 * @retval None
 * @note Enables Interrupt + System Reset mode (WDIE = 1, WDE = 1):
 *       first timeout calls ISR(WDT_vect), if the watchdog is not fed
 *       afterwards the next timeout resets the MCU
 *       Global interrupts are disabled during the timed sequence
 *       and restored to their previous state afterwards
 * ------------------------------------------------------- */
void wdt_Init(uint8_t _timeOut);

/* -------------------------------------------------------
 * @brief Disable the watchdog timer
 * @retval None
 * @note Performs the required timed sequence (WDCE + WDE) and clears
 *       WDE and WDIE
 *       WDRF in MCUSR must be cleared first (see get_Reset_Srouce()),
 *       otherwise WDE cannot be cleared by hardware rules
 *       Global interrupts are restored to their previous state afterwards
 * ------------------------------------------------------- */
void wdt_deInit(void);

/* -------------------------------------------------------
 * @brief Detect the source of the last MCU reset
 * @param _src: Pointer to variable that receives the detected reset source
 * @retval None
 * @note Reads PORF, EXTRF, BORF and WDRF flags in MCUSR (in that priority order)
 *       Clears MCUSR after reading so the next reset is reported correctly
 *       Should be called early after startup, before wdt_Init()
 * ------------------------------------------------------- */
void get_Reset_Srouce(Reset_Source_t* _src);

#endif /* _wdt_H_ */
