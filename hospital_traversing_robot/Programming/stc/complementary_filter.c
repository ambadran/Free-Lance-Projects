#include "project-defs.h"

static volatile uint32_t complementary_filter_next_dt = 0;
int8_t roll = 0;
int8_t pitch = 0;

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
      /* printf("Ax: %ldg, Ay: %ldg, Az: %ldg  \r", get_accel(0), get_accel(1), get_accel(2)); */
  }
}

int8_t get_roll(void) {
  // this calculation depends on the ACCEL_SCALE being a specific value to convert the gravity value directly to degrees using just a multiple
  // The value is still probably not correctly scaled as the single axis orientation equation is arctan2(Axy/Az). 
  // however in this case getting the correctly scaled angles is not important. Plus tiny differences is also registered as number change.
  // This calculation WILL NOT WORK in case multi-axis tilting at the same time. This will need the full equation arctan2(Axy/sqrt(Axy^2+Az^2))
  // Here I will just clamp the actual accerlation value and call it a day :P
  
  //TODO: I am not sure should I clamp in a function in the comp_filter_process?? or here?? or in the get_accel???
  return CLAMP_TO_ANGLE(get_accel(0)); 
}
int8_t get_pitch(void) { return pitch; }
