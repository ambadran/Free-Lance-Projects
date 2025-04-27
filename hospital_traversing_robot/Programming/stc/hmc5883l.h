#ifndef HMC5883L_H
#define HMC5883L_H

// #define HMC5883L_ADDRESS              0x1E  // the datasheet says that there are 2 i2c addresses?!
#define HMC5883L_ADDRESS              0x1E

// #define HMC5883L_ADDRESS_WRITE        0x3C
// #define HMC5883L_ADDRESS_READ         0x3D

#define HMC5883L_REG_CONFIG_A         0x00
#define HMC5883L_REG_CONFIG_B         0x01
#define HMC5883L_REG_MODE             0x02
#define HMC5883L_REG_OUT_X_M          0x03
#define HMC5883L_REG_OUT_X_L          0x04
#define HMC5883L_REG_OUT_Z_M          0x05
#define HMC5883L_REG_OUT_Z_L          0x06
#define HMC5883L_REG_OUT_Y_M          0x07
#define HMC5883L_REG_OUT_Y_L          0x08
#define HMC5883L_REG_STATUS           0x09
#define HMC5883L_REG_IDENT_A          0x0A
#define HMC5883L_REG_IDENT_B          0x0B
#define HMC5883L_REG_IDENT_C          0x0C

/* Responses */
#define HMC5883L_REG_IDENT_A_RESPONSE 0x48
#define HMC5883L_REG_IDENT_B_RESPONSE 0x34
#define HMC5883L_REG_IDENT_C_RESPONSE 0x33

void hmc5883l_init(void);
I2C_AckNak hmc5883l_write_byte(uint8_t register_to_write, uint8_t value);
I2C_AckNak hmc5883l_read_byte(uint8_t register_to_read, uint8_t* reg_value);
void hmc5883l_check_responsiveness(void);

#endif
