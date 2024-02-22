#include "definitions.h"

#define B_EXTEND_DELAY          1500
#define B_RETRACT_DELAY         1700
#define B_PAUSE                 300

void bottle_init()
{
        Serial.println("Initializing bottle filler...");

        pinMode(BOTTLE_1, OUTPUT);
        pinMode(BOTTLE_2, OUTPUT);

        digitalWrite(BOTTLE_1, LOW);
        digitalWrite(BOTTLE_2, LOW);
}

void bottle_extend()
{
        digitalWrite(BOTTLE_1, LOW);
        digitalWrite(BOTTLE_2, HIGH);
}

void bottle_retract()
{
        digitalWrite(BOTTLE_1, HIGH);
        digitalWrite(BOTTLE_2, LOW);
}

void bottle_stop()
{
        digitalWrite(BOTTLE_1, LOW);
        digitalWrite(BOTTLE_2, LOW);
}

void bottle_dispense()
{
	Serial.println("Dispensing...");
	int start_time = millis();

        digitalWrite(LED_RED, HIGH);
        bottle_extend();
        delay(B_EXTEND_DELAY);
        bottle_stop();
        digitalWrite(LED_RED, LOW);

        delay(B_PAUSE);

        digitalWrite(LED_RED, HIGH);
        bottle_retract();
        delay(B_RETRACT_DELAY);
        bottle_stop();
        digitalWrite(LED_RED, LOW);

        int end_time = millis();
	Serial.print("Took "); Serial.print(end_time - start_time); Serial.println("ms.");
}