#include "project-defs.h"

static GpioConfig led1 = GPIO_PIN_CONFIG(GPIO_PORT1, GPIO_PIN1, GPIO_PUSH_PULL_MODE);
static GpioConfig led2 = GPIO_PIN_CONFIG(GPIO_PORT1, GPIO_PIN0, GPIO_PUSH_PULL_MODE);
static GpioConfig led3 = GPIO_PIN_CONFIG(GPIO_PORT3, GPIO_PIN7, GPIO_PUSH_PULL_MODE);
static GpioConfig led4 = GPIO_PIN_CONFIG(GPIO_PORT3, GPIO_PIN6, GPIO_PUSH_PULL_MODE);
static GpioConfig led5 = GPIO_PIN_CONFIG(GPIO_PORT3, GPIO_PIN3, GPIO_PUSH_PULL_MODE);
static GpioConfig led6 = GPIO_PIN_CONFIG(GPIO_PORT3, GPIO_PIN2, GPIO_PUSH_PULL_MODE);
static GpioConfig led7 = GPIO_PIN_CONFIG(GPIO_PORT1, GPIO_PIN2, GPIO_PUSH_PULL_MODE);
static GpioConfig led8 = GPIO_PIN_CONFIG(GPIO_PORT1, GPIO_PIN3, GPIO_PUSH_PULL_MODE);
static GpioConfig led9 = GPIO_PIN_CONFIG(GPIO_PORT1, GPIO_PIN4, GPIO_PUSH_PULL_MODE);
static GpioConfig led10 = GPIO_PIN_CONFIG(GPIO_PORT1, GPIO_PIN5, GPIO_PUSH_PULL_MODE);

static GpioConfig* LEDS[] = {
  &led1,
  &led2,
  &led3,
  &led4,
  &led5,
  &led6,
  &led7,
  &led8,
  &led9,
  &led10
};

void main(void) {

  INIT_EXTENDED_SFR();
  EA = 1;

  gpioConfigure(&led1);
  gpioConfigure(&led2);
  gpioConfigure(&led3);
  gpioConfigure(&led4);
  gpioConfigure(&led5);
  gpioConfigure(&led6);
  gpioConfigure(&led7);
  gpioConfigure(&led8);
  gpioConfigure(&led9);
  gpioConfigure(&led10);

  serialConsoleInitialise(
      CONSOLE_UART,
      CONSOLE_SPEED,
      CONSOLE_PIN_CONFIG
      );

  int i = 0;
  while (1) {

    for (i=0 ; i<10 ; i++) {
      gpioWrite(LEDS[i], 1);
      delay1ms(DELAY_MS);
    }

    for (i=0 ; i<10 ; i++) {
      gpioWrite(LEDS[i], 0);
    }
  
  }
}
