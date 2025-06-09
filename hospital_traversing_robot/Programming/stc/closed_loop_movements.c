#include "project-defs.h"

uint8_t temp_counter = 1;

closed_loop_func_status_t closed_loop_exit_room(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing exit room\n");
    return CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    return CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

closed_loop_func_status_t closed_loop_enter_room(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing entry room\n");
    return CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    return CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

closed_loop_func_status_t closed_loop_corridor_north(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing north\n");
    return CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    return CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

closed_loop_func_status_t closed_loop_corridor_east(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing east\n");
    return CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    return CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

closed_loop_func_status_t closed_loop_corridor_west(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing west\n");
    return CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    return CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

closed_loop_func_status_t closed_loop_corridor_south(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing south\n");
    return CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    return CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

closed_loop_func_status_t closed_loop_stairs_up(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing stairs up\n");
    return CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    return CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}

closed_loop_func_status_t closed_loop_stairs_down(void) {
  if (temp_counter) {
    temp_counter = 0;
    printf("executing stairs down\n");
    return CLOSED_LOOP_MOVEMENT_IN_PROGRESS;
  } else {
    temp_counter = 1;
    return CLOSED_LOOP_MOVEMENT_SUCCESS;
  }
}


