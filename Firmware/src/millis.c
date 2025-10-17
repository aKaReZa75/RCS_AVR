#include "millis.h"

volatile uint32_t System_millis = 0;

/**
 * @brief Interrupt Service Routine for Timer0 Compare Match A
 * 
 * @note **Important**: For the interrupt to work, `globalInt_Enable` must be called to enable global interrupts. 
 *       Alternatively, you can set the I-bit (Interrupt Enable) in the `SREG` (Status Register) manually by writing `1` to the I-bit.
 */
ISR(TIMER0_COMPA_vect)
{
    System_millis++;
};


void millis_Init(void)
{
    /* Configure Timer0 in CTC mode (Mode 2) */
    bitClear(TCCR0A, WGM00);
    bitSet  (TCCR0A, WGM01);
    bitClear(TCCR0B, WGM02);

    // Prescaler = 64 => 16Mhz/64=250KHz
    /* Set Prescaler = 64 => Timer clock frequency = 16MHz/64 = 250 kHz */
    bitSet  (TCCR0B, CS00);
    bitSet  (TCCR0B, CS01);
    bitClear(TCCR0B, CS02);

    bitSet(TIMSK0, OCIE0A); /**< Enable Compare Match A interrupt */

    /* Clear the interrupt flags for Compare Match A*/
    intFlag_clear(TIFR0, OCF0A);
    OCR0A = 249;  /**< 1ms interval */
};