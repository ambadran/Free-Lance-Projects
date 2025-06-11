#ifndef ORIENTATION_H
#define ORIENTAITON_H

// These are tuned values as the gyroscope is not in the center of the robot :(
typedef enum {
  WEST = 0, // default direction
  NORTH = 36, // right direction
  SOUTH = -36, // left direction
  EAST = 72, // back direction
} directions_t;

void orientation_init(void);
void orientation_process(void);

// special yaw control functions, especially becase reading is through gyro which doesn't have an absolute value
void orientation_sum_latest_gyro_to_yaw(void);
void orientation_lock_yaw_measurement(void);
void orientation_unlock_yaw_measurement(void);
void orientation_set_gyro_yaw(int16_t yaw);

int16_t get_accel_roll_deg(void);
int16_t get_accel_pitch_deg(void);
int16_t get_gyro_yaw_deg(void);

#endif
