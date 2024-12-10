#include "project-defs.h"

const switch_status_t UINT8_TO_SWITCH_STATUS[] = {SWITCH_OFF, SWITCH_ON};
static GpioConfig switch_gpio = {.port = SWITCH_PORT, 
                             .pin = SWITCH_PIN, 
                             .count = 1, 
                             .pinMode = GPIO_HIGH_IMPEDANCE_MODE,	
                             .pinInterrupt = DISABLE_GPIO_PIN_INTERRUPT, 
                             .interruptTrigger = GPIO_RISING_EDGE, 
                             .wakeUpInterrupt = DISABLE_GPIO_PIN_WAKE_UP,
                             .schmidtTrigger = ENABLE_SCHMIDT_TRIGGER,
                             .internalPullUp = ENABLE_INTERNAL_PULL_UP,
                             .speed = GPIO_SPEED_SLOWEST,
                             .digitalInput = ENABLE_GPIO_DIGITAL_INPUT,
                             };

void switch_init(void) {

  gpioConfigure(&switch_gpio);

}

switch_status_t get_switch_status(void) { return UINT8_TO_SWITCH_STATUS[gpioRead(&switch_gpio)]; }
