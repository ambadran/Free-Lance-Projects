#include "project-defs.h"

void mpu6050_init(void) {
  i2cInitialiseMaster(0, I2C_CLOCK_400kHz);
}


