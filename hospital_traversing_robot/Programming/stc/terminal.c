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
  int16_t int_value = 0;  // temporary int value that gets read from the terminal then assigned to another variable of any other c component

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
        report("Error: >1 Command in 1 line!\n");
        return LINE_FAILED;
    }


    /* [ Step 2: Identify and Initiate command_t variable] */
    switch(letter) {

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

      case 'C':
        command.command_type = COMMAND_CLOSED_LOOP_MOVE;
        break;

      case 'A':
        command.command_type = COMMAND_ADVANCED_MOVEMENT;
        break;

      case 'P':
        command.command_type = COMMAND_PATH_PLAN;
        break;

      case 'E':
        command.command_type = COMMAND_EXECUTE_PATH;
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
        command.j = (int32_t)int_value;
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

        report("Error: 'i' out of range\n");
        report("cm range: >0 && <100\n");
        return LINE_FAILED;

      } else if (command.j < 0 || command.j > 65535) {
        report("Error: 'j' out of range\n");
        report("j is 16-bit PWM value");
          return LINE_FAILED;

      } else if (command.j == 0) {
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

    case COMMAND_MOVE_RIGHT: //TODO: should test for different i values
    case COMMAND_MOVE_LEFT: //TODO: should test for different i values
      if (command.i < -360 || command.i > 360 || command.i == 0) {

        report("Error: 'i' out of range\n");
        report("Range: >-360 && <360 && !=0\n");
        return LINE_FAILED;

      } else if (command.j < 0 || command.j > 65535) {
        report("Error: 'j' out of range\n");
        report("j is 16-bit PWM value");
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
        report("Error: 'i' out of range!\n");
        report("Max GPS data: %d \n", GPS_NUM_DATA);
        return LINE_FAILED;
      }

      break;

    case COMMAND_IMU:
      //TODO: test for max number of i, unfortuantely couldn't get an automated way for the maximum number of IMU i parameters
      break;

    case COMMAND_ULTRASONIC:
      if(command.i > 1 || command.i < -1) {
        report("Error: 'i' Out of range.\n");
        report("i-1 calls hcsr04_stop_cycle()\n");
        report("i0 returns distance & state\n");
        report("i1 calls hcsr04_start_cycle()\n");
        return LINE_FAILED;
      }
      break;

    case COMMAND_CLOSED_LOOP_MOVE:
      if(command.i > 3 || command.i < -1) {
        report("Error: 'i' Out of range.\n");
        report("i-1 reset CL status to idle\n");
        report("i0 returns CL status\n");
        report("i1 starts CL movement of j\n");
        report("i2 sets CL setpoint of j\n");
        report("i3 gets CL setpoint\n");
        return LINE_FAILED;

      } else if (command.i == 1) {
        if (command.j > CLOSED_LOOP_FUNC_NUM || command.j < 0) {
          report("Error: 'j' Out of range.\n");
          report("Closed loop functions:\n");
          report("#TODO\n");
          return LINE_FAILED;
        }

        switch(closed_loop_func_status) {
          case CLOSED_LOOP_MOVEMENT_FAILED:
            report("CLOSED LOOP FUNCTION FAILED!!\n");
            report("Must Ci-1 to reset CL status!\n");
            return LINE_FAILED;

          case CLOSED_LOOP_MOVEMENT_IN_PROGRESS:
            report("CL Func ALREADY IN PROGRESS!\n");
            return LINE_FAILED;

          case CLOSED_LOOP_MOVEMENT_SUCCESS:
            closed_loop_reset_to_idle();
            report("Resetting CL status to idle\n");
            break;
        }

      } else if (command.i == 2) {
        if (command.j > 180 || command.j < (-180)) {
          report("Error: 'j' Out of range!\n");
          report("j: %ld\n", command.j);
          report("WEST value: %d\n", WEST);
          report("NORTH value: %d\n", NORTH);
          report("EAST value: %d\n", EAST);
          report("SOUTH value: %d\n", SOUTH);
          return LINE_FAILED;
        }
      }
      break;

    case COMMAND_ADVANCED_MOVEMENT:
      if(command.i > 1 || command.i < -1) {
        report("Error: 'i' Out of range.\n");
        report("i-1 adv move status to idle\n");
        report("i0 returns adv move status\n");
        report("i1 starts adv move of ind j\n");
        return LINE_FAILED;

      } else if (command.i == 1) {
        if (command.j > ADV_MOVE_FUNC_NUM || command.j < 0) {
          report("Error: 'j' Out of range.\n");
          report("Advanced Movement functions:\n");
          report("#TODO\n");
          return LINE_FAILED;
        }
        switch(adv_move_func_status) {
          case ADV_MOVE_FAILED:
            report("ADV MOVE FUNCTION FAILED!!\n");
            report("Must Ai-1 to reset status!\n");
            return LINE_FAILED;

          case ADV_MOVE_START:
          case ADV_MOVE_IN_PROGRESS:
            report("Adv Move ALREADY IN PROGRESS!\n");
            return LINE_FAILED;

          case ADV_MOVE_IDLE:
            break;

          case ADV_MOVE_SUCCESS:
            adv_move_func_status = ADV_MOVE_IDLE;
            report("Resetting adv move to idle\n");
            break;
        }
      }

      break;

    case COMMAND_PATH_PLAN:
      //TODO: V.IMP add an option to draw from current position to wanted. for example if i==-1 then from whatever last saved current location to j value
      if (command.i > LOCATION_COUNT || command.j > LOCATION_COUNT || command.i < 0 || command.j < 0) {
        report("Error: location ind: 0-%d\n", LOCATION_COUNT-1);
        return LINE_FAILED;
      }
      break;
    
    case COMMAND_EXECUTE_PATH:
      if(command.i > 1 || command.i < -1) {
        report("Error: 'i' out of range\n");
        report("i-1 Stops Path Execution\n");
        report("i0 Path Execution State\n");
        report("i1 Starts Path Execution\n");
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
      break;

    case COMMAND_MOVE_FORWARD:
      differential_control_set_movement((uint8_t)command.i, (uint16_t)command.j, DIFFERENTIAL_MOVE_FORWARD);
      report("Forward: %d duty: %ld\n", command.i, command.j);
      break;

    case COMMAND_MOVE_BACKWARD:
      differential_control_set_movement((uint8_t)command.i, (uint16_t)command.j, DIFFERENTIAL_MOVE_BACKWARD);
      report("Backward: %d duty: %ld\n", command.i, command.j);
      break;

    case COMMAND_MOVE_RIGHT:
      differential_control_set_movement((uint8_t)command.i, (uint16_t)command.j, DIFFERENTIAL_MOVE_RIGHT);
      report("Right: %d duty: %ld\n", command.i, command.j);
      break;

    case COMMAND_MOVE_LEFT:
      differential_control_set_movement((uint8_t)command.i, (uint16_t)command.j, DIFFERENTIAL_MOVE_LEFT);
      report("Left: %d duty: %ld\n", command.i, command.j);
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
          report("Roll Angle: %ddeg\n", orientation_get_roll_deg());
          break;
        case 8:
          report("Pitch Angle: %ddeg\n", orientation_get_pitch_deg());
          break;
        case 9:
          report("Yaw Angle: %ddeg\n",  orientation_get_yaw_deg());
          break;
        case 10:
          orientation_unlock_yaw_measurement();
          report("Unlocking Yaw summing from gyro!\n");
          break;
        case 11:
          orientation_lock_yaw_measurement();
          report("locking Yaw summing from gyro!\n");
          break;
        case 12:
          orientation_set_gyro_yaw(STARTING_YAW_VALUE);
          report("Yaw Value is now Reset!\n");
        case 13:
          mpu6050_print_internal_registers();
          break;
        case 14:
          hmc5883l_print_internal_registers();
          break;

/* ("ACCEL X: %ldg\nACCEL Y: %ldg\nACCEL Z: %ldg\nGYRO X: %lddeg/sec\nGYRO Y: %lddeg/sec\nGYRO Z: %lddeg/sec\nMAG X: %lduT\nMAG Y: %lduT\nMAG Z: %lduT\n"),get_accel(0), get_accel(1), get_accel(2), get_gyro(0), get_gyro(1), get_gyro(2), get_mag(0), get_mag(1), get_mag(2), */
        default:
          report("Roll: %d\nPitch: %d\nYaw: %d\n", orientation_get_roll_deg(), orientation_get_pitch_deg(), orientation_get_yaw_deg());
      }

      break;

    case COMMAND_ULTRASONIC:
      switch(command.i) {
        case -1: {
          HCSR04_CYCLE_FUNC_RESPONSE hcsr04_cycle_command_response = hcsr04_stop_cycle();
          if(hcsr04_cycle_command_response == HCSR04_STOP_CYCLE_OK) {
            report("HCSR04 Ultrasonic Cycle Successfully Stopped!\n");
          } else if (hcsr04_cycle_command_response == HCSR04_STOP_CYCLE_ALREADY_IDLE){
            report("HCSR04 Ultrasonic Cycle is already Stopped!\n");
          }
          break;
        }

        case 1: {
          HCSR04_CYCLE_FUNC_RESPONSE hcsr04_cycle_command_response = hcsr04_start_cycle();
          if(hcsr04_cycle_command_response == HCSR04_START_CYCLE_OK) {
            report("HCSR04 Ultrasonic Cycle Successfully Started!\n");
          } else if (hcsr04_cycle_command_response == HCSR04_START_CYCLE_ALREADY_ACTIVE){
            report("HCSR04 Ultrasonic Cycle is already Active!\n");
          }
          break;
        }

        default: { //i==0, tested in testing phase
          HCSR04_STATUS hcsr04_status = get_hcsr04_status();

          if(hcsr04_status != HCSR04_ACTIVE) {
            report("HCSR04 State: %s, HCSR04 Phase: %s\n", HCSR04_STATUS_TO_STRING[hcsr04_status], HCSR04_PHASE_TO_STRING[get_hcsr04_phase()]);
          } else {
            report("Distance: %d (HCSR04 Machine State: HCSR04_ACTIVE)\n", get_ultrasonic_distance_cm());
          }
        }
      }

      break;

    case COMMAND_CLOSED_LOOP_MOVE:
      switch(command.i) {
        case -1:
          // reset closed loop status
          closed_loop_reset_to_idle();
          report("CL reset to IDLE\n");
          break;

        case 0:
          // return current closed loop status
          report("Closed loop control status: ");
          report("%s\n", CLOSED_LOOP_STATUS_TO_STRING[closed_loop_func_status]);
          break;

        case 1:
          switch(command.j) {
            case 0:
              report("Exec Closed loop Move\n");
              closed_loop_current_func = closed_loop_move;
              break;

            case 1:
              report("Exec Closed loop orient\n");
              closed_loop_current_func = closed_loop_orient;
              break;
          }
          break;

        case 2:
          closed_loop_set_setpoint((int16_t)command.j);
          report("New Closed loop Setpoint: %ld\n", command.j);
          break;

        case 3:
          report("Closed loop setpoint: %d\n", closed_loop_get_setpoint());
          break;

      }
      break;

    case COMMAND_ADVANCED_MOVEMENT:
      switch(command.i) {
        case -1:
          // reset closed loop status
          adv_move_func_status = ADV_MOVE_IDLE;
          report("Adv Move reset to IDLE\n");
          break;

        case 0:
          // return current closed loop status
          report("Adv Move control status: ");
          report("%s\n", ADV_MOVE_STATUS_TO_STRING[adv_move_func_status]);
          break;

        case 1:
          adv_move_func_status = ADV_MOVE_START;
          switch(command.j) {
            case 0:
              report("Exec Adv Move exit room\n");
              break;

            case 1:
              report("Exec Adv Move enter room\n");
              break;

            case 2:
              report("Exec Adv Move north\n");
              break;

            case 3:
              report("Exec Adv Move east\n");
              break;

            case 4:
              report("Exec Adv Move west\n");
              break;

            case 5:
              report("Exec Adv Move south\n");
              break;

            case 6:
              report("Exec Adv Move stairs up\n");
              break;

            case 7:
              report("Exec Adv Move stairs down\n");
              break;
          }
          break;
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

    case COMMAND_EXECUTE_PATH:
      switch(command.i) {
        case -1:
          execute_path_stop();
          report("Path Execution Stopped!\n");
          break;

        case 0:
          report("Path Execution Current Status: ");
          report("%s\n", EXECUTE_PATH_STATUS_TO_STRING[execute_path_get_status()]);
          break;

        case 1:
          execute_path_start();
          report("Path Execution Starting..\n");
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
