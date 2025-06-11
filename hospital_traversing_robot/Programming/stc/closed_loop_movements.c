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
/* Current running closed loop function */
closed_loop_movement_func_t closed_loop_current_func = closed_loop_move_idle;

// Internal variables
static int8_t sign_multiple = 1;


void closed_loop_reset_to_idle(void) {
  closed_loop_current_func = closed_loop_move_idle;
  closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IDLE;
}

void closed_loop_move_idle(void) {}

void closed_loop_orient(int16_t yaw_setpoint) {

  if(closed_loop_func_status == CLOSED_LOOP_MOVEMENT_IN_PROGRESS) {
    /* setup done, performing closed loop check */
    if((orientation_get_yaw_deg() - yaw_setpoint) <= CLOSED_LOOP_MINIMUM_YAW_TOLERANCE) {
      // Target is met :D
      closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
      orientation_lock_yaw_measurement();

    } else if (!differential_control_is_moving()) {
      // Target was missed and overshoot of setpoint is achieved :(
      closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED;
      orientation_lock_yaw_measurement();
    }

  } else {
    /* initial setup */
    // Step 1: get set value 
    int16_t error_term = yaw_setpoint - orientation_get_yaw_deg();

    // Overshoot error term to open loop differential control function
    if (error_term > 0) {
      differential_control_right(error_term + error_term*OVERSHOOT_MULTIPLE, DEFAULT_PWM_DUTY_CYCLE);

    } else if (error_term < 0) {
      differential_control_left(error_term + error_term*OVERSHOOT_MULTIPLE, DEFAULT_PWM_DUTY_CYCLE);

    }

    // Unlock Yaw measurement
    orientation_unlock_yaw_measurement();

    // Now we just wait until desired setpoint is met
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  }
}

void closed_loop_exit_room(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing exit room\n");
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

void closed_loop_enter_room(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing entry room\n");
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1; // for next function in find next function in path execution
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

void closed_loop_corridor_north(void) {
  switch(closed_loop_func_status) {
    case CLOSED_LOOP_MOVEMENT_IDLE: {
      /* initial setup */
      // Step 1: get set value 
      int16_t error_term = NORTH - orientation_get_yaw_deg();
      if (error_term <= CLOSED_LOOP_MINIMUM_YAW_TOLERANCE) {
        // check if already there
        closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
        report("CL ORIENT NORTH ALREADY!\n");
        break;
      }

      // Overshoot error term to open loop differential control function
      if (error_term > 0) {
        sign_multiple = 1;
        differential_control_right(error_term + error_term*OVERSHOOT_MULTIPLE, DEFAULT_PWM_DUTY_CYCLE);

      } else if (error_term < 0) {
        sign_multiple = -1;
        differential_control_left(-1*error_term + -1*error_term*OVERSHOOT_MULTIPLE, DEFAULT_PWM_DUTY_CYCLE);
      }

      // Unlock Yaw measurement
      orientation_unlock_yaw_measurement();

      // Now we just wait until desired setpoint is met
      closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
      break;
    }

    case CLOSED_LOOP_MOVEMENT_IN_PROGRESS:
      /* setup done, performing closed loop check */
      if (orientation_get_yaw_deg()*sign_multiple < NORTH) {
        // still didn't reach setpoint
        if (!differential_control_is_moving()) {
          // Target was missed and overshoot of setpoint is achieved :(
          closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED;
          orientation_lock_yaw_measurement();
          differential_control_stop();
          report("CL ORIENT NORTH TIMOUT:(\n");
        }

      } else if(orientation_get_yaw_deg() == NORTH) {
        // Target is met :D
        closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
        orientation_lock_yaw_measurement();
        differential_control_stop();
        report("CL ORIENT NORTH SUCCESS :D\n");

      } else if (orientation_get_yaw_deg()*sign_multiple > NORTH) {
        // overshoot
        if((orientation_get_yaw_deg()-NORTH) <= CLOSED_LOOP_MINIMUM_YAW_TOLERANCE) {
          // Target is met :D
          closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
          orientation_lock_yaw_measurement();
          differential_control_stop();
          report("CL ORIENT NORTH within tolerance\n");

        } else {
          // Target was missed and overshoot of setpoint is achieved :(
          closed_loop_func_status = CLOSED_LOOP_MOVEMENT_FAILED;
          orientation_lock_yaw_measurement();
          differential_control_stop();
          report("CL ORIENT NORTH OVERSHOOT :(\n");

        }
      } 
      break;

    case CLOSED_LOOP_MOVEMENT_SUCCESS:
    case CLOSED_LOOP_MOVEMENT_FAILED:
      break;
  }
}

void closed_loop_corridor_east(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing east\n");
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

void closed_loop_corridor_west(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing west\n");
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

void closed_loop_corridor_south(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing south\n");
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

void closed_loop_stairs_up(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing stairs up\n");
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

void closed_loop_stairs_down(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing stairs down\n");
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    closed_loop_func_status = CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}


