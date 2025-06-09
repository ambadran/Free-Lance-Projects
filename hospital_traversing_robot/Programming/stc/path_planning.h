#ifndef PATH_PLANNING_H
#define PATH_PLANNING_H

typedef enum {
  PATH_NOT_FOUND,
  PATH_FOUND
} path_result_status_t;

typedef enum {
  PATH_EXECUTE_IDLE,
  PATH_EXECUTE_MOVEMENT_FAILED,
  PATH_EXECUTE_STARTING,
  PATH_EXECUTE_INVALID_MOVE_WANTED,
  PATH_EXECUTE_GETTING_NEXT_MOVEMENT,
  PATH_EXECUTE_MOVEMENT_IN_PROGRESS,
  PATH_EXECUTE_FINISHED_SUCCESSFULLY,
  PATH_EXECUTE_STATUS_COUNT
} execute_path_status_t;
extern const char* EXECUTE_PATH_STATUS_TO_STRING[];

typedef enum {
  INVALID_LOCATION = -1,
  ROOM0 = 0,
  ROOM1,
  ROOM2,
  ROOM3,
  ROOM4,
  ROOM5,
  ROOM6,
  ROOM7,
  ROOM8,
  ROOM_FRONT0,
  ROOM_FRONT1,
  ROOM_FRONT2,
  ROOM_FRONT3,
  ROOM_FRONT4,
  ROOM_FRONT5,
  ROOM_FRONT6,
  ROOM_FRONT7,
  ROOM_FRONT8,
  STAIR0_FLOOR0_FRONT,
  STAIR0_FLOOR1_FRONT,
  STAIR1_FLOOR1_FRONT,
  STAIR1_FLOOR2_FRONT,
  STAIR2_FLOOR2_FRONT,
  STAIR2_FLOOR3_FRONT,
  LOCATION_COUNT,
} location_t;

#define MAX_NEIGHBORS 3
#define MAX_QUEUE LOCATION_COUNT
#define MAX_PATH_LEN LOCATION_COUNT

/*
  We will return the path as a statically allocated linked list of up to
  LOCATION_COUNT nodes.  No malloc/free.  Each node holds a location_t
  plus an “index of the next node” (−1 if none).
*/
typedef struct {
    location_t loc;
    int8_t     next;   // index in the array, or −1 if end of list
} PathNode;

path_result_status_t find_path(location_t start, location_t dest);
void print_path(void);

/* A function pointer type for a closed-loop movement from one location to the next
 * Mapping Returns the movement function to go from 'from' to 'to', or NULL if invalid
 */
typedef closed_loop_func_status_t (*movement_fn_t)(void);
movement_fn_t get_single_move_func(location_t from, location_t to);

// Executes the entire path: calls each segment's movement function in turn
// path_nodes[] is the array filled by find_path_nohead(), terminated by next == -1
void execute_path_process(void);

void execute_path_start(void);
void execute_path_stop(void);
execute_path_status_t execute_path_get_status(void);

#endif
