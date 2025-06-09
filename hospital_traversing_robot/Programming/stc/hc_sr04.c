#include "project-defs.h"

/* hcsr04 Status State machine State Variable*/
static volatile HCSR04_STATUS current_status = HCSR04_IDLE;
/* hcsr04 Phases State machine State Variable*/
static volatile HCSR04_PHASE current_phase = HCSR04_SEND_TRIGGER_PHASE;
/* Echo time in timer ticks -> the latest result */
static volatile uint16_t hcsr04_timer_echo_ticks = 0;
/* Ultrasonic Reading Frequency Counter Variable*/
static volatile uint32_t ultrasonic_reading_freq_counter_ms = 0;
/* Ultrasonic Echo Pin Rise timeout counter */
static uint32_t echo_rise_timeout_counter = 0;

/* Trigger Pin */
static GpioConfig trigger_pin = GPIO_PIN_CONFIG(HCSR04_TRIGGER_PORT, HCSR04_TRIGGER_PIN, GPIO_PUSH_PULL_MODE); 

/* Echo Pin */
static GpioConfig echo_pin = { 
  .port = HCSR04_ECHO_PORT, 
  .pin = HCSR04_ECHO_PIN, 
  .count = 1, 
  .pinMode = GPIO_HIGH_IMPEDANCE_MODE,	
  .pinInterrupt = DISABLE_GPIO_PIN_INTERRUPT, 
  /* .pinInterrupt = ENABLE_GPIO_PIN_INTERRUPT, */ 
  .interruptTrigger = GPIO_RISING_EDGE, 
  .wakeUpInterrupt = DISABLE_GPIO_PIN_WAKE_UP, \
	DEFAULTS_PU_NCS  DEFAULTS_SR_DR_IE  };

/* String mapping for user printing */
const char* HCSR04_STATUS_TO_STRING[] = {
  "HCSR04_IDLE",
  "HCSR04_FIRST_RUN",
  "HCSR04_ACTIVE",
  "HCSR04_UNRESPONSIVE" 
};

const char* HCSR04_PHASE_TO_STRING[] = {
  "HCSR04_SEND_TRIGGER_PHASE",
  "HCSR04_TRIGGER_SENT_PHASE",
  "HCSR04_AWAIT_ECHO_RISE_PHASE",
  "HCSR04_ECHO_RISE_CAPTURED_PHASE",
  "HCSR04_AWAIT_ECHO_FALL_PHASE",
  "HCSR04_ECHO_FALL_CAPTURED_PHASE", 
  "HCSR04_ECHO_TIMEOUT_PHASE" 
};

void hcsr04_init(void) {
  /* // Setting trigger pin GpioConfig */
  /* gpioConfigure(&trigger_pin); */
  /* gpioWrite(&trigger_pin, 0); */

  /* // Setting echo pin GpioConfig */
  /* gpioConfigure(&echo_pin); */

  ultrasonic_reading_freq_counter_ms = get_current_time();

}

HCSR04_CYCLE_FUNC_RESPONSE hcsr04_start_cycle(void) {

  HCSR04_CYCLE_FUNC_RESPONSE response = HCSR04_START_CYCLE_OK;

  switch(current_status) {

    case HCSR04_IDLE:
    case HCSR04_UNRESPONSIVE:
      current_status = HCSR04_FIRST_RUN;
      current_phase = HCSR04_SEND_TRIGGER_PHASE;
      response = HCSR04_START_CYCLE_OK;
      break;

    case HCSR04_ACTIVE:
    case HCSR04_FIRST_RUN:
      response = HCSR04_START_CYCLE_ALREADY_ACTIVE;
      break;
  }

  return response;
}

HCSR04_CYCLE_FUNC_RESPONSE hcsr04_stop_cycle(void) {
  HCSR04_CYCLE_FUNC_RESPONSE response = HCSR04_STOP_CYCLE_OK;
  switch (current_status) {
    case HCSR04_ACTIVE:
    case HCSR04_FIRST_RUN:
      echo_pin.pinInterrupt = DISABLE_GPIO_PIN_INTERRUPT;
      gpioConfigure(&echo_pin); // disable IRQ, for good measure
      /* stopTimer(HCSR04_TIMER_INTERRUPT); // disable timer, for good measure */

      current_status = HCSR04_IDLE;
      current_phase = HCSR04_SEND_TRIGGER_PHASE; // first phase
      response = HCSR04_STOP_CYCLE_OK;
      break;

    case HCSR04_UNRESPONSIVE:
      echo_pin.pinInterrupt = DISABLE_GPIO_PIN_INTERRUPT;
      gpioConfigure(&echo_pin); // disable IRQ, for good measure
      /* stopTimer(HCSR04_TIMER_INTERRUPT); // disable timer, for good measure */
      response = HCSR04_STOP_CYCLE_OK;
      break;

    case HCSR04_IDLE:
      response = HCSR04_STOP_CYCLE_ALREADY_IDLE;

  }
  return response;

}

void hcsr04_process_cycle_phases(void) {

  switch (current_status) {

    case HCSR04_ACTIVE:
    case HCSR04_FIRST_RUN: {

       printf("HCSR04 status: %s, Phase: %s\n", HCSR04_STATUS_TO_STRING[current_status], HCSR04_PHASE_TO_STRING[current_phase]);
      switch (current_phase) {

        case HCSR04_SEND_TRIGGER_PHASE:
          hcsr04_send_trigger();
          break;

        case HCSR04_TRIGGER_SENT_PHASE:
          hcsr04_await_echo_rise();
          break;

        case HCSR04_ECHO_RISE_CAPTURED_PHASE:
          hcsr04_await_echo_fall();
          break;

        case HCSR04_ECHO_FALL_CAPTURED_PHASE:
          printf("HORRAAAAAAYY!!!");
          // wait till next period ;)
        /*   if (get_current_time() >= ultrasonic_reading_freq_counter_ms) { */
/* #ifdef ULTRASONIC_DEBUG */
    /* printf("HCSR04 status: %s, Phase: %s\r    ", HCSR04_STATUS_TO_STRING[current_status], HCSR04_PHASE_TO_STRING[current_phase]); */
/* #endif */
        /*     ultrasonic_reading_freq_counter_ms += HCSR04_READ_PERIOD_MS; */
        /*     hcsr04_start_cycle(); */
        /*   } else { return; } */
        /*   break; */

        /* case HCSR04_ECHO_TIMEOUT_PHASE: */
        /*   hcsr04_start_cycle(); */
          break;

        case HCSR04_AWAIT_ECHO_RISE_PHASE:
          if(get_current_time() > echo_rise_timeout_counter) {
            // time out error, HCSR04_UNRESPONSIVE !!
            current_status = HCSR04_UNRESPONSIVE;
            hcsr04_stop_cycle();
          } else { printf("Echo: %d\n", gpioRead(&echo_pin)); }
          break;

        case HCSR04_AWAIT_ECHO_FALL_PHASE:
          break;

      }
      break;
    }

    case HCSR04_UNRESPONSIVE:
      report("HCSR04 Sensor UNRESPONSIVE!!! Restarting Cycle..\n"); // notify user
      /* hcsr04_start_cycle(); */
      current_status = HCSR04_IDLE; //TODO: REMOVE THIS
      break;

    case HCSR04_IDLE:
    break;

  }

}

HCSR04_STATUS get_hcsr04_status(void) { return current_status; }
HCSR04_PHASE get_hcsr04_phase(void) { return current_phase; }

uint16_t get_ultrasonic_distance_cm(void) { return hcsr04_timer_echo_ticks*HCSR04_TIMER_COUNTER_TO_CM; }

static void hcsr04_send_trigger(void) {

  while(!uartIsTransmissionComplete(CONSOLE_UART)); // needed because same as console port unfortuantely
  gpioWrite(&trigger_pin, 1);
  delay10us(1);
  gpioWrite(&trigger_pin, 0);

  current_phase = HCSR04_TRIGGER_SENT_PHASE;

}

static void hcsr04_await_echo_rise(void) {

  // Time-out time variable if no echo rise detected
  echo_rise_timeout_counter = get_current_time() + 5;
 
  current_phase = HCSR04_AWAIT_ECHO_RISE_PHASE;

}

static void hcsr04_await_echo_fall(void) {

  // Time-out timer if no echo rise detected
  /* startTimer( */
  /*     HCSR04_TIMER, */ 
  /*     frequencyToSysclkDivisor(ECHO_FALL_TIMEOUT_FREQ), */
  /*     DISABLE_OUTPUT, */ 
  /*     ENABLE_INTERRUPT, */ 
  /*     FREE_RUNNING */
  /*     ); */

  // changing to interrupt on falling edge if it was changed by a previous phase
  echo_pin.interruptTrigger = GPIO_FALLING_EDGE; 
  gpioConfigure(&echo_pin);

  current_phase = HCSR04_AWAIT_ECHO_FALL_PHASE;

}

/* INTERRUPT(HCSR04_TIMER_ISR, HCSR04_TIMER_INTERRUPT) { */
  /* uartGetCharacter(CONSOLE_UART, 'E', NON_BLOCKING); */

/*   switch (current_phase) { */

/*     case HCSR04_AWAIT_ECHO_FALL_PHASE: */
/*       // time out error, HCSR04_UNRESPONSIVE !! */
/*       current_status = HCSR04_UNRESPONSIVE;  // update state */
/*       current_phase = HCSR04_ECHO_TIMEOUT_PHASE;  // restarting cycle */
/*       break; */
      
/*     /1* default: *1/ */
/*     /1*   while(1) {report("WTF?!? State Machine main process func has un-handled situation!!!!\r");} *1/ */

/*   } */

/* } */

/* INTERRUPT(HCSR04_INT_PIN_ISR, HCSR04_INT_PIN_INTERRUPT) { */

/*   switch (current_phase) { */

/*     case HCSR04_AWAIT_ECHO_RISE_PHASE: */
/*       //TODO: start already set timer like TMR1ON = 1 */
/*       current_phase = HCSR04_ECHO_RISE_CAPTURED_PHASE; */
/*       break; */

/*     case HCSR04_AWAIT_ECHO_FALL_PHASE: */
/*       // GOTCHA BITCH :D */
/*       /1* hcsr04_timer_echo_ticks = stopTimer(HCSR04_TIMER); //TODO assign actual timer register now then call this later in main loop *1/ */
/*       current_phase = HCSR04_ECHO_FALL_CAPTURED_PHASE; */
/*       current_status = HCSR04_ACTIVE; */
/*       break; */
/*   } */
/* } */
