#include "project-defs.h"

static GpioConfig led_pin = GPIO_PIN_CONFIG(LED_PORT, LED_PIN, GPIO_BIDIRECTIONAL_MODE);

void report_init(void) {

  gpioConfigure(&led_pin);

}

void report_toggle_led(void) {
  gpioToggle(&led_pin); // TODO: use global timer to toggle led every second!
}

void report(const uint8_t* string) {
  printf("%s", string);
  // TODO: payload_size
  // TODO: check transmit status
  /* bool transmit_status = nrf24_transmit(string, 20); */
} 


