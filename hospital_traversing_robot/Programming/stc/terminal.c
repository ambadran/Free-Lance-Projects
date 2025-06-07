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
      report("\rExpected a letter!\n"); 
      return LINE_FAILED;
    }

    // small letter case is not a command, it's parameter
    if (!(letter >= 'a' && letter < 'z') && \
        (command.command_type != COMMAND_NOT_SET)) {
        report("Can't have >1 command letter in one command!\n");
        return LINE_FAILED;
    }


    /* [ Step 2: Identify and Initiate command_t variable] */
    switch(letter) {

      case 'A':
        // testing reading int argument
        if (!read_int(line, &char_count, &int_value)) {
          report("Bad integer Number Format\n");
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

      case 'U':
        command.command_type = COMMAND_ULTRASONIC;
        break;

      case 'P':
        command.command_type = COMMAND_PATH_PLAN;
        break;

      case 'i':
        // reading int argument for a multi-argument command
        if (!read_int(line, &char_count, &int_value)) {
          report("Bad int Number Format\n");
          return LINE_FAILED;
        }
        command.i = int_value;
        break;

      case 'j':
        // reading int argument for a multi-argument command
        if (!read_int(line, &char_count, &int_value)) {
          report("Bad int Number Format\n");
          return LINE_FAILED;
        }
        command.j = int_value;
        break;

      default:
        report("Command Letter Not Implemented\n");
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
      if (command.i <= 0 || command.i > 100) {

        report("Error: Parameter 'i' out of range. Forward/Backward cm value >0 and <100\n");
        return LINE_FAILED;

      } else if (command.j == 0) {
        //IMP: support for default PWM duty cycle 'j' value
        // since duty cycle = 0 means no movement, so it makes no sense
        // Also if pwm signal needs to be inverted, the advpwm lib supports that!
        command.j = DEFAULT_PWM_DUTY_CYCLE; 
      }
      break;

    case COMMAND_MOVE_RIGHT: //TODO: should test for different i values
    case COMMAND_MOVE_LEFT: //TODO: should test for different i values
      if (command.i < -360 || command.i > 360 || command.i == 0) {

        report("Error Parameter 'i' out of range. Right/Left degree value >-360 && <360 && !=0\n");
        return LINE_FAILED;

      }  else if (command.j == 0) {
        //IMP: support for default PWM duty cycle 'j' value
        // since duty cycle = 0 means no movement, so it makes no sense
        // Also if pwm signal needs to be inverted, the advpwm lib supports that!
        command.j = DEFAULT_PWM_DUTY_CYCLE; 
      }

      if (differential_control_is_moving()) {
        report("Robot ALready Moving!\n");
        return LINE_FAILED;
      }

      break;

    case COMMAND_GPS:
      if(command.i >= GPS_NUM_DATA) {
        report("Error: only %d data supported\n", GPS_NUM_DATA);
        return LINE_FAILED;
      }

      break;

    case COMMAND_IMU:
      //TODO: test for max number of i, unfortuantely couldn't get an automated way for the maximum number of IMU i parameters
      break;

    case COMMAND_ULTRASONIC:
      if(command.i > 1 || command.i < -1) {
        report("Error: 'i' Out of range.\ni==-1 calls hc05_stop_cycle()\ni==0 returns latest distance and HC05 state\ni==1 calls hc05_start_cycle()\n");
      }
      break;

    case COMMAND_PATH_PLAN:
      //TODO: V.IMP add an option to draw from current position to wanted. for example if i==-1 then from whatever last saved current location to j value
      if (command.i > LOCATION_COUNT || command.j > LOCATION_COUNT) {
        report("Error: invalid location index, max location index is %d", LOCATION_COUNT-1);
        return LINE_FAILED;
      }
      break;

    default:
      // if the command_type is not set (COMMAND_NOT_SET).
      // then a correct command wasn't passed in Step2.
      // which means that the default: of Step2 should run
      // so WTF?!??!
      report("SHOULD NEVER REACH HERE IN ERROR CHECKING.\n");
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
      report("Forward: %d @ freq: %u\n", command.i, command.j);
      break;

    case COMMAND_MOVE_BACKWARD:
      differential_control_backward((uint8_t)command.i, command.j);
      report("Backward: %d @ freq: %u\n", command.i, command.j);
      break;

    case COMMAND_MOVE_RIGHT:
      differential_control_right((uint8_t)command.i, command.j);
      report("Right: %d @ freq: %u\n", command.i, command.j);
      break;

    case COMMAND_MOVE_LEFT:
      differential_control_left((uint8_t)command.i, command.j);
      report("Left: %d @ freq: %u\n", command.i, command.j);
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
      switch(command.i) {
        case 1:
          report("ACCEL X: %ldg\nACCEL Y: %ldg\nACCEL Z: %ldg\n", get_accel(0), get_accel(1), get_accel(2));
          break;
        case 2:
          report("GYRO X: %lddeg/sec\nGYRO Y: %lddeg/sec\nGYRO Z: %lddeg/sec\n", get_gyro(0), get_gyro(1), get_gyro(2));
          break;
        case 3:
          report("MAG X: %lddeg/sec\nMAG Y: %lddeg/sec\nMAG Z: %lddeg/sec\n", get_mag(0), get_mag(1), get_mag(2));
          break;
        case 4:
          report("Accel offset x: %d\nAccel offset y: %d\nAccel offset z: %d\n", get_accel_calibration_values(0), get_accel_calibration_values(1), get_accel_calibration_values(2));
          break;
        case 5:
          report("Gyro offset x: %d\nGyro offset y: %d\nGyro offset z: %d\n", get_gyro_calibration_values(0), get_gyro_calibration_values(1), get_gyro_calibration_values(2));
          break;
        case 6:
          report("Mag offset x: %d\nMag offset y: %d\nMag offset z: %d\n", get_mag_calibration_values(0), get_mag_calibration_values(1), get_mag_calibration_values(2));
          break;
        case 7:
          //TODO: test mpu6050 device status, is it active?
          report("Roll Angle: %ddeg\n", get_compl_roll_deg());
          break;
        case 8:
          //TODO: test mpu6050 device status, is it active?
          report("Pitch Angle: %ddeg\n", get_compl_pitch_deg());
          break;
        case 9:
          //TODO: test hmc5883l device status, is it active?
          report("Yaw Angle: %ddeg\n",  get_compl_yaw_deg());
          break;
        case 10:
          mpu6050_print_internal_registers();
          break;
        case 11:
          hmc5883l_print_internal_registers();
          break;

/* ("ACCEL X: %ldg\nACCEL Y: %ldg\nACCEL Z: %ldg\nGYRO X: %lddeg/sec\nGYRO Y: %lddeg/sec\nGYRO Z: %lddeg/sec\nMAG X: %lduT\nMAG Y: %lduT\nMAG Z: %lduT\n"),get_accel(0), get_accel(1), get_accel(2), get_gyro(0), get_gyro(1), get_gyro(2), get_mag(0), get_mag(1), get_mag(2), */
        default:
          report("Roll: %d\nPitch: %d\nYaw: %d\n", get_compl_roll_deg(), get_compl_pitch_deg(), get_compl_yaw_deg());
      }

      break;

    case COMMAND_ULTRASONIC:
      HC05_CYCLE_FUNC_RESPONSE hc05_cycle_command_response;
      switch(command.i) {
        case -1:
          hc05_cycle_command_response = hc05_stop_cycle();

          if(hc05_cycle_command_response == HC05_STOP_CYCLE_OK) {
            report("HC05 Ultrasonic Cycle Successfully Stopped!\n");
          } else if (hc05_cycle_command_response == HC05_STOP_CYCLE_ALREADY_IDLE){
            report("HC05 Ultrasonic Cycle is already Stopped!\n");
          }
          break;

        case 1:
          hc05_cycle_command_response = hc05_start_cycle();

          if(hc05_cycle_command_response == HC05_START_CYCLE_OK) {
            report("HC05 Ultrasonic Cycle Successfully Started!\n");
          } else if (hc05_cycle_command_response == HC05_START_CYCLE_ALREADY_ACTIVE){
            report("HC05 Ultrasonic Cycle is already Active!\n");
          }
          break;

        default: //i==0, tested in testing phase
          HC05_STATUS hc05_status = get_hc05_status();

          if(hc05_status != HC05_ACTIVE) {
            report("Current HC05 machine State: %s\n", HC05_STATUS_TO_STRING[hc05_status]);
          } else {
            report("Distance: %d (HC05 Machine State: HC05_ACTIVE)\n", get_ultrasonic_distance_cm());
          }
      }

      break;

    case COMMAND_PATH_PLAN:
      //TODO: V.IMP add an option to draw from current position to wanted. for example if i==-1 then from whatever last saved current location to j value
      if(find_path(command.i, command.j) == PATH_FOUND) {
        print_path();
      } else {
        report("No Path was found!");
      }
      break;

    default:
      // if the command_type is not set (COMMAND_NOT_SET).
      // then a correct command wasn't passed in Step2.
      // which means that the default: of Step2 should run
      // so WTF?!??!
      report("SHOULD NEVER REACH HERE.");
      return LINE_FAILED;
  }

  return LINE_PASSED;
 
}
