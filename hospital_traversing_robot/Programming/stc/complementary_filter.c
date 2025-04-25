#include "project-defs.h"

static volatile uint32_t complementary_filter_next_dt = 0;
int32_t tilt_x = 0;
int32_t tilt_y = 0;

void complementary_filter_init(void) {
  complementary_filter_next_dt = get_current_time();

}
void complementary_filter_process(void) {
    if(get_current_time() >= complementary_filter_next_dt) {
      complementary_filter_next_dt += COMP_FILTER_DT;

      // read latest accel values
      read_accel();
      read_gyro_no_z();

      // updating tilt values
    }
}

int32_t get_tilt_x(void) { return tilt_x; }
int32_t get_tilt_y(void) { return tilt_y; }
