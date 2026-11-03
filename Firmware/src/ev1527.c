/**
 ******************************************************************************
 * @file     ev1527.c
 * @brief    EV1527 RF remote control decoder library implementation for AVR
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75/AVR_ev1527
 * 
 * @note     This file provides:
 *           - INT0 interrupt state machine for EV1527 pulse capture
 *           - Timer1 based pulse width measurement
 *           - 24-bit frame decoding (20-bit address + 4-bit key)
 *           - Decoder initialization and deinitialization functions
 * 
 * @note     EXECUTION FLOW:
 *           1. Initialization:
 *              ev1527_Init() -> Configure INT0 (rising edge) -> Enable INT0
 *              -> Configure Timer1 (normal mode, prescaler /8)
 * 
 *           2. INT0 state machine (ISR):
 *              - First edge (rising):
 *                  Reset variables -> Reset Timer1 -> Switch to falling edge
 *              - Falling edge:
 *                  Capture HIGH pulse width -> Reset Timer1 -> Switch to rising edge
 *              - Rising edge:
 *                  Capture LOW pulse width -> Set measureDone flag
 *              - Measurement complete:
 *                  No preamble yet -> Check preamble (LOW = 25-40x HIGH)
 *                  Preamble found  -> Validate pulse -> Decode bit -> Store in rawValue
 *                  24 bits received -> Set Detect flag -> Disable decoder
 * 
 *           3. Data extraction:
 *              ev1527_Data.Bits.Detect = 1 -> Read Address and Keys
 *              -> Clear Detect flag -> Call ev1527_Init() to re-enable decoder
 * 
 *           4. Deinitialization:
 *              ev1527_deInit() -> Disable INT0 -> Stop Timer1
 * 
 * @note     FUNCTION SUMMARY:
 *           - ISR(INT0_vect) : Edge detection and pulse measurement state machine
 *           - ev1527_Init    : Initialize Timer1 and INT0 for RF signal capture
 *           - ev1527_deInit  : Disable Timer1 and INT0 to stop decoding
 * 
 * @note     Timing Analysis (16MHz AVR, Timer1 prescaler /8):
 *           - Timer resolution: 0.5us per tick (16MHz / 8 = 2MHz)
 *           - Base period (T): ~300-350us (600-700 ticks)
 *           - Logic '0': HIGH=300us, LOW=900us (600 + 1800 ticks)
 *           - Logic '1': HIGH=900us, LOW=300us (1800 + 600 ticks)
 *           - Preamble : HIGH=~320us, LOW=~10ms (640 + 20000 ticks)
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_ev1527
 ******************************************************************************
 */

#include "ev1527.h"


/* ============================================================================
 *                         GLOBAL VARIABLES
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Decoded RF data (address, key and detection flag)
 * @note Declared volatile because it is written inside the ISR
 * ------------------------------------------------------- */
volatile ev1527_T ev1527_Data = {.rawValue = 0x0};


/* ============================================================================
 *                         INTERRUPT SERVICE ROUTINE
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief External interrupt service routine for INT0 (RF data pin)
 * @retval None
 * @note State machine steps:
 *       1. Detect first edge and initialize measurement
 *       2. Alternate between measuring HIGH and LOW pulses
 *       3. Validate preamble pattern
 *       4. Decode 24 data bits by comparing HIGH and LOW widths
 *       5. Disable decoder after successful reception
 * @note State variables:
 *       - firstTime_Trigger : true = waiting for first edge
 *       - measureDone       : true = HIGH and LOW captured, ready to decode
 *       - preambleDetec     : true = valid preamble found, decoding data bits
 *       - _Index            : current bit position (0-23)
 * ------------------------------------------------------- */
ISR(INT0_vect) 
{
  /* State machine static variables (persist between ISR calls) */
  static bool firstTime_Trigger = true;                    /**< true = first edge not yet detected */
  static bool measureDone = false;                         /**< true = complete HIGH+LOW pulse measured */
  static bool preambleDetec = false;                       /**< true = valid preamble detected */
  static uint8_t _Index = 0;                               /**< Current bit index (0-23) in the 24-bit frame */
  static uint16_t Signal_High_Tick = 0;                    /**< HIGH pulse duration in timer ticks */
  static uint16_t Signal_Low_Tick = 0;                     /**< LOW pulse duration in timer ticks */
  
  /* ===== FIRST EDGE DETECTION (INITIALIZATION) ===== */
  if(firstTime_Trigger)                                    /**< First edge detected - initialize decoder */
  {
    /* Reset all measurement variables */
    Signal_High_Tick = 0x00;                               /**< Clear HIGH pulse measurement */
    Signal_Low_Tick  = 0x00;                               /**< Clear LOW pulse measurement */
    _Index = 0;                                            /**< Reset bit index */
    ev1527_Data.rawValue = 0x0;                            /**< Clear decoded data buffer */
    preambleDetec = false;                                 /**< Clear preamble detection flag */
    
    /* Start timer and configure for next edge */
    EV_Timer_Reset;                                        /**< Reset Timer1 to start timing */
    firstTime_Trigger = false;                             /**< Mark initialization complete */
    bitClear(EICRA, ISC00);                                /**< Set INT0 to falling edge (ISC01=1, ISC00=0) */
  }
  /* ===== SUBSEQUENT EDGES (MEASUREMENT) ===== */
  else
  {
    /* Check which edge triggered the interrupt */
    if(bitCheck(EICRA, ISC00))                             /**< ISC00=1: rising edge just detected */
    {
      /* Rising edge detected - LOW pulse measurement complete */
      Signal_Low_Tick = EV_Timer_Value;                    /**< Capture LOW pulse duration from timer */
      measureDone = true;                                  /**< Complete HIGH+LOW pulse captured */
      bitClear(EICRA, ISC00);                              /**< Switch to falling edge detection */
    }
    else                                                   /**< ISC00=0: falling edge just detected */
    {
      /* Falling edge detected - HIGH pulse measurement complete */
      Signal_High_Tick = EV_Timer_Value;                   /**< Capture HIGH pulse duration from timer */
      EV_Timer_Reset;                                      /**< Reset timer to start measuring LOW pulse */
      bitSet(EICRA, ISC00);                                /**< Switch to rising edge detection */
    }
  };
  
  /* ===== PULSE PROCESSING (DECODE BITS) ===== */
  if(measureDone)                                          /**< Complete HIGH+LOW pulse captured */
  {
    /* Check if preamble already detected */
    if(preambleDetec)                                      /**< Preamble found - decode data bits */
    {
      /* Validate pulse timing is within acceptable range */
      if(EV_pulseIsValid(Signal_Low_Tick, Signal_High_Tick))  /**< Valid pulse duration (450-8500 ticks) */
      {
        /* Decode bit and store in result */
        bitChange(ev1527_Data.rawValue, _Index, EV_bitCheck(Signal_Low_Tick, Signal_High_Tick));  /**< HIGH >= 1.5xLOW -> '1', else '0' */
        _Index++;                                          /**< Move to next bit position */
        
        /* Check if all 24 bits received */
        if(_Index > EV_maxIndexData)                       /**< Index exceeded 23 - all 24 bits received */
        {
          ev1527_Data.Bits.Detect = true;                  /**< Set detection flag - valid code received */
          firstTime_Trigger = true;                        /**< Reset state machine for next frame */
          preambleDetec = false;                           /**< Clear preamble flag */
          ev1527_deInit();                                 /**< Disable decoder until it is re-enabled manually */
        };
      }
      else                                                 /**< Invalid pulse timing */
      {
        /* Reset decoder on invalid pulse */
        firstTime_Trigger = true;                          /**< Reset state machine */
        preambleDetec = false;                             /**< Clear preamble flag */
        bitSet(EICRA, ISC00);                              /**< Set to rising edge detection */
      };
    }
    /* Preamble not yet detected - check for preamble pattern */
    else
    {
      /* Check if pulse matches preamble timing (LOW 25-40x HIGH) */
      if(EV_PrembleCheck(Signal_Low_Tick, Signal_High_Tick))  /**< Validate preamble pattern */
      {   
        preambleDetec = true;                              /**< Preamble found - ready to decode data */
      }
      else
      {
        /* Not a preamble - continue waiting */
      };
    };
    
    /* Prepare for next pulse measurement */
    EV_Timer_Reset;                                        /**< Reset timer for next pulse */
    measureDone = false;                                   /**< Clear measurement complete flag */
  };
};


/* ============================================================================
 *                       INITIALIZATION FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize EV1527 decoder hardware (Timer1 and INT0)
 * @retval None
 * @note Configuration:
 *       - PD2 (INT0): input, rising edge trigger, interrupt enabled
 *       - Timer1: normal mode, prescaler /8 (0.5us resolution at 16MHz)
 * @note Must be called before decoding RF signals
 *       Global interrupts (sei()) must be enabled separately
 * ------------------------------------------------------- */
void ev1527_Init(void)
{
  /* ===== Configure INT0 External Interrupt ===== */
  GPIO_Config_INPUT(DDRD, 2);
  /* Set INT0 to trigger on rising edge (ISC01:ISC00 = 11) */
  bitSet(EICRA, ISC00);                                    /**< ISC00=1: Rising edge select (part 1) */
  bitSet(EICRA, ISC01);                                    /**< ISC01=1: Rising edge select (part 2) */
  
  /* Enable INT0 interrupt */
  bitSet(EIMSK, INT0);                                     /**< Enable INT0 in External Interrupt Mask Register */
  
  /* ===== Configure Timer1 ===== */
  /* Set Timer1 to Normal mode (WGM13:WGM10 = 0000) */
  bitClear(TCCR1A, WGM10);                                 /**< WGM10=0: Normal mode (part 1) */
  bitClear(TCCR1A, WGM11);                                 /**< WGM11=0: Normal mode (part 2) */
  bitClear(TCCR1B, WGM12);                                 /**< WGM12=0: Normal mode (part 3) */
  
  /* Set Timer1 prescaler to /8 (CS12:CS10 = 010) */
  /* At 16MHz: Timer frequency = 16MHz/8 = 2MHz -> 0.5us per tick */
  bitClear(TCCR1B, CS10);                                  /**< CS10=0: Prescaler /8 (part 1) */
  bitSet(TCCR1B, CS11);                                    /**< CS11=1: Prescaler /8 (part 2) */
  bitClear(TCCR1B, CS12);                                  /**< CS12=0: Prescaler /8 (part 3) */
};


/* ============================================================================
 *                       DEINITIALIZATION FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Disable EV1527 decoder and release hardware resources
 * @retval None
 * @note Deinitialization sequence:
 *       1. Disable INT0 external interrupt
 *       2. Set Timer1 to normal mode (clear WGM bits)
 *       3. Stop Timer1 (prescaler = 0, no clock source)
 * @note Use this to save power when RF reception is not needed,
 *       or after a successful reception to prevent re-triggering
 * ------------------------------------------------------- */
void ev1527_deInit(void)
{
  /* ===== Disable INT0 External Interrupt ===== */
  /* Clear INT0 edge detection configuration */
  bitClear(EICRA, ISC00);                                  /**< ISC00=0: Clear edge select (part 1) */
  bitClear(EICRA, ISC01);                                  /**< ISC01=0: Clear edge select (part 2) */
  
  /* Disable INT0 interrupt */
  bitClear(EIMSK, INT0);                                   /**< Disable INT0 in External Interrupt Mask Register */
  
  /* ===== Disable Timer1 ===== */
  /* Clear Timer1 mode configuration (Normal mode - all WGM bits = 0) */
  bitClear(TCCR1A, WGM10);                                 /**< WGM10=0: Clear mode bit */
  bitClear(TCCR1A, WGM11);                                 /**< WGM11=0: Clear mode bit */
  bitClear(TCCR1B, WGM12);                                 /**< WGM12=0: Clear mode bit */
  
  /* Stop Timer1 by clearing prescaler (CS12:CS10 = 000 = no clock source) */
  bitClear(TCCR1B, CS10);                                  /**< CS10=0: Stop timer (part 1) */
  bitClear(TCCR1B, CS11);                                  /**< CS11=0: Stop timer (part 2) */
  bitClear(TCCR1B, CS12);                                  /**< CS12=0: Stop timer (part 3) */
};
