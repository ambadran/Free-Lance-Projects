#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_H

/* serial-console Settings */
#define CONSOLE_SPEED 115200UL
#define CONSOLE_UART UART1
#define CONSOLE_PIN_CONFIG 0

/* adv-pwm Settings */
/* #define PWM_MOTOR_FREQ 100UL */
/* #define PWM_MOTOR_COUNTER PWM_COUNTER_A */
/* #define PWM_MOTOR_CHANNEL PWM_Channel0 */
/* #define PWM_MOTOR_PIN_CONFIG 0 */

/* timer-hal configs */
#define HAL_TIMER_API_STOP_TIMER
#define ULTRASONIC_TIMER TIMER3
#define ULTRASONIC_TIMER_ISR timer3_isr
#define ULTRASONIC_TIMER_INTERRUPT TIMER3_INTERRUPT

/* external pin interrupts */
#define ULTRASONIC_INT_PIN_ISR extint0_isr
#define ULTRASONIC_INT_PIN_INTERRUPT EXTINT0_INTERRUPT

/* SPI settings */
#define SPI_PIN_CONFIG 0
#define SPI_SPEED 4000000UL

/* I2C settings

/* nRF24L01 settings */
/* #define PRINT_NRF24_REGISTERS */ //uncomment to print all the nrf24l01 registers after setting them with nrf24_device()
#define NRF24_CSN_PORT 2
#define NRF24_CSN_PIN 7
#define NRF24_CE_PORT 5
#define NRF24_CE_PIN 4
#define RF_CHANNEL_DEFAULT 43

/* Ultrasonic Sensor Settings*/
#define ULTRASONIC_TRIGGER_PORT 3
#define ULTRASONIC_TRIGGER_PIN 3
#define ULTRASONIC_ECHO_PORT 3
#define ULTRASONIC_ECHO_PIN 2
/* #define ULTRASONIC_STATE_MACHINE */

/* MPU9250 Settings */

/* NEO M8N Settings */

/* Differential Control Settings */




// Others 
#define LED_PORT 2
#define LED_PIN 6
/* #define GPIO_HAS_INT_WK */



#include <STC/8H8KxxU/SKDIP28.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <delay.h>
#include <gpio-hal.h>
#include <timer-hal.h>
#include <uart-hal.h>
#include <spi-hal.h>
#include <i2c-hal.h>
#include <serial-console.h>
#include "nrf24l01.h"
#include "ultrasonic.h"
#include "differential_control.h"
#include "neo_m8n.h"
#include "mpu9250.h"
#include "terminal.h"
#include "protocol.h"

#endif
