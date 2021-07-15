/* 
* class_HD44780.h
*
* Created: 18.03.2020 16:52:57
* Author: Rodin
*/


#ifndef __CLASS_HD44780_H__
#define __CLASS_HD44780_H__

#include <avr/io.h>
#include <util/delay.h>

// LCD DB4-DB7 <-->  PORT Bit 0-3
#define LCD_PORT      PORTB
#define LCD_DDR       DDRB
#define LCD_DB        PB0

// LCD RS      <-->  PORTB Bit PB2     (RS: 1=Data, 0=Command)
#define LCD_RS_PORT   PORTB
#define LCD_RS_DDR    DDRB
#define LCD_RS        PB5

// LCD EN      <-->  PORTB Bit PB0     (EN: 1-pulse for data)
#define LCD_E_PORT    PORTB
#define LCD_E_DDR     DDRB
#define LCD_EN        PB4




// Delay Time LCD (MS=Milliseconds, US=Microseconds)
#define LCD_BOOTUP_MS           15
#define LCD_ENABLE_US           20
#define LCD_WRITEDATA_US        46
#define LCD_COMMAND_US          42
#define LCD_SOFT_RESET_MS1      5
#define LCD_SOFT_RESET_MS2      1
#define LCD_SOFT_RESET_MS3      1
#define LCD_SET_4BITMODE_MS     5
#define LCD_CLEAR_DISPLAY_MS    2
#define LCD_CURSOR_HOME_MS      2

/*
Specifying the address lines of the LCD.
Such addresses are for LCD 16 characters.
For different size should consult documentation LCD
*/

#define LCD_DDADR_LINE1         0x00
#define LCD_DDADR_LINE2         0x40
#define LCD_DDADR_LINE3         0x10
#define LCD_DDADR_LINE4         0x50

// Commands and their arguments on the LCD.
// Send the function lcd_command.

// Clear Display -------------- 0b00000001
#define LCD_CLEAR_DISPLAY       0x01

// Cursor Home ---------------- 0b0000001x
#define LCD_CURSOR_HOME         0x02

// Set Entry Mode ------------- 0b000001xx
#define LCD_SET_ENTRY           0x04
#define LCD_ENTRY_DECREASE      0x00
#define LCD_ENTRY_INCREASE      0x02
#define LCD_ENTRY_NOSHIFT       0x00
#define LCD_ENTRY_SHIFT         0x01

// Set Display ---------------- 0b00001xxx
#define LCD_SET_DISPLAY         0x08
#define LCD_DISPLAY_OFF         0x00
#define LCD_DISPLAY_ON          0x04
#define LCD_CURSOR_OFF          0x00
#define LCD_CURSOR_ON           0x02
#define LCD_BLINKING_OFF        0x00
#define LCD_BLINKING_ON         0x01

// Set Shift ------------------ 0b0001xxxx
#define LCD_SET_SHIFT           0x10
#define LCD_CURSOR_MOVE         0x00
#define LCD_DISPLAY_SHIFT       0x08
#define LCD_SHIFT_LEFT          0x00
#define LCD_SHIFT_RIGHT         0x04

// Set Function --------------- 0b001xxxxx
#define LCD_SET_FUNCTION        0x20
#define LCD_FUNCTION_4BIT       0x00
#define LCD_FUNCTION_8BIT       0x10
#define LCD_FUNCTION_1LINE      0x00
#define LCD_FUNCTION_2LINE      0x08
#define LCD_FUNCTION_5X7        0x00
#define LCD_FUNCTION_5X10       0x04
#define LCD_SOFT_RESET          0x30

// Set DD RAM Address --------- 0b1xxxxxxx  (Display Data RAM)
#define LCD_SET_DDADR           0x80

class lcd
{
private:
	// Production of a pulse for Enable.
	static void enable( void );
	// Send data to function 4-bit.
	static void out(uint8_t data);
	// Sends commands to the LCD.
	static void command( uint8_t data );

public:
	// Initialize the LCD. You must call at the beginning of the program.
	static void init( void );

	// Cleaning the LCD.
	static void clear( void );

	// Positioning the cursor in position 0,0.
	// First row, first character.
	static void home( void );

	// Positioning the cursor to a location.
	static void setcursor( uint8_t col, uint8_t row );

	// Returns the character at the current cursor position.
	static void send_data( uint8_t data );

	// Loads a strings in the current
	// Cursor position.
	static void str( const char *data );
	
}; //class_lcd

#endif //__CLASS_HD44780_H__
