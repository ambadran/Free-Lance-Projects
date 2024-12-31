#ifndef DIFFERENTIAL_CONTROL_H
#define DIFFERENTIAL_CONTROL_H

#define DUTY_CYCLE_MAX 65535
#define DUTY_CYCLE_MIN 0

#define CM_TO_MOVEMENT_MS 500  // 1cm is moved in CM_TO_MOVEMENT_MS
#define DEGREE_TO_MOVEMENT_MS 15 // 1 degree is moved in DEGREE_TO_MOVEMENT_MS

void differential_control_init(void);
void differential_control_forward(uint8_t distance_cm, uint16_t duty_cycle);
void differential_control_backward(uint8_t distance_cm, uint16_t duty_cycle);
void differential_control_right(uint8_t angle_deg, uint16_t duty_cycle);
void differential_control_left(uint8_t angle_deg, uint16_t duty_cycle);
void differential_control_stop(void);
void differential_control_process(void);

#endif
