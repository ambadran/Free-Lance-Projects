#include "project-defs.h"

static GpioConfig led_pin = GPIO_PIN_CONFIG(LED_PORT, LED_PIN, GPIO_BIDIRECTIONAL_MODE);
static uint32_t led_timer_count;

void report_init(void) {

  gpioConfigure(&led_pin);
  led_timer_count = get_current_time();

}

void report_toggle_led(void) {
  if ((get_current_time() - led_timer_count) >= LED_BLINK_PERIOD) {

    gpioToggle(&led_pin);
    led_timer_count = get_current_time();                          

  }
}

void report(const uint8_t* string) {
  printf("%s", string);
  // TODO: payload_size
  // TODO: check transmit status
  /* bool transmit_status = nrf24_transmit(string, 20); */
} 


