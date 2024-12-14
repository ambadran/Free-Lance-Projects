#include "project-defs.h"

static button_t button1;
static button_t button2;
static button_t encoder_button;
static button_t* buttons[] = {&button1, &button2, &encoder_button};

void buttons_init(void) {
  /* setting up button GpioConfig variable */
  button1.gpio.port = BUTTON1_PORT,
  button1.gpio.pin = BUTTON1_PIN, 
  button1.gpio.count = 1, 
  button1.gpio.pinMode = GPIO_HIGH_IMPEDANCE_MODE,	
  button1.gpio.pinInterrupt = DISABLE_GPIO_PIN_INTERRUPT, 
  button1.gpio.interruptTrigger = GPIO_RISING_EDGE, 
  button1.gpio.wakeUpInterrupt = DISABLE_GPIO_PIN_WAKE_UP,
  button1.gpio.schmidtTrigger = ENABLE_SCHMIDT_TRIGGER,
  button1.gpio.internalPullUp = ENABLE_INTERNAL_PULL_UP,
  button1.gpio.speed = GPIO_SPEED_SLOWEST,
  button1.gpio.digitalInput = ENABLE_GPIO_DIGITAL_INPUT,

  button2.gpio.port = BUTTON2_PORT, 
  button2.gpio.pin = BUTTON2_PIN, 
  button2.gpio.count = 1, 
  button2.gpio.pinMode = GPIO_HIGH_IMPEDANCE_MODE,	
  button2.gpio.pinInterrupt = DISABLE_GPIO_PIN_INTERRUPT, 
  button2.gpio.interruptTrigger = GPIO_RISING_EDGE, 
  button2.gpio.wakeUpInterrupt = DISABLE_GPIO_PIN_WAKE_UP,
  button2.gpio.schmidtTrigger = ENABLE_SCHMIDT_TRIGGER,
  button2.gpio.internalPullUp = ENABLE_INTERNAL_PULL_UP,
  button2.gpio.speed = GPIO_SPEED_SLOWEST,
  button2.gpio.digitalInput = ENABLE_GPIO_DIGITAL_INPUT,

  encoder_button.gpio.port = ENCODER_BUTTON_PORT, 
  encoder_button.gpio.pin = ENCODER_BUTTON_PIN, 
  encoder_button.gpio.count = 1, 
  encoder_button.gpio.pinMode = GPIO_HIGH_IMPEDANCE_MODE,	
  encoder_button.gpio.pinInterrupt = DISABLE_GPIO_PIN_INTERRUPT, 
  encoder_button.gpio.interruptTrigger = GPIO_RISING_EDGE, 
  encoder_button.gpio.wakeUpInterrupt = DISABLE_GPIO_PIN_WAKE_UP,
  encoder_button.gpio.schmidtTrigger = ENABLE_SCHMIDT_TRIGGER,
  encoder_button.gpio.internalPullUp = ENABLE_INTERNAL_PULL_UP,
  encoder_button.gpio.speed = GPIO_SPEED_SLOWEST,
  encoder_button.gpio.digitalInput = ENABLE_GPIO_DIGITAL_INPUT,

  /* initiating the GPIOs */
  gpioConfigure(&button1.gpio);
  gpioConfigure(&button2.gpio);
  gpioConfigure(&encoder_button.gpio);

  button1.button_status = BUTTON_IDLE;
  button2.button_status = BUTTON_IDLE;
  encoder_button.button_status = BUTTON_IDLE;

  button1.reactivation_time = 0;
  button2.reactivation_time = 0;
  encoder_button.reactivation_time = 0;
  
}

// This function should be constantly running in Main Routine
// It looks for any button change. 
// if found will stop reading that button for BUTTON_COOLDOWN_PERIOD
void buttons_process(void) {

  for (uint8_t i=0;i<BUTTON_COUNT;i++) {

    if(buttons[i]->button_status == BUTTON_IDLE) {

      if(!gpioRead(&buttons[i]->gpio)) {
        buttons[i]->reactivation_time = get_current_time() + BUTTON_COOLDOWN_PERIOD;
        buttons[i]->button_status = BUTTON_PRESSED;
      }

    } else {
      if(get_current_time() >= buttons[i]->reactivation_time) {
        buttons[i]->button_status = BUTTON_IDLE;
      }
    }

  }
}

// return current button index
// if it's BUTTON_PRESSED, it will return that and make it BUTTON_COOLDOWN
// this is make sure a button press is not processed more than once
button_status_t get_button_status(uint8_t button_ind) {
  if (buttons[button_ind]->button_status == BUTTON_PRESSED) {
    buttons[button_ind]->button_status = BUTTON_COOLDOWN;
    return BUTTON_PRESSED;
  }
  return buttons[button_ind]->button_status;
}

button_status_t get_encoder_button_status(void) { return get_button_status(ENCODER_BUTTON_IND); }
button_status_t get_button1_status(void) { return get_button_status(BUTTON1_IND); }
button_status_t get_button2_status(void) { return get_button_status(BUTTON2_IND); }


