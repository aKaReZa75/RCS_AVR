/**
 ******************************************************************************
 * @file     wdt.c
 * @brief    Watchdog Timer (WDT) library implementation for AVR microcontrollers
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This library provides watchdog timer management using the hardware
 *           WDT peripheral on AVR microcontrollers, plus reset-source detection.
 * 
 * @note     FUNCTION SUMMARY:
 *           - ISR(WDT_vect)      : Watchdog interrupt handler (prints debug message over USART)
 *           - get_Reset_Srouce   : Detect the cause of the last MCU reset and clear MCUSR
 *           - wdt_deInit         : Safely disable the watchdog timer
 *           - wdt_Init           : Enable WDT in interrupt + system reset mode
 * 
 * @note     Watchdog Configuration (WDTCSR):
 *           - Mode: Interrupt + System Reset (WDIE = 1, WDE = 1)
 *           - Timeout: selected by WDP3:0 bits (WDT_TIMEOUT_xxx values)
 *           - Timed sequence: WDCE and WDE must be set, then WDTCSR must be
 *             written with the new value within 4 clock cycles
 *           - Interrupts are disabled during the sequence to guarantee timing
 * 
 * @note     Dependencies:
 *           - ISR(WDT_vect) calls usart_Putsln(), so the USART library must be
 *             available (included through aKaReZa.h or the project)
 ******************************************************************************
 */

#include "wdt.h"


/* ============================================================================
 *                       INTERRUPT SERVICE ROUTINE
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Watchdog timeout interrupt handler
 * @retval None
 * @note Executes on the first watchdog timeout when WDIE = 1
 *       Hardware then clears WDIE automatically, so if the watchdog is
 *       not fed (wdt_Reset) the next timeout causes a system reset
 *       Currently used for debugging: prints a message over USART
 * ------------------------------------------------------- */
ISR(WDT_vect)
{
    // usart_Putsln("WDT interrupt fired!");   /**< Debug message: watchdog interrupt occurred */
};


/* ============================================================================
 *                       RESET SOURCE DETECTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Detect the source of the last MCU reset
 * @param _src: Pointer to variable that receives the reset source
 *              (Reset_Source_t value)
 * @retval None
 * @note Checks MCUSR flags in the following priority order:
 *       1. PORF  -> RESET_SOURCE_POWER_ON
 *       2. EXTRF -> RESET_SOURCE_EXTERNAL
 *       3. BORF  -> RESET_SOURCE_BROWN_OUT
 *       4. WDRF  -> RESET_SOURCE_WATCHDOG
 *       5. none  -> RESET_SOURCE_UNKNOWN
 * @note MCUSR is cleared at the end so the next reset is reported correctly
 *       Clearing WDRF is also required before the watchdog can be disabled
 * @note Call as early as possible after startup
 * ------------------------------------------------------- */
void get_Reset_Srouce(Reset_Source_t* _src)
{
    /* Power-On Reset */
    if(bitCheckHigh(MCUSR, PORF))
    {
        *_src = RESET_SOURCE_POWER_ON;           /**< PORF set: MCU powered up */
    }    
    /* External Reset */
    else if(bitCheckHigh(MCUSR, EXTRF))
    {
        *_src = RESET_SOURCE_EXTERNAL;           /**< EXTRF set: RESET pin pulled low */
    }    
    /* Brown-Out Reset */
    else if(bitCheckHigh(MCUSR, BORF))  
    {
        *_src = RESET_SOURCE_BROWN_OUT;          /**< BORF set: supply voltage dropped below threshold */
    }
    /* Watchdog Reset */
    else if(bitCheckHigh(MCUSR, WDRF))
    {
        *_src = RESET_SOURCE_WATCHDOG;           /**< WDRF set: watchdog timeout reset */
    }
    /* No flag set */
    else
    {
        *_src = RESET_SOURCE_UNKNOWN;            /**< No known reset flag found */
    };

    /* Clear all reset flags for the next reset event */
    MCUSR = 0x00;                                /**< Clear MCUSR (also clears WDRF so WDT can be disabled) */
};


/* ============================================================================
 *                       WATCHDOG DE-INITIALIZATION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Disable the watchdog timer
 * @retval None
 * @note Sequence:
 *       1. Save and disable global interrupts (timed sequence must not be interrupted)
 *       2. Reset watchdog counter
 *       3. Set WDCE and WDE to unlock WDTCSR changes
 *       4. Within 4 clock cycles write 0 to WDE, WDIE (watchdog off)
 *       5. Restore previous global interrupt state
 * @note WDRF in MCUSR must be cleared beforehand (see get_Reset_Srouce())
 * ------------------------------------------------------- */
void wdt_deInit(void)
{
    /* Save current global interrupt state */
    bool SREG_I_backUp = bitCheck(SREG, SREG_I);   /**< Backup I-bit of SREG */

    /* Disable global interrupts for the timed sequence */
    bitClear(SREG, SREG_I);                        /**< Clear I-bit: interrupts disabled */
    
    /* Reset watchdog counter */
    wdt_Reset();                                   /**< Execute "wdr" to restart WDT counter */

    /* Step 1: Enable configuration change */
    WDTCSR = (1<<WDCE) | (1<<WDE);                 /**< Set WDCE + WDE: starts the 4-cycle timed window */
    
    /* Step 2: Turn watchdog off (must be done within 4 clock cycles) */
    WDTCSR = (0<<WDCE) | (0<<WDE) | (0<<WDIE);     /**< Clear WDE and WDIE: watchdog disabled */

    /* Restore global interrupt state */
    bitChange(SREG, SREG_I, SREG_I_backUp);        /**< Restore I-bit to its previous value */
};


/* ============================================================================
 *                       WATCHDOG INITIALIZATION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize and enable the watchdog timer
 * @param _timeOut: Timeout selection (WDT_TIMEOUT_16MS ... WDT_TIMEOUT_8S)
 * @retval None
 * @note Enables Interrupt + System Reset mode (WDIE = 1, WDE = 1):
 *       - First timeout  -> ISR(WDT_vect) executes
 *       - Second timeout -> system reset (if the watchdog was not fed)
 * @note Sequence:
 *       1. Save and disable global interrupts (timed sequence must not be interrupted)
 *       2. Reset watchdog counter
 *       3. Set WDCE and WDE to unlock WDTCSR changes
 *       4. Within 4 clock cycles write WDE, WDIE and the timeout bits
 *       5. Restore previous global interrupt state
 * ------------------------------------------------------- */
void wdt_Init(uint8_t _timeOut)
{
    /* Save current global interrupt state */
    bool SREG_I_backUp = bitCheck(SREG, SREG_I);   /**< Backup I-bit of SREG */

    /* Disable global interrupts for the timed sequence */
    bitClear(SREG, SREG_I);                        /**< Clear I-bit: interrupts disabled */
    
    /* Reset watchdog counter */
    wdt_Reset();                                   /**< Execute "wdr" to restart WDT counter */

    /* Step 1: Enable configuration change */
    WDTCSR = (1<<WDCE) | (1<<WDE);                 /**< Set WDCE + WDE: starts the 4-cycle timed window */
    
    /* Step 2: Apply new configuration (must be done within 4 clock cycles) */
    WDTCSR = (0<<WDCE) | (1<<WDE) | (1<<WDIE) | _timeOut;  /**< WDE + WDIE enabled, _timeOut sets WDP3:0 prescaler */

    /* Restore global interrupt state */
    bitChange(SREG, SREG_I, SREG_I_backUp);        /**< Restore I-bit to its previous value */
};
