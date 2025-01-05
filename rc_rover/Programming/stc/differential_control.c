#include "project-defs.h"

static GpioConfig in1_pin = GPIO_PIN_CONFIG(IN1_PORT, IN1_PIN, GPIO_PUSH_PULL_MODE); 
static GpioConfig in2_pin = GPIO_PIN_CONFIG(IN2_PORT, IN2_PIN, GPIO_PUSH_PULL_MODE); 
static GpioConfig in3_pin = GPIO_PIN_CONFIG(IN3_PORT, IN3_PIN, GPIO_PUSH_PULL_MODE); 
static GpioConfig in4_pin = GPIO_PIN_CONFIG(IN4_PORT, IN4_PIN, GPIO_PUSH_PULL_MODE); 

static volatile __bit is_moving = 0;
// This is a very important value which determines the movement of the DC Motors will continue till when. It's basically the current time + the time needed to achieve a specific cm moved or a degree rotated.
static volatile uint32_t differential_control_movement_ms;

#pragma save
// Suppress warning "unreferenced function argument"
#pragma disable_warning 85
/* FROM advpwm-hal.h:
 * **IMPORTANT:** You MUST define both pwmOnCounterInterrupt() and
 * pwmOnChannelInterrupt(), even if you don't use them. */
void pwmOnCounterInterrupt(PWM_Counter counter, PWM_CounterInterrupt HAL_PWM_SEGMENT event) {}
void pwmOnChannelInterrupt(PWM_Channel channel, uint16_t HAL_PWM_SEGMENT counterValue) {}
#pragma restore

void differential_control_init(void) {

  // Initializing EnA Pin
  pwmConfigureCounter(
    PWM_MOTOR_RIGHT_COUNTER,
    PWM_MOTOR_FREQ * 65535UL, 
    PWM_MOTOR_FREQ, 
    PWM_FREE_RUNNING, 
    PWM_NO_TRIGGER,
    0, 
    PWM_IMMEDIATE_UPDATE,
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
    32578
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
    PWM_MOTOR_FREQ * 65535UL, 
    PWM_MOTOR_FREQ, 
    PWM_FREE_RUNNING, 
    PWM_NO_TRIGGER,
    0, 
    PWM_IMMEDIATE_UPDATE,
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
    32578
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

  gpioWrite(&in1_pin, 1);
  gpioWrite(&in2_pin, 0);
  gpioWrite(&in3_pin, 1);
  gpioWrite(&in4_pin, 0);
  /* pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, duty_cycle); */
  /* pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, duty_cycle); */

  differential_control_movement_ms = get_current_time()+distance_cm*CM_TO_MOVEMENT_MS;
  printf("d: %lu", differential_control_movement_ms);
  is_moving = 1;

}

void differential_control_backward(uint8_t distance_cm, uint16_t duty_cycle) {

  gpioWrite(&in1_pin, 0);
  gpioWrite(&in2_pin, 1);
  gpioWrite(&in3_pin, 0);
  gpioWrite(&in4_pin, 1);
  pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, duty_cycle);
  pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, duty_cycle);

  differential_control_movement_ms = get_current_time()+distance_cm*CM_TO_MOVEMENT_MS;
  is_moving = 1;

}

void differential_control_right(uint8_t angle_deg, uint16_t duty_cycle) {

  gpioWrite(&in1_pin, 1);
  gpioWrite(&in2_pin, 0);
  gpioWrite(&in3_pin, 0);
  gpioWrite(&in4_pin, 1);
  pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, duty_cycle);
  pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, duty_cycle);

  differential_control_movement_ms = get_current_time()+angle_deg*DEGREE_TO_MOVEMENT_MS;
  is_moving = 1;

}

void differential_control_left(uint8_t angle_deg, uint16_t duty_cycle) {

  gpioWrite(&in1_pin, 0);
  gpioWrite(&in2_pin, 1);
  gpioWrite(&in3_pin, 1);
  gpioWrite(&in4_pin, 0);
  pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, duty_cycle);
  pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, duty_cycle);

  differential_control_movement_ms = get_current_time()+angle_deg*DEGREE_TO_MOVEMENT_MS;
  is_moving = 1;

}

void differential_control_stop(void) {

  gpioWrite(&in1_pin, 0);
  gpioWrite(&in2_pin, 0);
  gpioWrite(&in3_pin, 0);
  gpioWrite(&in4_pin, 0);
  pwmSetDutyCycle(PWM_MOTOR_RIGHT_CHANNEL, DUTY_CYCLE_MIN);
  pwmSetDutyCycle(PWM_MOTOR_LEFT_CHANNEL, DUTY_CYCLE_MIN);

  is_moving = 0;

}

void differential_control_process(void) {

  if(is_moving) {
    if(get_current_time() >= differential_control_movement_ms) { 
      differential_control_stop(); 
    }
  }

}


__bit differential_control_is_moving(void) { return is_moving; }
