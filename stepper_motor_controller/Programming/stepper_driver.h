#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#define STEPPER_CLOCKWISE_DIR 0
#define STEPPER_ANTICLOCKWISE_DIR 1

#define STEPPER_ENABLE 1
#define STEPPER_DISABLE 0

typedef enum {
  STEP_1X8,
  STEP_1X32,
  STEP_1X64,
  STEP_1X16,
} microstepping_value_t;

void stepper_motor_init(void);
void stepper_motor_set_freq(uint32_t frequency);
void stepper_motor_move(int16_t distance);
__bit get_stepper_state(void);
void stepper_set_microstep(microstepping_value_t microstepping_value);


#endif
