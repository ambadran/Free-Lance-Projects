#ifndef SWITCH_H
#define SWITCH_H

typedef enum {
  SWITCH_GND,
  SWITCH_VCC
} switch_status_t

void switch_init(void);

#endif
