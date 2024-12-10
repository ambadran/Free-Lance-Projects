#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#define STEPPER_CLOCKWISE_DIR 0
#define STEPPER_ANTICLOCKWISE_DIR 1

#define STEPPER_ENABLE 1
#define STEPPER_DISABLE 0

void stepper_motor_init(void);
void stepper_motor_set_freq(uint32_t frequency);
void stepper_motor_move(int16_t distance);
uint8_t get_stepper_state(void);

#endif
