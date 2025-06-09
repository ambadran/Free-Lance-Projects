#include "project-defs.h"

static GpioConfig in1_pin = GPIO_PIN_CONFIG(IN1_PORT, IN1_PIN, GPIO_PUSH_PULL_MODE); 
static GpioConfig in2_pin = GPIO_PIN_CONFIG(IN2_PORT, IN2_PIN, GPIO_PUSH_PULL_MODE); 
static GpioConfig in3_pin = GPIO_PIN_CONFIG(IN3_PORT, IN3_PIN, GPIO_PUSH_PULL_MODE); 
static GpioConfig in4_pin = GPIO_PIN_CONFIG(IN4_PORT, IN4_PIN, GPIO_PUSH_PULL_MODE); 

// This is a very important value which determines the movement of the DC Motors will continue till when. It's basically the current time + the time needed to achieve a specific cm moved or a degree rotated.
static volatile uint32_t differential_control_movement_ms;
static volatile differential_movement_t differential_movement = {0, 0, DIFFERENTIAL_MOVE_IDLE};

char* DIFFERENTIAL_MOVE_MODE_TO_STR[DIFFERENTIAL_MOVE_MODE_COUNT] = {
  "DIFFERENTIAL_MOVE_IDLE",
  "DIFFERENTIAL_MOVE_FORWARD",
  "DIFFERENTIAL_MOVE_BACKWARD",
  "DIFFERENTIAL_MOVE_RIGHT",
  "DIFFERENTIAL_MOVE_LEFT" 
};

void differential_control_init(void) {

  uint16_t reloadValue;
  uint16_t prescalar = pwmCalculatePrescalerAndReloadValue(PWM_MOTOR_FREQ, &reloadValue);

  // Initializing EnA Pin
  pwmConfigureCounter(
    PWM_MOTOR_RIGHT_COUNTER,
    prescalar,
    reloadValue,
    PWM_FREE_RUNNING, 
    PWM_NO_TRIGGER,
    0, 
    PWM_BUFFERED_UPDATE,
    PWM_CONTINUOUS,
    PWM_EDGE_ALIGNED_UP,
    PWM_DISABLE_ALL_UE,
    DISABLE_INTERRUPT
  );
  pwmInitialisePWM(
    PWM_MOTOR_RIGHT_CHANNEL, 
    OUTPUT_HIGH, 
    DISABLE_INTERRUPT, 
    PWM_IMMEDIATE_UPDATE,
    DEFAULT_PWM_DUTY_CYCLE
  );
  pwmConfigureOutput(
    PWM_MOTOR_RIGHT_CHANNEL,
    PWM_MOTOR_RIGHT_PIN_CONFIG, 
    GPIO_PUSH_PULL_MODE,
    PWM_ACTIVE_HIGH, 
    PWM_DISABLE_FAULT_CONTROL, 
    OUTPUT_HIGH,
    PWM_MOTOR_RIGHT_OUTPUT_MODE
  );
  pwmEnableMainOutput(PWM_MOTOR_RIGHT_COUNTER);
  pwmEnableCounter(PWM_MOTOR_RIGHT_COUNTER);

  // Initializing EnB Pin
  pwmConfigureCounter(
    PWM_MOTOR_LEFT_COUNTER,
    prescalar,
    reloadValue,
    PWM_FREE_RUNNING, 
    PWM_NO_TRIGGER,
    0, 
    PWM_BUFFERED_UPDATE,
    PWM_CONTINUOUS,
    PWM_EDGE_ALIGNED_UP,
    PWM_DISABLE_ALL_UE,
    DISABLE_INTERRUPT
  );
  pwmInitialisePWM(
    PWM_MOTOR_LEFT_CHANNEL, 
    OUTPUT_HIGH, 
    DISABLE_INTERRUPT, 
    PWM_IMMEDIATE_UPDATE,
    DEFAULT_PWM_DUTY_CYCLE 
  );
  pwmConfigureOutput(
    PWM_MOTOR_LEFT_CHANNEL,
    PWM_MOTOR_LEFT_PIN_CONFIG, 
    GPIO_PUSH_PULL_MODE,
    PWM_ACTIVE_HIGH, 
    PWM_DISABLE_FAULT_CONTROL, 
    OUTPUT_HIGH,
    PWM_MOTOR_LEFT_OUTPUT_MODE
  );
  pwmEnableMainOutput(PWM_MOTOR_LEFT_COUNTER);
  pwmEnableCounter(PWM_MOTOR_LEFT_COUNTER);

  // Setting Duty Cycles to 0% so as keep Motor IDLE
  pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, DUTY_CYCLE_MIN);
  pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, DUTY_CYCLE_MIN);

  // Initializing the IN1, 2, 3, 4 pins
  gpioConfigure(&in1_pin);
  gpioConfigure(&in2_pin);
  gpioConfigure(&in3_pin);
  gpioConfigure(&in4_pin);

  // Setting ALL Pins to GND, so as keep Motor IDLE
  gpioWrite(&in1_pin, 0);
  gpioWrite(&in2_pin, 0);
  gpioWrite(&in3_pin, 0);
  gpioWrite(&in4_pin, 0);
}

void differential_control_forward(uint8_t distance_cm, uint16_t duty_cycle) {

  while(!uartIsTransmissionComplete(CONSOLE_UART)); // needed because same as console port unfortuantely
  gpioWrite(&in1_pin, 1);
  gpioWrite(&in2_pin, 0);
  gpioWrite(&in3_pin, 1);
  gpioWrite(&in4_pin, 0);
  pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, duty_cycle);
  pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, duty_cycle);

  differential_control_movement_ms = get_current_time()+distance_cm*CM_TO_MOVEMENT_MS;
  differential_movement.differential_movement_mode = DIFFERENTIAL_MOVE_IN_PROGRESS;

}

void differential_control_backward(uint8_t distance_cm, uint16_t duty_cycle) {

  while(!uartIsTransmissionComplete(CONSOLE_UART)); // needed because same as console port unfortuantely
  gpioWrite(&in1_pin, 0);
  gpioWrite(&in2_pin, 1);
  gpioWrite(&in3_pin, 0);
  gpioWrite(&in4_pin, 1);
  pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, duty_cycle);
  pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, duty_cycle);

  differential_control_movement_ms = get_current_time()+distance_cm*CM_TO_MOVEMENT_MS;
  differential_movement.differential_movement_mode = DIFFERENTIAL_MOVE_IN_PROGRESS;

}

void differential_control_right(uint8_t angle_deg, uint16_t duty_cycle) {

  while(!uartIsTransmissionComplete(CONSOLE_UART)); // needed because same as console port unfortuantely
  gpioWrite(&in1_pin, 0);
  gpioWrite(&in2_pin, 1);
  gpioWrite(&in3_pin, 1);
  gpioWrite(&in4_pin, 0);
  pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, duty_cycle);
  pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, duty_cycle);

  differential_control_movement_ms = get_current_time()+angle_deg*DEGREE_TO_MOVEMENT_MS;
  differential_movement.differential_movement_mode = DIFFERENTIAL_MOVE_IN_PROGRESS;

}

void differential_control_left(uint8_t angle_deg, uint16_t duty_cycle) {

  while(!uartIsTransmissionComplete(CONSOLE_UART)); // needed because same as console port unfortuantely
  gpioWrite(&in1_pin, 1);
  gpioWrite(&in2_pin, 0);
  gpioWrite(&in3_pin, 0);
  gpioWrite(&in4_pin, 1);
  pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, duty_cycle);
  pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, duty_cycle);

  differential_control_movement_ms = get_current_time()+angle_deg*DEGREE_TO_MOVEMENT_MS;
  differential_movement.differential_movement_mode = DIFFERENTIAL_MOVE_IN_PROGRESS;

}

void differential_control_stop(void) {

  while(!uartIsTransmissionComplete(CONSOLE_UART)); // needed because same as console port unfortuantely
  gpioWrite(&in1_pin, 0);
  gpioWrite(&in2_pin, 0);
  gpioWrite(&in3_pin, 0);
  gpioWrite(&in4_pin, 0);
  pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, DUTY_CYCLE_MIN);
  pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, DUTY_CYCLE_MIN);

  differential_movement.differential_movement_mode = DIFFERENTIAL_MOVE_IDLE;

}

void differential_control_process(void) {
  switch (differential_movement.differential_movement_mode) {

    case DIFFERENTIAL_MOVE_IDLE:
      break;

    case DIFFERENTIAL_MOVE_IN_PROGRESS:
      if(get_current_time() >= differential_control_movement_ms) { 
        differential_control_stop(); 
        report("Finished Movement\n");
      }
      break;

    case DIFFERENTIAL_MOVE_FORWARD:
      differential_control_forward(differential_movement.i_value, differential_movement.j_value);
      break;

    case DIFFERENTIAL_MOVE_BACKWARD:
      differential_control_backward(differential_movement.i_value, differential_movement.j_value);
      break;

    case DIFFERENTIAL_MOVE_RIGHT:
      differential_control_right(differential_movement.i_value, differential_movement.j_value);
      break;

    case DIFFERENTIAL_MOVE_LEFT:
      differential_control_left(differential_movement.i_value, differential_movement.j_value);
      break;
  }
}


uint8_t differential_control_is_moving(void) { return differential_movement.differential_movement_mode != DIFFERENTIAL_MOVE_IDLE; }


void differential_control_set_movement(uint8_t i_value, uint16_t j_value, differential_movement_mode_t differential_movement_mode) {
  differential_movement.i_value = i_value;
  differential_movement.j_value = j_value;
  differential_movement.differential_movement_mode = differential_movement_mode;

}
