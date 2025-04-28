#ifndef HMC5883L_H
#define HMC5883L_H

#define HMC5883L_ADDRESS              0x1E

#define HMC5883L_NUM_REGISTER         12
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

typedef enum {
    HMC5883L_AVG_SAMPLE_1 = 0b00,
    HMC5883L_AVG_SAMPLE_2 = 0b10,
    HMC5883L_AVG_SAMPLE_4 = 0b01,
    HMC5883L_AVG_SAMPLE_8 = 0b11
} hmc5883l_avg_sample_t;

typedef enum {
    HMC5883L_DATARATE_0_75_HZ    = 0b000,
    HMC5883L_DATARATE_1_5HZ      = 0b001,
    HMC5883L_DATARATE_3HZ        = 0b010,
    HMC5883L_DATARATE_7_5HZ      = 0b011,
    HMC5883L_DATARATE_15HZ       = 0b100,
    HMC5883L_DATARATE_30HZ       = 0b101,
    HMC5883L_DATARATE_75HZ       = 0b110
} hmc5883l_datarate_t;

typedef enum {
    HMC5883L_MEASUREMENT_MODE_NORMAL         = 0b00,
    HMC5883L_MEASUREMENT_MODE_POSITIVE_BIAS  = 0b01,
    HMC5883L_MEASUREMENT_MODE_NEGATIVE_BIAS  = 0b10
} hmc5883l_measurement_mode_t;

typedef enum {
    HMC5883L_GAIN_1370 = 0b000,        /*0- gain +/- 0.88Ga */
    HMC5883L_GAIN_1090 = 0b001,        /*1- gain +/- 1.3 Ga */
    HMC5883L_GAIN_820  = 0b010,        /*2- gain +/- 1.9 Ga */
    HMC5883L_GAIN_660  = 0b011,        /*3- gain +/- 2.5 Ga */
    HMC5883L_GAIN_440  = 0b100,        /*4- gain +/- 4.0 Ga */
    HMC5883L_GAIN_390  = 0b101,        /*5- gain +/- 4.7 Ga */
    HMC5883L_GAIN_330  = 0b110,        /*6- gain +/- 5.6 Ga */
    HMC5883L_GAIN_230  = 0b111,        /*7- gain +/- 8.1 Ga */
} hmc5883l_gain_t;

typedef enum {
  HMC5883L_OPERATING_MODE_CONTINOUS   = 0b00,
  HMC5883L_OPERATING_MODE_SINGLE      = 0b01,
  HMC5883L_OPERATING_MODE_IDLE        = 0b10
} hmc5883l_operating_mode_t;

void hmc5883l_init(void);
I2C_AckNak hmc5883l_write_byte(uint8_t register_to_write, uint8_t value);
I2C_AckNak hmc5883l_read_byte(uint8_t register_to_read, uint8_t* reg_value);
I2C_AckNak hmc5883l_read_bytes(uint8_t register_to_read, uint8_t* reg_values, uint8_t bytes_num);
void hmc5883l_check_responsiveness(void);
void hmc5883l_print_internal_registers(void);
void hmc5883l_set_avg_sampling(hmc5883l_avg_sample_t hmc5883l_avg_sample);
void hmc5883l_set_datarate(hmc5883l_datarate_t hmc5883l_datarate);
void hmc5883l_set_measurement_mode(hmc5883l_measurement_mode_t hmc5883l_measurement_mode);
void hmc5883l_set_gain(hmc5883l_gain_t hmc5883l_gain);
void hmc5883l_set_operating_mode(hmc5883l_operating_mode_t hmc5883l_operating_mode);
void hmc5883l_calibrate(void);
int16_t get_mag_calibration_values(uint8_t ind);
I2C_AckNak read_raw_mag(void);
I2C_AckNak read_mag(void);
int16_t get_raw_mag(uint8_t ind);
int32_t get_mag(uint8_t ind);

#endif
