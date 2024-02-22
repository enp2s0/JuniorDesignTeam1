#include "definitions.h"

#define P_SOFT_START            1000
#define P_STEP_DELAY	        600
#define P_RAMP_TIME	        500
#define P_STEPS_PER_ROT         400
#define P_PILL_SLOTS            20
#define P_PILL_FILL_DELAY       600
#define P_DISPENSE_RATE         40
#define P_DISPENSE_COUNT        30
#define P_SOFT_DUR              3  

void pill_init()
{
        Serial.println("Initialzing pill dispenser...");

        pinMode(PILL_DIR, OUTPUT);
        pinMode(PILL_PUL, OUTPUT);

        digitalWrite(PILL_DIR, LOW);
}

void pill_dispense(int cnt)
{
	Serial.println("Filling...");
	int start_time = millis();

	digitalWrite(PILL_DIR, LOW);
	int total_steps = P_STEPS_PER_ROT / P_PILL_SLOTS;

	for(int i = 0; i < cnt; i++)
	{
		digitalWrite(LED_RED, HIGH);
		for(int i = 0; i < total_steps; i++)
		{
			int d = P_PILL_FILL_DELAY;
			if(d < P_SOFT_DUR || d > P_PILL_FILL_DELAY - P_SOFT_DUR)
				d /= 2;

			digitalWrite(PILL_PUL, HIGH);  
			delayMicroseconds(P_PILL_FILL_DELAY); 
			digitalWrite(PILL_PUL, LOW);  
			delayMicroseconds(P_PILL_FILL_DELAY);
		}
		digitalWrite(LED_RED, LOW);
		delay(P_DISPENSE_RATE);
	}

	int end_time = millis();
	Serial.print("Took "); Serial.print(end_time - start_time); Serial.println("ms.");
}