#ifndef PATH_PLANNING_H
#define PATH_PLANNING_H

typedef enum {
  PATH_NOT_FOUND,
  PATH_FOUND
} path_result_status_t;

typedef enum {
  ROOM0,
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
  STAIR0,
  STAIR1,
  STAIR2,
  LOCATION_COUNT
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

#endif
