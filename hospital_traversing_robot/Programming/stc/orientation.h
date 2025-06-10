#ifndef ORIENTATION_H
#define ORIENTAITON_H

typedef enum {
  NORTH = 0,
  EAST = 90,
  WEST = -90,
  SOUTH = 180,
} directions_t;

void orientation_init(void);
void orientation_process(void);
void orientation_lock_yaw_measurement(void);
void orientation_unlock_yaw_measurement(void);

void set_gyro_yaw(int16_t yaw);
int16_t get_accel_roll_deg(void);
int16_t get_accel_pitch_deg(void);
int16_t get_gyro_yaw_deg(void);

#endif
