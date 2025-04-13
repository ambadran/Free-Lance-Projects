#include "project-defs.h"

void mpu6050_init(void) {
  i2cInitialiseMaster(0, I2C_CLOCK_400kHz);

  // testing responsiveness
  mpu6050_test_responsiveness();

  // setting the registers

}

void mpu6050_test_responsiveness(void) {

  i2cStartCommand(DEVICE_ADDRESS, I2C_READ);
  i2cSendByte(WHO_AM_I);
  uint8_t reg = i2cReadByteSendAck(I2C_ACK);
  if(reg != 113) {
    while (1) {
      printf("MPU6050 WHO_AM_I register responded with '%d' (not 113) \r", reg);
      delay1ms(200);
    }
  }
  printf("MPU6050 responded to WHO_AM_I!! :D");

}


