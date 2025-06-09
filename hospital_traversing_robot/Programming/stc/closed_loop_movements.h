#ifndef CLOSED_LOOP_MOVEMENTS_H
#define CLOSED_LOOP_MOVEMENTS_H

typedef enum {
  CLOSED_LOOP_MOVEMENT_IDLE,
  CLOSED_LOOP_MOVEMENT_FAILED,
  CLOSED_LOOP_MOVEMENT_IN_PROGRESS,
  CLOSED_LOOP_MOVEMENT_SUCCESS
} closed_loop_func_status_t;

/* Actual closed loop function for each single movement!! :D */
closed_loop_func_status_t closed_loop_exit_room(void);
closed_loop_func_status_t closed_loop_enter_room(void);
closed_loop_func_status_t closed_loop_corridor_north(void);
closed_loop_func_status_t closed_loop_corridor_east(void);
closed_loop_func_status_t closed_loop_corridor_west(void);
closed_loop_func_status_t closed_loop_corridor_south(void);
closed_loop_func_status_t closed_loop_stairs_up(void);
closed_loop_func_status_t closed_loop_stairs_down(void);

#endif
