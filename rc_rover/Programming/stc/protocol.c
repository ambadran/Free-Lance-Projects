#include "project-defs.h"

static char line[LINE_BUFFER_SIZE];
uint8_t char_count = 0;
uint8_t c;


static GpioConfig led_pin = GPIO_PIN_CONFIG(LED_PORT, LED_PIN, GPIO_BIDIRECTIONAL_MODE);

// Assign myFunction to a function pointer
uint8_t (*uart_receive_func_ptr)(uint8_t*, uint8_t) __reentrant = uartGetCharacter_modified;
uint8_t (*nrf24_receive_func_ptr)(uint8_t*, uint8_t) __reentrant = nrf24_receive;


static uint8_t uartGetCharacter_modified(uint8_t* data, uint8_t size) __reentrant {
  *data = UART_RECEIVE_EMPTY;
  uartGetBlock(CONSOLE_UART, data, size, NON_BLOCKING);
  return *data;
}

static void protocol_read_line(uint8_t (*func)(uint8_t*, uint8_t) __reentrant, uint8_t flag_to_compare_to) {

    do { 
      // line end
      if ((c == '\n') || (c == '\r')) {
        line[char_count] = 0; // string termination character
        char_count = 0;

#ifdef ECHO_TERMINAL
        uartSendCharacter(CONSOLE_UART, c, NON_BLOCKING);
#endif
        // executing line!
        protocol_execute_line(line);

      // throw away whitespaces and control characters
      } else if (c <= ' '){
        // do nothing

      // buffer overflow
      } else if (char_count >= (LINE_BUFFER_SIZE-1)) {

        printf("BUFFER OVERFLOW!!!!\n");
        char_count = 0;

      // store uart_rx_buffer in line character array
      } else {

        line[char_count++] = c;
#ifdef ECHO_TERMINAL 
        uartSendCharacter(CONSOLE_UART, c, NON_BLOCKING);
#endif
      }
    } while (func(&c, 1) != flag_to_compare_to);
}

static void protocol_execute_line(char* line) {

  if(line[0] == 0) {

    printf("No Command, Only Enter Received..\n\n");
    
  } else {

    LINE_STATUS line_state = terminal_execute_line(line);

    if (line_state == LINE_PASSED) { printf("Command passed..\n\n"); } 
    else if (line_state == LINE_FAILED) { printf("Command Failed..\n\n"); }

  }

}

void protocol_main_loop(void) {

  gpioConfigure(&led_pin);

  while(1) {

    if (uartGetCharacter_modified(&c, 1) != UART_RECEIVE_EMPTY) {

      protocol_read_line(uart_receive_func_ptr, UART_RECEIVE_EMPTY);

    } else if (nrf24_receive(&c, 1) != RECEIVE_FIFO_EMPTY) {

      protocol_read_line(nrf24_receive_func_ptr, RECEIVE_FIFO_EMPTY);

    }

#if ULTRASONIC_STATE_MACHINE
    // ultrasonic_reading
    processs_ultrasonic_phases();
#endif

    // toggle led
    gpioToggle(&led_pin);

    /* while (get_stepper_state()); // don't receive other tasks */
  }
}
