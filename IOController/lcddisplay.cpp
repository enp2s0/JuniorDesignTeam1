#include "definitions.h"
#include "lcddisplay.h"
#include "I2C_LCD.h"

I2C_LCD lcd(LCD_ROWS, LCD_COLS, LCD_I2C_ADDRESS, &Wire);

void lcd_init()
{
        Serial.println("Initializing LCD display...");

        pinMode(PIN_WIRE_SDA, INPUT_PULLUP);
	pinMode(PIN_WIRE_SCL, INPUT_PULLUP);

        lcd.init(lcd.LCDCursorTypeOn);
        lcd.clear();
        lcd.set_backlight(true);

	lcd.clear();
	lcd.go_to(0, 0);
}

void lcd_statusmsg(char* line1, char* line2)
{
        lcd.clear();
	lcd.go_to(0, 0);
	lcd.send_string(line1);
	lcd.go_to(1, 0);
	lcd.send_string(line2);
}