/**
 ******************************************************************************
 * @file     alcd.c
 * @brief    16x2 Alphanumeric LCD display library implementation for AVR
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This library provides:
 *           - 4-bit parallel interface control for HD44780 compatible LCDs
 *           - Custom character generation (CGRAM)
 *           - Cursor positioning and display control
 *           - Backlight control support
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_Display_aLCD
 ******************************************************************************
 */
#include "alcd.h"


/* ============================================================================
 *                         PRIVATE VARIABLES
 * ============================================================================ */
bool __alcd_initStatus = false;
uint8_t __alcd_x_position = 0;
uint8_t __alcd_y_position = 0;


/* ============================================================================
 *                      CUSTOM CHARACTER FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Create custom character in CGRAM
 * @param _alcd_CGRAMadd: CGRAM address (0-7)
 * @param _alcd_CGRAMdata: Pointer to 8-byte character data
 * @retval None
 * @note Each character is 5x8 pixels, represented by 8 bytes
 *       After writing, cursor returns to previous position
 * ------------------------------------------------------- */
void alcd_customChar(uint8_t _alcd_CGRAMadd, const uint8_t *_alcd_CGRAMdata)
{
    uint8_t _forCounter = 0;
    uint8_t _CG_Add = __alcd_CGRAM_Start + (_alcd_CGRAMadd << 3);
    
    for(_forCounter = 0; _forCounter < 8; _forCounter++)
    {
        alcd_write(_CG_Add++, __alcd_writeCmd);
        alcd_write(*_alcd_CGRAMdata++, __alcd_writeData);
    };
    
    alcd_gotoxy(__alcd_x_position, __alcd_y_position);
};


/* ============================================================================
 *                       BACKLIGHT CONTROL
 * ============================================================================ */

#if __alcd_useBL == true
/* -------------------------------------------------------
 * @brief Control LCD backlight
 * @param _alcd_BL: Backlight state (true=ON, false=OFF)
 * @retval None
 * ------------------------------------------------------- */
void alcd_backLight(bool _alcd_BL)
{
    bitChange(__alcd_BL_Control, __alcd_BL_Pin, _alcd_BL);
};
#endif


/* ============================================================================
 *                       DISPLAY CONTROL FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Configure display, cursor, and blink settings
 * @param _alcd_Display: Display ON/OFF
 * @param _alcd_Cursor: Cursor visibility ON/OFF
 * @param _alcd_Blink: Cursor blink ON/OFF
 * @retval None
 * ------------------------------------------------------- */
void alcd_display(bool _alcd_Display, bool _alcd_Cursor, bool _alcd_Blink)
{
    uint8_t _cursorState = __alcd_Display_OFF;
    
    bitChange(_cursorState, 0, _alcd_Blink);
    bitChange(_cursorState, 1, _alcd_Cursor);
    bitChange(_cursorState, 2, _alcd_Display);
    
    alcd_write(_cursorState, __alcd_writeCmd);
};

/* -------------------------------------------------------
 * @brief Clear entire display and reset cursor to home
 * @retval None
 * @note Resets internal position tracking to (0,0)
 * ------------------------------------------------------- */
void alcd_clear(void)
{
    alcd_write(__alcd_Display_Clear, __alcd_writeCmd);
    __alcd_x_position = 0;
    __alcd_y_position = 0;
    __alcd_delay(__alcd_delay_modeSet);
};


/* ============================================================================
 *                       CURSOR POSITIONING
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Move cursor to specific position
 * @param _alcd_x: Column position (0 to __alcd_max_x-1)
 * @param _alcd_y: Row position (0 to __alcd_max_y-1)
 * @retval None
 * @note Invalid positions are clamped to (0,0)
 * ------------------------------------------------------- */
void alcd_gotoxy(uint8_t _alcd_x, uint8_t _alcd_y)
{
    uint8_t _address = 0x00;

    if(_alcd_x >= __alcd_max_x || _alcd_y >= __alcd_max_y)
    {
        __alcd_x_position = 0;
        __alcd_y_position = 0;
    }
    else
    {
        __alcd_x_position = _alcd_x;
        __alcd_y_position = _alcd_y;
    };

    _address = (__alcd_y_position == 0) ? __alcd_Line1_Start : __alcd_Line2_Start;
    _address = _address + __alcd_x_position;

    alcd_write(_address, __alcd_writeCmd);
};


/* ============================================================================
 *                       CHARACTER OUTPUT FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Print null-terminated string to LCD
 * @param _str: Pointer to string
 * @retval None
 * @note String is printed at current cursor position
 *       Cursor advances with each character
 * ------------------------------------------------------- */
void alcd_puts(char* _str)
{
    while (*_str != '\0')
    {
        alcd_putc(*_str++);
    };
};

/* -------------------------------------------------------
 * @brief Print single character to LCD
 * @param _char: Character to print
 * @retval None
 * @note Automatically wraps to next line at end of row
 * ------------------------------------------------------- */
void alcd_putc(char _char)
{
    alcd_write(_char, __alcd_writeData);
    __alcd_x_position++;
    
    if(__alcd_x_position >= __alcd_max_x)
    {
        __alcd_x_position = 0;
        __alcd_y_position++;
        alcd_gotoxy(__alcd_x_position, __alcd_y_position);
    };
};


/* ============================================================================
 *                       LOW-LEVEL WRITE FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Send data/command to LCD in 4-bit mode
 * @param _data: 8-bit data/command to send
 * @param _alcd_cmdData: Command mode (false) or Data mode (true)
 * @retval None
 * @note Sends high nibble first, then low nibble
 *       Uses different delays during initialization
 * ------------------------------------------------------- */
void alcd_write(uint8_t _data, bool _alcd_cmdData)
{
    bitChange(__alcd_RS_Control, __alcd_RS_Pin, _alcd_cmdData);

    /* Send high nibble (bits 4-7) */
    bitChange(__alcd_DB4_Control, __alcd_DB4_Pin, bitCheck(_data, 4));
    bitChange(__alcd_DB5_Control, __alcd_DB5_Pin, bitCheck(_data, 5));
    bitChange(__alcd_DB6_Control, __alcd_DB6_Pin, bitCheck(_data, 6));
    bitChange(__alcd_DB7_Control, __alcd_DB7_Pin, bitCheck(_data, 7));

    bitSet(__alcd_EN_Control, __alcd_EN_Pin);
    
    if(__alcd_initStatus == false)
    {
        __alcd_delay(__alcd_delay_modeSet);
    }
    else
    {
        __alcd_delay(__alcd_delay_CMD);
    };
    
    bitClear(__alcd_EN_Control, __alcd_EN_Pin);

    /* Send low nibble (bits 0-3) */
    bitChange(__alcd_DB4_Control, __alcd_DB4_Pin, bitCheck(_data, 0));
    bitChange(__alcd_DB5_Control, __alcd_DB5_Pin, bitCheck(_data, 1));
    bitChange(__alcd_DB6_Control, __alcd_DB6_Pin, bitCheck(_data, 2));
    bitChange(__alcd_DB7_Control, __alcd_DB7_Pin, bitCheck(_data, 3));

    bitSet(__alcd_EN_Control, __alcd_EN_Pin);
    
    if(__alcd_initStatus == false)
    {
        __alcd_delay(__alcd_delay_modeSet);
    }
    else
    {
        __alcd_delay(__alcd_delay_CMD);
    };
    
    bitClear(__alcd_EN_Control, __alcd_EN_Pin);
};


/* ============================================================================
 *                       INITIALIZATION FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize LCD in 4-bit mode
 * @retval None
 * @note Initialization sequence:
 *       1. Configure GPIO pins
 *       2. Wait for LCD power-on
 *       3. Set 4-bit mode
 *       4. Configure display settings
 *       5. Clear display
 * @note Follows HD44780 initialization procedure
 * ------------------------------------------------------- */
void alcd_init(void)
{
    __alcd_initStatus = false;
    __alcd_delay(__alcd_delay_powerON);
    
    /* Configure control pins as outputs */
    bitSet(__alcd_RS_Config , __alcd_RS_Pin);
    bitSet(__alcd_EN_Config , __alcd_EN_Pin);
    bitSet(__alcd_DB4_Config, __alcd_DB4_Pin);
    bitSet(__alcd_DB5_Config, __alcd_DB5_Pin);
    bitSet(__alcd_DB6_Config, __alcd_DB6_Pin);
    bitSet(__alcd_DB7_Config, __alcd_DB7_Pin);

    #if __alcd_useBL == true
        bitSet(__alcd_BL_Config, __alcd_BL_Pin);
        bitSet(__alcd_BL_Control, __alcd_BL_Pin);
    #endif

    /* HD44780 initialization sequence for 4-bit mode */
    alcd_write(__alcd_Mode_4bit_Step1, __alcd_writeCmd);
    __alcd_delay(__alcd_delay_modeSet);
    
    alcd_write(__alcd_Mode_4bit_Step2, __alcd_writeCmd);
    __alcd_delay(__alcd_delay_modeSet);
    
    alcd_write(__alcd_Mode_4bit_2line_5x8, __alcd_writeCmd);
    __alcd_delay(__alcd_delay_modeSet);
    
    alcd_write(__alcd_Display_ON, __alcd_writeCmd);
    __alcd_delay(__alcd_delay_modeSet);
    
    alcd_write(__alcd_Cursor_OFF, __alcd_writeCmd);
    __alcd_delay(__alcd_delay_modeSet);
    
    alcd_write(__alcd_Entry_Inc, __alcd_writeCmd);
    __alcd_delay(__alcd_delay_modeSet);
    
    alcd_write(__alcd_Display_Clear, __alcd_writeCmd);
    __alcd_delay(__alcd_delay_modeSet);
    
    __alcd_initStatus = true;
};