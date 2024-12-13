#ifndef BUTTONS_H
#define BUTTONS_H

#define BUTTON_COUNT 3
#define BUTTON1_IND 0
#define BUTTON2_IND 1
#define ENCODER_BUTTON_IND 2

typedef enum {
  BUTTON_IDLE,
  BUTTON_PRESSED,
  BUTTON_COOLDOWN
} button_status_t;

typedef struct {
  GpioConfig gpio;
  button_status_t button_status;
  uint32_t reactivation_time;
} button_t;

void buttons_init(void);
void buttons_process(void);
button_status_t get_button_status(uint8_t button_ind);
button_status_t get_encoder_button_status(void);
button_status_t get_button1_status(void);
button_status_t get_button2_status(void);

#endif
