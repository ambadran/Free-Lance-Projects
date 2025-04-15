#include "project-defs.h"

// a global variable to store read register value instaed of creating one repeatedly
uint8_t reg_value = 0;

void mpu6050_init(void) {
  i2cInitialiseMaster(CHOOSEN_I2C_PIN_SWITCH, I2C_CLOCK_400kHz);

  // testing responsiveness
  mpu6050_test_responsiveness();

  // setting the registers
  mpu6050_write_byte(PWR_MGMT_1, 0x00);
  delay1ms(200);  // power up delay
  mpu6050_write_byte(CONFIG, 0x00);
  mpu6050_write_byte(CONFIG, 0b00000011)
  mpu6050_write_byte(SMPLRT_DIV, 0x04)
  mpu6050_write_byte(GYRO_CONFIG, gyro_range_ind << 3)
  mpu6050_write_byte(ACCEL_CONFIG, accel_sensitivity_ind << 3)
  mpu6050_write_byte(INT_PIN_CFG, 0x02)  // i don't know if this will make problems
  mpu6050_write_byte(FIFO_EN, 0b00000000)
  mpu6050_write_byte(I2C_MST_CTRL, 0b00000000)
  mpu6050_write_byte(INT_ENABLE, 0b00000000)
  mpu6050_write_byte(I2C_MST_DELAY_CTRL, 0b00000000)
  mpu6050_write_byte(PWR_MGMT_2, 0b00000000)


}

I2C_AckNak mpu6050_write_byte(uint8_t register_to_write, uint8_t value) {
  I2C_AckNak ack_state = i2cStartCommand(MPU6050_ADDRESS, I2C_WRITE);  // should return I2C_ACK 
  i2cSendByte(register_to_write);  // should return I2C_ACK 
  i2cSendByte(value);  // should return I2C_ACK 
  i2cStop();

  return ack_state;
}

/*
 * MPU6050 reading from internal register as described in datasheet
 */
I2C_AckNak mpu6050_read_byte(uint8_t register_to_read, uint8_t* reg_value) {

  I2C_AckNak ack_state = i2cStartCommand(MPU6050_ADDRESS, I2C_WRITE);  // should return I2C_ACK 
  i2cSendByte(register_to_read);  // should return I2C_ACK 
  i2cStartCommand(MPU6050_ADDRESS, I2C_READ);  // should return I2C_ACK 
  *reg_value = i2cReadByteSendAck(I2C_NAK);
  i2cStop();

  return ack_state;
}

void mpu6050_test_responsiveness(void) {

   I2C_AckNak ack_state = mpu6050_read_byte(WHO_AM_I, &reg_value);
   if (ack_state != I2C_ACK) {
      while (1) {
        printf("MPU6050 didn't acknowledge start condition! \r");
        delay1ms(200);
      }
   }
  if(reg_value != WHO_AM_I_RESPONSE) {
    while (1) {
      printf("MPU6050 WHO_AM_I register responded with '%d' (not %d) \r", reg_value, WHO_AM_I_RESPONSE);
      delay1ms(200);
    }
  }

}


