#include "project-defs.h"

/**** MUST DECLARE INTERRUPTS IN MAIN.C FILE AS PER SDCC REQUIREMENT ****/
INTERRUPT(GLOBAL_TIMER_ISR, GLOBAL_TIMER_INTERRUPT);
/* INTERRUPT(HCSR04_TIMER_ISR, HCSR04_TIMER_INTERRUPT); */
/* INTERRUPT(HCSR04_INT_PIN_ISR, HCSR04_INT_PIN_INTERRUPT); */

void main(void) {
  INIT_EXTENDED_SFR();

  // Inits
  EA = 1;
  global_timer_init();
  report_init();
  nrf24_device(RECEIVER, RESET);
  neo_m8n_init();
  mpu6050_init();
  hmc5883l_init();
  hcsr04_init();
  differential_control_init();
  orientation_init();

  // Main Routine
  protocol_main_loop();

}
