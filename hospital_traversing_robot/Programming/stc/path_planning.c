/* This component implements BFS algorithm to find the shortest path from a specific location to the next
 * The components of BFS:
 *  1- State: the enum to hold the room IDs
 *  2- Successors: The adjacency array to return what are the successors of a specific location (state)
 *  3- Frontier: the Queue that will hold the result as it being built and be returned in the end
 *  4- Explored: the set that we save wrong trials in
 *  5- node path: the current location (state) being tested and the rest of the path
 */
#include "project-defs.h"

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
  "STAIR0",
  "STAIR1",
  "STAIR2"
};

/* Please refer to the diagram drawn by hand to have better understanding of the map.
 * This is an adjacency list implemented as 2D array to perform the successors functionality.
 * This is the Graph representation as well as the successor_counts
 */
static const location_t successors[LOCATION_COUNT][MAX_NEIGHBORS] = {
    // ROOM0 .. ROOM8 each only connects to its “front”
    [ROOM0] = { ROOM_FRONT0,         -1,                  -1 },
    [ROOM1] = { ROOM_FRONT1,         -1,                  -1 },
    [ROOM2] = { ROOM_FRONT2,         -1,                  -1 },
    [ROOM3] = { ROOM_FRONT3,         -1,                  -1 },
    [ROOM4] = { ROOM_FRONT4,         -1,                  -1 },
    [ROOM5] = { ROOM_FRONT5,         -1,                  -1 },
    [ROOM6] = { ROOM_FRONT6,         -1,                  -1 },
    [ROOM7] = { ROOM_FRONT7,         -1,                  -1 },
    [ROOM8] = { ROOM_FRONT8,         -1,                  -1 },

    // ROOM_FRONTx each connects to its ROOMx, and to the corridor chain, 
    // and for floors 0 and 2, also to the appropriate stair‐front.
    [ROOM_FRONT0] = { ROOM_FRONT1,         ROOM0,        -1          },
    [ROOM_FRONT1] = { ROOM_FRONT0,         ROOM1,        ROOM_FRONT2 },
    [ROOM_FRONT2] = { STAIR0_FLOOR0_FRONT, ROOM0,        ROOM_FRONT1 },

    [ROOM_FRONT3] = { STAIR0_FLOOR1_FRONT, ROOM3,        ROOM_FRONT4 },
    [ROOM_FRONT4] = { ROOM_FRONT3,         ROOM4,        ROOM_FRONT5 },
    [ROOM_FRONT5] = { ROOM_FRONT4,         ROOM5,        STAIR1_FLOOR1_FRONT },

    [ROOM_FRONT6] = { STAIR1_FLOOR2_FRONT, ROOM6,        ROOM_FRONT7 },
    [ROOM_FRONT7] = { ROOM_FRONT6,         ROOM7,        ROOM_FRONT8 },
    [ROOM_FRONT8] = { ROOM_FRONT7,         ROOM8,        STAIR2_FLOOR2_FRONT },

    // Stair‐fronts each connect back to one Stair and to exactly one room‐front (except STAIR2_FLOOR3_FRONT)
    [STAIR0_FLOOR0_FRONT] = { STAIR0, ROOM_FRONT2, -1 },
    [STAIR0_FLOOR1_FRONT] = { STAIR0, ROOM_FRONT3, -1 },
    [STAIR1_FLOOR1_FRONT] = { STAIR1, ROOM_FRONT5, -1 },
    [STAIR1_FLOOR2_FRONT] = { STAIR1, ROOM_FRONT6, -1 },
    [STAIR2_FLOOR2_FRONT] = { STAIR2, ROOM_FRONT8, -1 },
    [STAIR2_FLOOR3_FRONT] = { STAIR2,          -1, -1 },

    // STAIR0, STAIR1, STAIR2 each connect to their two front‐nodes
    [STAIR0] = { STAIR0_FLOOR0_FRONT, STAIR0_FLOOR1_FRONT, -1 },
    [STAIR1] = { STAIR1_FLOOR1_FRONT, STAIR1_FLOOR2_FRONT, -1 },
    [STAIR2] = { STAIR2_FLOOR2_FRONT, STAIR2_FLOOR3_FRONT, -1 }
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

    // Stairs themselves:
    [STAIR0] = 2,
    [STAIR1] = 2,
    [STAIR2] = 2
};

// Result Path linkedlist
static PathNode path_nodes[LOCATION_COUNT];

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
    for (int i = 0; i < LOCATION_COUNT; i++) {
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
    for (int i = path_len - 1; i >= 0; i--) {
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
      printf("%s\n", IND_TO_STR_MAP[path_nodes[walker].loc]);
      return;
    } else {
      printf("%s -> ", IND_TO_STR_MAP[path_nodes[walker].loc]);
      walker = path_nodes[walker].next;
    }
  }
}
