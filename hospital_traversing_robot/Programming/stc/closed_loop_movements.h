#ifndef CLOSED_LOOP_MOVEMENTS_H
#define CLOSED_LOOP_MOVEMENTS_H

#define CLOSED_LOOP_FUNC_NUM 3

typedef enum {
  CL_FAIL_NONE,
  CL_FAIL_MOTOR_ALREADY_MOVING,
  CL_FAIL_MOTOR_RUNAWAY,
  CL_FAIL_MOTOR_WRONG_MOVEMENT,
  CL_FAIL_MOVEMENT_TIMEOUT
} closed_loop_fail_status_t;
extern closed_loop_fail_status_t closed_loop_fail_status;

//TODO: make it static and make get func
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
void closed_loop_set_setpoint(int16_t yaw_setpoint_value);
direction_t closed_loop_get_setpoint(void);
void closed_loop_orient(void);
void closed_loop_move(void);
void closed_loop_up_ramp(void);

#endif
