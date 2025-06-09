#ifndef HC_SR04_H
#define HC_SR04_H

// Time the trigger pin is ON for
#define TRIGGER_ON_TIME_FREQ 100000 // 10us
// Timeout for echo pin to rise after trigger
#define ECHO_RISE_TIMEOUT_FREQ 33333 // 30us
// Timeout for echo pin to fall (echo time)
#define ECHO_FALL_TIMEOUT_FREQ 33 // 30ms

/* This is the state machine states definition
 * to track the working status of the sensor
 */
typedef enum {
  HCSR04_IDLE,
  HCSR04_FIRST_RUN,
  HCSR04_ACTIVE,
  HCSR04_UNRESPONSIVE
} HCSR04_STATUS;
extern const char* HCSR04_STATUS_TO_STRING[];

/* This is the state machine states definition
 * to track the reading cycle.
 */
typedef enum {
  HCSR04_SEND_TRIGGER_PHASE,
  HCSR04_TRIGGER_SENT_PHASE,
  HCSR04_AWAIT_ECHO_RISE_PHASE,
  HCSR04_ECHO_RISE_CAPTURED_PHASE,
  HCSR04_AWAIT_ECHO_FALL_PHASE,
  HCSR04_ECHO_FALL_CAPTURED_PHASE, 
  HCSR04_ECHO_TIMEOUT_PHASE
} HCSR04_PHASE;
extern const char* HCSR04_PHASE_TO_STRING[];

typedef enum {
  HCSR04_START_CYCLE_ALREADY_ACTIVE,
  HCSR04_START_CYCLE_OK,
  HCSR04_STOP_CYCLE_OK,
  HCSR04_STOP_CYCLE_ALREADY_IDLE
} HCSR04_CYCLE_FUNC_RESPONSE;

void hcsr04_init(void);
HCSR04_CYCLE_FUNC_RESPONSE hcsr04_start_cycle(void);
HCSR04_CYCLE_FUNC_RESPONSE hcsr04_stop_cycle(void);
void hcsr04_process_cycle_phases(void);
HCSR04_STATUS get_hcsr04_status(void);
HCSR04_PHASE get_hcsr04_phase(void);
uint16_t get_ultrasonic_distance_cm(void);
static void hcsr04_send_trigger(void);
static void hcsr04_await_echo_rise(void);
static void hcsr04_await_echo_fall(void);


#endif
