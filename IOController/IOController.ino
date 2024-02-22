#include "definitions.h"
#include "commbus.h"
#include "lcddisplay.h"

void setup()
{
        Serial.begin(115200);
	Serial.println("Initializing...");

	lcd_init();
	comm_init();

	lcd_statusmsg("System ready!", "");
	Serial.println("Ready.");
}

void loop() {
	
}
