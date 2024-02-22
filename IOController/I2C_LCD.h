#include "Arduino.h"
#include "Print.h"
#include "Wire.h"

#ifndef _I2C_LCD_H_
#define _I2C_LCD_H_

class I2C_LCD: public Print
{
public:
	enum LCDBackLight_e: uint8_t
	{
		LCDBackLightOnMask = 0x0F,
		LCDBackLightOffMask = 0x07 				
	}; 
	
	enum LCDEntryMode_e: uint8_t
	{
		LCDEntryModeOne = 0x04,
		LCDEntryModeTwo = 0x05,
		LCDEntryModeThree = 0x06,
		LCDEntryModeFour = 0x07
	}; 

	enum LCDCursorType_e: uint8_t
	{
		LCDCursorTypeOff= 0x0C,
		LCDCursorTypeBlink = 0x0D,
		LCDCursorTypeOn = 0x0E,
		LCDCursorTypeOnBlink  = 0x0F
	}; 
	
	enum LCDDirectionType_e : uint8_t
	{
		LCDMoveRight= 1,
		LCDMoveLeft = 2
	}; 
	
	I2C_LCD(uint8_t NumRow, uint8_t NumCol, uint8_t I2Caddress,TwoWire *twi );
	~I2C_LCD(){};
	
	bool init(LCDCursorType_e);
	void display(bool);
	void reset(LCDCursorType_e);
	
	void set_backlight(bool);
	bool get_backlight(void);
	
	void send_string (char *str);
	void send_char (char data);
	void create_custom_char(uint8_t location, uint8_t* charmap);
	void print_custom_char(uint8_t location);

	void move_cursor(LCDDirectionType_e, uint8_t moveSize);
	void scroll(LCDDirectionType_e, uint8_t ScrollSize);
	void go_to(uint8_t  lineNo, uint8_t  col);
	void clear_line(uint8_t lineNo);
	void clear(void);
	void clear_screen_cmd(void);
	void home(void);
	void change_entry_mode(LCDEntryMode_e mode);

	virtual size_t write(uint8_t);
private:
	enum LCDAddress_e : uint8_t {
		LCDLineAddressOne =  0x80,  /**< Line 1 */
		LCDLineAddressTwo =  0xC0, /**< Line 2 */
		LCDLineAddress3Col20 = 0x94, /**< Line 3 20x04 line 3 */
		LCDLineAddress4Col20 = 0xD4, /**< Line 4 20x04 line 4 */
		LCDLineAddress3Col16  = 0x90, /**< Line 3 16x04  untested, no part */
		LCDLineAddress4Col16  = 0xD0 /**< Line 4 16x04 untested, no part */
	}; 

	enum LCDCmdBytesGeneral_e : uint8_t {
		LCDModeFourBit = 0x28, /**< Function set (4-bit interface, 2 lines, 5*7 Pixels) */
		LCDHomePosition  = 0x02, /**< Home (move cursor to top/left character position) */
		LCDDisplayOn = 0x0C,  /**< Restore the display (with cursor hidden) */
		LCDDisplayOff = 0x08, /**< Blank the display (without clearing) */
		LCDClearScreen = 0x01 /**< clear screen */
	};

	enum  LCDBackLight_e _LCDBackLight= LCDBackLightOnMask;  /**< Enum to store backlight status*/
	
	uint8_t _LCDSlaveAddresI2C = 0x27 ; /**< I2C  address for I2C module PCF8574 backpack on LCD*/
	uint8_t _NumRowsLCD = 2; /**< Number of rows on LCD */
	uint8_t _NumColsLCD = 16; /**< Number of Cols on LCD */
	TwoWire *wire;  /**< I2C wire interface */

	// private methods
	void send_cmd(unsigned char cmd);
	void send_data(unsigned char data);
	bool init_lcd_i2c(void);
	
};

#endif // guard header ending
