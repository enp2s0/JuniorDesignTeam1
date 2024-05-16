#include "definitions.h"

#define R_SOFT_START            1000
#define R_STEP_DELAY	        300
#define R_RAMP_TIME	        500
#define R_STEPS_PER_ROT         1600
#define R_TABLE_SLOTS           6
#define R_TABLE_ROT_DELAY       400
#define R_TABLE_NUDGE_DELAY     1500
#define R_TABLE_INIT_SPEED      1000
#define R_SOFT_DUR              250
#define R_GEAR_RATIO            4

void table_init()
{
        Serial.println("Initializing rotary table...");

        pinMode(TABLE_DIR, OUTPUT);
        pinMode(TABLE_PUL, OUTPUT);
        pinMode(PIN_TABLE_LIMIT, INPUT_PULLUP);

        digitalWrite(TABLE_DIR, HIGH);

        Serial.println("Homing rotary table...");
        table_home();
}

void table_rotate(int cnt)
{
	Serial.print("Rotating "); Serial.print(cnt); Serial.println(" times...");
	int start_time = millis();

        unsigned long total_steps = (R_STEPS_PER_ROT * R_GEAR_RATIO) / R_TABLE_SLOTS;
        total_steps *= cnt;
        for(unsigned long i = 0; i < total_steps; i++)
        {
                int d = R_TABLE_ROT_DELAY;
                if(i < R_SOFT_DUR || i > total_steps - R_SOFT_DUR)
                {
                        int stage;
                        if(i < R_SOFT_DUR)               stage = i;
                        if(i > total_steps - R_SOFT_DUR) stage = (total_steps - i);

                        d = map(stage, 0, R_SOFT_DUR, R_TABLE_INIT_SPEED, R_TABLE_ROT_DELAY);
                }

                digitalWrite(TABLE_PUL, HIGH);  
                delayMicroseconds(d); 
                digitalWrite(TABLE_PUL, LOW);  
                delayMicroseconds(d);
        }
        digitalWrite(LED_RED, LOW);

	int end_time = millis();
	Serial.print("Took "); Serial.print(end_time - start_time); Serial.println("ms.");
}

void table_nudge(int cnt)
{
	Serial.println("Nudging...");
	int start_time = millis();

        digitalWrite(LED_RED, HIGH);
        for(int i = 0; i < cnt; i++)
        {

                digitalWrite(TABLE_PUL, HIGH);  
                delayMicroseconds(R_TABLE_NUDGE_DELAY); 
                digitalWrite(TABLE_PUL, LOW);  
                delayMicroseconds(R_TABLE_NUDGE_DELAY);
        }
        digitalWrite(LED_RED, LOW);

	int end_time = millis();
	Serial.print("Took "); Serial.print(end_time - start_time); Serial.println("ms.");
}

void table_home()
{
        while(digitalRead(PIN_TABLE_LIMIT))
                table_nudge(1);
}