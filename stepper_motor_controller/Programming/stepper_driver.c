#include "project-defs.h"

static const microstepping_value_t MICROSTEP_TO_MS1_VALUE[] = {0, 1, 0, 1};
static const microstepping_value_t MICROSTEP_TO_MS2_VALUE[] = {0, 0, 1, 1};

static GpioConfig stepper_enable_pin = GPIO_PIN_CONFIG(STEPPER_MOTOR_PORT, STEPPER_ENABLE_PIN, GPIO_PUSH_PULL_MODE);
static volatile GpioConfig stepper_step_pin = GPIO_PIN_CONFIG(STEPPER_MOTOR_PORT, STEPPER_STEP_PIN, GPIO_PUSH_PULL_MODE);
static GpioConfig stepper_dir_pin = GPIO_PIN_CONFIG(STEPPER_MOTOR_PORT, STEPPER_DIR_PIN, GPIO_PUSH_PULL_MODE);
static GpioConfig stepper_ms1_pin = GPIO_PIN_CONFIG(STEPPER_MOTOR_PORT, STEPPER_MS1_pin, GPIO_PUSH_PULL_MODE);
static GpioConfig stepper_ms2_pin = GPIO_PIN_CONFIG(STEPPER_MOTOR_PORT, STEPPER_MS2_pin, GPIO_PUSH_PULL_MODE);

static volatile uint16_t step_counter = 0;
static volatile __bit stepper_active = 0;
static volatile uint32_t frequency = DEFAULT_STEPPER_FREQUENCY;

void stepper_motor_init(void) {

  // GPIOs init
  gpioConfigure(&stepper_enable_pin);
  gpioWrite(&stepper_enable_pin, STEPPER_DISABLE);
  gpioConfigure(&stepper_step_pin);
  gpioConfigure(&stepper_dir_pin);
  gpioConfigure(&stepper_ms1_pin);
  gpioConfigure(&stepper_ms2_pin);

  stepper_set_microstep(STEP_1X8);

}

void stepper_motor_set_freq(uint32_t frequency_input) { frequency = frequency_input; }

void stepper_motor_move(int16_t distance) {

  if (distance < 0) {
    distance = -distance;
    gpioWrite(&stepper_dir_pin, STEPPER_CLOCKWISE_DIR);
  } else if (distance > 0) {
    gpioWrite(&stepper_dir_pin, STEPPER_ANTICLOCKWISE_DIR);
  }

  step_counter = distance * STEPPER_CM_TO_STEPS;
  stepper_active = 1;
  printf("Steps to move: %d \n", step_counter);

  // Timer init
	startTimer(
		STEPPER_TIMER, 
		frequencyToSysclkDivisor(frequency),
		DISABLE_OUTPUT, 
		ENABLE_INTERRUPT, 
		FREE_RUNNING
	);

}

__bit get_stepper_state(void) { return stepper_active; }

void stepper_set_microstep(microstepping_value_t microstepping_value) {

  gpioWrite(&stepper_ms1_pin, MICROSTEP_TO_MS1_VALUE[microstepping_value]) ;
  gpioWrite(&stepper_ms2_pin, MICROSTEP_TO_MS2_VALUE[microstepping_value]) ;

}

INTERRUPT(STEPPER_TIMER_ISR, STEPPER_TIMER_INTERRUPT) {

  if (step_counter) {

    step_counter--;
    gpioToggle(&stepper_step_pin);

  } else if (stepper_active) {

    gpioWrite(&stepper_enable_pin, STEPPER_DISABLE);
    stepper_active = 0;
    stopTimer(STEPPER_TIMER);

  }

}

