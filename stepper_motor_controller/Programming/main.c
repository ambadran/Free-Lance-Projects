#include "project-defs.h"

/**** MUST DECLARE INTERRUPTS IN MAIN.C FILE AS PER SDCC REQUIREMENT ****/
INTERRUPT(STEPPER_TIMER_ISR, STEPPER_TIMER_INTERRUPT);
INTERRUPT(GLOBAL_TIMER_ISR, GLOBAL_TIMER_INTERRUPT);

void main(void) {

  // inits
  INIT_EXTENDED_SFR();
  serialConsoleInitialise(
      CONSOLE_UART, 
      CONSOLE_SPEED, 
      CONSOLE_PIN_CONFIG
      );
  global_timer_init();
  buttons_init();
  switch_init();
  encoder_init();
  stepper_motor_init();
  SSD1306_Init();
  EA = 1; // enable interrupts
  delay1ms(500);
  application_init();

  // Sending start 

  // Main Routine
  while(1) {

    // Reading latest inputs
    // switch doesn't need any processing
    encoder_process();  // getting latest encoder readings
    buttons_process();  // getting latest button readings
    
    // Processing application

  }
}
