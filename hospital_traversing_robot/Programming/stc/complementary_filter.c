#include "project-defs.h"

static volatile uint32_t complementary_filter_next_dt = 0;
static int16_t pitch = 0;
static int16_t roll = 0;
static int16_t yaw = 0;

void complementary_filter_init(void) {
  complementary_filter_next_dt = get_current_time();
}

void complementary_filter_process(void) {
    if(get_current_time() >= complementary_filter_next_dt) {

#if (defined(TEST_MPU_ACCEL) || defined(TEST_MPU_GYRO) || defined(TEST_MPU_MAG))
      uint32_t start = get_current_time();
#endif

      complementary_filter_next_dt += COMP_FILTER_DT;

      // read latest accel values
      read_accel();
      read_gyro();
      read_mag();

      // compute Accel with Complementary Filter
      /* int16_t pitch = (get_accel_pitch()*COMP_FILTER_ALPHA + get_gyro(0)*COMP_FILTER_BETA)/100; */
      /* int16_t roll = (get_accel_roll()*COMP_FILTER_ALPHA + get_gyro(1)*COMP_FILTER_BETA)/100; */
      pitch = get_accel_pitch();
      roll = get_accel_roll();
      yaw = get_mag_yaw(roll, pitch);

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

int16_t get_compl_pitch(void) { return pitch; }
int16_t get_compl_roll(void) { return roll; }
int16_t get_compl_yaw(void) { return yaw; }

