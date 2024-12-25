#ifndef TERMINAL_H
#define TERMINAL_H

#define MAX_INT_DIGITS 8

#define MODE_FUNC_NONE 0
#define MODE_FUNC_MOVE 1
#define MODE_FUNC_ULTRASONIC 2

typedef enum {
  LINE_PASSED, 
  LINE_FAILED
} LINE_STATUS;

/* typedef struct { */
/*   uint8_t direction;  // 0 for backward, 1 for forward */
/*   float distance;  // in meters */
/* } movement_command_t; */

bool read_int(char *line, uint8_t *char_count, int16_t *value_ptr);
LINE_STATUS terminal_execute_line(char* line);

#endif
