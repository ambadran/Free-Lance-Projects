#include "project-defs.h"

// a global variable to store read register value instaed of creating one repeatedly
static uint8_t reg_value = 0;

// Offset values
static int16_t MAG_OFFSET[] = {DEFAULT_MAG_OFFSET_X, DEFAULT_MAG_OFFSET_Y, DEFAULT_MAG_OFFSET_Z};

// Actual Values
static int16_t mag_raw_values[] = {0, 0, 0};
static int32_t mag_values[] = {0, 0, 0};

void hmc5883l_init(void) {

  // initializing the i2c hardware
  //TODO: need to somehow check if the i2c is already activated by another component, e.g-mpu6050.c

  // testing responsiveness
  hmc5883l_check_responsiveness();

  // settings the registers

}

I2C_AckNak hmc5883l_write_byte(uint8_t register_to_write, uint8_t value) {

  I2C_AckNak ack_state = i2cStartCommand(HMC5883L_ADDRESS, I2C_WRITE);
  i2cSendByte(register_to_write);
  i2cSendByte(value);
  i2cStop();

  return ack_state;
}

I2C_AckNak hmc5883l_read_byte(uint8_t register_to_read, uint8_t* reg_value) {

  I2C_AckNak ack_state = i2cStartCommand(HMC5883L_ADDRESS, I2C_WRITE);  // should return I2C_ACK 
  i2cSendByte(register_to_read);
  i2cStartCommand(HMC5883L_ADDRESS, I2C_READ);  
  *reg_value = i2cReadByteSendAck(I2C_NAK);
  i2cStop();

  return ack_state;
}

void hmc5883l_check_responsiveness(void) {

  I2C_AckNak ack_state = hmc5883l_read_byte(HMC5883L_REG_IDENT_A, &reg_value);

  // Testing if the device acknoledges the I2C Start Condition
  if (ack_state != I2C_ACK) {
    while (1) {
      printf("HMC5883L didn't acknowledge start condition! \r");
      delay1ms(200);
    }
  }

  // Testing if the devices replies with appropriate Identification No.1
  if(reg_value != HMC5883L_REG_IDENT_A_RESPONSE) {
    while (1) {
      printf("HMC5883L WHO_AM_I register responded with '%d' (not %d) \r", reg_value, HMC5883L_REG_IDENT_A_RESPONSE);
      delay1ms(200);
    }
  }

  // Testing if the devices replies with appropriate Identification No.2
  hmc5883l_read_byte(HMC5883L_REG_IDENT_B, &reg_value);
  if(reg_value != HMC5883L_REG_IDENT_B_RESPONSE) {
    while (1) {
      printf("HMC5883L WHO_AM_I register responded with '%d' (not %d) \r", reg_value, HMC5883L_REG_IDENT_B_RESPONSE);
      delay1ms(200);
    }
  }

  // Testing if the devices replies with appropriate Identification No.3
  hmc5883l_read_byte(HMC5883L_REG_IDENT_C, &reg_value);
  if(reg_value != HMC5883L_REG_IDENT_C_RESPONSE) {
    while (1) {
      printf("HMC5883L WHO_AM_I register responded with '%d' (not %d) \r", reg_value, HMC5883L_REG_IDENT_C_RESPONSE);
      delay1ms(200);
    }
  }

}
