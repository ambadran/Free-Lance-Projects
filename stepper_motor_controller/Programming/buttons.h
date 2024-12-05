#ifndef BUTTONS_H
#define BUTTONS_H

typedef enum {
  BUTTON_IDLE,
  BUTTON_PRESSED,
  BUTTON_COOLDOWN
} button_status_t

void buttons_init(void);
void buttons_process(void);

#endif
