#include "project-defs.h"

static GpioConfig switch1 = GPIO_PIN_CONFIG(SWITCH1_PORT, SWITCH1_PIN, GPIO_BIDIRECTIONAL_MODE);

void switch_init(void) {

  gpioConfigure(&switch1);

}

switch_status_t get_switch_status(uint8_t switch_ind) {

}
