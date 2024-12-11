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
  EA = 1; // enable interrupts

  // Sending start 
  delay1ms(1000);

  // Main Routine
  /* uint32_t time; */
  int16_t distance = 30;
  while(1) {
    // encoder testing
    /* encoder_process(); */
    /* printf("Encoder Value: %d \r", encoder_count()); */
    /* delay1ms(200); */

    // global timer testing
    /* time = get_current_time(); */
    /* printf("Time since power on: %lu ms\n", time-1); */
    /* delay1ms(997); */

    // buttons test
    /* buttons_process(); */
    /* for(uint8_t button_ind=0; button_ind<BUTTON_COUNT ; button_ind++) { */
    /*   if(get_button_status(button_ind) == BUTTON_PRESSED) { */
    /*     printf("Button%d: Pressed\n", button_ind); */
    /*   } */
    /* } */

    // switch test
    /* printf("Switch value: %d \r", get_switch_status()); */

    // stepper_driver test
    /* stepper_motor_move(distance); */
    /* distance *= -1; */
    /* while(get_stepper_state()); // wait until finish */
  }
}
