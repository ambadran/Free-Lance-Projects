#include "project-defs.h"

static command_t command;
gps_data_t gps_data;

// reads int only not floats and increments the char_count to the index of after the number
bool read_int(char *line, uint8_t* char_count, int16_t* int_value_ptr) {

  char *ptr = line + *char_count; // the variable that will extract the next digit from
  unsigned char c; // the variable that will hold each digit

  c = *ptr++;  // extracting the next digit!

  // catching negative values :)
  bool isnegative = false;
  if (c == '-') {
    isnegative = true;
    c = *ptr++;

  // incrementing pointer anyway if +ve sign is there
  } else if (c == '+') {
    c = *ptr++;
  }

  // Algorithm to convert char digits to int
  uint32_t intval = 0;
  uint8_t ndigit = 0;  // current digit read index
  int8_t exp = 0;  // NOT uint8_t as this must hold +ve and -ve numbers
  while (1) {

    c -= '0';
    if (c <= 9) {
      ndigit++;
      if (ndigit <= MAX_INT_DIGITS) {

        intval = (((intval << 2) + intval) << 1) + c; // intval*10 + c
                                                      //
      } else {

        exp++;  // Drop overflow digits
      }

    } else {
      // encountered a non integer character
      break;
    }
    c = *ptr++;
  }

  // Return if no digits have been read.
  if (!ndigit) { return(false); };

  long int lival;
  lival = (long int)intval;
  
  if (exp > 0) {
    do {
      lival *= 10.0;
    } while (--exp > 0);
  } 

  // Assign floating point value with correct sign.    
  if (isnegative) {
    *int_value_ptr = -lival;
  } else {
    *int_value_ptr = lival;
  }

  *char_count = ptr - line - 1; // Set char_counter to next statement
 
  return (true);
}


LINE_STATUS terminal_execute_line(char* line) {

  uint8_t char_count = 0;
  char letter;
  uint16_t int_value = 0;  // temporary int value that gets read from the terminal then assigned to another variable of any other c component

  // Resetting the command
  memset(&command, 0, sizeof(command_t));

  while (line[char_count] != 0) {

    /* [ Step 1: Capture read character and type check] */
    // Reading letter argument, aka which function to execute
    letter = line[char_count];

    // incrementing char_count for next iteration
    char_count++;

    // IMP: capital letters are command letters
    //      small letters are argument letters
    // Checking letter is a letter
    // checking if it's before A or after z or any character in between Z and a in the ascii table
    if ( letter < 'A' || letter > 'z' || (letter > 'Z' && letter < 'a')) {
      printf("\rExpected a letter!\n"); 
      return LINE_FAILED;
    }

    // small letter case is not a command, it's parameter
    if (!(letter >= 'a' && letter < 'z') && \
        (command.command_type != COMMAND_NOT_SET)) {
        printf("Can't have >1 command letter in one command!\n");
        return LINE_FAILED;
    }


    /* [ Step 2: Identify and Initiate command_t variable] */
    switch(letter) {

      case 'A':
        // testing reading int argument
        if (!read_int(line, &char_count, &int_value)) {
          printf("Bad integer Number Format\n");
          return LINE_FAILED;

        } 
        command.command_type = COMMAND_TEST_INT_READING;
        break;

      case 'N':
        command.command_type = COMMAND_GET_NRF24_REGISTERS;
        break;

      case 'T':
        command.command_type = COMMAND_GET_CURRENT_TIME;
        break;

      case 'F':
        command.command_type = COMMAND_MOVE_FORWARD;
        break;

       case 'B':
        command.command_type = COMMAND_MOVE_BACKWARD;
        break;
  
       case 'R':
        command.command_type = COMMAND_MOVE_RIGHT;
        break;

       case 'L':
        command.command_type = COMMAND_MOVE_LEFT;
        break;

      case 'G':
        command.command_type = COMMAND_GPS;
        break;

      case 'M':
        command.command_type = COMMAND_IMU;
        break;

      case 'i':
        // reading int argument for a multi-argument command
        if (!read_int(line, &char_count, &int_value)) {
          printf("Bad int Number Format\n");
          return LINE_FAILED;
        }
        command.i = int_value;
        break;

      case 'j':
        // reading int argument for a multi-argument command
        if (!read_int(line, &char_count, &int_value)) {
          printf("Bad int Number Format\n");
          return LINE_FAILED;
        }
        command.j = int_value;
        break;

      default:
        printf("Command Letter Not Implemented\n");
        return LINE_FAILED;
    }
  }

  /* [ Step 3: Error checking ] */
  switch(command.command_type) {

    case COMMAND_TEST_INT_READING:
    case COMMAND_GET_CURRENT_TIME:
    case COMMAND_GET_NRF24_REGISTERS:
      break;

    case COMMAND_MOVE_FORWARD:
    case COMMAND_MOVE_BACKWARD:
    case COMMAND_MOVE_RIGHT: //TODO: should test for different i values
    case COMMAND_MOVE_LEFT: //TODO: should test for different i values
      if (command.i <= 0 || command.i > 100) {

        printf("Distance Parameter 'i' out of range!\n");
        return LINE_FAILED;

      }  else if (command.j == 0) {
        //IMP: support for default PWM duty cycle 'j' value
        // since duty cycle = 0 means no movement, so it makes no sense
        // Also if pwm signal needs to be inverted, the advpwm lib supports that!
        command.j = DEFAULT_PWM_DUTY_CYCLE; //TODO: replace all these constant with #defs
      }

      if (differential_control_is_moving()) {
        printf("Robot ALready Moving!\n");
        return LINE_FAILED;
      }

      // Passed
      break;

    case COMMAND_GPS:
      if(command.i >= GPS_NUM_DATA) {
        printf("Error: only %d data supported\n", GPS_NUM_DATA);
        return LINE_FAILED;
      }

      break;

    case COMMAND_IMU:
      break;

    default:
      // if the command_type is not set (COMMAND_NOT_SET).
      // then a correct command wasn't passed in Step2.
      // which means that the default: of Step2 should run
      // so WTF?!??!
      printf("SHOULD NEVER REACH HERE IN ERROR CHECKING.\n");
      return LINE_FAILED;
  }

  /* [ Step 4: Execute! ] */
  switch(command.command_type) {

    case COMMAND_TEST_INT_READING:
      report("Read INT value: %d\n", int_value);
      break;

    case COMMAND_GET_NRF24_REGISTERS:
      nrf24_print_internal_register_values();
      break;

    case COMMAND_GET_CURRENT_TIME:
      report("Current Time Passed: %lu\n", get_current_time());
      /* report("testing!\n"); */
      break;

    case COMMAND_MOVE_FORWARD:
      differential_control_forward((uint8_t)command.i, command.j);
      printf("Forward: %d @ freq: %u\n", command.i, command.j);
      break;

    case COMMAND_MOVE_BACKWARD:
      differential_control_backward((uint8_t)command.i, command.j);
      printf("Backward: %d @ freq: %u\n", command.i, command.j);
      break;

    case COMMAND_MOVE_RIGHT:
      differential_control_right((uint8_t)command.i, command.j);
      printf("Right: %d @ freq: %u\n", command.i, command.j);
      break;

    case COMMAND_MOVE_LEFT:
      differential_control_left((uint8_t)command.i, command.j);
      printf("Left: %d @ freq: %u\n", command.i, command.j);
      break;

    case COMMAND_GPS:
      neo_m8n_read_statement(&gps_data);
      switch(command.i) {
        case 1:
          report("latitude: %s\n", gps_data.latitude);
          break;

        case 2:
          report("longitude: %s\n", gps_data.longitude);
          break;

        case 3:
          report("heading: %s\n", gps_data.heading);
          break;

        case 4:
          report("time: %s\n", gps_data.time);
          break;

        default: // case 0
          report("latitude: %s\nlongitude: %s\nheading: %s\ntime: %s\n", gps_data.latitude, gps_data.longitude, gps_data.heading, gps_data.time);
      }
      break;

    case COMMAND_IMU:
      read_accel();  //TODO: remove after implementing period check
      read_gyro();  //TODO: remove after implementing period check
      switch(command.i) {
        case 1:
          report("ACCEL X: %ldg\n", get_accel(0));
          break;
        case 2:
          report("ACCEL Y: %ldg\n", get_accel(1));
          break;
        case 3:
          report("ACCEL Z: %ldg\n", get_accel(2));
          break;
        case 4:
          report("GYRO X: %lddeg/sec\n", get_gyro(0));
          break;
        case 5:
          report("GYRO Y: %lddeg/sec\n", get_gyro(1));
          break;
        case 6:
          report("GYRO Z: %lddeg/sec\n", get_gyro(2));
          break;
        case 7:
          report("Gyro offset x: %d\nGyro offset y: %d\nGyro offset z: %d\n", get_gyro_calibration_values(0), get_gyro_calibration_values(1), get_gyro_calibration_values(2));
          break;

        case 8:
          report("Tilt x: %ld\n", get_tilt_x());
          break;
          
        case 9:
          report("Tilt y: %ld\n", get_tilt_y());
          break;

        default:
          report("ACCEL X: %ldg\nACCEL Y: %ldg\nACCEL Z: %ldg\nGYRO X: %lddeg/sec\nGYRO Y: %lddeg/sec\nGYRO Z: %lddeg/sec\nTilt x: %ld\nTilt y: %ld\n", get_accel(0), get_accel(1), get_accel(2), get_gyro(0), get_gyro(1), get_gyro(2), get_tilt_x(), get_tilt_y());
      }

      break;

    default:
      // if the command_type is not set (COMMAND_NOT_SET).
      // then a correct command wasn't passed in Step2.
      // which means that the default: of Step2 should run
      // so WTF?!??!
      printf("SHOULD NEVER REACH HERE.");
      return LINE_FAILED;
  }

  return LINE_PASSED;
 
}
