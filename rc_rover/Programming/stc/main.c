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
  neo_m8n_init();

  // Main Routine
  while(1) {

   neo_m8n_read_statement();

  }

}
