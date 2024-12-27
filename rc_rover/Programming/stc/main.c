#include "project-defs.h"

void main(void) {

  // inits
  INIT_EXTENDED_SFR();
  EA = 1; // enable interrupts
  serialConsoleInitialise(
      CONSOLE_UART, 
      CONSOLE_SPEED, 
      CONSOLE_PIN_CONFIG
      );
  neo_m8n_init();

  delay1ms(2000);

  //initializing nrf24l01+ as a receiver device
  /* nrf24_device(RECEIVER, RESET); */

  // Main Routine
  protocol_main_loop();

}
