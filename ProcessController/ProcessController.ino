#include "definitions.h"

#include "pilldispenser.h"
#include "rotarytable.h"
#include "bottledispenser.h"

String cmd_str;

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

	table_init();
	pill_init();
	bottle_init();

	Serial.println("Ready.");
	ready();
}

void loop()
{
	while(!Serial.available());;

	auto new_cmd = Serial.readStringUntil('\n');
	notready();

	if(new_cmd == "") { Serial.print("> REPEAT: "); Serial.println(cmd_str); }
	else { cmd_str = new_cmd; }

	for(int i = 0; i < cmd_str.length(); i++)
	{
		char cmd = cmd_str.charAt(i);
		Serial.print("> CMD #"); Serial.print(i); Serial.print(": "); Serial.println(cmd);
		do_command(cmd);
	}

	ready();
}

void do_command(char ch)
{
	switch(ch)
	{
		case 'p':
			pill_dispense(30);
			break;
		case 't':
			table_rotate(1);
			break;
		case 'r':
			table_rotate(6);
			break;
		case 'n':
			table_nudge(30);
			break;
		case 'b':
			bottle_dispense();
			break;
		default:
			Serial.print("Invalid command byte: ");
			Serial.println(ch);
			break;
	}
}
