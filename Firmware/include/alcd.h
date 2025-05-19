/**
 * @file     alcd.h
 * @brief    16x2 Alphanumeric LCD display Library
 * @note     This file is for _AVR_ microcontrollers
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     For detailed explanations of this library, along with additional notes and examples of usage, 
 *           please visit the following repository: 
 *           https://github.com/aKaReZa75/AVR_Display_aLCD_Parallel
 */
#ifndef _alcd_H
#define _alcd_H

#include "aKaReZa.h"

/** 
 * @def __alcd_useBL
 * @brief Macro to enable or disable the backlight feature.
 * If set to `true`, backlight functionality will be enabled.
 */
#define __alcd_useBL  true

/** @name LCD Control Pins and Configuration Registers
 *  @{
 */
#define __alcd_RS_Config   DDRC      /**< @brief Register to configure the RS pin direction */
#define __alcd_RS_Control  PORTC     /**< @brief Port register to control the RS pin */
#define __alcd_RS_Pin      0         /**< @brief Pin number for the RS pin */

#define __alcd_EN_Config   DDRC      /**< @brief Register to configure the EN pin direction */
#define __alcd_EN_Control  PORTC     /**< @brief Port register to control the EN pin */
#define __alcd_EN_Pin      1         /**< @brief Pin number for the EN pin */

#define __alcd_DB4_Config  DDRD      /**< @brief Register to configure the DB4 pin direction */
#define __alcd_DB4_Control PORTD     /**< @brief Port register to control the DB4 pin */
#define __alcd_DB4_Pin     5         /**< @brief Pin number for the DB4 pin */

#define __alcd_DB5_Config  DDRC      /**< @brief Register to configure the DB5 pin direction */
#define __alcd_DB5_Control PORTC     /**< @brief Port register to control the DB5 pin */
#define __alcd_DB5_Pin     2         /**< @brief Pin number for the DB5 pin */

#define __alcd_DB6_Config  DDRD      /**< @brief Register to configure the DB6 pin direction */
#define __alcd_DB6_Control PORTD     /**< @brief Port register to control the DB6 pin */
#define __alcd_DB6_Pin     6         /**< @brief Pin number for the DB6 pin */

#define __alcd_DB7_Config  DDRC      /**< @brief Register to configure the DB7 pin direction */
#define __alcd_DB7_Control PORTC     /**< @brief Port register to control the DB7 pin */
#define __alcd_DB7_Pin     3         /**< @brief Pin number for the DB7 pin */

/** @brief Backlight Pin Configuration (Optional) */
#ifdef __alcd_useBL
    #define __alcd_BL_Config   DDRD /**< @brief Register to configure the backlight pin direction */
    #define __alcd_BL_Control  PORTD /**< @brief Port register to control the backlight pin */
    #define __alcd_BL_Pin      7    /**< @brief Pin number for the backlight pin */
#endif
/** @} */

/** @def __alcd_delay(_alcd_delayValue)
 * @brief Macro for adding delays.
 * @param _alcd_delayValue Delay time in milliseconds.
 */
#define __alcd_delay(_alcd_delayValue) delay_ms(_alcd_delayValue)
#define __alcd_delay_powerON 15 
#define __alcd_delay_modeSet  5 
#define __alcd_delay_CMD      2

#define __alcd_writeCmd           0
#define __alcd_writeData          1      
#define __alcd_DISPLAY_CLEAR      0x01
#define __alcd_DISPLAY_OFF        0x08
#define __alcd_DISPLAY_ON         0x0C
#define __alcd_CURSOR_OFF         0x0C
#define __alcd_CURSOR_ON          0x0E
#define __alcd_CURSOR_BLINK       0x0D
#define __alcd_CURSOR_dashBLINK   0x0F


/**
 * @brief Initializes the LCD display.
 */
void alcd_init(void);

/**
 * @brief Clears the LCD display.
 */
void alcd_clear(void);

/**
 * @brief Sets the cursor to the specified position on the display.
 * @param _alcd_x_position X position (column).
 * @param _alcd_y_position Y position (row).
 */
void alcd_gotoxy(uint8_t _alcd_x_position, uint8_t _alcd_y_position);

/**
 * @brief Writes a null-terminated string to the LCD.
 * @param _alcd_str The string to display.
 */
void alcd_puts(char *_alcd_str);

/**
 * @brief Configures the cursor (on/off) and blinking behavior.
 * @param _alcd_Cursor Enable/Disable the cursor.
 * @param _alcd_Blink Enable/Disable blinking of the cursor.
 */
void alcd_cursor(_Bool _alcd_Cursor, _Bool _alcd_Blink);

/**
 * @brief Controls the display on/off state.
 * @param _alcd_displayStatus Enable/Disable the display.
 */
void alcd_display(_Bool _alcd_displayStatus);

/**
 * @brief Sends a command or data byte to the LCD (low-level function).
 * @param _alcd_write_value The byte value (command or data).
 * @param _alcd_cmdData A flag indicating whether the byte is a command (`false`) or data (`true`).
 */
void alcd_write(uint8_t _alcd_write_value, bool _alcd_cmdData);

/**
 * @brief Displays a single character at the current cursor position.
 * @param c The character to display.
 */
void alcd_putc(char _alcd_putcr);

/**
 * @brief Writes custom characters to the LCD.
 * @param _alcd_CGRAMadd The CGRAM address to store the custom character.
 * @param _alcd_CGRAMdata The custom character data.
 */
void alcd_customChar(uint8_t _alcd_CGRAMadd, const uint8_t *_alcd_CGRAMdata);


#ifdef __alcd_useBL
    /**
     * @brief Controls the backlight of the LCD.
     * @param _alcd_backLightStatus Enable/Disable the backlight.
     */
    void alcd_backLight(bool _alcd_backLightStatus);
#endif
    
#endif
