#include "project-defs.h"

void main(void) {

  // inits
  INIT_EXTENDED_SFR();
  EA = 1;
  serialConsoleInitialise(
      CONSOLE_UART, 
      CONSOLE_SPEED, 
      CONSOLE_PIN_CONFIG
      );
  buttons_init();
  switch_init();
  encoder_init();

  // Sending start 
  uartSendBlock(CONSOLE_UART, "\rStarting..\n\n", 12, NON_BLOCKING);

  // Main Routine
  while(1) {

    

  }
}
