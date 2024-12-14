#ifndef SWITCH_H
#define SWITCH_H

typedef enum {
  SWITCH_OFF = 0,
  SWITCH_ON
} switch_status_t;

void switch_init(void);
switch_status_t get_switch_status(void);

#endif
