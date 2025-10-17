/**
 ******************************************************************************
 * @file     aKaReZa.h
 * @brief    Common utility macros and functions for AVR microcontrollers
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This header file provides:
 *           - Bit manipulation macros for efficient register operations
 *           - Data type conversion utilities (8-bit to 16-bit and vice versa)
 *           - Mathematical constants and helper functions
 *           - GPIO configuration macros
 *           - Interrupt control macros
 * 
 * @note     For complete documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR/blob/main/Macros.md
 * 
 * @warning  This library is designed for AVR microcontrollers only
 ******************************************************************************
 */
#ifndef _aKaReZa_H_
#define _aKaReZa_H_


/* ============================================================================
 *                         STANDARD INCLUDES
 * ============================================================================ */
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <avr/interrupt.h>

/* ============================================================================
 *                         PROJECT INCLUDES
 * ============================================================================ */
#include "alcd.h"
#include "display.h"
#include "outputs.h"
#include "M66.h"
#include "usart.h"
#include "millis.h"
#include "task.h"

/* ============================================================================
 *                        BIT MANIPULATION MACROS
 * ============================================================================
 * Note: These macros support up to 64-bit registers depending on register type
 * ---------------------------------------------------------------------------- */

/* -------------------------------------------------------
 * @brief Set a specific bit in a register
 * @param _Reg: Register or variable (8/16/32/64-bit)
 * @param _Bit: Bit position (0-63 for 64-bit)
 * ------------------------------------------------------- */
#define bitSet(_Reg, _Bit)    ((_Reg) |= (1ULL << (_Bit)))

/* -------------------------------------------------------
 * @brief Clear a specific bit in a register
 * @param _Reg: Register or variable (8/16/32/64-bit)
 * @param _Bit: Bit position (0-63 for 64-bit)
 * ------------------------------------------------------- */
#define bitClear(_Reg, _Bit)  ((_Reg) &= ~(1ULL << (_Bit)))

/* -------------------------------------------------------
 * @brief Toggle a specific bit in a register
 * @param _Reg: Register or variable (8/16/32/64-bit)
 * @param _Bit: Bit position (0-63 for 64-bit)
 * ------------------------------------------------------- */
#define bitToggle(_Reg, _Bit) ((_Reg) ^= (1ULL << (_Bit)))

/* -------------------------------------------------------
 * @brief Change a bit to specific value (0 or 1)
 * @param _Reg: Register or variable (8/16/32/64-bit)
 * @param _Bit: Bit position (0-63 for 64-bit)
 * @param Value: New bit value (0 or 1)
 * ------------------------------------------------------- */
#define bitChange(_Reg, _Bit, Value) ((Value) == 1 ? bitSet(_Reg, _Bit) : bitClear(_Reg, _Bit))

/* -------------------------------------------------------
 * @brief Check the state of a specific bit
 * @param _Reg: Register or variable (8/16/32/64-bit)
 * @param _Bit: Bit position (0-63 for 64-bit)
 * @return 1 if bit is set, 0 if cleared
 * ------------------------------------------------------- */
#define bitCheck(_Reg, _Bit)  (((_Reg) >> (_Bit)) & 0x01U)

/* -------------------------------------------------------
 * @brief Check if a bit is high (set)
 * @param _Reg: Register or variable
 * @param _Bit: Bit position
 * @return 1 if bit is high, 0 otherwise
 * ------------------------------------------------------- */
#define bitCheckHigh(_Reg, _Bit)  (bitCheck(_Reg, _Bit))

/* -------------------------------------------------------
 * @brief Check if a bit is low (cleared)
 * @param _Reg: Register or variable
 * @param _Bit: Bit position
 * @return 1 if bit is low, 0 otherwise
 * ------------------------------------------------------- */
#define bitCheckLow(_Reg, _Bit)   (!bitCheck(_Reg, _Bit))

/* -------------------------------------------------------
 * @brief Wait until a bit becomes high
 * @param _Reg: Register or variable
 * @param _Bit: Bit position
 * @warning This is a blocking operation
 * ------------------------------------------------------- */
#define bitWaitHigh(_Reg, _Bit)   while(bitCheckLow(_Reg, _Bit))

/* -------------------------------------------------------
 * @brief Wait until a bit becomes low
 * @param _Reg: Register or variable
 * @param _Bit: Bit position
 * @warning This is a blocking operation
 * ------------------------------------------------------- */
#define bitWaitLow(_Reg, _Bit)    while(bitCheckHigh(_Reg, _Bit))

/* -------------------------------------------------------
 * @brief Shift register left by N positions
 * @param _Reg: Register or variable
 * @param _Pos: Number of positions to shift
 * ------------------------------------------------------- */
#define bitShiftLeft(_Reg, _Pos)  ((_Reg) = (_Reg) << (_Pos))

/* -------------------------------------------------------
 * @brief Shift register right by N positions
 * @param _Reg: Register or variable
 * @param _Pos: Number of positions to shift
 * ------------------------------------------------------- */
#define bitShiftRight(_Reg, _Pos) ((_Reg) = (_Reg) >> (_Pos))


/* ============================================================================
 *                      DATA TYPE CONVERSION MACROS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Extract MSB (Most Significant Byte) from 16-bit value
 * @param _Value: 16-bit value
 * @return Upper 8 bits
 * ------------------------------------------------------- */
#define Conv_16to8_MSB(_Value) ((uint8_t)((_Value) >> 8))

/* -------------------------------------------------------
 * @brief Extract LSB (Least Significant Byte) from 16-bit value
 * @param _Value: 16-bit value
 * @return Lower 8 bits
 * ------------------------------------------------------- */
#define Conv_16to8_LSB(_Value) ((uint8_t)((_Value) & 0xFFU))

/* -------------------------------------------------------
 * @brief Combine two 8-bit values into one 16-bit value
 * @param _valueHigh: High byte (MSB)
 * @param _valueLow: Low byte (LSB)
 * @return Combined 16-bit value
 * ------------------------------------------------------- */
#define Combine_8to16(_valueHigh, _valueLow) ((uint16_t)((_valueLow) + ((_valueHigh) << 8)))

/* -------------------------------------------------------
 * @brief Extract highest 32 bits from 64-bit value
 * @param _Value: 64-bit value
 * @return Upper 32 bits
 * ------------------------------------------------------- */
#define Conv_64to32_MSB(_Value) ((uint32_t)((_Value) >> 32))

/* -------------------------------------------------------
 * @brief Extract lowest 32 bits from 64-bit value
 * @param _Value: 64-bit value
 * @return Lower 32 bits
 * ------------------------------------------------------- */
#define Conv_64to32_LSB(_Value) ((uint32_t)((_Value) & 0xFFFFFFFFULL))

/* -------------------------------------------------------
 * @brief Combine two 32-bit values into one 64-bit value
 * @param _valueHigh: High 32 bits (MSB)
 * @param _valueLow: Low 32 bits (LSB)
 * @return Combined 64-bit value
 * ------------------------------------------------------- */
#define Combine_32to64(_valueHigh, _valueLow) ((uint64_t)((_valueLow) + (((uint64_t)(_valueHigh)) << 32)))


/* ============================================================================
 *                         MATHEMATICAL MACROS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Compare two floating-point numbers with tolerance
 * @param a: First number
 * @param b: Second number
 * @param epsilon: Tolerance value (e.g., 0.0001)
 * @return true if numbers are equal within epsilon range
 * ------------------------------------------------------- */
#define Float_IsEqual(a, b, epsilon)  ((MATH_Abs((a) - (b))) <= (epsilon))

/* -------------------------------------------------------
 * @brief Mathematical constant PI (π)
 * ------------------------------------------------------- */
#define MATH_Const_PI  3.14159265358979

/* -------------------------------------------------------
 * @brief Mathematical constant E (Euler's number)
 * ------------------------------------------------------- */
#define MATH_Const_E   2.71828182845904

/* -------------------------------------------------------
 * @brief Calculate absolute value
 * @param x: Input value
 * @return Absolute value of x
 * ------------------------------------------------------- */
#define MATH_Abs(x)    (((x) < 0) ? -(x) : (x))


/* ============================================================================
 *                         AVR SPECIFIC MACROS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Configure GPIO pin as output
 * @param _Reg: DDR register (e.g., DDRA, DDRB)
 * @param _Bit: Pin number (0-7)
 * ------------------------------------------------------- */
#define GPIO_Config_OUTPUT(_Reg, _Bit) bitSet(_Reg, _Bit)

/* -------------------------------------------------------
 * @brief Configure GPIO pin as input
 * @param _Reg: DDR register (e.g., DDRA, DDRB)
 * @param _Bit: Pin number (0-7)
 * ------------------------------------------------------- */
#define GPIO_Config_INPUT(_Reg, _Bit) bitClear(_Reg, _Bit)

/* -------------------------------------------------------
 * @brief Millisecond delay function
 * @param _delay_msValue: Delay duration in milliseconds
 * @note Uses AVR's built-in _delay_ms() function
 * ------------------------------------------------------- */
#define delay_ms(_delay_msValue) _delay_ms(_delay_msValue)

/* -------------------------------------------------------
 * @brief Enable global interrupts
 * @note Sets the I-bit in SREG
 * ------------------------------------------------------- */
#define globalInt_Enable    bitSet(SREG, SREG_I)

/* -------------------------------------------------------
 * @brief Disable global interrupts
 * @note Clears the I-bit in SREG
 * ------------------------------------------------------- */
#define globalInt_Disable   bitClear(SREG, SREG_I)

/* -------------------------------------------------------
 * @brief Clear interrupt flag by writing 1
 * @param Reg: Interrupt flag register
 * @param Bit: Flag bit position
 * @note AVR clears flags by writing 1 to them
 * ------------------------------------------------------- */
#define intFlag_clear(Reg, Bit) bitSet(Reg, Bit)

/* -------------------------------------------------------
 * @brief Initialization state constants
 * ------------------------------------------------------- */
#define Initialize   true    /**< Use for initialization state */
#define deInitialize false   /**< Use for de-initialization state */

#endif /* _aKaReZa_H_ */