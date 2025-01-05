#include "project-defs.h"

/**** MUST DECLARE INTERRUPTS IN MAIN.C FILE AS PER SDCC REQUIREMENT ****/
INTERRUPT(GLOBAL_TIMER_ISR, GLOBAL_TIMER_INTERRUPT);

void main(void) {
  INIT_EXTENDED_SFR();

  // Inits
  EA = 1;
  serialConsoleInitialise(
      CONSOLE_UART, 
      CONSOLE_SPEED, 
      CONSOLE_PIN_CONFIG
      );

  global_timer_init();
  report_init();
  mpu9250_init();
  differential_control_init();
  neo_m8n_init();
  nrf24_device(RECEIVER, RESET);

  // Main Routine
  protocol_main_loop();

}
