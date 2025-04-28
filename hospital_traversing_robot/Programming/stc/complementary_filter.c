#include "project-defs.h"

static volatile uint32_t complementary_filter_next_dt = 0;

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

      // testing the IMU
#if defined(TEST_MPU_ACCEL)
      /* printf("Ax: %ldg, Ay: %ldg, Az: %ldg  \r", get_accel(0), get_accel(1), get_accel(2)); */
      printf("Roll: %ddeg, Pitch: %ddeg (in %ldms)   \r", get_accel_roll(), get_accel_pitch(), get_current_time()-start);
#elif defined(TEST_MPU_GYRO)
      /* printf("Roll: %ddeg, Pitch: %ddeg    \r", get_accel_roll(), get_accel_pitch()); */
#elif defined(TEST_MPU_MAG)
      printf("MAG X: %lduT, MAG Y: %lduT, MAG Z: %lduT  (in %ldms) \r", get_mag(0), get_mag(1), get_mag(2), get_current_time()-start);
#endif
  }
}


