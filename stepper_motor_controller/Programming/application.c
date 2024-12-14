#include "project-defs.h"

page_t current_page = WELCOME_PAGE;
const void (*application_process_func[])(void) = {application_process_welcome_page, // WELCOME_PAGE
                                            application_process_menu_page, // MENU_PAGE
                                            application_process_step_control_page,
                                            application_process_distance_control_page,
                                            application_process_encoder_control_page
                                            };


all_inputs_t inputs = {
    .button1 = {BUTTON_IDLE, get_button1_status},
    .button2 = {BUTTON_IDLE, get_button2_status},
    .encoder_button = {BUTTON_IDLE, get_encoder_button_status},
    .switch_ = {SWITCH_OFF, get_switch_status},
    .encoder_value = {0, get_encoder_count}
};

stepper_movement_t stepper_movement = {
  .stepper_direction = STEPPER_CLOCKWISE_DIR,
  .microstepping_value = DEFAULT_MICROSTEPPING,
  .frequency = DEFAULT_STEPPER_FREQUENCY,
  .steps = DEFAULT_STEP_NUM
};


void application_update_input_values(void) {
  inputs.button1.current_val = inputs.button1.get_func();
  inputs.button2.current_val = inputs.button2.get_func();
  inputs.encoder_button.current_val = inputs.encoder_button.get_func();
  inputs.switch_.current_val = inputs.switch_.get_func();
  inputs.encoder_value.current_val = inputs.encoder_value.get_func();
}

void application_process_welcome_page(void) {

  // Read inputs
  application_update_input_values();

  // Process inputs
  if (inputs.button1.current_val == BUTTON_PRESSED \
      || inputs.button2.current_val == BUTTON_PRESSED \
      || inputs.encoder_button.current_val == BUTTON_PRESSED) {
    current_page = MENU_PAGE;
    display_menu_page(STEP_CONTROL_PAGE_SELECTED);
    return;
  }

}

void application_process_menu_page(void) {

  /* Read inputs */
  inputs.encoder_value.current_val = inputs.encoder_value.get_func();
  inputs.encoder_button.current_val = inputs.encoder_button.get_func();

  /* Process inputs */
  // constraining the encoder value to (0-CONTROL_PAGE_NUM)
  if (inputs.encoder_value.current_val >= CONTROL_PAGE_NUM) {
    encoder_count_reset();
    inputs.encoder_value.current_val = inputs.encoder_value.get_func();

  } else if (inputs.encoder_value.current_val < 0) {
    encoder_count_set(CONTROL_PAGE_NUM-1);
    inputs.encoder_value.current_val = inputs.encoder_value.get_func();
  }
  // Go to selected page if encoder button is pressed
  if(inputs.encoder_button.current_val == BUTTON_PRESSED) {
    switch(inputs.encoder_value.current_val) {
      case 0:
        current_page = STEP_CONTROL_PAGE;
        display_step_control_page_first_time();
        return;

      case 1:
        current_page = DISTANCE_CONTROL_PAGE;
        display_distance_control_page_first_time();
        return;

      case 2:
        current_page = ENCODER_CONTROL_PAGE;
        display_encoder_control_page_first_time();
        return;
    }
  }

  /* Displaying */
  display_menu_page(inputs.encoder_value.current_val);

}

void application_process_step_control_page(void) {

  /* Read inputs */
  application_process_inputs();

  /* Process inputs */

  /* Executing */

  /* Displaying */

}

void application_process_distance_control_page(void) {
  printf("distance control page not implemented!\n");
  application_process_welcome_page();
}

void application_process_encoder_control_page(void) {
  printf("reached encoder control page not implemented!\n");
  application_process_welcome_page();
}



void application_process_current_page(void) { application_process_func[current_page](); }

