#include "project-defs.h"

/* hc05 Status State machine State Variable*/
static volatile HC05_STATUS current_status = HC05_IDLE;
/* hc05 Phases State machine State Variable*/
static volatile HC05_PHASE current_phase = HC05_SEND_TRIGGER_PHASE;
/* Echo time in timer ticks -> the latest result */
static volatile uint16_t hc05_timer_echo_ticks = 0;

/* Trigger Pin */
static GpioConfig trigger_pin = GPIO_PIN_CONFIG(HC05_TRIGGER_PORT, HC05_TRIGGER_PIN, GPIO_BIDIRECTIONAL_MODE); 

/* Echo Pin */
static GpioConfig echo_pin = { 
  .port = HC05_ECHO_PORT, 
  .pin = HC05_ECHO_PIN, 
  .count = 1, 
  .pinMode = GPIO_BIDIRECTIONAL_MODE,	
  .pinInterrupt = ENABLE_GPIO_PIN_INTERRUPT, 
  .interruptTrigger = GPIO_RISING_EDGE, 
  .wakeUpInterrupt = DISABLE_GPIO_PIN_WAKE_UP, \
	DEFAULTS_PU_NCS  DEFAULTS_SR_DR_IE  };

/* String mapping for user printing */
const char* HC05_STATUS_TO_STRING[] = {
  "HC05_IDLE",
  "HC05_ACTIVE",
  "HC05_FIRST_RUN",
  "HC05_UNRESPONSIVE" 
};

void hc05_init(void) {
  // Setting trigger pin GpioConfig
  gpioConfigure(&trigger_pin);
  gpioWrite(&trigger_pin, 0);

  // Setting echo pin GpioConfig
  gpioConfigure(&echo_pin);

}

HC05_START_CYCLE_FUNC_RESPONSE hc05_start_cycle(void) {

  // in any case, enable IRQ now
  echo_pin.pinInterrupt = ENABLE_GPIO_PIN_INTERRUPT;
  gpioConfigure(&echo_pin);

  switch(current_status) {

    case HC05_IDLE:
    case HC05_UNRESPONSIVE:
      current_status = HC05_FIRST_RUN;
      current_phase = HC05_SEND_TRIGGER_PHASE;
      return HC05_START_CYCLE_OK;

    case HC05_ACTIVE:
    case HC05_FIRST_RUN:
      return HC05_START_CYCLE_ALREADY_ACTIVE;
  }

}

HC05_CYCLE_FUNC_RESPONSE hc05_stop_cycle(void) {

  if(current_status != HC05_IDLE) {

    echo_pin.pinInterrupt = DISABLE_GPIO_PIN_INTERRUPT;
    gpioConfigure(&echo_pin); // disable IRQ, for good measure
    stopTimer(HC05_TIMER_INTERRUPT); // disable timer, for good measure

    current_status = HC05_IDLE;
    current_phase = HC05_SEND_TRIGGER_PHASE; // first phase

    return HC05_STOP_CYCLE_OK;

  } else { return HC05_STOP_CYCLE_ALREADY_IDLE; }

}

void hc05_process_cycle_phases(void) {
  switch (hc05_status) {

    case HC05_ACTIVE:
    case HC05_FIRST_RUN:
    /* printf("Current HC05 status: %s\n", HC05_STATUS_TO_STRING[HC05_status]); */
    /* printf("Current HC05 Phase: %s\n", HC05_PHASE_TO_STRING[HC05_current_phase]); */
    switch (current_phase) {

      case HC05_SEND_TRIGGER_PHASE:
        HC05_send_trigger();
        /* printf("HC05 status: %d\n", HC05_status); */
        /* printf("HC05 Phase: %d\n", HC05_current_phase); */

        break;

      case HC05_TRIGGER_SENT_PHASE:
        HC05_await_echo_rise();
        break;

      case HC05_ECHO_RISE_CAPTURED_PHASE:
        HC05_await_echo_fall();
        break;

      case HC05_ECHO_FALL_CAPTURED_PHASE:
      case HC05_ECHO_TIMEOUT_PHASE:
        HC05_start_sequence();
        break;

      default:
        while(1) {report("WTF?!? State Machine main process func has un-handled situation!!!!\r");}
    }
    break;

    case HC05_UNRESPONSIVE:
    report("HC05 Sensor UNRESPONSIVE!!! Restarting Cycle.."); // notify user
      hc05_stop_cycle();
      hc05_start_cycle();
      break;

    case HC05_IDLE:
    break;

  }

}

HC05_STATUS get_hc05_status(void) { return current_status; }

uint16_t get_ultrasonic_distance_cm(void) { return hc05_timer_echo_ticks*HC05_TIMER_COUNTER_TO_CM; }

static void hc05_send_trigger(void) {

  gpioWrite(&trigger_pin, 1);

  startTimer(
      HC05_TIMER, 
      frequencyToSysclkDivisor(PERIOD_us_TO_FREQ_INT(ULTRASONIC_TRIGGER_ON_TIME_us)),
      DISABLE_OUTPUT, 
      ENABLE_INTERRUPT, 
      FREE_RUNNING
      );

  current_phase = HC05_AWAIT_TRIGGER_PHASE;

}

static void hc05_await_echo_rise(void) {

  // Time-out timer if no echo rise detected
  //TODO: make custom startTimer that takes into account that TMRxON = 1; is already called in the interrupt, here is just the rest of the setup
  startTimer(
      HC05_TIMER, 
      frequencyToSysclkDivisor(ECHO_RISE_TIMEOUT_FREQ),
      DISABLE_OUTPUT, 
      ENABLE_INTERRUPT, 
      FREE_RUNNING
      );

  // changing to interrupt on rising edge if it was changed by a previous phase
  echo_pin.interruptTrigger = GPIO_RISING_EDGE; 
  gpioConfigure(&echo_pin);

  current_phase = HC05_AWAIT_ECHO_RISE_PHASE;

}

static void hc05_await_echo_fall(void) {

  // Time-out timer if no echo rise detected
  startTimer(
      HC05_TIMER, 
      frequencyToSysclkDivisor(ECHO_FALL_TIMEOUT_FREQ),
      DISABLE_OUTPUT, 
      ENABLE_INTERRUPT, 
      FREE_RUNNING
      );

  // changing to interrupt on falling edge if it was changed by a previous phase
  echo_pin.interruptTrigger = GPIO_FALLING_EDGE; 
  gpioConfigure(&echo_pin);

  current_phase = HC05_AWAIT_ECHO_FALL_PHASE;

}

INTERRUPT(HC05_TIMER_ISR, HC05_TIMER_INTERRUPT) {
  /* uartGetCharacter(CONSOLE_UART, 'E', NON_BLOCKING); */

  switch (current_phase) {

    case HC05_AWAIT_TRIGGER_PHASE:
      gpioWrite(&trigger_pin, 0); //TODO: close actual port register now then call function in main loop later
      current_phase = HC05_TRIGGER_SENT_PHASE;
      break;

    case HC05_AWAIT_ECHO_RISE_PHASE:
      // time out error, HC05_UNRESPONSIVE !!
      current_status = HC05_UNRESPONSIVE;  // update state
      current_phase = HC05_ECHO_TIMEOUT_PHASE;  // restarting cycle
      break;

    case HC05_AWAIT_ECHO_FALL_PHASE:
      // time out error, HC05_UNRESPONSIVE !!
      current_status = HC05_UNRESPONSIVE;  // update state
      current_phase = HC05_ECHO_TIMEOUT_PHASE;  // restarting cycle
      break;

  }

}

INTERRUPT(HC05_INT_PIN_ISR, HC05_INT_PIN_INTERRUPT) {
  /* uartGetCharacter(CONSOLE_UART, 'Q', NON_BLOCKING); */

  switch (current_phase) {

    case HC05_AWAIT_ECHO_RISE_PHASE:
      //TODO: start already set timer like TMR1ON = 1
      current_phase = HC05_ECHO_RISE_CAPTURED_PHASE;
      break;

    case HC05_AWAIT_ECHO_FALL_PHASE:
      // GOTCHA BITCH :D
      hc05_timer_echo_ticks = stopTimer(HC05_TIMER); //TODO assign actual timer register now then call this later in main loop
      current_phase = HC05_ECHO_FALL_CAPTURED_PHASE;
      current_status = HC05_ACTIVE;
      break;

  }

}
