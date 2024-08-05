#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_H

#include <STC/15W4xxAS/DIP16.h>
#define SMALL_FLASH
#define SMALL_RAM
#define BASIC_GPIO_HAL

#define MAX_LED_NUM 10
#define DELAY_MS 400

#define CONSOLE_SPEED 115200
#define CONSOLE_UART UART1
#define CONSOLE_PIN_CONFIG 0

#include <gpio-hal.h>
#include <delay.h>
#include <uart-hal.h>
#include <serial-console.h>
#include <stdio.h>

#endif
