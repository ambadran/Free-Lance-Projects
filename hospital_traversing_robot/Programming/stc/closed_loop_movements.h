#ifndef CLOSED_LOOP_MOVEMENTS_H
#define CLOSED_LOOP_MOVEMENTS_H

#define CLOSED_LOOP_FUNC_NUM 11

// should never be tampered with except inside closed loop functions
// needs to be reseted manually if fault is detected, user is supposed to set physical orientation of robot to default value if fail
typedef enum {
  CLOSED_LOOP_MOVEMENT_IDLE,
  CLOSED_LOOP_MOVEMENT_FAILED,
  CLOSED_LOOP_MOVEMENT_IN_PROGRESS,
  CLOSED_LOOP_MOVEMENT_SUCCESS
} closed_loop_func_status_t;
extern volatile closed_loop_func_status_t closed_loop_func_status;
extern const char* CLOSED_LOOP_STATUS_TO_STRING[];
void closed_loop_reset_to_idle(void);

typedef void (*closed_loop_movement_func_t)(void);
extern closed_loop_movement_func_t closed_loop_current_func;

/* Idle movement func */
void closed_loop_move_idle(void);

/* Basic Closed loop movements that other advanced movements will made from :) */
/* this function orients the robot to a specific angle using differential control and orientation yaw value
 * angle parameter is -180 to 180
*/
void closed_loop_orient(int16_t angle);

/* Actual closed loop function for each single movement!! :D */
void closed_loop_exit_room(void);
void closed_loop_enter_room(void);
void closed_loop_corridor_north(void);
void closed_loop_corridor_east(void);
void closed_loop_corridor_west(void);
void closed_loop_corridor_south(void);
void closed_loop_stairs_up(void);
void closed_loop_stairs_down(void);

#endif
