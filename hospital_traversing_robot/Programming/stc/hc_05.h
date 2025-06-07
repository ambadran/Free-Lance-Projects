#ifndef HC_05_H
#define HC_05_H

// Time the trigger pin is ON for
#define TRIGGER_ON_TIME_us 10  
// Timeout for echo pin to rise after trigger
#define ECHO_RISE_TIMEOUT_FREQ 33333 // 30us
// Timeout for echo pin to fall (echo time)
#define ECHO_FALL_TIMEOUT_FREQ 33 // 30ms

/* This is the state machine states definition
 * to track the working status of the sensor
 */
typedef enum {
  HC05_FIRST_RUN,
  HC05_ACTIVE,
  HC05_IDLE,
  HC05_UNRESPONSIVE
} HC05_STATUS;
extern const char* HC05_STATUS_TO_STRING[];

/* This is the state machine states definition
 * to track the reading cycle.
 */
typedef enum {
  HC05_SEND_TRIGGER_PHASE,
  HC05_AWAIT_TRIGGER_PHASE,
  HC05_TRIGGER_SENT_PHASE,
  HC05_AWAIT_ECHO_RISE_PHASE,
  HC05_ECHO_RISE_CAPTURED_PHASE,
  HC05_AWAIT_ECHO_FALL_PHASE,
  HC05_ECHO_FALL_CAPTURED_PHASE, 
  HC05_ECHO_TIMEOUT_PHASE
} HC05_PHASE;

typedef enum {
  HC05_START_CYCLE_ALREADY_ACTIVE,
  HC05_START_CYCLE_OK,
  HC05_STOP_CYCLE_OK,
  HC05_STOP_CYCLE_ALREADY_IDLE
} HC05_CYCLE_FUNC_RESPONSE;

void hc05_init(void);
HC05_CYCLE_FUNC_RESPONSE hc05_start_cycle(void);
HC05_CYCLE_FUNC_RESPONSE hc05_stop_cycle(void);
void hc05_process_cycle_phases(void);
HC05_STATUS get_hc05_status(void);
uint16_t get_ultrasonic_distance_cm(void);
static void hc05_send_trigger(void);
static void hc05_await_echo_rise(void);
static void hc05_await_echo_fall(void);


#endif
