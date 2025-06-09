
// closed_loop_movements.h
#ifndef CLOSED_LOOP_MOVEMENTS_H
#define CLOSED_LOOP_MOVEMENTS_H

#include <stdint.h>
#include <stdbool.h>
#include "path_planning.h"  // your PathNode and location_t definitions

// A function pointer type for a closed-loop movement from one location to the next
typedef void (*movement_fn_t)(void);

// Returns the movement function to go from 'from' to 'to', or NULL if invalid
movement_fn_t closed_loop_get_movement(location_t from, location_t to);

// Executes the entire path: calls each segment's movement function in turn
// path_nodes[] is the array filled by find_path_nohead(), terminated by next == -1
void closed_loop_execute_path(const PathNode path_nodes[]);

// -- You should implement these in closed_loop_movements.c --
// Prototypes for each primitive movement between adjacent locations:
// For example:
// void move_room_front0_to_room0(void);
// void move_room_front0_to_stair0_floor0_front(void);
// void move_stair0_to_stair0_floor1_front(void);
// ... one prototype per adjacent pair

#endif // CLOSED_LOOP_MOVEMENTS_H


// closed_loop_movements.c
#include "closed_loop_movements.h"
#include <stddef.h>

// Forward declarations of your primitive movements (fill these out):
// e.g.
// void move_ROOM_FRONT0_to_ROOM0(void)    { /* your control code */ }
// void move_ROOM_FRONT0_to_STAIR0_FLOOR0_FRONT(void) { /* ... */ }
// ...

// Internal lookup table for from→to mapping
static struct {
    location_t from;
    location_t to;
    movement_fn_t fn;
} movement_map[] = {
    // Floor 0:
    { ROOM_FRONT0, ROOM0,                  move_ROOM_FRONT0_to_ROOM0 },
    { ROOM_FRONT0, STAIR0_FLOOR0_FRONT,    move_ROOM_FRONT0_to_STAIR0_FLOOR0_FRONT },
    { ROOM0,       ROOM_FRONT0,            move_ROOM0_to_ROOM_FRONT0 },
    { STAIR0_FLOOR0_FRONT, ROOM_FRONT0,    move_STAIR0_FLOOR0_FRONT_to_ROOM_FRONT0 },
    // Floor 1:
    { ROOM_FRONT3, ROOM3,                  move_ROOM_FRONT3_to_ROOM3 },
    { ROOM_FRONT3, STAIR0_FLOOR1_FRONT,    move_ROOM_FRONT3_to_STAIR0_FLOOR1_FRONT },
    // ... add entries for every adjacent pair ...
    
    // Termination entry
    { LOCATION_COUNT, LOCATION_COUNT, NULL }
};

movement_fn_t closed_loop_get_movement(location_t from, location_t to)
{
    for (int i = 0; movement_map[i].fn != NULL; i++) {
        if (movement_map[i].from == from && movement_map[i].to == to) {
            return movement_map[i].fn;
        }
    }
    return NULL;
}

void closed_loop_execute_path(const PathNode path_nodes[])
{
    int idx = 0;
    while (path_nodes[idx].next >= 0) {
        location_t cur = path_nodes[idx].loc;
        int next_idx = path_nodes[idx].next;
        location_t nxt = path_nodes[next_idx].loc;

        movement_fn_t mv = closed_loop_get_movement(cur, nxt);
        if (mv) {
            mv();  // perform the movement from cur to nxt
        } else {
            // invalid step: no mapping found
            // handle error (e.g. stop, report)
        }
        idx = next_idx;
    }
}
