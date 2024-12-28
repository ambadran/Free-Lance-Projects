#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_H

/* gpio-hal Settings */
#define NRF24_CSN_PORT GPIO_PORT2
#define NRF24_CSN_PIN GPIO_PIN2
#define NRF24_CE_PORT GPIO_PORT2
#define NRF24_CE_PIN GPIO_PIN6

#define ULTRASONIC_TRIGGER_PORT GPIO_PORT3
#define ULTRASONIC_TRIGGER_PIN GPIO_PIN2
#define ULTRASONIC_ECHO_PORT GPIO_PORT3
#define ULTRASONIC_ECHO_PIN GPIO_PIN3

#define NEO_M8N_PORT GPIO_PORT1
#define NEO_M8N_RX_PIN GPIO_PIN0
#define NEO_M8N_TX_PIN GPIO_PIN1

#define LED_PORT GPIO_PORT1
#define LED_PIN GPIO_PIN3

/* uart-hal Settings */
#define HAL_UARTS 2
#define UART2_RX_BUFFER_SIZE 233

#define CONSOLE_SPEED 115200
#define CONSOLE_UART UART1
#define CONSOLE_PIN_CONFIG 0  // TX->P3.1, RX->P3.0

#define NEO_M8N_UART_SPEED 9600
#define NEO_M8N_UART UART2
#define NEO_M8N_UART_PIN_CONFIG 0  // TX->P1.1, RX->P1.0

/* adv-pwm Settings */
/* #define PWM_MOTOR_FREQ 100UL */
/* #define PWM_MOTOR_COUNTER PWM_COUNTER_A */
/* #define PWM_MOTOR_CHANNEL PWM_Channel0 */
/* #define PWM_MOTOR_PIN_CONFIG 0 */

/* timer-hal configs */
// TIMER1 is used for CONSOLE_UART
#define NEO_M8N_TIMER UART_USE_OWN_TIMER // TIMER2 is used for UART2
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

/* I2C settings */

/* nRF24L01 settings */
/* #define PRINT_NRF24_REGISTERS */ //uncomment to print all the nrf24l01 registers after setting them with nrf24_device()
#define RF_CHANNEL_DEFAULT 43

/* Ultrasonic Sensor Settings*/
/* #define ULTRASONIC_STATE_MACHINE */

/* MPU9250 Settings */

/* NEO M8N Settings */

/* Differential Control Settings */

// Others 
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
// #include <i2c-hal.h>
#include <serial-console.h>
#include "neo_m8n.h"

#endif
