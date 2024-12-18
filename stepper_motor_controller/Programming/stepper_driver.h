#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

typedef enum {
  STEPPER_ENABLE = 0,
  STEPPER_DISABLE,
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
  stepper_enable_status_t stepper_enable_status;  // refers to the value of the EN when not in motion.
  stepper_direction_t stepper_direction;
  microstepping_value_t microstepping_value;
  uint32_t frequency;
  uint32_t steps;
} stepper_movement_t;

void stepper_motor_init(void);
void stepper_motor_set_freq(uint32_t frequency);
void stepper_motor_move(stepper_movement_t* stepper_movement);
__bit get_stepper_state(void);
void stepper_set_microstep(microstepping_value_t microstepping_value);
void stepper_set_enable(stepper_enable_status_t stepper_enable_status);
void stepper_set_dir(stepper_direction_t stepper_direction);
void stepper_set_steps_from_float_digits(stepper_movement_t* stepper_movement, uint8_t* float_digits);

#endif
