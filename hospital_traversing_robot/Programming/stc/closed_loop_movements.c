#include "project-defs.h"
//TODO: remove this
uint8_t temp_counter = 1;


const char* CLOSED_LOOP_STATUS_TO_STRING[] = {
  "CLOSED_LOOP_MOVE_IDLE",
  "CLOSED_LOOP_MOVE_FAILED",
  "CLOSED_LOOP_MOVE_IN_PROGRESS",
  "CLOSED_LOOP_MOVE_SUCCESS" 
};

/* Current closed loop control status */
volatile closed_loop_func_status_t closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IDLE;
closed_loop_fail_status_t closed_loop_fail_status = CL_FAIL_NONE;
/* Current running closed loop function */
closed_loop_movement_func_t closed_loop_current_func = closed_loop_move_idle;

// Internal variables
static int16_t setpoint = 0;
static uint32_t operation_start_time = 0;
static uint32_t last_input_check_time = 0;
static int16_t last_input_value = 0;
static int8_t expected_direction = 0;  // 1 = CW, -1 = CCW, 0 = not set


void closed_loop_reset_to_idle(void) {
  closed_loop_current_func = closed_loop_move_idle;
  closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IDLE;
  closed_loop_fail_status = CL_FAIL_NONE;
}

void closed_loop_move_idle(void) {}

void closed_loop_set_setpoint(int16_t setpoint_value) { setpoint = setpoint_value; }
direction_t closed_loop_get_setpoint(void) { return setpoint; }

/* Master closed loop control algorithm to change orientation
 *  - INPUT: orientation_get_yaw_deg
 *  - OUTPUT: differential_control_left OR differential_control_right
 *
 * 1- sets open loop differential_control_left/right with overshooted values with default PWM
 * 2- Calls open loop differential_control_left/right in case of overshoot to correct error
 * 3- Fails if motors are already moving
 * 4- Fails if movement causes wrong yaw direction change 
 * 5- Fails unless there is indeed a MINIMUM_YAW_CHANGE happenning every STUCK_ORIENT_TIMEOUT_MS
 * 6- Fails the function in case of ABSOLUTE timeout from moment function began running
 * 7- Succeed if current current yaw value is within yaw setpoint tolerance value 
 *
 * function will only run if closed_loop_current_func is set to closed_loop_orient 
 * AND if closed_loop_func_status is at IDLE. 
 * closed_loop_func_status is NOT RESET AUTOMATICALLY BY CODE after successful or failed execution.
 * This should be done by the code that calls sets the function.
 * LIKE IN terminal.c and path_planning.c, if status is SUCCESS, it will automatically move to IDLE
 */
/* void closed_loop_orient(void) { */
/*   switch(closed_loop_func_status) { */
/*     case CLOSED_LOOP_MOVEMENT_IDLE: { */

/*       if (!differential_control_is_moving()) { */
/*         orientation_unlock_yaw_measurement(); */
/*         operation_start_time = get_current_time(); */
/*         closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS; */

/*       } else { */
/*         closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED; */
/*         closed_loop_fail_status = CL_FAIL_MOTOR_ALREADY_MOVING; */
/*         report("CL_FAIL_MOTOR_ALREADY_MOVING\n"); */
/*       } */

/*       break; */
/*     } */

/*     case CLOSED_LOOP_MOVEMENT_IN_PROGRESS: { */

/*       int16_t current_yaw = orientation_get_yaw_deg(); */
/*       uint32_t current_time = get_current_time(); */

/*       // Check total operation timeout */
/*       if ((current_time - operation_start_time) > TOTAL_ORIENT_TIMEOUT_MS) { */
/*         differential_control_stop(); */
/*         orientation_lock_yaw_measurement(); */
/*         closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED; */
/*         closed_loop_fail_status = CL_FAIL_MOVEMENT_TIMEOUT; */
/*         report("CL_FAIL_MOVEMENT_TIMEOUT\n"); */
/*         break; */
/*       } */

/*       // Calculate normalized error (-180 to 180) with shortest path in mind (180 - -90 -> -90 not 270) */
/*       int16_t error = angle_diff_normalized(setpoint, current_yaw); */

/*       // Check if target reached */
/*       if (abs(error) <= DEG_TOLERANCE) { */
/*           differential_control_stop(); */
/*           orientation_lock_yaw_measurement(); */
/*           closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS; */
/*           closed_loop_fail_status = CL_FAIL_NONE; */
/*           report("CLOSED_LOOP_MOVEMENT_SUCCESS\n"); */
/*           break; */
/*       } */

/*       /1* Movement monitoring logic *1/ */
/*       if (differential_control_is_moving()) { */

/*         // Only check movement every STUCK_TIMEOUT_MS */
/*         if ((current_time - last_input_check_time) >= STUCK_ORIENT_TIMEOUT_MS) { */

/*           int16_t yaw_delta = normalize_angle(current_yaw - last_input_value); */

/*           // check for minimum movement detected */
/*           if (abs(yaw_delta) < MINIMUM_YAW_CHANGE) { */
/*             differential_control_stop(); */
/*             orientation_lock_yaw_measurement(); */
/*             closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED; */
/*             closed_loop_fail_status = CL_FAIL_MOTOR_RUNAWAY; */
/*             report("CL_FAIL_MOTOR_RUNAWAY\n"); */
/*           } */

/*           // check movement is in correct direction */
/*           if((expected_direction == 1 && yaw_delta < 0) || (expected_direction == -1 && yaw_delta > 0)) { */
/*             differential_control_stop(); */
/*             orientation_lock_yaw_measurement(); */
/*             closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED; */
/*             closed_loop_fail_status = CL_FAIL_MOTOR_WRONG_MOVEMENT; */
/*             report("CL_FAIL_MOTOR_WRONG_MOVEMENT\n"); */
/*           } */

/*           // Update for next check */
/*           last_input_value = current_yaw; */
/*           last_input_check_time = current_time; */
/*         } */

/*       } else { */
/*       // Should be here only when: */
/*       // 1- movement didn't start yet, this is first time after IDLE */
/*       // 2- movement overshooted for some reason, open loop with overshooted cm/deg finished and didn't catch setpoint */

/*         //TODO overshoot MUST BE much higher for smaller errors as motor movement is very non-linear */
/*         int16_t target_deg = error + (error*OVERSHOOT_ORIENT_PERCENT) / 100; */

/*         if (target_deg > 0) { */
/*           differential_control_right(target_deg, DEFAULT_PWM_DUTY_CYCLE); */
/*           expected_direction = 1; // Clockwise */

/*         } else { */
/*           differential_control_left(target_deg, DEFAULT_PWM_DUTY_CYCLE); */
/*           expected_direction = -1; // Anti-Clockwise */
/*         } */

/*         // Reset internal movement monitoring values */
/*         last_input_value = current_yaw; */
/*         last_input_check_time = current_time; */

/*       } */

/*       break; */
/*     } */

/*     case CLOSED_LOOP_MOVEMENT_SUCCESS: */
/*     case CLOSED_LOOP_MOVEMENT_FAILED: */
/*       break; */
/*   } */
/* } */
void closed_loop_orient(void) {
  switch(closed_loop_func_status) {
    case CLOSED_LOOP_MOVEMENT_IDLE: {

      if (!differential_control_is_moving()) {

        // calculate error
        int16_t error = angle_diff_normalized(setpoint, orientation_get_yaw_deg());

        if (error > 0) {
          differential_control_right(error, DEFAULT_PWM_DUTY_CYCLE);
        } else {
          differential_control_left(abs(error), DEFAULT_PWM_DUTY_CYCLE);
        }

        operation_start_time = get_current_time();
        closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;

      } else {
        closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED;
        closed_loop_fail_status = CL_FAIL_MOTOR_ALREADY_MOVING;
        report("CL_FAIL_MOTOR_ALREADY_MOVING\n");
      }

      break;
    }

    case CLOSED_LOOP_MOVEMENT_IN_PROGRESS: {
      if(!differential_control_is_moving()) {
        closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
        closed_loop_fail_status = CL_FAIL_NONE;
        orientation_set_gyro_yaw(setpoint);
        report("CLOSED_LOOP_MOVEMENT_SUCCESS\n");
      }
      break;

    case CLOSED_LOOP_MOVEMENT_SUCCESS:
    case CLOSED_LOOP_MOVEMENT_FAILED:
      break;
    }
  }
}

/* Master closed loop control algorithm to change orientation
 *  - INPUT: get_ultrasonic_distance_cm()
 *  - OUTPUT: differential_control_forward OR differential_control_backward
 *
 * 1- sets open loop differential_control_forward with overshooted values with default PWM
 * 2- Calls open loop differential_control_backward/forward in case of overshoot to correct error
 * 3- Fails if motors are already moving
 * 4- Fails if movement causes wrong distance change direction change 
 * 5- Fails unless there is indeed a MINIMUM_DISTANCE_CHANGE happenning every STUCK_DISTANCE_TIMEOUT_MS
 * 6- Fails the function in case of ABSOLUTE timeout from moment function began running
 * 7- Succeed if current current distance value is within distance tolerance setpoint value 
 *
 * function will only run if closed_loop_current_func is set to closed_loop_orient 
 * AND if closed_loop_func_status is at IDLE. 
 * closed_loop_func_status is NOT RESET AUTOMATICALLY BY CODE after successful or failed execution.
 * This should be done by the code that calls sets the function.
 * LIKE IN terminal.c and path_planning.c, if status is SUCCESS, it will automatically move to IDLE
 */
/* void closed_loop_move(void) { */
/*   switch(closed_loop_func_status) { */
/*     case CLOSED_LOOP_MOVEMENT_IDLE: { */

/*       if (!differential_control_is_moving()) { */
/*         hcsr04_start_cycle(); */
/*         operation_start_time = get_current_time(); */
/*         closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS; */

/*       } else { */
/*         closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED; */
/*         closed_loop_fail_status = CL_FAIL_MOTOR_ALREADY_MOVING; */
/*         report("CL_FAIL_MOTOR_ALREADY_MOVING\n"); */
/*       } */

/*       break; */
/*     } */

/*     case CLOSED_LOOP_MOVEMENT_IN_PROGRESS: { */

/*       int16_t current_distance = get_ultrasonic_distance_cm(); */
/*       uint32_t current_time = get_current_time(); */

/*       // Check total operation timeout */
/*       if ((current_time - operation_start_time) > TOTAL_MOVE_TIMEOUT_MS) { */
/*         differential_control_stop(); */
/*         hcsr04_stop_cycle(); */
/*         closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED; */
/*         closed_loop_fail_status = CL_FAIL_MOVEMENT_TIMEOUT; */
/*         report("CL_FAIL_MOVEMENT_TIMEOUT\n"); */
/*         break; */
/*       } */

/*       // Calculate normalized error (-180 to 180) */
/*       int16_t error = setpoint - current_distance; */
/*       // Check if target reached */
/*       if (abs(error) <= CM_TOLERANCE) { */
/*           differential_control_stop(); */
/*           hcsr04_stop_cycle(); */
/*           closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS; */
/*           closed_loop_fail_status = CL_FAIL_NONE; */
/*           report("CLOSED_LOOP_MOVEMENT_SUCCESS\n"); */
/*           break; */
/*       } */

/*       /1* Movement monitoring logic *1/ */
/*       if (differential_control_is_moving()) { */

/*         // Only check movement every STUCK_TIMEOUT_MS */
/*         if ((current_time - last_input_check_time) >= STUCK_DISTANCE_TIMEOUT_MS) { */

/*           int16_t distance_delta = current_distance - last_input_value; */

/*           // check for minimum movement detected */
/*           if (abs(distance_delta) < MINIMUM_DISTANCE_CHANGE) { */
/*             differential_control_stop(); */
/*             hcsr04_stop_cycle(); */
/*             closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED; */
/*             closed_loop_fail_status = CL_FAIL_MOTOR_RUNAWAY; */
/*             report("CL_FAIL_MOTOR_RUNAWAY\n"); */
/*           } */

/*           // check movement is in correct direction */
/*           if((expected_direction == 1 && distance_delta < 0) || (expected_direction == -1 && distance_delta > 0)) { */
/*             differential_control_stop(); */
/*             hcsr04_stop_cycle(); */
/*             closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED; */
/*             closed_loop_fail_status = CL_FAIL_MOTOR_WRONG_MOVEMENT; */
/*             report("CL_FAIL_MOTOR_WRONG_MOVEMENT\n"); */
/*           } */

/*           // Update for next check */
/*           last_input_value = current_distance; */
/*           last_input_check_time = current_time; */
/*         } */

/*       } else { */
/*       // Should be here only when: */
/*       // 1- movement didn't start yet, this is first time after IDLE */
/*       // 2- movement overshooted for some reason, open loop with overshooted cm/deg finished and didn't catch setpoint */

/*         int16_t target_distance = error + (error*OVERSHOOT_MOVE_PERCENT) / 100; */

/*         if (target_distance > 0) { */
/*           differential_control_right(target_distance, DEFAULT_PWM_DUTY_CYCLE); */
/*           expected_direction = 1; // Clockwise */

/*         } else { */
/*           differential_control_left(target_distance, DEFAULT_PWM_DUTY_CYCLE); */
/*           expected_direction = -1; // Anti-Clockwise */
/*         } */

/*         // Reset internal movement monitoring values */
/*         last_input_value = current_distance; */
/*         last_input_check_time = current_time; */

/*       } */

/*       break; */
/*     } */

/*     case CLOSED_LOOP_MOVEMENT_SUCCESS: */
/*     case CLOSED_LOOP_MOVEMENT_FAILED: */
/*       break; */
/*   } */


/* } */
void closed_loop_move(void) {
  switch(closed_loop_func_status) {
    case CLOSED_LOOP_MOVEMENT_IDLE: {

      if (!differential_control_is_moving()) {
        differential_control_forward(setpoint, DEFAULT_PWM_DUTY_CYCLE);
        closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;

      } else {
        closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED;
        closed_loop_fail_status = CL_FAIL_MOTOR_ALREADY_MOVING;
        report("CL_FAIL_MOTOR_ALREADY_MOVING\n");
      }

      break;
    }

    case CLOSED_LOOP_MOVEMENT_IN_PROGRESS: 
      if(!differential_control_is_moving()) {
        closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
        closed_loop_fail_status = CL_FAIL_NONE;
        report("CLOSED_LOOP_MOVEMENT_SUCCESS\n");
      }

      break;

    case CLOSED_LOOP_MOVEMENT_SUCCESS:
    case CLOSED_LOOP_MOVEMENT_FAILED:
      break;
  }


}



void closed_loop_up_ramp(void) {

}
