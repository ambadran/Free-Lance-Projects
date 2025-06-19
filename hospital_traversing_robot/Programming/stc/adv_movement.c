#include "project-defs.h"

const char* ADV_MOVE_STATUS_TO_STRING[] = {
  "ADV_MOVE_IDLE",
  "ADV_MOVE_FAILED",
  "ADV_MOVE_IN_PROGRESS",
  "ADV_MOVE_SUCCESS" 
};

/* Current advanced movement status */
volatile adv_move_func_status_t adv_move_func_status = ADV_MOVE_IDLE;
adv_move_fail_status_t adv_move_fail_status = ADV_MOVE_FAIL_NONE;

/* Current advanced movment function */
adv_movement_func_index_t adv_movement_func_index = ADV_MOVE_FUNC_NONE;

/* Internal variables */
static uint8_t adv_move_index = 0;
// The mapping from a adv_movement_func_index_t to the sequence of closed loop functions and their setpoints!!! :D
static const struct {
  closed_loop_movement_func_t adv_move_cl_func_sequence[3];
  int16_t adv_move_cl_setpoint_sequence[2];
} adv_move_cl_func_setpoint_sequence_map[] = {
  // ADV_MOVE_FUNC_EXIT_ROOM
  { {closed_loop_orient, closed_loop_move, NULL},
    {SOUTH, EXIT_ROOM_DISTANCE}
  },

  // ADV_MOVE_FUNC_ENTER_ROOM
  { {closed_loop_orient, closed_loop_move, NULL},
    {NORTH, ENTER_ROOM_DISTANCE}
  },

  // ADV_MOVE_FUNC_CORRIDOR_NORTH
  { {closed_loop_orient, closed_loop_move, NULL},
    {NORTH, CORRIDOR_MOVEMENT_DISTANCE}
  },

  // ADV_MOVE_FUNC_CORRIDOR_SOUTH
  { {closed_loop_orient, closed_loop_move, NULL},
    {SOUTH, CORRIDOR_MOVEMENT_DISTANCE}
  },

  // ADV_MOVE_FUNC_CORRIDOR_EAST
  { {closed_loop_orient, closed_loop_move, NULL},
    {EAST, CORRIDOR_MOVEMENT_DISTANCE}
  },

  // ADV_MOVE_FUNC_CORRIDOR_WEST
  { {closed_loop_orient, closed_loop_move, NULL},
    {WEST, CORRIDOR_MOVEMENT_DISTANCE}
  },

  // ADV_MOVE_FUNC_STAIRS_UP
  { {closed_loop_orient, closed_loop_up_ramp, NULL},
    {WEST, RAMP_ANGLE}
  },

  // ADV_MOVE_FUNC_STAIRS_DOWN
  { {closed_loop_orient, closed_loop_up_ramp, NULL},
    {EAST, RAMP_ANGLE}
  }
};


void adv_move_reset_idle(void) {
  adv_move_func_status = ADV_MOVE_IDLE;
  adv_move_fail_status = ADV_MOVE_FAIL_NONE;
  adv_movement_func_index = ADV_MOVE_FUNC_NONE;
}

void adv_move_process(void) {
  switch(adv_move_func_status) {

    case ADV_MOVE_IDLE:
      // NOP
      break;

    case ADV_MOVE_START:
      // First make sure closed loop status is IDLE
      switch(closed_loop_func_status) {
        case CLOSED_LOOP_MOVEMENT_SUCCESS:
          closed_loop_reset_to_idle();
        case CLOSED_LOOP_MOVEMENT_IDLE:
          adv_move_func_status = ADV_MOVE_IN_PROGRESS;
          adv_move_index = 0;
#ifdef ADV_MOVEMENT_DEBUG
          printf("starting adv movmement\n");
#endif
          break;

        case CLOSED_LOOP_MOVEMENT_IN_PROGRESS:
          adv_move_func_status = ADV_MOVE_FAILED;
          adv_move_fail_status = ADV_MOVE_FAIL_CL_IN_PROGRESS;
          report("Failed: CL in progress\n");
          report("Ai-1 to reset status!\n");
          break;

        case CLOSED_LOOP_MOVEMENT_FAILED:
          adv_move_func_status = ADV_MOVE_FAILED;
          adv_move_fail_status = ADV_MOVE_FAIL_CL_FAILED;
          report("Failed CL status");
          report("Ai-1 & Ci-1 to reset status!\n");
          break;
      }
      break;

    case ADV_MOVE_IN_PROGRESS:

      switch(closed_loop_func_status) {
        case CLOSED_LOOP_MOVEMENT_IDLE:
          // move on to next
          closed_loop_current_func = adv_move_cl_func_setpoint_sequence_map[adv_movement_func_index].adv_move_cl_func_sequence[adv_move_index];
          closed_loop_set_setpoint(adv_move_cl_func_setpoint_sequence_map[adv_movement_func_index].adv_move_cl_setpoint_sequence[adv_move_index]);
#ifdef ADV_MOVEMENT_DEBUG
          printf("Assigned func ind %d, setpoint: %d\n", adv_movement_func_index, adv_move_cl_func_setpoint_sequence_map[adv_movement_func_index].adv_move_cl_setpoint_sequence[adv_move_index]);
#endif

          // Incrementing index
          adv_move_index++;

        break;

        case CLOSED_LOOP_MOVEMENT_IN_PROGRESS:
        // wait
        break;

        case CLOSED_LOOP_MOVEMENT_SUCCESS:
          // check if it's finished
          if(adv_move_cl_func_setpoint_sequence_map[adv_movement_func_index].adv_move_cl_func_sequence[adv_move_index]== NULL) {
            adv_move_func_status = ADV_MOVE_SUCCESS;
#ifdef ADV_MOVEMENT_DEBUG
            printf("Advanced Movement finished successfully!!\n");
#endif

          } else {
            closed_loop_reset_to_idle();

          }
          break;

        case CLOSED_LOOP_MOVEMENT_FAILED:
          adv_move_func_status = ADV_MOVE_FAILED;
          adv_move_fail_status = ADV_MOVE_FAIL_CL_FAILED;
          report("Failed CL status");
          report("Ai-1 & Ci-1 to reset status!\n");
          break;
      }
      break;

    case ADV_MOVE_FAILED:
    case ADV_MOVE_SUCCESS:
      break;

  }
}
