#include "project-defs.h"

// Setting options
static uint8_t mag_gain_value = DEFAULT_MAG_GAIN;
static const int16_t GAIN_VALUE_MULTIPLE[8] = {73, 920, 1220, 1520, 2270, 2560, 3030, 4350};  // these are fixed-point values, the true value is multiply by  one of them THEN divide by 1000

// a global variable to store read register value instaed of creating one repeatedly
static uint8_t reg_value = 0;

// Offset values
static int16_t MAG_OFFSET[] = {DEFAULT_MAG_OFFSET_X, DEFAULT_MAG_OFFSET_Y, DEFAULT_MAG_OFFSET_Z};

// Actual Values
static uint8_t raw_values[] = {0, 0, 0, 0, 0, 0};  // the temporary buffer to read the values fresh out of the mpu6050
static int16_t raw_mag_values[] = {0, 0, 0};
static int32_t mag_values[] = {0, 0, 0};

void hmc5883l_init(void) {

  // initializing the i2c hardware
  //TODO: need to somehow check if the i2c is already activated by another component, e.g-mpu6050.c

  // testing responsiveness
  hmc5883l_check_responsiveness();

  // settings the registers
  hmc5883l_set_avg_sampling(HMC5883L_AVG_SAMPLE_8);
  hmc5883l_set_datarate(HMC5883L_DATARATE_15HZ);
  hmc5883l_set_measurement_mode(HMC5883L_MEASUREMENT_MODE_NORMAL);
  hmc5883l_set_gain(HMC5883L_GAIN_1090);
  hmc5883l_set_operating_mode(HMC5883L_OPERATING_MODE_CONTINOUS);

  // calibration
  hmc5883l_calibrate();
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

I2C_AckNak hmc5883l_read_bytes(uint8_t register_to_read, uint8_t* reg_values, uint8_t bytes_num) {

  I2C_AckNak ack_state = i2cStartCommand(HMC5883L_ADDRESS, I2C_WRITE);  // should return I2C_ACK 
  i2cSendByte(register_to_read);
  i2cStartCommand(HMC5883L_ADDRESS, I2C_READ);
  for( ; bytes_num >= 1 ; bytes_num--) {
    *reg_values++ = i2cReadByteSendAck(I2C_ACK);
  }
  *reg_values++ = i2cReadByteSendAck(I2C_NAK);
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

void hmc5883l_print_internal_registers(void) {
  char bin_buf[20];

  for (int i=0; i <= HMC5883L_NUM_REGISTER; i++) {
    hmc5883l_read_byte(i, &reg_value);
    uint8_to_bin_str(reg_value, bin_buf);
    printf("Register %d: 0b%s\n", i, bin_buf);

    delay1ms(20);
  }
}

void hmc5883l_set_avg_sampling(hmc5883l_avg_sample_t hmc5883l_avg_sample) {
  hmc5883l_read_byte(HMC5883L_REG_CONFIG_A, &reg_value);

  reg_value &= ~(0b11 << 5);
  reg_value |= (hmc5883l_avg_sample << 5);

  hmc5883l_write_byte(HMC5883L_REG_CONFIG_A, reg_value);
}

void hmc5883l_set_datarate(hmc5883l_datarate_t hmc5883l_datarate) {
  hmc5883l_read_byte(HMC5883L_REG_CONFIG_A, &reg_value); 

  reg_value &= ~(0b111 << 2);
  reg_value |= (hmc5883l_datarate << 2); 

  hmc5883l_write_byte(HMC5883L_REG_CONFIG_A, reg_value);
}

void hmc5883l_set_measurement_mode(hmc5883l_measurement_mode_t hmc5883l_measurement_mode) {
  hmc5883l_read_byte(HMC5883L_REG_CONFIG_A, &reg_value);

  reg_value &= ~(0b11);
  reg_value |= (hmc5883l_measurement_mode);

  hmc5883l_write_byte(HMC5883L_REG_CONFIG_A, reg_value);
}

void hmc5883l_set_gain(hmc5883l_gain_t hmc5883l_gain) {

  // updating the local variable to have correct multiple for real mag value
  mag_gain_value = hmc5883l_gain;

  hmc5883l_read_byte(HMC5883L_REG_CONFIG_B, &reg_value); 

  reg_value &= ~(0b111 << 5);
  reg_value |= (hmc5883l_gain << 5); 

  hmc5883l_write_byte(HMC5883L_REG_CONFIG_B, reg_value);

}

void hmc5883l_set_operating_mode(hmc5883l_operating_mode_t hmc5883l_operating_mode) {
  hmc5883l_read_byte(HMC5883L_REG_MODE, &reg_value);

  reg_value &= ~(0b11);
  reg_value |= (hmc5883l_operating_mode);

  hmc5883l_write_byte(HMC5883L_REG_MODE, reg_value);

}

void hmc5883l_calibrate(void) {

}
int16_t get_mag_calibration_values(uint8_t ind) { return MAG_OFFSET[ind]; }

I2C_AckNak read_raw_mag(void) {
  
  I2C_AckNak ack_state = hmc5883l_read_bytes(HMC5883L_REG_OUT_X_M, raw_values, 2);
  raw_mag_values[0] = (int16_t)((raw_values[0] << 8) | raw_values[1]);

  ack_state = hmc5883l_read_bytes(HMC5883L_REG_OUT_Z_M, raw_values, 2);
  raw_mag_values[2] = (int16_t)((raw_values[0] << 8) | raw_values[1]);
 
  ack_state = hmc5883l_read_bytes(HMC5883L_REG_OUT_Y_M, raw_values, 2);
  raw_mag_values[1] = (int16_t)((raw_values[0] << 8) | raw_values[1]);

  return ack_state;
}

I2C_AckNak read_mag(void) {

  // reading the raw values
  I2C_AckNak ack_state = read_raw_mag();

  // Apply Offset, multiple by scale value to get fixed-point value (instead of floating-point) then apply scale offset.
  // the accel values are now (x)*scale uT //TODO: check really from the datasheet
  mag_values[0] = (int32_t)(raw_mag_values[0] - MAG_OFFSET[0]) * GAIN_VALUE_MULTIPLE[mag_gain_value] / 1000;
  mag_values[1] = (int32_t)(raw_mag_values[1] - MAG_OFFSET[1]) * GAIN_VALUE_MULTIPLE[mag_gain_value] / 1000;
  mag_values[2] = (int32_t)(raw_mag_values[2] - MAG_OFFSET[2]) * GAIN_VALUE_MULTIPLE[mag_gain_value] / 1000;

  return ack_state;
}
int16_t get_raw_mag(uint8_t ind) { return raw_mag_values[ind]; }
int32_t get_mag(uint8_t ind) { return mag_values[ind]; }

/* int16_t get_mag_yaw_deg(int16_t roll, int16_t pitch) { */

/*   // Convert roll and pitch from degrees to radians */
/*   float rollRad = (float)roll * 0.01745; */
/*   float pitchRad = (float)pitch * 0.01745; */

/*   // Tilt compensation */
/*   float xh = get_mag(0) * cosf(pitchRad) + get_mag(2) * sinf(pitchRad); */
/*   float yh = get_mag(0) * sinf(rollRad) * sinf(pitchRad) + get_mag(1) * cosf(rollRad) - get_mag(2) * sinf(rollRad) * cosf(pitchRad); */

/*     // Heading calculation */
/*   float heading = atan2f(yh, xh); */
/*   /1* float heading = atan2f(get_mag(1), get_mag(0)); *1/ */
/*   // Correct for when signs are reversed. */
/*   if(heading < 0) */
/*     heading += 6.283185307; */
    
/*   // Check for wrap due to addition of declination. */
/*   if(heading > 6.283185307) */
/*     heading -= 6.283185307; */

/*   return (int16_t)(heading * RAD_TO_DEG); */
/* } */
