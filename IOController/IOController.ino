#include "definitions.h"
#include "commbus.h"
#include "lcddisplay.h"

void setup()
{
        Serial.begin(115200);
	Serial.println("Initializing...");

	lcd_init();
	comm_init();
	comm_link_establish();

	lcd_statusmsg("Initializing...", "Complete!");
	Serial.println("Ready.");
}

void loop()
{	
}
