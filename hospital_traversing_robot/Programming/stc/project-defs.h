#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_H

/* gpio-hal Settings */
#define EnA_PORT GPIO_PORT2 // initiated by advpwm
#define EnA_PIN GPIO_PIN1  // initiated by advpwm
#define EnB_PORT GPIO_PORT3  // initiated by advpwm
#define EnB_PIN GPIO_PIN4  // initiated by advpwm
#define IN1_PORT GPIO_PORT2
#define IN1_PIN GPIO_PIN0
#define IN2_PORT GPIO_PORT3
#define IN2_PIN GPIO_PIN7
#define IN3_PORT GPIO_PORT3
#define IN3_PIN GPIO_PIN5
#define IN4_PORT GPIO_PORT3
#define IN4_PIN GPIO_PIN6

#define NRF24_CSN_PORT GPIO_PORT2 
#define NRF24_CSN_PIN GPIO_PIN2
#define NRF24_MOSI_PORT GPIO_PORT2  // iniated by spi-hal
#define NRF24_MOSI_PIN GPIO_PIN3  // iniated by spi-hal
#define NRF24_MISO_PORT GPIO_PORT2  // iniated by spi-hal
#define NRF24_MISO_PIN GPIO_PIN4  // iniated by spi-hal
#define NRF24_SCLK_PORT GPIO_PORT2  // iniated by spi-hal
#define NRF24_SCLK_PIN GPIO_PIN5  // iniated by spi-hal
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
#define UART2_RX_BUFFER_SIZE 233 //MUST DO THIS TO RECEIVE ALL NMEA STATEMENT!

#define CONSOLE_SPEED 115200
#define CONSOLE_UART UART1
#define CONSOLE_PIN_CONFIG 0  // TX->P3.1, RX->P3.0

#define NEO_M8N_UART_SPEED 9600
#define NEO_M8N_UART UART2
#define NEO_M8N_UART_PIN_CONFIG 0  // TX->P1.1, RX->P1.0

/* pwm Settings */
#define HAL_PWM_CALCULATE_PARAMETERS
#define HAL_PWM_NO_COUNTER_INT_HANDLER
#define HAL_PWM_NO_CHANNEL_INT_HANDLER
// EnA pin is P2.1/PWM1N_2
#define PWM_MOTOR_RIGHT_COUNTER PWM_COUNTER_A
#define PWM_MOTOR_RIGHT_CHANNEL PWM_Channel0
#define PWM_MOTOR_RIGHT_PIN_CONFIG 1
#define PWM_MOTOR_RIGHT_OUTPUT_MODE PWM_OUTPUT_N_ONLY 
// EnB in is P3.4/PWM4P_4/PWM8_2
#define PWM_MOTOR_LEFT_COUNTER PWM_COUNTER_B
#define PWM_MOTOR_LEFT_CHANNEL PWM_Channel7
#define PWM_MOTOR_LEFT_PIN_CONFIG 1
#define PWM_MOTOR_LEFT_OUTPUT_MODE PWM_OUTPUT_P_ONLY


/* timer-hal configs */
#define HAL_TIMER_API_STOP_TIMER
// Timer assignments
#define GLOBAL_TIMER TIMER0
#define GLOBAL_TIMER_ISR timer0_isr
#define GLOBAL_TIMER_INTERRUPT TIMER0_INTERRUPT
// TIMER1 is used for CONSOLE_UART
#define NEO_M8N_TIMER UART_USE_OWN_TIMER // TIMER2 is used for UART2
                                         
/* #define ULTRASONIC_TIMER TIMER4 */
/* #define ULTRASONIC_TIMER_ISR timer4_isr */
/* #define ULTRASONIC_TIMER_INTERRUPT TIMER4_INTERRUPT */

/* external pin interrupts */
#define ULTRASONIC_INT_PIN_ISR extint0_isr
#define ULTRASONIC_INT_PIN_INTERRUPT EXTINT0_INTERRUPT

/* SPI settings */
#define SPI_PIN_CONFIG 1
/* #define SPI_SPEED 8000000 */
#define SPI_SPEED 4000000UL

/* I2C settings */
#define CHOOSEN_I2C_PIN_SWITCH 0

/* nRF24L01 settings */
//#define NRF_DEBUGGING  //uncomment to print all the nrf24l01 registers after setting them with nrf24_device()
#define RF_CHANNEL_DEFAULT 46

/* Ultrasonic Sensor Settings*/
/* #define ULTRASONIC_STATE_MACHINE */

/* MPU6050 Settings */
// If this setting is enabled the MCU will do nothing but keep printing the MPU values
#define TEST_MPU 
// Since there is no FPU in this MCU, we'll resort into using fixed point values
// 16384(16-bit reg) = 1g (floating point) = 1000 (fixed point value)
#define ACCEL_SCALE 90  // fixed-point value instead of floating point, 
// I made it 90 instead of 100, because I will take the Ax and Ay value to be the exact degree value -90 to 90
#define GYRO_SCALE 100  // fixed-point value instead of floating point
#define ACCEL_SENSITIVITY ACCEL_SENSITIVITY_0  // 16-bit range is divided into max 2g
#define GYRO_SENSITIVITY GYRO_SENSITIVITY_1  // 16-bit range is divided into max 500deg/sec
//TODO: These values are set in place and no calibration routine yet
#define DEFAULT_ACCEL_OFFSET_X 0
#define DEFAULT_ACCEL_OFFSET_Y 0
#define DEFAULT_ACCEL_OFFSET_Z 0
//NOTE: gyro offset is calculated with every power up using a calibration routine
#define DEFAULT_GYRO_OFFSET_X 0
#define DEFAULT_GYRO_OFFSET_Y 0
#define DEFAULT_GYRO_OFFSET_Z 0
#define GYRO_CALIBRATION_SAMPLES 1000

/* Complementary Filter */
#define COMP_FILTER_DT 100
#define COMP_FILTER_ALPHA 30  // %
#define COMP_FILTER_BETA  (100-COMP_FILTER_ALPHA)  // %

/* NEO M8N Settings */

/* Differential Control Settings */
#define CM_TO_MOVEMENT_MS 500  // 1cm is moved in CM_TO_MOVEMENT_MS
#define DEGREE_TO_MOVEMENT_MS 15 // 1 degree is moved in DEGREE_TO_MOVEMENT_MS
#define PWM_MOTOR_FREQ 10000
#define DEFAULT_PWM_DUTY_CYCLE 50000  // The range is (0-2^16)

// Others 
/* #define GPIO_HAS_INT_WK */
#define LED_BLINK_PERIOD 400

#include <STC/8H8KxxU/SKDIP28.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <delay.h>
#include <gpio-hal.h>
#include <timer-hal.h>
#include <uart-hal.h>
#include <serial-console.h>
#include <spi-hal.h>
#include <i2c-hal.h>
#include <advpwm-hal.h>
#include "global_timer.h"
#include "report.h"
#include "differential_control.h"
#include "neo_m8n.h"
#include "nrf24l01.h"
#include "mpu6050.h"
#include "hmc5883l.h"
#include "complementary_filter.h"
#include "terminal.h"
#include "protocol.h"

#endif
