#include "project-defs.h"

static volatile uint32_t complementary_filter_next_dt = 0;
// This is an amazing idea I came up with to stop summing gyro values to prevent eventual drift :D
static volatile uint8_t yaw_lock = 1;
static int16_t pitch = 0;
static int16_t roll = 0;
static int16_t yaw = STARTING_YAW_VALUE;

void orientation_init(void) {
  complementary_filter_next_dt = get_current_time();
}

void orientation_process(void) {
    if(get_current_time() >= complementary_filter_next_dt) {

#if (defined(TEST_MPU_ACCEL) || defined(TEST_MPU_GYRO) || defined(TEST_MPU_MAG))
      uint32_t start = get_current_time();
#endif

      complementary_filter_next_dt += COMP_FILTER_DT;

      // read latest accel values and compute
      if(read_accel() != I2C_ACK || read_gyro() != I2C_ACK) {
        report("MPU6050 NOT RESPONSIVE!\n");
      }
      if(!yaw_lock) {
        orientation_sum_latest_gyro_to_yaw();
      }

      // FUCK YOU MAGNETOMETER
      // read latest magnetometer values and compute
      /* if(read_mag() != I2C_ACK) { */
      /*   report("HMC5883L NOT RESPONSIVE!\n"); */
      /* } */
      /* yaw = get_mag_yaw_deg(roll, pitch); */

      // compute Accel with Complementary Filter
      /* int16_t pitch = (get_accel_pitch()*COMP_FILTER_ALPHA + get_gyro(0)*COMP_FILTER_BETA)/100; */
      /* int16_t roll = (get_accel_roll()*COMP_FILTER_ALPHA + get_gyro(1)*COMP_FILTER_BETA)/100; */

      // testing the IMU
#if defined(TEST_MPU_ACCEL)
      printf("Roll: %ddeg, Pitch: %ddeg, yaw: %ddeg (in %ldms)   \r", roll, pitch, yaw, get_current_time()-start);
#elif defined(TEST_MPU_GYRO)
      /* printf("Roll: %ddeg, Pitch: %ddeg    \r", get_gyro_roll(), get_gyro_pitch()); */
#elif defined(TEST_MPU_MAG)
      printf("MAG X: %lduT, MAG Y: %lduT, MAG Z: %lduT  (in %ldms) \r", get_mag(0), get_mag(1), get_mag(2), get_current_time()-start);
#endif
  }
}

void orientation_sum_latest_gyro_to_yaw(void) { 
  yaw += (get_gyro(2)*COMP_FILTER_DT)/1000; 
  if (yaw < -1*(180/YAW_MULTIPLE_COMPENSATE)) {
    yaw += (360/YAW_MULTIPLE_COMPENSATE);
  } else if (yaw > (180/YAW_MULTIPLE_COMPENSATE)) {
    yaw -= (360/YAW_MULTIPLE_COMPENSATE);
  }
}
void orientation_lock_yaw_measurement(void) { yaw_lock = 1; }
void orientation_unlock_yaw_measurement(void) { yaw_lock = 0; }
void orientation_set_gyro_yaw(int16_t current_yaw) { yaw=current_yaw; }
int16_t normalize_angle(int16_t angle) {
    while (angle > 180) { angle -= 360; }
    while (angle < -180) { angle += 360; }
    return angle;
}
int16_t angle_diff_normalized(int16_t target, int16_t current) {
    int16_t diff = target - current;

    // Normalize to [-180, 180]
    diff = (diff + 180) % 360;
    if (diff < 0) diff += 360;
    diff -= 180;

    return diff;
}

int16_t get_accel_pitch_deg(void) { return (int16_t)(atan2f(get_accel(1), sqrtf(get_accel(0)*get_accel(0) + get_accel(2)*get_accel(2)))*RAD_TO_DEG); }
int16_t get_accel_roll_deg(void) { return (int16_t)(atan2f(get_accel(0), sqrtf(get_accel(1)*get_accel(1) + get_accel(2)*get_accel(2)))*RAD_TO_DEG); }
int16_t get_gyro_yaw_deg(void) { return yaw*2.5; } //TODO: VERY IMP: fix the out of axis measurement, maybe apply multiple??!

