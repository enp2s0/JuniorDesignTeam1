#include "definitions.h"

#include "pilldispenser.h"
#include "rotarytable.h"
#include "bottledispenser.h"

void notready()
{
	digitalWrite(LED_GREEN, LOW);
}

void ready()
{
	digitalWrite(LED_GREEN, HIGH);
}

void error()
{
	while(1)
	{
		digitalWrite(LED_RED, HIGH);
		digitalWrite(LED_GREEN, HIGH);
		delay(250);
		digitalWrite(LED_RED, LOW);
		digitalWrite(LED_GREEN, LOW);
		delay(250);
	}
}

void setup()
{
	digitalWrite(PANEL_GND, LOW);
	Serial.begin(115200);
	Serial.println("Initializing...");

	pinMode(BUTTON_1, INPUT_PULLUP);
	pinMode(BUTTON_2, INPUT_PULLUP);
	pinMode(BUTTON_3, INPUT_PULLUP);
	pinMode(BUTTON_4, INPUT_PULLUP);
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_RED, OUTPUT);

	Serial.println("Ready.");
	digitalWrite(LED_RED, LOW);
}

void loop()
{
	ready();
	if(!digitalRead(BUTTON_1))
	{
		notready();
		pill_dispense(30);
	}
	if(!digitalRead(BUTTON_2))
	{
		notready();
		table_rotate(1);
	}
	if(!digitalRead(BUTTON_3))
	{
		notready();
		bottle_dispense();
	}
}
