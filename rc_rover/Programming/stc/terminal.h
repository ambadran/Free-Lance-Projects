#ifndef TERMINAL_H
#define TERMINAL_H

#define MAX_INT_DIGITS 8

typedef enum {
  COMMAND_NON_MODAL,
  COMMAND_MODAL
} command_modal_status_t;

typedef enum {
  COMMAND_NOT_SET = 0,
  COMMAND_TEST_INT_READING,
  COMMAND_TOGGLE_LED,
  COMMAND_GET_NRF24_REGISTERS,
  COMMAND_GET_CURRENT_TIME,
  COMMAND_MOVE_FORWARD,
  COMMAND_MOVE_BACKWARD,
  COMMAND_MOVE_RIGHT,
  COMMAND_MOVE_LEFT,
  COMMAND_GPS_LONGITUDE,
  COMMAND_GPS_LATITUDE,
  COMMAND_IMU_X_ORIENTATION,
  COMMAND_IMU_Y_ORIENTATION,
  COMMAND_IMU_Z_ORIENTATION,
  COMMAND_ULTRASONIC_DISTANCE
} command_type_t;

typedef struct {
  command_type_t command_type;
  uint16_t i;
  uint16_t j;
  command_modal_status_t command_modal_status;
} command_t;

typedef enum {
  LINE_PASSED, 
  LINE_FAILED
} LINE_STATUS;

bool read_int(char *line, uint8_t *char_count, int16_t *value_ptr);
LINE_STATUS terminal_execute_line(char* line);

#endif
