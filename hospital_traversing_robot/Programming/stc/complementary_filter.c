#include "project-defs.h"

static volatile uint32_t complementary_filter_next_dt = 0;

void complementary_filter_init(void) {
  complementary_filter_next_dt = get_current_time();
}

void complementary_filter_process(void) {
    if(get_current_time() >= complementary_filter_next_dt) {
      complementary_filter_next_dt += COMP_FILTER_DT;

      // read latest accel values
      read_accel();
      read_gyro_no_z();

      // testing the IMU
#ifdef TEST_MPU 
      /* printf("Ax: %ldg, Ay: %ldg, Az: %ldg  \r", get_accel(0), get_accel(1), get_accel(2)); */
      printf("Roll: %ddeg, Pitch: %ddeg    \r", get_accel_roll(), get_accel_pitch());
#endif
  }
}


