#include "project-defs.h"

static GpioConfig button1 = GPIO_PIN_CONFIG(BUTTON1_PORT, BUTTON1_PIN, GPIO_BIDIRECTIONAL_MODE);
static GpioConfig button2 = GPIO_PIN_CONFIG(BUTTON2_PORT, BUTTON2_PIN, GPIO_BIDIRECTIONAL_MODE);
static GpioConfig encoder_button = GPIO_PIN_CONFIG(ENCODER_BUTTON_PORT, ENCODER_BUTTON_PIN, GPIO_BIDIRECTIONAL_MODE);

static button_status_t button1_status = BUTTON_IDLE;
static button_status_t button2_status = BUTTON_IDLE;
static button_status_t encoder_button_status = BUTTON_IDLE;

void buttons_init(void) {

  gpioConfigure(&button1);
  gpioConfigure(&button2);
  gpioConfigure(&encoder_button);
  
}

// This function should be constantly running in Main Routine
// It looks for any button change. 
// if found will stop reading that button for BUTTON_COOLDOWN_PERIOD
void buttons_process(void) {
  //TODO

}

// return current button index
button_status_t get_button_status(uint8_t button_ind) {
  //TODO:
}
