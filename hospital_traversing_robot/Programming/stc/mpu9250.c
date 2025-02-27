#include "project-defs.h"

void mpu9250_init(void) {
  i2cInitialiseMaster(0, I2C_CLOCK_400kHz);
}


