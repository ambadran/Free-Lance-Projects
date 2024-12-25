#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#define ULTRASONIC_COUNTER_TO_CM  233 //TODO: actually calculate and get this value
#define ULTRASONIC_TRIGGER_ON_TIME_us 10  // 10us TODO: is this really 10us ?!

#ifdef ULTRASONIC_STATE_MACHINE
#define ECHO_RISE_TIMEOUT_us 30 // 30us wait until echo rise
#define ECHO_FALL_TIMEOUT_us 30000 // 30ms
#define PERIOD_us_TO_FREQ_INT(us) ((us) != 0 ? (1000000 + (us) / 2) / (us) : 0)
                                         

/* this status enum will be used to return whether a working ultrasonic is connected or not */
typedef enum {
  ULTRASONIC_FIRST_RUN,
  ULTRASONIC_ACTIVE,
  ULTRASONIC_IDLE,
  ULTRASONIC_UNRESPONSIVE
} ULTRASONIC_STATUS;
extern const char* ULTRASONIC_STATUS_TO_STRING[];

typedef enum {
  ULTRASONIC_SEND_TRIGGER_PHASE,
  ULTRASONIC_AWAIT_TRIGGER_PHASE,
  ULTRASONIC_TRIGGER_SENT_PHASE,
  ULTRASONIC_AWAIT_ECHO_RISE_PHASE,
  ULTRASONIC_ECHO_RISE_CAPTURED_PHASE,
  ULTRASONIC_AWAIT_ECHO_FALL_PHASE,
  ULTRASONIC_ECHO_FALL_CAPTURED_PHASE, 
  ULTRASONIC_ECHO_TIMEOUT_PHASE
} ULTRASONIC_PHASE;
extern const char* ULTRASONIC_PHASE_TO_STRING[];

void ultrasonic_init(void);
void ultrasonic_start_sequence(void); // sets global flag
void ultrasonic_stop_sequence(void);  // resets _state and clear global flag
void static ultrasonic_send_trigger(void);
void static ultrasonic_await_echo_rise(void);
void static ultrasonic_await_echo_fall(void);
void processs_ultrasonic_phases(void);
ULTRASONIC_STATUS ultrasonic_get_distance(uint16_t* distance);

#else

void ultrasonic_init(void);
void ultrasonic_get_distance(uint16_t* distance);
#endif

#endif
