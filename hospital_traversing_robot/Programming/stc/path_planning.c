/* This component implements BFS algorithm to find the shortest path from a specific location to the next
 * The components of BFS:
 *  1- State: the enum to hold the room IDs
 *  2- Successors: The adjacency array to return what are the successors of a specific location (state)
 *  3- Frontier: the Queue that will hold the result as it being built and be returned in the end
 *  4- Explored: the set that we save wrong trials in
 *  5- node path: the current location (state) being tested and the rest of the path
 */
#include "project-defs.h"

// Result Path linkedlist
static PathNode path_nodes[LOCATION_COUNT];

// Execute path status
static execute_path_status_t execute_path_status = PATH_EXECUTE_IDLE;

// Current movement
static movement_fn_t current_movement_func = NULL;
static int8_t idx = 0;
static int8_t next_idx = 0;
static location_t cur = INVALID_LOCATION;
static location_t next = INVALID_LOCATION;
static closed_loop_func_status_t closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IDLE;

const char* EXECUTE_PATH_STATUS_TO_STRING[PATH_EXECUTE_STATUS_COUNT] = {
  "PATH_EXECUTE_IDLE",
  "PATH_EXECUTE_MOVEMENT_FAILED",
  "PATH_EXECUTE_STARTING",
  "PATH_EXECUTE_INVALID_MOVE_WANTED",
  "PATH_EXECUTE_GETTING_NEXT_MOVEMENT",
  "PATH_EXECUTE_MOVEMENT_IN_PROGRESS",
  "PATH_EXECUTE_FINISHED_SUCCESSFULLY"
};

/* This is a mapping from the location_t indexes to the string names
 */
static const char* IND_TO_STR_MAP[LOCATION_COUNT] = {
  "ROOM0",
  "ROOM1",
  "ROOM2",
  "ROOM3",
  "ROOM4",
  "ROOM5",
  "ROOM6",
  "ROOM7",
  "ROOM8",
  "ROOM_FRONT0",
  "ROOM_FRONT1",
  "ROOM_FRONT2",
  "ROOM_FRONT3",
  "ROOM_FRONT4",
  "ROOM_FRONT5",
  "ROOM_FRONT6",
  "ROOM_FRONT7",
  "ROOM_FRONT8",
  "STAIR0_FLOOR0_FRONT",
  "STAIR0_FLOOR1_FRONT",
  "STAIR1_FLOOR1_FRONT",
  "STAIR1_FLOOR2_FRONT",
  "STAIR2_FLOOR2_FRONT",
  "STAIR2_FLOOR3_FRONT",
};

/* Please refer to the diagram drawn by hand to have better understanding of the map.
 * This is an adjacency list implemented as 2D array to perform the successors functionality.
 * This is the Graph representation as well as the successor_counts
 */
static const location_t successors[LOCATION_COUNT][MAX_NEIGHBORS] = {
    // ROOM0 .. ROOM8 each only connects to its “front”
    [ROOM0] = { ROOM_FRONT0, INVALID_LOCATION, INVALID_LOCATION },
    [ROOM1] = { ROOM_FRONT1, INVALID_LOCATION, INVALID_LOCATION },
    [ROOM2] = { ROOM_FRONT2, INVALID_LOCATION, INVALID_LOCATION },
    [ROOM3] = { ROOM_FRONT3, INVALID_LOCATION, INVALID_LOCATION },
    [ROOM4] = { ROOM_FRONT4, INVALID_LOCATION, INVALID_LOCATION },
    [ROOM5] = { ROOM_FRONT5, INVALID_LOCATION, INVALID_LOCATION },
    [ROOM6] = { ROOM_FRONT6, INVALID_LOCATION, INVALID_LOCATION },
    [ROOM7] = { ROOM_FRONT7, INVALID_LOCATION, INVALID_LOCATION },
    [ROOM8] = { ROOM_FRONT8, INVALID_LOCATION, INVALID_LOCATION },

    // ROOM_FRONTx each connects to its ROOMx, and to the corridor chain, 
    // and for floors 0 and 2, also to the appropriate stair‐front.
    [ROOM_FRONT0] = { ROOM_FRONT1,         ROOM0,        INVALID_LOCATION          },
    [ROOM_FRONT1] = { ROOM_FRONT0,         ROOM1,        ROOM_FRONT2 },
    [ROOM_FRONT2] = { STAIR0_FLOOR0_FRONT, ROOM0,        ROOM_FRONT1 },

    [ROOM_FRONT3] = { STAIR0_FLOOR1_FRONT, ROOM3,        ROOM_FRONT4 },
    [ROOM_FRONT4] = { ROOM_FRONT3,         ROOM4,        ROOM_FRONT5 },
    [ROOM_FRONT5] = { ROOM_FRONT4,         ROOM5,        STAIR1_FLOOR1_FRONT },

    [ROOM_FRONT6] = { STAIR1_FLOOR2_FRONT, ROOM6,        ROOM_FRONT7 },
    [ROOM_FRONT7] = { ROOM_FRONT6,         ROOM7,        ROOM_FRONT8 },
    [ROOM_FRONT8] = { ROOM_FRONT7,         ROOM8,        STAIR2_FLOOR2_FRONT },

    // Stair‐fronts each connect back to one Stair and to exactly one room‐front (except STAIR2_FLOOR3_FRONT)
    [STAIR0_FLOOR0_FRONT] = { STAIR0_FLOOR1_FRONT, ROOM_FRONT2,      INVALID_LOCATION },
    [STAIR0_FLOOR1_FRONT] = { STAIR0_FLOOR0_FRONT, ROOM_FRONT3,      INVALID_LOCATION },
    [STAIR1_FLOOR1_FRONT] = { STAIR1_FLOOR2_FRONT, ROOM_FRONT5,      INVALID_LOCATION },
    [STAIR1_FLOOR2_FRONT] = { STAIR1_FLOOR1_FRONT, ROOM_FRONT6,      INVALID_LOCATION },
    [STAIR2_FLOOR2_FRONT] = { STAIR2_FLOOR3_FRONT, ROOM_FRONT8,      INVALID_LOCATION },
    [STAIR2_FLOOR3_FRONT] = { STAIR2_FLOOR2_FRONT, INVALID_LOCATION, INVALID_LOCATION },

};

/*
  For convenience, a parallel array that stores exactly how many valid
  neighbors each location actually has (so BFS only iterates those).
*/
static const int successor_counts[LOCATION_COUNT] = {
    // ROOM0 .. ROOM8 each have exactly 1 neighbor
    [ROOM0] = 1,  [ROOM1] = 1,  [ROOM2] = 1,
    [ROOM3] = 1,  [ROOM4] = 1,  [ROOM5] = 1,
    [ROOM6] = 1,  [ROOM7] = 1,  [ROOM8] = 1,

    // ROOM_FRONT0..ROOM_FRONT8 each have 2 or 3 neighbors
    [ROOM_FRONT0] = 2,
    [ROOM_FRONT1] = 3,
    [ROOM_FRONT2] = 3,
    [ROOM_FRONT3] = 3,
    [ROOM_FRONT4] = 3,
    [ROOM_FRONT5] = 3,
    [ROOM_FRONT6] = 3,
    [ROOM_FRONT7] = 3,
    [ROOM_FRONT8] = 3,

    // Stair fronts:
    [STAIR0_FLOOR0_FRONT] = 2,
    [STAIR0_FLOOR1_FRONT] = 2,
    [STAIR1_FLOOR1_FRONT] = 2,
    [STAIR1_FLOOR2_FRONT] = 2,
    [STAIR2_FLOOR2_FRONT] = 2,
    [STAIR2_FLOOR3_FRONT] = 1,

};

// Internal lookup table for from→to mapping
static const struct {
    location_t from;
    location_t to;
    movement_fn_t fn;
} single_move_func_map[] = {
  // Enter room
  {ROOM0, ROOM_FRONT0, closed_loop_exit_room},
  {ROOM1, ROOM_FRONT1, closed_loop_exit_room},
  {ROOM2, ROOM_FRONT2, closed_loop_exit_room},
  {ROOM3, ROOM_FRONT3, closed_loop_exit_room},
  {ROOM4, ROOM_FRONT4, closed_loop_exit_room},
  {ROOM5, ROOM_FRONT5, closed_loop_exit_room},
  {ROOM6, ROOM_FRONT6, closed_loop_exit_room},
  {ROOM7, ROOM_FRONT7, closed_loop_exit_room},
  {ROOM8, ROOM_FRONT8, closed_loop_exit_room},
  // Exit room
  {ROOM_FRONT0, ROOM0, closed_loop_enter_room},
  {ROOM_FRONT1, ROOM1, closed_loop_enter_room},
  {ROOM_FRONT2, ROOM2, closed_loop_enter_room},
  {ROOM_FRONT3, ROOM3, closed_loop_enter_room},
  {ROOM_FRONT4, ROOM4, closed_loop_enter_room},
  {ROOM_FRONT5, ROOM5, closed_loop_enter_room},
  {ROOM_FRONT6, ROOM6, closed_loop_enter_room},
  {ROOM_FRONT7, ROOM7, closed_loop_enter_room},
  {ROOM_FRONT8, ROOM8, closed_loop_enter_room},

  // Corridor Movements First Floor
  {ROOM_FRONT0, ROOM_FRONT1, closed_loop_corridor_east},
  {ROOM_FRONT1, ROOM_FRONT2, closed_loop_corridor_east},
  {ROOM_FRONT1, ROOM_FRONT0, closed_loop_corridor_west},
  {ROOM_FRONT2, ROOM_FRONT1, closed_loop_corridor_west},
  {ROOM_FRONT2, STAIR0_FLOOR0_FRONT, closed_loop_corridor_south},

  // Corridor Movements Second Floor
  {STAIR0_FLOOR1_FRONT, ROOM_FRONT3, closed_loop_corridor_north},
  {ROOM_FRONT3, ROOM_FRONT4, closed_loop_corridor_east},
  {ROOM_FRONT4, ROOM_FRONT5, closed_loop_corridor_east},
  {ROOM_FRONT4, ROOM_FRONT3, closed_loop_corridor_west},
  {ROOM_FRONT5, ROOM_FRONT4, closed_loop_corridor_west},
  {ROOM_FRONT5, STAIR1_FLOOR1_FRONT, closed_loop_corridor_south},

  // Corridor Movements Third Floor
  {STAIR1_FLOOR2_FRONT, ROOM_FRONT6, closed_loop_corridor_north},
  {ROOM_FRONT6, ROOM_FRONT7, closed_loop_corridor_east},
  {ROOM_FRONT7, ROOM_FRONT8, closed_loop_corridor_east},
  {ROOM_FRONT7, ROOM_FRONT6, closed_loop_corridor_west},
  {ROOM_FRONT8, ROOM_FRONT7, closed_loop_corridor_west},
  {ROOM_FRONT8, STAIR2_FLOOR2_FRONT, closed_loop_corridor_south},

  // Stairs!
  {STAIR0_FLOOR0_FRONT, STAIR0_FLOOR1_FRONT, closed_loop_stairs_up},
  {STAIR0_FLOOR1_FRONT, STAIR0_FLOOR0_FRONT, closed_loop_stairs_down},
  {STAIR1_FLOOR1_FRONT, STAIR1_FLOOR2_FRONT, closed_loop_stairs_up},
  {STAIR1_FLOOR2_FRONT, STAIR1_FLOOR1_FRONT, closed_loop_stairs_down},
  {STAIR2_FLOOR2_FRONT, STAIR2_FLOOR3_FRONT, closed_loop_stairs_up},
  {STAIR2_FLOOR3_FRONT, STAIR2_FLOOR2_FRONT, closed_loop_stairs_down},

  // Termination entry
  { LOCATION_COUNT, LOCATION_COUNT, NULL }
};

movement_fn_t get_single_move_func(location_t from, location_t to) {
  for (int i = 0; single_move_func_map[i].fn != NULL; i++) {
    if (single_move_func_map[i].from == from && single_move_func_map[i].to == to) {
      return single_move_func_map[i].fn;
    }
  }
  return NULL;
}

/*
  find_path(…):
    - Inputs:  start, dest:  two location_t’s (0..LOCATION_COUNT−1)
               path_nodes: caller must supply a PathNode array of size LOCATION_COUNT
    - Returns: PATH_FOUND if a path was found, PATH_NOT_FOUND if unreachable.
    - Output:  path_nodes[.] form a singly linked list from start → … → dest.
*/
path_result_status_t find_path(location_t start, location_t dest) {
    //TODO: V.IMP add an option to draw from current position to wanted. for example if i==-1 then from whatever last saved current location to j value
 
    // Special case: if start == dest, return a single-node list
    if (start == dest) {
        path_nodes[0].loc  = start;
        path_nodes[0].next = -1;
        return PATH_FOUND;
    }

    // Standard BFS bookkeeping (fixed‐size arrays only)
    bool    visited[LOCATION_COUNT] = { PATH_NOT_FOUND };
    int8_t  pred[LOCATION_COUNT];    // predecessor in BFS tree
    int i;
    for (i = 0; i < LOCATION_COUNT; i++) {
        pred[i] = -1;
    }

    int8_t queue[MAX_QUEUE];
    int   head = 0, tail = 0;

    // Initialize BFS
    visited[start] = PATH_FOUND;
    queue[tail++] = (int8_t)start;

    bool found = PATH_NOT_FOUND;
    while (head < tail) {
        int8_t current = queue[head++];
        int   ncount  = successor_counts[current];

        for (int i = 0; i < ncount; i++) {
            int8_t nbr = successors[current][i];
            if (nbr < 0) continue;
            if (!visited[nbr]) {
                visited[nbr] = PATH_FOUND;
                pred[nbr]    = current;
                queue[tail++] = nbr;
                if (nbr == dest) {
                    found = PATH_FOUND;
                    break;
                }
            }
        }
        if (found) break;
    }

    // If we never reached dest, return “no path.”
    if (!found) {
        return PATH_NOT_FOUND;
    }

    // Reconstruct the path by walking pred[] from dest back to start
    location_t reversed_path[MAX_PATH_LEN];
    int   path_len = 0;
    int8_t walker = (int8_t)dest;
    while (walker != -1) {
        reversed_path[path_len++] = (location_t)walker;
        walker = pred[walker];
    }
    // Now reversed_path[0] = dest, reversed_path[path_len−1] = start.

    // Build a forward‐linked list in path_nodes[]:
    //   path_nodes[0] → path_nodes[1] → … → path_nodes[path_len−1], where
    //   each node.l oc = actual location, and node.next = next‐index or −1
    int8_t prev_index = -1;
    int8_t next_index = 0;
    for (i = path_len - 1; i >= 0; i--) {
        path_nodes[next_index].loc  = reversed_path[i];
        path_nodes[next_index].next = -1;
        if (prev_index >= 0) {
            path_nodes[prev_index].next = next_index;
        }
        prev_index = next_index;
        next_index++;
    }

    // The head of the list is always index 0
    return PATH_FOUND;
}

void print_path(void) {
  int8_t walker = 0;
  while(walker >= 0) {
    if (path_nodes[walker].next == -1) {
      report("%s\n", IND_TO_STR_MAP[path_nodes[walker].loc]);
      return;
    } else {
      report("%s -> ", IND_TO_STR_MAP[path_nodes[walker].loc]);
      walker = path_nodes[walker].next;
    }
  }
}

void execute_path_process(void) {

  switch(execute_path_status) {

    case PATH_EXECUTE_IDLE:
      break;

    case PATH_EXECUTE_MOVEMENT_FAILED:
    case PATH_EXECUTE_INVALID_MOVE_WANTED:
      report("Path Execution Failed! Status: %s \n", EXECUTE_PATH_STATUS_TO_STRING[execute_path_status]);
      idx = 0;
      next_idx = 0;
      execute_path_status = PATH_EXECUTE_IDLE;
      break;

    case PATH_EXECUTE_STARTING:
      //TODO: test if path_nodes is assigned
#ifdef PATH_PLANNING_DEBUG
      printf("Path Execution Status: %s \n", EXECUTE_PATH_STATUS_TO_STRING[execute_path_status]);
#endif
      execute_path_status = PATH_EXECUTE_GETTING_NEXT_MOVEMENT;

#ifdef PATH_PLANNING_DEBUG
      printf("Path Execution Status: %s \n", EXECUTE_PATH_STATUS_TO_STRING[execute_path_status]);
#endif
      break;

    case PATH_EXECUTE_GETTING_NEXT_MOVEMENT:
      if (path_nodes[idx].next >= 0) {
        cur = path_nodes[idx].loc;
        int next_idx = path_nodes[idx].next;
        next = path_nodes[next_idx].loc;

        current_movement_func = get_single_move_func(cur, next);
        if (current_movement_func == NULL) { 
          execute_path_status = PATH_EXECUTE_INVALID_MOVE_WANTED;
        } else {
          idx = next_idx;
          execute_path_status = PATH_EXECUTE_MOVEMENT_IN_PROGRESS;
        }

      } else {
        // finished traversing all the path_node linkedlist :D
        execute_path_status = PATH_EXECUTE_FINISHED_SUCCESSFULLY;
      }
#ifdef PATH_PLANNING_DEBUG
      printf("Path Execution Status: %s \n", EXECUTE_PATH_STATUS_TO_STRING[execute_path_status]);
#endif
      break;

    case PATH_EXECUTE_MOVEMENT_IN_PROGRESS:
      closed_loop_func_status = current_movement_func();  // perform the movement from cur to next
      switch(closed_loop_func_status) {

        case CLOSED_LOOP_MOVEMENT_FAILED:
#ifndef PATH_PLANNING_DEBUG
          printf("current movement func failed!!!\n");
#endif
          execute_path_status = PATH_EXECUTE_MOVEMENT_FAILED;
          break;

        case CLOSED_LOOP_MOVEMENT_IN_PROGRESS:
          break;

        case CLOSED_LOOP_MOVEMENT_SUCCESS:
#ifdef PATH_PLANNING_DEBUG
          printf("current movement func finished successfully!!!\n");
#endif
          execute_path_status = PATH_EXECUTE_GETTING_NEXT_MOVEMENT;
          break;

        /* case CLOSED_LOOP_MOVEMENT_IDLE: */
          /* //WTF?? IMPOSSIBLE */
          /* break; */
      }
#ifdef PATH_PLANNING_DEBUG
      printf("Path Execution Status: %s \n", EXECUTE_PATH_STATUS_TO_STRING[execute_path_status]);
#endif
      break;

    case PATH_EXECUTE_FINISHED_SUCCESSFULLY:
      idx = 0;
      next_idx = 0;
      closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IDLE;
      execute_path_status = PATH_EXECUTE_IDLE;
      report("Path Execution Finished Successfully!");
      break;
  }
}

void execute_path_start(void) {
  execute_path_status = PATH_EXECUTE_STARTING;
}

void execute_path_stop(void) {
  idx = 0;
  next_idx = 0;
  closed_loop_func_status = CLOSED_LOOP_MOVEMENT_IDLE;
  execute_path_status = PATH_EXECUTE_IDLE;
}

execute_path_status_t execute_path_get_status(void) { return execute_path_status; }

