#ifndef ADV_MOVEMENT_H
#define ADV_MOVEMENT_H

#define ADV_MOVE_FUNC_NUM 8

typedef enum {
  ADV_MOVE_FAIL_NONE,
  ADV_MOVE_FAIL_CL_IN_PROGRESS,
  ADV_MOVE_FAIL_CL_FAILED,
} adv_move_fail_status_t;
extern adv_move_fail_status_t adv_move_fail_status;

//TODO: make it static and make get func
// should never be tampered with except inside closed loop functions
// needs to be reseted manually if fault is detected, user is supposed to set physical orientation of robot to default value if fail
typedef enum {
  ADV_MOVE_IDLE,
  ADV_MOVE_START,
  ADV_MOVE_FAILED,
  ADV_MOVE_IN_PROGRESS,
  ADV_MOVE_SUCCESS
} adv_move_func_status_t;
extern volatile adv_move_func_status_t adv_move_func_status;
extern const char* ADV_MOVE_STATUS_TO_STRING[];

typedef enum {
  ADV_MOVE_FUNC_NONE = -1,
  ADV_MOVE_FUNC_EXIT_ROOM = 0,
  ADV_MOVE_FUNC_ENTER_ROOM,
  ADV_MOVE_FUNC_CORRIDOR_NORTH,
  ADV_MOVE_FUNC_CORRIDOR_SOUTH,
  ADV_MOVE_FUNC_CORRIDOR_EAST,
  ADV_MOVE_FUNC_CORRIDOR_WEST,
  ADV_MOVE_FUNC_STAIRS_UP,
  ADV_MOVE_FUNC_STAIRS_DOWN
} adv_movement_func_index_t;
extern adv_movement_func_index_t adv_movement_func_index;

void adv_move_reset_idle(void);
void adv_move_process(void);

#endif
