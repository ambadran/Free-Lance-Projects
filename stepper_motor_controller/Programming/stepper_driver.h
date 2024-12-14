#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

typedef enum {
  STEPPER_DISABLE = 0,
  STEPPER_ENABLE,
} stepper_enable_status_t;

typedef enum {
  STEPPER_CLOCKWISE_DIR = 0,
  STEPPER_ANTICLOCKWISE_DIR
} stepper_direction_t;

typedef enum {
  STEP_1X8 = 0,
  STEP_1X32,
  STEP_1X64,
  STEP_1X16,
} microstepping_value_t;

typedef struct {
  stepper_direction_t stepper_direction;
  microstepping_value_t microstepping_value;
  uint32_t frequency;
  uint16_t steps;
} stepper_movement_t;

void stepper_motor_init(void);
void stepper_motor_set_freq(uint32_t frequency);
void stepper_motor_move(stepper_movement_t* stepper_movement);
__bit get_stepper_state(void);
void stepper_set_microstep(microstepping_value_t microstepping_value);
void stepper_set_enable(stepper_enable_status_t stepper_enable_status);


#endif
