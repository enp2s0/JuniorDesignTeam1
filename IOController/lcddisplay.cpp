#include "definitions.h"
#include "lcddisplay.h"
#include "HD44780_LCD_PCF8574.h"

HD44780LCD lcd(LCD_ROWS, LCD_COLS, LCD_I2C_ADDRESS, &Wire);

void lcd_init()
{
        Serial.println("Initializing LCD display...");

        pinMode(PIN_WIRE_SDA, INPUT_PULLUP);
	pinMode(PIN_WIRE_SCL, INPUT_PULLUP);

        lcd.PCF8574_LCDInit(lcd.LCDCursorTypeOn);
        lcd.PCF8574_LCDClearScreen();
        lcd.PCF8574_LCDBackLightSet(true);

	lcd.PCF8574_LCDClearScreen();
	lcd.PCF8574_LCDGOTO(lcd.LCDLineNumberOne, 0);
	lcd.PCF8574_LCDSendString("Initializing... ");
}

void lcd_statusmsg(char* line1, char* line2)
{
        lcd.PCF8574_LCDClearScreen();
	lcd.PCF8574_LCDGOTO(lcd.LCDLineNumberOne, 0);
	lcd.PCF8574_LCDSendString(line1);
	lcd.PCF8574_LCDGOTO(lcd.LCDLineNumberTwo, 1);
	lcd.PCF8574_LCDSendString(line2);
}