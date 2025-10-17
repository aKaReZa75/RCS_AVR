/**
 ******************************************************************************
 * @file     alcd.h
 * @brief    16x2 Alphanumeric LCD display library header for AVR
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This header file provides definitions for:
 *           - HD44780 compatible LCD controller commands
 *           - GPIO pin mappings for 4-bit parallel interface
 *           - Display control functions
 *           - Custom character (CGRAM) support
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_Display_aLCD
 ******************************************************************************
 */
#ifndef _alcd_H_
#define _alcd_H_

#include "aKaReZa.h"


/* ============================================================================
 *                         FEATURE CONFIGURATION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Enable/disable backlight control feature
 * @note Set to true to enable backlight control pin
 * ------------------------------------------------------- */
#define __alcd_useBL  true


/* ============================================================================
 *                         GPIO PIN DEFINITIONS
 * ============================================================================
 * @note These definitions are for AVR compatibility
 *       For STM32, use HAL GPIO functions in implementation
 * ---------------------------------------------------------------------------- */

/* -------------------------------------------------------
 * @brief LCD RS (Register Select) pin configuration
 * @note RS=0: Command mode, RS=1: Data mode
 * ------------------------------------------------------- */
#define __alcd_RS_Config   DDRC      /**< Register to configure the RS pin direction */
#define __alcd_RS_Control  PORTC     /**< Port register to control the RS pin */
#define __alcd_RS_Pin      0         /**< Pin number for the RS pin */

/* -------------------------------------------------------
 * @brief LCD EN (Enable) pin configuration
 * @note Enable signal triggers data latch on LCD
 * ------------------------------------------------------- */
#define __alcd_EN_Config   DDRC      /**< Register to configure the EN pin direction */
#define __alcd_EN_Control  PORTC     /**< Port register to control the EN pin */
#define __alcd_EN_Pin      1         /**< Pin number for the EN pin */

/* -------------------------------------------------------
 * @brief LCD DB4 (Data Bit 4) pin configuration
 * ------------------------------------------------------- */
#define __alcd_DB4_Config  DDRD      /**< Register to configure the DB4 pin direction */
#define __alcd_DB4_Control PORTD     /**< Port register to control the DB4 pin */
#define __alcd_DB4_Pin     5         /**< Pin number for the DB4 pin */

/* -------------------------------------------------------
 * @brief LCD DB5 (Data Bit 5) pin configuration
 * ------------------------------------------------------- */
#define __alcd_DB5_Config  DDRC      /**< Register to configure the DB5 pin direction */
#define __alcd_DB5_Control PORTC     /**< Port register to control the DB5 pin */
#define __alcd_DB5_Pin     2         /**< Pin number for the DB5 pin */

/* -------------------------------------------------------
 * @brief LCD DB6 (Data Bit 6) pin configuration
 * ------------------------------------------------------- */
#define __alcd_DB6_Config  DDRD      /**< Register to configure the DB6 pin direction */
#define __alcd_DB6_Control PORTD     /**< Port register to control the DB6 pin */
#define __alcd_DB6_Pin     6         /**< Pin number for the DB6 pin */

/* -------------------------------------------------------
 * @brief LCD DB7 (Data Bit 7) pin configuration
 * ------------------------------------------------------- */
#define __alcd_DB7_Config  DDRC      /**< Register to configure the DB7 pin direction */
#define __alcd_DB7_Control PORTC     /**< Port register to control the DB7 pin */
#define __alcd_DB7_Pin     3         /**< Pin number for the DB7 pin */

/* -------------------------------------------------------
 * @brief LCD Backlight pin configuration (Optional)
 * @note Only defined if __alcd_useBL is enabled
 * ------------------------------------------------------- */
#ifdef __alcd_useBL
    #define __alcd_BL_Config   DDRD  /**< Register to configure the backlight pin direction */
    #define __alcd_BL_Control  PORTD /**< Port register to control the backlight pin */
    #define __alcd_BL_Pin      7     /**< Pin number for the backlight pin */
#endif


/* ============================================================================
 *                         TIMING CONFIGURATION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Delay function macro
 * @note Uses delay_us() from aKaReZa.h
 * ------------------------------------------------------- */
#define __alcd_delay(_delayValue)  _delay_us(_delayValue)

/* -------------------------------------------------------
 * @brief Timing constants in microseconds
 * ------------------------------------------------------- */
#define __alcd_delay_CMD      50      /**< Standard command execution time */
#define __alcd_delay_modeSet  5000    /**< Mode setting and initialization delay */
#define __alcd_delay_powerON  50000   /**< Power-on stabilization delay */


/* ============================================================================
 *                         LCD DISPLAY CONFIGURATION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Command/Data selection for RS pin
 * ------------------------------------------------------- */
#define __alcd_writeCmd   0  /**< RS=0: Send command to LCD */
#define __alcd_writeData  1  /**< RS=1: Send data to LCD */

/* -------------------------------------------------------
 * @brief LCD dimensions
 * ------------------------------------------------------- */
#define __alcd_max_x  16  /**< Maximum columns (16 characters per line) */
#define __alcd_max_y  2   /**< Maximum rows (2 lines) */


/* ============================================================================
 *                         HD44780 COMMAND DEFINITIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Function Set Commands
 * @note Configure LCD interface and display format
 * ------------------------------------------------------- */
#define __alcd_Mode_8bit_2line_5x8   0x38  /**< 8-bit mode, 2 lines, 5x8 font */
#define __alcd_Mode_8bit_1line_5x8   0x30  /**< 8-bit mode, 1 line, 5x8 font */
#define __alcd_Mode_4bit_2line_5x8   0x28  /**< 4-bit mode, 2 lines, 5x8 font */
#define __alcd_Mode_4bit_1line_5x8   0x20  /**< 4-bit mode, 1 line, 5x8 font */
#define __alcd_Mode_4bit_Step1       0x33  /**< Initialize LCD for 4-bit mode (sends 0x03 twice) */
#define __alcd_Mode_4bit_Step2       0x32  /**< Set 4-bit mode (sends 0x03 then 0x02) */

/* -------------------------------------------------------
 * @brief Display Control Commands
 * @note Control display on/off, cursor, and blink
 * ------------------------------------------------------- */
#define __alcd_Display_Clear 0x01  /**< Clear entire display and return home */
#define __alcd_Display_Home  0x02  /**< Return cursor to home position (0,0) */
#define __alcd_Display_OFF   0x08  /**< Display OFF, cursor OFF, blink OFF */
#define __alcd_Display_ON    0x0C  /**< Display ON, cursor OFF, blink OFF */

/* -------------------------------------------------------
 * @brief Cursor Control Commands
 * @note Configure cursor appearance
 * ------------------------------------------------------- */
#define __alcd_Cursor_OFF         0x0C  /**< Display ON, cursor OFF, blink OFF */
#define __alcd_Cursor_ON          0x0E  /**< Display ON, cursor ON (underline), blink OFF */
#define __alcd_Cursor_Blink       0x0D  /**< Display ON, cursor OFF, blink ON (block blink) */
#define __alcd_Cursor_dashBLINK   0x0F  /**< Display ON, cursor ON, blink ON (underline + blink) */

/* -------------------------------------------------------
 * @brief Entry Mode Set Commands
 * @note Configure cursor movement direction
 * ------------------------------------------------------- */
#define __alcd_Entry_Inc     0x06  /**< Increment cursor (shift cursor to right) */
#define __alcd_Entry_Dec     0x04  /**< Decrement cursor (shift cursor to left) */
#define __alcd_Entry_Shift   0x07  /**< Increment with display shift */

/* -------------------------------------------------------
 * @brief DDRAM Address Commands
 * @note Line start addresses for cursor positioning
 * ------------------------------------------------------- */
#define __alcd_Line1_Start   0x80  /**< Start address of line 1 (first row) */
#define __alcd_Line2_Start   0xC0  /**< Start address of line 2 (second row) */

/* -------------------------------------------------------
 * @brief CGRAM Address Command
 * @note Custom character generation RAM start address
 * ------------------------------------------------------- */
#define __alcd_CGRAM_Start   0x40  /**< Start address for custom character (8 slots: 0-7) */


/* ============================================================================
 *                         FUNCTION PROTOTYPES
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize LCD in 4-bit mode
 * @retval None
 * @note Must be called before any other LCD functions
 *       Follows HD44780 initialization sequence
 * ------------------------------------------------------- */
void alcd_init(void);

/* -------------------------------------------------------
 * @brief Send data or command to LCD
 * @param _data: 8-bit data/command to send
 * @param _alcd_cmdData: false=Command, true=Data
 * @retval None
 * @note Low-level function, typically not called directly
 * ------------------------------------------------------- */
void alcd_write(uint8_t _data, bool _alcd_cmdData);

/* -------------------------------------------------------
 * @brief Print single character at current position
 * @param _char: Character to display
 * @retval None
 * ------------------------------------------------------- */
void alcd_putc(char _char);

/* -------------------------------------------------------
 * @brief Print null-terminated string
 * @param _str: Pointer to string
 * @retval None
 * ------------------------------------------------------- */
void alcd_puts(char* _str);

/* -------------------------------------------------------
 * @brief Move cursor to specified position
 * @param _alcd_x: Column (0 to 15)
 * @param _alcd_y: Row (0 or 1)
 * @retval None
 * ------------------------------------------------------- */
void alcd_gotoxy(uint8_t _alcd_x, uint8_t _alcd_y);

/* -------------------------------------------------------
 * @brief Clear display and return cursor to home
 * @retval None
 * ------------------------------------------------------- */
void alcd_clear(void);

/* -------------------------------------------------------
 * @brief Configure display, cursor, and blink settings
 * @param _alcd_Display: Display ON/OFF
 * @param _alcd_Cursor: Cursor visibility ON/OFF
 * @param _alcd_Blink: Cursor blink ON/OFF
 * @retval None
 * ------------------------------------------------------- */
void alcd_display(bool _alcd_Display, bool _alcd_Cursor, bool _alcd_Blink);

/* -------------------------------------------------------
 * @brief Create custom character in CGRAM
 * @param _alcd_CGRAMadd: Character slot (0-7)
 * @param _alcd_CGRAMdata: Pointer to 8-byte character pattern
 * @retval None
 * @note Each byte represents one row (5 bits used)
 * ------------------------------------------------------- */
void alcd_customChar(uint8_t _alcd_CGRAMadd, const uint8_t *_alcd_CGRAMdata);

/* -------------------------------------------------------
 * @brief Control LCD backlight
 * @param _alcd_BL: true=ON, false=OFF
 * @retval None
 * @note Only available if __alcd_useBL is enabled
 * ------------------------------------------------------- */
#if __alcd_useBL == true
    void alcd_backLight(bool _alcd_BL);
#endif

#endif /* _alcd_H_ */