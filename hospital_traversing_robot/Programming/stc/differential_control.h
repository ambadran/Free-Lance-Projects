#ifndef DIFFERENTIAL_CONTROL_H
#define DIFFERENTIAL_CONTROL_H

#define DUTY_CYCLE_MAX 65535
#define DUTY_CYCLE_MIN 0

typedef enum {
  DIFFERENTIAL_MOVE_IDLE,
  DIFFERENTIAL_MOVE_IN_PROGRESS,
  DIFFERENTIAL_MOVE_FORWARD,
  DIFFERENTIAL_MOVE_BACKWARD,
  DIFFERENTIAL_MOVE_RIGHT,
  DIFFERENTIAL_MOVE_LEFT,
  DIFFERENTIAL_MOVE_MODE_COUNT
} differential_movement_mode_t;

typedef struct {
  uint8_t i_value;
  uint16_t j_value;
  differential_movement_mode_t differential_movement_mode;
} differential_movement_t;

void differential_control_init(void);
void differential_control_forward(uint8_t distance_cm, uint16_t duty_cycle);
void differential_control_backward(uint8_t distance_cm, uint16_t duty_cycle);
void differential_control_right(uint8_t angle_deg, uint16_t duty_cycle);
void differential_control_left(uint8_t angle_deg, uint16_t duty_cycle);
void differential_control_stop(void);
void differential_control_process(void);
uint8_t differential_control_is_moving(void);
void differential_control_set_movement(uint8_t i_value, uint16_t j_value, differential_movement_mode_t differential_movement_mode);

#endif
