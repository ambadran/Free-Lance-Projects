#include "project-defs.h"

// a global variable to store read register value instaed of creating one repeatedly
uint8_t reg_value = 0;

// Offset values
int16_t ACCEL_OFFSET = {DEFAULT_ACCEL_OFFSET_X, DEFAULT_ACCEL_OFFSET_Y, DEFAULT_ACCEL_OFFSET_Z};
int16_t GYRO_OFFSET = {DEFAULT_GYRO_OFFSET_X, DEFAULT_GYRO_OFFSET_Y, DEFAULT_GYRO_OFFSET_Z};

// Actual Values
uint8_t raw_values = {0, 0, 0, 0, 0, 0};  // the temporary buffer to read the values fresh out of the mpu6050
int16_t raw_accel_values = {0, 0, 0};
int16_t raw_gyro_values = {0, 0, 0};
float accel_values = {0.0, 0.0, 0.0};
float gyro_values = {0.0, 0.0, 0.0};

void mpu6050_init(void) {
  i2cInitialiseMaster(CHOOSEN_I2C_PIN_SWITCH, I2C_CLOCK_400kHz);

  // testing responsiveness
  mpu6050_test_responsiveness();

  // setting the registers
  mpu6050_write_byte(PWR_MGMT_1, 0x00);
  delay1ms(200);  // power up delay
  mpu6050_write_byte(CONFIG, 0x00);
  mpu6050_write_byte(CONFIG, 0b00000011);
  mpu6050_write_byte(SMPLRT_DIV, 0x04);
  mpu6050_write_byte(ACCEL_CONFIG, ACCEL_SENSITIVITY << 3);
  mpu6050_write_byte(GYRO_CONFIG, GYRO_SENSITIVITY << 3);
  mpu6050_write_byte(INT_PIN_CFG, 0x02);  // i don't know if this will make problems
  mpu6050_write_byte(FIFO_EN, 0b00000000);
  mpu6050_write_byte(I2C_MST_CTRL, 0b00000000);
  mpu6050_write_byte(INT_ENABLE, 0b00000000);
  mpu6050_write_byte(I2C_MST_DELAY_CTRL, 0b00000000);
  mpu6050_write_byte(PWR_MGMT_2, 0b00000000);

  // Calibration
  calibrate_gyro();
}

/*
 * MPU6050 writing to internal register as described in datasheet
 */
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

I2C_AckNak mpu6050_write_bytes(uint8_t register_to_write, uint8_t* values, int8_t bytes_num) {
//TODO: CHECK THIS IS THE SAME ROUTINE AS THE DATASHEET
  I2C_AckNak ack_state = i2cStartCommand(MPU6050_ADDRESS, I2C_WRITE);  // should return I2C_ACK 
  i2cSendByte(register_to_write);  // should return I2C_ACK 
  for( ; bytes_num >= 0 ; bytes_num--) {
    i2cSendByte(*values++);  // should return I2C_ACK 
  }
  i2cStop();

  return ack_state;
}

/*
 * MPU6050 reading from internal register as described in datasheet
 */
I2C_AckNak mpu6050_read_bytes(uint8_t register_to_read, uint8_t* reg_values, int8_t bytes_num) {

  I2C_AckNak ack_state = i2cStartCommand(MPU6050_ADDRESS, I2C_WRITE);  // should return I2C_ACK 
  i2cSendByte(register_to_read);  // should return I2C_ACK 
  i2cStartCommand(MPU6050_ADDRESS, I2C_READ);  // should return I2C_ACK 
  for( ; bytes_num >= 1 ; bytes_num--) {
    *reg_values++ = i2cReadByteSendAck(I2C_ACK);
  }
  *reg_values++ = i2cReadByteSendAck(I2C_NAK);
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

void calibrate_gyro(void) {
  
}

I2C_AckNak read_raw_accel(void) {

  // reading the raw values
  I2C_AckNak ack_state = mpu6050_read_bytes(ACCEL_XOUT_H, raw_values, 6);

  // assigning
  raw_accel_values[0] = (int16_t)((raw_values[0] << 8) | (raw_values[1]));
  raw_accel_values[1] = (int16_t)((raw_values[2] << 8) | (raw_values[3]));
  raw_accel_values[2] = (int16_t)((raw_values[4] << 8) | (raw_values[5]));

  return ack_state;
}

I2C_AckNak read_raw_gyro(void) {

  // reading the raw values
  I2C_AckNak ack_state = mpu6050_read_bytes(GYRO_XOUT_H, raw_values, 6);

  // assigning
  raw_gyro_values[0] = (int16_t)((raw_values[0] << 8) | (raw_values[1]));
  raw_gyro_values[1] = (int16_t)((raw_values[2] << 8) | (raw_values[3]));
  raw_gyro_values[2] = (int16_t)((raw_values[4] << 8) | (raw_values[5]));

  return ack_state;
}

I2C_AckNak read_accel(void) {

  // reading the raw values
  I2C_AckNak ack_state = mpu6050_read_bytes(ACCEL_XOUT_H, raw_values, 6);

  // assigning
  accel_values[0] = (float)((int16_t)((raw_values[0] << 8) | (raw_values[1])) - ACCEL_OFFSET[0]) / ACCEL_SENSITIVITY_VALUES[ACCEL_SENSITIVITY];
  accel_values[1] = (float)((int16_t)((raw_values[2] << 8) | (raw_values[3])) - ACCEL_OFFSET[1]) / ACCEL_SENSITIVITY_VALUES[ACCEL_SENSITIVITY];
  accel_values[2] = (float)((int16_t)((raw_values[4] << 8) | (raw_values[5])) - ACCEL_OFFSET[2]) / ACCEL_SENSITIVITY_VALUES[ACCEL_SENSITIVITY];


  return ack_state;
}

I2C_AckNak read_gyro(void) {

  // reading the raw values
  I2C_AckNak ack_state = mpu6050_read_bytes(GYRO_XOUT_H, raw_values, 6);

  // assigning
  gyro_values[0] = (float)((int16_t)((raw_values[0] << 8) | (raw_values[1])) - GYRO_OFFSET[0]) / GYRO_SENSITIVITY_VALUES[GYRO_SENSITIVITY];
  gyro_values[1] = (float)((int16_t)((raw_values[2] << 8) | (raw_values[3])) - GYRO_OFFSET[1]) / GYRO_SENSITIVITY_VALUES[GYRO_SENSITIVITY];;
  gyro_values[2] = (float)((int16_t)((raw_values[4] << 8) | (raw_values[5])) - GYRO_OFFSET[2]) / GYRO_SENSITIVITY_VALUES[GYRO_SENSITIVITY];;

  return ack_state;
}

int16_t get_raw_accel(uint8_t ind) { return raw_accel_values[ind]; }
int16_t get_raw_gyro(uint8_t ind) { return raw_gyro_values[ind]; }
float get_accel(uint8_t ind) { return accel_values[ind]; }
float get_gyro(uint8_t ind) { return gyro_values[ind] }

