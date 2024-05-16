#include <avr/io.h>
#include <Servo.h>

#define PIN_LED_3                       2
#define PIN_LED_1                       3
#define PIN_LED_2                       4
#define PIN_TABLE_HOMING_LIMIT          5
#define PIN_BOTTLE_SERVO_1              6
#define PIN_BOTTLE_SERVO_2              7 
#define PIN_BOTTLE_LIMIT_1              8
#define PIN_BOTTLE_LIMIT_2              9
#define PIN_TABLE_DIR                   10
#define PIN_TABLE_PUL                   11
#define PIN_PILL_DIR                    12
#define PIN_PILL_PUL                    13
#define PIN_SWITCH_1                    52
#define PIN_SWITCH_2                    53

#define DATA_RATE_DEBUG                 115200
#define DATA_RATE_COMM                  9600

#define TABLE_SLOTS                     6
#define TABLE_GEAR_RATIO                4
#define TABLE_STEPS_PER_ROT             1600
#define TABLE_HOME_DELAY                1500
#define TABLE_MOVE_DELAY                400
#define TABLE_MOVE_ONE_STEPS            ((TABLE_STEPS_PER_ROT * TABLE_GEAR_RATIO) / TABLE_SLOTS)

#define PILL_SLOTS                      20
#define PILL_STEPS_PER_ROT              400
#define PILL_PAUSE_US                   50000
#define PILL_STEP_RATE                  600
#define PILL_STATE_PAUSING              0
#define PILL_STATE_FILLING              1
#define PILL_ONE_STEPS                  (PILL_STEPS_PER_ROT / PILL_SLOTS)

#define BOTTLE_TIME_US                  1000000
#define BOTTLE_CW_US                    600
#define BOTTLE_CCW_US                   2400

#define N_BOTTLES                       12
#define N_PILLS                         30

#define SLOT_BOTTLE                     0
#define SLOT_PILLS                      2
#define SLOT_CAPS                       3
#define SLOT_TWIST                      4
#define SLOT_EJECT                      5

#define STATE_INIT                      0
#define STATE_PROCESSING_START          1
#define STATE_PROCESSING_RUN            2
#define STATE_MOVING                    3
#define STATE_CLEARING                  4
#define STATE_COMPLETE                  5
#define STATE_MOVE_COMPLETE             6

byte slots[TABLE_SLOTS];
byte slots_running[TABLE_SLOTS];
byte machine_state;

byte pill_state;

uint8_t bottles_remaining;
uint8_t pills_remaining;

uint16_t sr_table;
uint16_t sr_pills;

uint32_t time_table_step;
uint32_t time_pill_step;
uint32_t time_bottle_pause;

bool table_pul_state;
bool pill_pul_state;
bool state_first_iter;
bool bottle_extending;

Servo bottle1;
Servo bottle2;

void set_pin_states()
{
        Serial.println("Setting I/O pin modes...");

        pinMode(PIN_TABLE_HOMING_LIMIT, INPUT_PULLUP);
        pinMode(PIN_TABLE_DIR, OUTPUT);
        pinMode(PIN_TABLE_PUL, OUTPUT);
        pinMode(PIN_PILL_DIR, OUTPUT);
        pinMode(PIN_PILL_PUL, OUTPUT);
        pinMode(PIN_BOTTLE_LIMIT_1, INPUT_PULLUP);
        pinMode(PIN_BOTTLE_LIMIT_2, INPUT_PULLUP);
        pinMode(PIN_SWITCH_1, INPUT_PULLUP);
        pinMode(PIN_SWITCH_2, INPUT_PULLUP);
        pinMode(PIN_LED_1, OUTPUT);
        pinMode(PIN_LED_2, OUTPUT);
        pinMode(PIN_LED_3, OUTPUT);

        digitalWrite(PIN_TABLE_DIR, HIGH);
        digitalWrite(PIN_LED_1, HIGH);
        digitalWrite(PIN_LED_2, HIGH);
        digitalWrite(PIN_LED_3, HIGH);

}

void init_servos()
{
        Serial.println("Configuring servos...");
        bottle1.attach(PIN_BOTTLE_SERVO_1);
        bottle2.attach(PIN_BOTTLE_SERVO_2);
        bottle1.writeMicroseconds(1500);
        bottle2.writeMicroseconds(1500);
}

void init_state_machine()
{
        Serial.println("Clearing state machine...");
        for(int i = 0; i < TABLE_SLOTS; i++)
        {
                slots[i] = false;
                slots_running[i] = false;
        }
        machine_state = STATE_COMPLETE;
        state_first_iter = true;
}

bool all_zero(byte* sarr)
{
        for(int i = 0; i < TABLE_SLOTS; i++)
                if(sarr[i] == true)
                        return false;

        return true;
}

void home_table()
{
        Serial.println("Homing rotary table...");
        while(digitalRead(PIN_TABLE_HOMING_LIMIT))
        {
                digitalWrite(PIN_TABLE_PUL, HIGH);  
                delayMicroseconds(TABLE_HOME_DELAY); 
                digitalWrite(PIN_TABLE_PUL, LOW);  
                delayMicroseconds(TABLE_HOME_DELAY);
        }
}

void clear_leds()
{
        digitalWrite(PIN_LED_1, LOW);
        digitalWrite(PIN_LED_2, LOW);
        digitalWrite(PIN_LED_3, LOW);
}

void state_transition(byte new_state)
{
        Serial.print("State transition: ");
        Serial.print(machine_state);
        Serial.print(" -> ");
        Serial.println(new_state);

        state_first_iter = true;
        machine_state = new_state;

        if(machine_state == STATE_MOVING || machine_state == STATE_CLEARING || machine_state == STATE_MOVE_COMPLETE)
        {
                digitalWrite(PIN_LED_1, HIGH);
                digitalWrite(PIN_LED_2, LOW);
                digitalWrite(PIN_LED_3, LOW);
        }
        else if(machine_state == STATE_PROCESSING_RUN || machine_state == STATE_PROCESSING_START)
        {
                digitalWrite(PIN_LED_1, LOW);
                digitalWrite(PIN_LED_2, HIGH);
                digitalWrite(PIN_LED_3, LOW);
        }
        else if(machine_state == STATE_INIT || machine_state == STATE_COMPLETE)
        {
                digitalWrite(PIN_LED_1, LOW);
                digitalWrite(PIN_LED_2, LOW);
                digitalWrite(PIN_LED_3, HIGH);
        }
}

#define __digitalPinToPortReg(P) \
(((P) >= 22 && (P) <= 29) ? &PORTA : \
((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &PORTB : \
(((P) >= 30 && (P) <= 37) ? &PORTC : \
((((P) >= 18 && (P) <= 21) || (P) == 38) ? &PORTD : \
((((P) <= 3) || (P) == 5) ? &PORTE : \
(((P) >= 54 && (P) <= 61) ? &PORTF : \
((((P) >= 39 && (P) <= 41) || (P) == 4) ? &PORTG : \
((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &PORTH : \
(((P) == 14 || (P) == 15) ? &PORTJ : \
(((P) >= 62 && (P) <= 69) ? &PORTK : &PORTL))))))))))

#define __digitalPinToBit(P) \
(((P) >=  7 && (P) <=  9) ? (P) - 3 : \
(((P) >= 10 && (P) <= 13) ? (P) - 6 : \
(((P) >= 22 && (P) <= 29) ? (P) - 22 : \
(((P) >= 30 && (P) <= 37) ? 37 - (P) : \
(((P) >= 39 && (P) <= 41) ? 41 - (P) : \
(((P) >= 42 && (P) <= 49) ? 49 - (P) : \
(((P) >= 50 && (P) <= 53) ? 53 - (P) : \
(((P) >= 54 && (P) <= 61) ? (P) - 54 : \
(((P) >= 62 && (P) <= 69) ? (P) - 62 : \
(((P) == 0 || (P) == 15 || (P) == 17 || (P) == 21) ? 0 : \
(((P) == 1 || (P) == 14 || (P) == 16 || (P) == 20) ? 1 : \
(((P) == 19) ? 2 : \
(((P) == 5 || (P) == 6 || (P) == 18) ? 3 : \
(((P) == 2) ? 4 : \
(((P) == 3 || (P) == 4) ? 5 : 7)))))))))))))))

inline void fast_toggle(int pin)
{
        uint8_t oldSREG = SREG;
        cli();
        *__digitalPinToPortReg(pin) ^= (1 << __digitalPinToBit(pin));
        SREG = oldSREG;
}

void setup()
{
        Serial.begin(DATA_RATE_DEBUG);
        Serial.println("Initializing...");
        
        set_pin_states();
        init_state_machine();
        init_servos();

        while(digitalRead(PIN_SWITCH_2));;

        clear_leds();
        home_table();

        delay(1000);
        Serial.println("Machine ready.");
        state_transition(STATE_COMPLETE);
}

void loop()
{
        // the current epoch time for synchronization.
        uint32_t time_cur = micros();

        if(!digitalRead(PIN_SWITCH_2))
        {
                state_transition(STATE_INIT);
                delay(15);
        }
        if(!digitalRead(PIN_SWITCH_1))
        {
                init_state_machine();
                state_transition(STATE_CLEARING);
                delay(15);
        }

        switch(machine_state)
        {
                case STATE_INIT:
                        bottles_remaining = N_BOTTLES;
                        state_transition(STATE_PROCESSING_START);
                        break;

                case STATE_PROCESSING_START:
                        // set up what we need to do for this step
                        if(bottles_remaining > 0)
                        {
                                // we need to dispense a bottle
                                slots_running[SLOT_BOTTLE] = true;
                                bottle_extending = true;
                        }
                        if(bottles_remaining == 0)
                        {
                                state_transition(STATE_CLEARING);
                                break;
                        }
                        if(slots[SLOT_PILLS] == true)
                        {
                                // we need to fill pills
                                slots_running[SLOT_PILLS] = true;
                                pills_remaining = N_PILLS;
                                pill_state = PILL_STATE_PAUSING;
                                time_pill_step = time_cur;
                        }
                        state_transition(STATE_PROCESSING_RUN);
                        break;
                
                case STATE_PROCESSING_RUN:
                        // We need to dispense bottles
                        if(slots_running[SLOT_BOTTLE] == true)
                        {       
                                // pick which side to dispense bottles from
                                bool side = (bottles_remaining > 6);

                                // select the appropriate servo and limit switch configuration for the chosen side
                                auto servo = side ? &bottle1 : &bottle2;
                                auto limit_pin = side ? PIN_BOTTLE_LIMIT_1 : PIN_BOTTLE_LIMIT_2;
                                auto extend_us = side ? BOTTLE_CW_US : BOTTLE_CCW_US;
                                auto retract_us = side ? BOTTLE_CCW_US : BOTTLE_CW_US;

                                if(state_first_iter)
                                {
                                        servo->write(extend_us);
                                        bottle_extending = true;
                                        time_bottle_pause = time_cur;
                                        state_first_iter = false;
                                }
                                if(bottle_extending && time_cur - time_bottle_pause > BOTTLE_TIME_US)
                                {
                                        servo->write(retract_us);

                                        bottle_extending = false;
                                }
                                if(!bottle_extending && !digitalRead(limit_pin))
                                {
                                        servo->write(1500);
                                        slots_running[SLOT_BOTTLE] = false;
                                        bottles_remaining -= 1;
                                        slots[SLOT_BOTTLE] = 1;
                                        bottle_extending = true;
                                        Serial.println("Bottle complete.");
                                }
                        }

                        // We need to fill pills
                        if(slots_running[SLOT_PILLS] == true)
                        {
                                if(pills_remaining > 0)
                                {
                                        if(pill_state == PILL_STATE_PAUSING && time_cur - time_pill_step > PILL_PAUSE_US)
                                        {
                                                sr_pills = PILL_ONE_STEPS;
                                                pill_state = PILL_STATE_FILLING;
                                        }
                                        if(pill_state == PILL_STATE_FILLING && sr_pills > 0 && time_cur - time_pill_step > PILL_STEP_RATE)
                                        {
                                                digitalWrite(PIN_PILL_PUL, !pill_pul_state);
                                                pill_pul_state = !pill_pul_state;
                                                time_pill_step = time_cur;

                                                if(!pill_pul_state)
                                                        sr_pills -= 1;

                                                if(sr_pills == 0)
                                                {
                                                        pills_remaining -= 1;
                                                        pill_state = PILL_STATE_PAUSING;
                                                }
                                        }
                                }
                                if(pills_remaining == 0)
                                {
                                        slots_running[SLOT_PILLS] = false;
                                        Serial.println("Pills finished...");
                                }
                        }

                        // Once all tasks are done, move the table.
                        if(all_zero(slots_running))
                                state_transition(STATE_MOVING);
                        break;

                case STATE_MOVING:
                        if(state_first_iter == true)
                        {
                                Serial.println("Move start.");
                                sr_table = TABLE_MOVE_ONE_STEPS;
                                state_first_iter = false;

                                for(int j = TABLE_SLOTS - 1; j > 0; j--)
                                        slots[j] = slots[j - 1];
                                slots[SLOT_BOTTLE] = 0;
                        }
                        if(sr_table > 0 && (time_cur - time_table_step) > TABLE_MOVE_DELAY)
                        {
                                // we are due for a step of the table motor
                                digitalWrite(PIN_TABLE_PUL, !table_pul_state);
                                table_pul_state = !table_pul_state;
                                time_table_step = time_cur;
                                if(!table_pul_state)
                                        sr_table -= 1;
                        }
                        if(sr_table == 0)
                        {
                                Serial.println("Move complete.");
                                state_transition(STATE_PROCESSING_START);
                        }
                        break;

                case STATE_CLEARING:
                        if(!digitalRead(PIN_BOTTLE_LIMIT_1) || !digitalRead(PIN_BOTTLE_LIMIT_2))
                        {
                                bottle1.write(600);
                                bottle2.write(600);
                        }
                        else
                        {
                                bottle1.write(1500);
                                bottle2.write(1500);
                                state_transition(STATE_COMPLETE);
                        }
                        break;

                case STATE_COMPLETE:
                        
                        break;

                default:
                        Serial.println("State machine reached invalid state, stopping!");
                        while(true) ;;
                        break;
        }
}