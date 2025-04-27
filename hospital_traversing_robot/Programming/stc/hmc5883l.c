#include "project-defs.h"

void hmc5883l_init(void) {

  // initializing the i2c hardware
  //TODO: need to somehow check if the i2c is already activated by another component, e.g-mpu6050.c

  hmc5883l_check_responsiveness();

}

void hmc5883l_check_responsiveness(void) {

}
