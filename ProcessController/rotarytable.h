#include "definitions.h"

#define R_SOFT_START            1000
#define R_STEP_DELAY	        300
#define R_RAMP_TIME	        500
#define R_STEPS_PER_ROT         400
#define R_TABLE_SLOTS           6
#define R_TABLE_ROT_DELAY       800
#define R_TABLE_SPIN_RATE       500
#define R_SOFT_DUR              3
#define R_GEAR_RATIO            3

void table_init()
{
        Serial.println("Initializing rotary table...");

        pinMode(TABLE_DIR, OUTPUT);
        pinMode(TABLE_PUL, OUTPUT);

        digitalWrite(TABLE_DIR, LOW);
}

void table_rotate(int cnt)
{
	Serial.println("Rotating...");
	int start_time = millis();

        int total_steps = R_STEPS_PER_ROT / R_TABLE_SLOTS;
        total_steps *= R_GEAR_RATIO;

        digitalWrite(LED_RED, HIGH);
        for(int i = 0; i < total_steps; i++)
        {
                int d = R_TABLE_ROT_DELAY;
                if(d < R_SOFT_DUR || d > R_TABLE_ROT_DELAY - R_SOFT_DUR)
                        d /= 2;

                digitalWrite(TABLE_PUL, HIGH);  
                delayMicroseconds(R_TABLE_ROT_DELAY); 
                digitalWrite(TABLE_PUL, LOW);  
                delayMicroseconds(R_TABLE_ROT_DELAY);
        }
        digitalWrite(LED_RED, LOW);

	int end_time = millis();
	Serial.print("Took "); Serial.print(end_time - start_time); Serial.println("ms.");
}