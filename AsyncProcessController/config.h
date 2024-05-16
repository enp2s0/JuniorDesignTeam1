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