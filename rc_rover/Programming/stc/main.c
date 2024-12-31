#include "project-defs.h"

/**** MUST DECLARE INTERRUPTS IN MAIN.C FILE AS PER SDCC REQUIREMENT ****/
INTERRUPT(GLOBAL_TIMER_ISR, GLOBAL_TIMER_INTERRUPT);

void main(void) {

  // Inits
  INIT_EXTENDED_SFR();
  EA = 1;
  serialConsoleInitialise(
      CONSOLE_UART, 
      CONSOLE_SPEED, 
      CONSOLE_PIN_CONFIG
      );
  nrf24_device(RECEIVER, RESET);
  differential_control_init();
  neo_m8n_init();

  // Main Routine
  protocol_main_loop();

}
