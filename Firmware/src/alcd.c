/**
 * @file     alcd.C
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
#include "alcd.h"

uint8_t alcd_x_position = 0; /**< Current cursor x position on the LCD */
uint8_t alcd_y_position = 0; /**< Current cursor y position on the LCD */

/**
 * @brief Sends a command or data byte to the LCD.
 * @param _alcd_write_value The byte value to write (command or data).
 * @param _alcd_cmdData Boolean flag to specify if the byte is command (0) or data (1).
 */
void alcd_write(uint8_t _alcd_write_value, bool _alcd_cmdData)
{
	/* Set RS pin to command or data mode */
	bitChange(__alcd_RS_Control, __alcd_RS_Pin, _alcd_cmdData); //cmd = 0; Data = 1

	/* Set data bits */
	bitChange(__alcd_DB7_Control, __alcd_DB7_Pin, bitCheck(_alcd_write_value, 7));
	bitChange(__alcd_DB6_Control, __alcd_DB6_Pin, bitCheck(_alcd_write_value, 6));
	bitChange(__alcd_DB5_Control, __alcd_DB5_Pin, bitCheck(_alcd_write_value, 5));
	bitChange(__alcd_DB4_Control, __alcd_DB4_Pin, bitCheck(_alcd_write_value, 4));

	bitSet(__alcd_EN_Control, __alcd_EN_Pin);
	__alcd_delay(__alcd_delay_CMD);
	bitClear(__alcd_EN_Control, __alcd_EN_Pin);

	/* Set data bits */
	bitChange(__alcd_DB7_Control, __alcd_DB7_Pin, bitCheck(_alcd_write_value, 3));
	bitChange(__alcd_DB6_Control, __alcd_DB6_Pin, bitCheck(_alcd_write_value, 2));
	bitChange(__alcd_DB5_Control, __alcd_DB5_Pin, bitCheck(_alcd_write_value, 1));
	bitChange(__alcd_DB4_Control, __alcd_DB4_Pin, bitCheck(_alcd_write_value, 0));
	
	bitSet(__alcd_EN_Control, __alcd_EN_Pin);
	__alcd_delay(__alcd_delay_CMD);
	bitClear(__alcd_EN_Control, __alcd_EN_Pin);	
};



/**
 * @brief Displays a single character at the current cursor position.
 * @param _alcd_putcr The character to display.
 */
void alcd_putc(char _alcd_putcr)
{
	/* Write each character to LCD */
	alcd_write(_alcd_putcr, __alcd_writeData);
	if(++alcd_x_position == 16)	
	{
		/* Move to next line if x position exceeds 15 */
		alcd_gotoxy(0, ++alcd_y_position);	
	};
};


/**
 * @brief Displays a null-terminated string on the LCD.
 * @param _alcd_str The string to display.
 */
void alcd_puts(char *_alcd_str)
{
	while(*_alcd_str != 0)
	{
		/* Write each character to LCD */
		alcd_write(*_alcd_str++, __alcd_writeData);
		if(++alcd_x_position == 16)	
		{
			/* Move to next line if x position exceeds 15 */
			alcd_gotoxy(0, ++alcd_y_position);	
		};
	};
};


/**
 * @brief Writes a custom character to the LCD's CGRAM.
 * @param _alcd_CGRAMadd The address in the CGRAM to store the custom character.
 * @param _alcd_CGRAMdata The character data to write to CGRAM.
 */
void alcd_customChar(uint8_t _alcd_CGRAMadd, const uint8_t *_alcd_CGRAMdata)
{
	uint8_t _forCounter = 0;
	/* Address for custom character in CGRAM */
	_alcd_CGRAMadd = 0x40 + (_alcd_CGRAMadd << 3);
	for(_forCounter=0; _forCounter < 8; _forCounter++)
	{	
		alcd_write(_alcd_CGRAMadd++  , __alcd_writeCmd );	/**< Set CGRAM address */
		alcd_write(*_alcd_CGRAMdata++, __alcd_writeData);	/**< Write custom character data */
	};
	alcd_gotoxy(alcd_x_position, alcd_y_position);	/**< Return to the current cursor position */
};


/**
 * @brief Initializes the LCD display.
 * Sets the LCD in 4-bit mode and configures the display settings.
 */
void alcd_init(void)
{
	__alcd_delay(__alcd_delay_powerON);	/**< Power-on delay */

	/* Configure data pins */
	GPIO_Config_OUTPUT(__alcd_DB4_Config, __alcd_DB4_Pin);
	GPIO_Config_OUTPUT(__alcd_DB5_Config, __alcd_DB5_Pin);
	GPIO_Config_OUTPUT(__alcd_DB6_Config, __alcd_DB6_Pin);
	GPIO_Config_OUTPUT(__alcd_DB7_Config, __alcd_DB7_Pin);
	GPIO_Config_OUTPUT(__alcd_RS_Config , __alcd_RS_Pin);
	GPIO_Config_OUTPUT(__alcd_EN_Config , __alcd_EN_Pin);

	#ifdef __alcd_useBL
		GPIO_Config_OUTPUT(__alcd_BL_Config , __alcd_BL_Pin);	/**< Configure backlight pin */
		bitSet(__alcd_BL_Control, __alcd_BL_Pin); /**< Turn on the backlight */
	#endif

    alcd_write(0x33, __alcd_writeCmd); /**< Initialize LCD for 4-bit mode */
    __alcd_delay(__alcd_delay_modeSet);
    alcd_write(0x32, __alcd_writeCmd); /**< Set 4-bit mode */
    __alcd_delay(__alcd_delay_modeSet);
    alcd_write(0x28, __alcd_writeCmd);  /**< Two Row 4bit mode */
    __alcd_delay(__alcd_delay_modeSet);
    alcd_write(0x0C, __alcd_writeCmd);  /**< Display on, cursor off */
    alcd_write(0x06, __alcd_writeCmd);  /**< Auto increment cursor */
    alcd_write(__alcd_DISPLAY_CLEAR, __alcd_writeCmd);  /**< Clear display */
	alcd_x_position = 0;
	alcd_y_position = 0;	
};


/**
 * @brief Clears the LCD display.
 */
void alcd_clear(void)
{
	alcd_write(__alcd_DISPLAY_CLEAR, __alcd_writeCmd);
	/**< Reset cursor to position (0, 0) */
	alcd_x_position = 0;
	alcd_y_position = 0;	
};


/**
 * @brief Sets the cursor position on the LCD.
 * @param _alcd_x_position The x position (column) to move the cursor to.
 * @param _alcd_y_position The y position (row) to move the cursor to.
 */
void alcd_gotoxy(uint8_t _alcd_x_position, uint8_t _alcd_y_position)
{	
	if(_alcd_y_position == 2)
	{
		alcd_x_position = 0;
		alcd_y_position = 0;
	}
	else
	{
		alcd_x_position = _alcd_x_position;
		alcd_y_position = _alcd_y_position;	
	};
	bitSet   (_alcd_x_position, 7);						/**< if y==0 */
	bitChange(_alcd_x_position, 6, _alcd_y_position);	/**< if y==1 */
	alcd_write(_alcd_x_position, __alcd_writeCmd);	/**< Move cursor to desired position */
};


/**
 * @brief Controls the cursor visibility and blinking.
 * @param _alcd_Cursor Boolean flag to enable/disable the cursor.
 * @param _alcd_Blink Boolean flag to enable/disable the cursor blinking.
 */
void alcd_cursor(_Bool _alcd_Cursor, _Bool _alcd_Blink)
{
    uint8_t _lcdCursorCMD = 0x0C; /**< Default cursor off */
    bitChange(_lcdCursorCMD, 1, _alcd_Cursor); /**< Set cursor on/off */
    bitChange(_lcdCursorCMD, 0, _alcd_Blink); /**< Set blink on/off */
    alcd_write(_lcdCursorCMD, __alcd_writeCmd); /**< Apply cursor settings */
};


/**
 * @brief Controls the display on/off state.
 * @param _alcd_displayStatus Boolean flag to enable/disable the display.
 */
void alcd_display(_Bool _alcd_displayStatus)
{
    uint8_t _lcdDisplayStatus = 0x08; /**< Default display off */
    bitChange(_lcdDisplayStatus, 2, _alcd_displayStatus); /**< Set display on/off */
    alcd_write(_lcdDisplayStatus, __alcd_writeCmd); /**< Apply display settings */
};


#ifdef __alcd_useBL
    /**
     * @brief Controls the backlight status.
     * @param _alcd_backLightStatus Boolean flag to enable/disable backlight.
     */
	void alcd_backLight(bool _alcd_backLightStatus)
	{
		/* Turn backlight on/off */
		bitChange(__alcd_BL_Control, __alcd_BL_Pin, _alcd_backLightStatus);
	};
#endif
