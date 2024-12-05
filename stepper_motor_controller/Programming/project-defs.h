#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_H

/* Console settings */
#define CONSOLE_SPEED 115200
#define CONSOLE_UART UART1
#define CONSOLE_PIN_CONFIG 0

/* Buttons Pin Definitions */
#define BUTTON1_PORT
#define BUTTON1_PIN
#define BUTTON2_PORT
#define BUTTON2_PIN
#define ENCODER_BUTTON_PORT
#define ENCODER_BUTTON_PIN
#define BUTTON_COOLDOWN_PERIOD 300 // time in ms to stop reading button if pressed

/* Switch Pin Definitions */
#define SWITCH1_PORT
#define SWITCH1_PIN

/* Encoder Pin Definitions */


#include <STC/8H8KxxU/SKDIP28.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <delay.h>
#include <gpio-hal.h>
#include <uart-hal.h>
#include <serial-console.h>
#include <timer-hal.h>
#include <advpwm-hal.h>
#include <i2c-hal.h>
#include "buttons.h"
#include "switch.h"
#include "encoder.h"


#endif
