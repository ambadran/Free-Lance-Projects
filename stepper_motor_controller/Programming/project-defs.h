#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_H

/* Console settings */
#define CONSOLE_SPEED 115200
#define CONSOLE_UART UART1
#define CONSOLE_PIN_CONFIG 0

/* Buttons Pin Definitions - external pin interrupts */
#define BUTTON1_PORT 3
#define BUTTON1_PIN 6
#define BUTTON2_PORT 3
#define BUTTON2_PIN 3
#define ENCODER_BUTTON_PORT 3
#define ENCODER_BUTTON_PIN 2
#define BUTTON_COOLDOWN_PERIOD 400 // time in ms to stop reading button if pressed

/* Switch Pin Definitions */
#define SWITCH_PORT 3
#define SWITCH_PIN 7

/* Encoder Pin Definitions */
#define HAL_PWM_API_QUADRATURE_ENCODER

/* Stepper Motor Settings */
#define STEPPER_MOTOR_PORT 2
#define STEPPER_ENABLE_PIN 4
#define STEPPER_MS1_pin 3
#define STEPPER_MS2_pin 2
#define STEPPER_STEP_PIN 1
#define STEPPER_DIR_PIN 0
#define DEFAULT_STEPPER_FREQUENCY 50
#define STEPPER_CM_TO_STEPS 100 //TODO: needs testing
#define DEFAULT_MICROSTEPPING STEP_1X8
#define DEFAULT_STEP_NUM 3200

/* timer-hal configs */
#define HAL_TIMER_API_STOP_TIMER
#define STEPPER_TIMER TIMER2
#define STEPPER_TIMER_ISR timer2_isr
#define STEPPER_TIMER_INTERRUPT TIMER2_INTERRUPT
#define GLOBAL_TIMER TIMER3
#define GLOBAL_TIMER_ISR timer3_isr
#define GLOBAL_TIMER_INTERRUPT TIMER3_INTERRUPT


/* includes */
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
#include "global_timer.h"
#include "buttons.h"
#include "switch.h"
#include "encoder.h"
#include "stepper_driver.h"
#include "font.h"
#include "ssd1306.h"
#include "display.h"
#include "application.h"


#endif
