/* 
* class_HD44780.cpp
*
* Created: 18.03.2020 16:52:56
* Author: Rodin
*/

#include "class_HD44780.h"

// Production of a pulse for Enable.
void lcd::enable( void ) {
	LCD_E_PORT |= (1<<LCD_EN);     // Enable a logical 1.
	_delay_us(LCD_ENABLE_US);      // Delay.
	LCD_E_PORT &= ~(1<<LCD_EN);    // Enable a logical 0.
}

// Send data to function 4-bit.
void lcd::out(uint8_t data) {
	data &= 0xF0;                       // Mask the 4 upper bit.
	LCD_PORT &= ~(0xF0>>(4-LCD_DB));    // Remove the mask.
	LCD_PORT |= (data>>(4-LCD_DB));     // Send the bit.
	enable();
}

// Initialize the LCD.
void lcd::init( void ) {

	LCD_E_DDR |= (1<<LCD_EN);   // To LCD_EN as output.
	LCD_RS_DDR |= (1<<LCD_RS);  // To LCD_RS as output.

	LCD_DDR |= 0b00001111;      // The DB4 ~ DB7 as outputs.


	// Waiting for the start of LCD.
	// Page 46 documentation of HD44780.
	_delay_ms(LCD_BOOTUP_MS);

	// Send the 000011, and wait.
	// Will be sent 3 times.
	// Page 46 documentation of HD44780.
	out(LCD_SOFT_RESET);
	_delay_ms(LCD_SOFT_RESET_MS1);

	// Send pulse in EN for execution.
	// Second execute the above command
	// And delay> 100 usec.
	enable();
	_delay_ms(LCD_SOFT_RESET_MS2);

	// Send pulse in EN for execution.
	// Tuesday execute the above command
	// And delay> 100 usec.
	enable();
	_delay_ms(LCD_SOFT_RESET_MS3);

	// Enable 4-bit mode.
	out(LCD_SET_FUNCTION|LCD_FUNCTION_4BIT);

	_delay_ms(LCD_SET_4BITMODE_MS);

	// 4-bit mode, two lines, 5x7.
	command(LCD_SET_FUNCTION|LCD_FUNCTION_4BIT|LCD_FUNCTION_2LINE|LCD_FUNCTION_5X7);

	// LCD 1, cursor off, off-lit efface the _
	command(LCD_SET_DISPLAY|LCD_DISPLAY_ON|LCD_CURSOR_OFF|LCD_BLINKING_OFF);

	// Increase the cursor off the scroll.
	command(LCD_SET_ENTRY|LCD_ENTRY_INCREASE|LCD_ENTRY_NOSHIFT);

	// Clear the screen.
	clear();
}

// Send data to the LCD.
void lcd::send_data(uint8_t data) {

	LCD_RS_PORT |= (1<<LCD_RS);  // RS a reasonable one.

	out(data);          // Send the fourth upper bit.
	out(data<<4 );      // Send the next 4 bit.

	_delay_us(LCD_WRITEDATA_US); // Delay.
}

// Send command to the LCD.
void lcd::command(uint8_t data) {

	LCD_RS_PORT &= ~(1<<LCD_RS); // RS in logical 0.

	out(data);      // Send the 4 upper bit.
	out(data<<4);   // Send the next 4 bit.

	_delay_us(LCD_COMMAND_US); // Delay.
}

// The LCD cleaning command.
void lcd::clear(void) {
	// Page 24 of documentation HD44780.
	command(LCD_CLEAR_DISPLAY );
	_delay_ms(LCD_CLEAR_DISPLAY_MS);
}

// Command to return the cursor
// The starting position.
void lcd::home(void) {
	// Page 24 of documentation HD44780.
	command(LCD_CURSOR_HOME);
	_delay_ms(LCD_CURSOR_HOME_MS);
}

// Command for a specific position of the cursor.
void lcd::setcursor( uint8_t col, uint8_t row ) {

	uint8_t data;
	switch (row)
	{
		case 0:    // First line.
		data = LCD_SET_DDADR + LCD_DDADR_LINE1 + col;
		break;

		case 1:    // Second line.
		data = LCD_SET_DDADR + LCD_DDADR_LINE2 + col;
		break;

		case 2:    // 3rd line.
		data = LCD_SET_DDADR + LCD_DDADR_LINE3 + col;
		break;

		case 3:    // 4rd line.
		data = LCD_SET_DDADR + LCD_DDADR_LINE4 + col;
		break;

		default:
		return;   // Here we avoid inappropriate line price.
	}

	command(data);
}

// Send strings in LCD.
void lcd::str(const char *data) {
	while( *data != '\0' )
	send_data( *data++ );
}
