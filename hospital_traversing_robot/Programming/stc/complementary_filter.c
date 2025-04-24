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
      // 1. Accelerometer tilt (no atan2! Use ratio directly)
      int32_t accel_tilt_x = get_accel(1) / (get_accel(2) + 1);  // Avoid div/0 (≈sinθ)
      int32_t accel_tilt_y = -get_accel(0) / (get_accel(2) + 1); // ≈sinϕ

      // 2. Gyro integration (convert °/s to angle change)
      tilt_x += (get_gyro(0) * COMP_FILTER_DT) / 1000;  // Δθ = gyro * Δt, 
      tilt_y += (get_gyro(1) * COMP_FILTER_DT) / 1000;  // /1000 to convert ms to s

      // 3. Complementary filter (fixed-point arithmetic)
      tilt_x = (COMP_FILTER_ALPHA * tilt_x + COMP_FILTER_BETA * accel_tilt_x) / 100;
      tilt_y = (COMP_FILTER_ALPHA * tilt_y + COMP_FILTER_BETA * accel_tilt_y) / 100;
    }
}

int32_t get_tilt_x() { return tilt_x; }
int32_t get_tilt_y() { return tilt_y; }
