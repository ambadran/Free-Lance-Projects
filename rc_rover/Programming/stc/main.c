#include "project-defs.h"

/**** MUST DECLARE INTERRUPTS IN MAIN.C FILE AS PER SDCC REQUIREMENT ****/
INTERRUPT(GLOBAL_TIMER_ISR, GLOBAL_TIMER_INTERRUPT);

void main(void) {

  // inits
  INIT_EXTENDED_SFR();
  EA = 1;
  serialConsoleInitialise(
      CONSOLE_UART, 
      CONSOLE_SPEED, 
      CONSOLE_PIN_CONFIG
      );
  differential_control_init();
  neo_m8n_init();

  // Main Routine
  while(1) {

    // Testing NEO module
    /* neo_m8n_read_statement(); */

    // Testing Differential Control
    differential_control_process();

  }

}
