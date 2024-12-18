#include "project-defs.h"

page_t current_page = WELCOME_PAGE;
const void (*application_process_func[])(void) = {application_process_welcome_page, // WELCOME_PAGE
                                            application_process_menu_page, // MENU_PAGE
                                            application_process_step_control_page,
                                            application_process_distance_control_page,
                                            application_process_encoder_control_page,
                                            application_process_step_control_set_frequency,
                                            application_process_step_control_set_microstepping,
                                            application_process_step_control_set_steps,
                                            };

#define FLOAT_POINTER_MAX 4
float_digit_pointer_t float_digit_pointer = 0;
uint8_t float_digits[] = {0, 0, 0, 0, 0};  // 000.00\0

all_inputs_t inputs = {
    .button1 = {BUTTON_IDLE, get_button1_status},
    .button2 = {BUTTON_IDLE, get_button2_status},
    .encoder_button = {BUTTON_IDLE, get_encoder_button_status},
    .switch_ = {SWITCH_OFF, get_switch_status},
    .encoder_value = {0, get_encoder_count}
};

stepper_movement_t stepper_movement = {
  .stepper_enable_status = STEPPER_DISABLE,
  .stepper_direction = STEPPER_CLOCKWISE_DIR,
  .microstepping_value = DEFAULT_MICROSTEPPING,
  .frequency = DEFAULT_STEPPER_FREQUENCY,
  .steps = DEFAULT_STEP_NUM
};

void application_process_welcome_page(void) {

  // Read inputs
  inputs.button1.current_val = inputs.button1.get_func();
  inputs.button2.current_val = inputs.button2.get_func();
  inputs.encoder_button.current_val = inputs.encoder_button.get_func();

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
        encoder_count_reset();
        display_step_control_page_first_time();
        return;

      case 1:
        current_page = DISTANCE_CONTROL_PAGE;
        encoder_count_reset();
        display_distance_control_page_first_time();
        return;

      case 2:
        current_page = ENCODER_CONTROL_PAGE;
        encoder_count_reset();
        display_encoder_control_page_first_time();
        return;
    }
  }

  /* Displaying */
  display_menu_page(inputs.encoder_value.current_val);

}

void application_process_step_control_page(void) {

  /* Read inputs */
  inputs.button1.current_val = inputs.button1.get_func();
  inputs.button2.current_val = inputs.button2.get_func();
  inputs.encoder_button.current_val = inputs.encoder_button.get_func();
  inputs.encoder_value.current_val = inputs.encoder_value.get_func();
  inputs.switch_.current_val = inputs.switch_.get_func();

  /* Process inputs */
  // toggling CW and CCW if button2 is pushed
  if (inputs.button2.current_val == BUTTON_PRESSED) {
    stepper_movement.stepper_direction = !stepper_movement.stepper_direction;
    stepper_set_dir(stepper_movement.stepper_direction);
    display_update_stepper_dir(stepper_movement.stepper_direction);
  }

  // toggling ENABLE/DISABLE state by checking if switch is changed
  if (inputs.switch_.current_val != stepper_movement.stepper_enable_status) {
    stepper_movement.stepper_enable_status = !stepper_movement.stepper_enable_status;
    stepper_set_enable(stepper_movement.stepper_enable_status);
    display_update_stepper_enable(stepper_movement.stepper_enable_status);
  }

  // constraining the encoder value to (0-STEP_CONTROL_PAGE_OPTIONS_NUM)
  if (inputs.encoder_value.current_val >= STEP_CONTROL_PAGE_OPTIONS_NUM) {
    encoder_count_reset();
    inputs.encoder_value.current_val = inputs.encoder_value.get_func();
  } else if (inputs.encoder_value.current_val < 0) {
    encoder_count_set(STEP_CONTROL_PAGE_OPTIONS_NUM-1);
    inputs.encoder_value.current_val = inputs.encoder_value.get_func();
  }

  // Run Movement if button1 triggered
  if (inputs.button1.current_val == BUTTON_PRESSED) {
    if(!get_stepper_state()){
      stepper_motor_move(&stepper_movement);
      printf("Runing Stepper Motor!\n");
    }
  }

  // Go to selected option if encoder button is pressed
  if(inputs.encoder_button.current_val == BUTTON_PRESSED) {
    switch(inputs.encoder_value.current_val) {
      case 0:
        current_page = STEP_CONTROL_SET_FREQUENCY_OPTION;
        encoder_count_set(stepper_movement.frequency);
        return;

      case 1:
        current_page = STEP_CONTROL_SET_MICROSTEPPING_OPTION;
        encoder_count_set(stepper_movement.microstepping_value);
        return;

      case 2:
        current_page = STEP_CONTROL_SET_STEPS_OPTION;
        encoder_count_set(stepper_movement.steps);
        return;

      case 3:  // Go Back Option
        current_page = MENU_PAGE;
        display_menu_page(NOTHING_SELECTED);
        return;

      case 4:
        // Run the stepper motor movement!!
        if(!get_stepper_state()){
          stepper_motor_move(&stepper_movement);
          printf("Runing Stepper Motor!\n");
        }
        break;
    }
  }

  /* Displaying */
  display_step_control_page(inputs.encoder_value.current_val);

}

void application_process_step_control_set_frequency(void) {
  /* Read inputs */
  inputs.encoder_button.current_val = inputs.encoder_button.get_func();
  inputs.encoder_value.current_val = inputs.encoder_value.get_func();

  /* Process inputs */
  if(inputs.encoder_value.current_val < 0) { 
    inputs.encoder_value.current_val = 0; 
  }
  if(inputs.encoder_button.current_val == BUTTON_PRESSED) {
    // set stepper steps
    stepper_movement.frequency = inputs.encoder_value.current_val;

    // Go back to step control page
    current_page = STEP_CONTROL_PAGE;
    encoder_count_reset();
    display_step_control_page_first_time();

    return;

  }

  /* Display */
  display_step_control_set_frequency_option(inputs.encoder_value.current_val);
}
void application_process_step_control_set_microstepping(void) {
  /* Read inputs */
  inputs.encoder_button.current_val = inputs.encoder_button.get_func();
  inputs.encoder_value.current_val = inputs.encoder_value.get_func();

  /* Process inputs */
  if(inputs.encoder_value.current_val < 0) { 
    inputs.encoder_value.current_val = 0; 
  }
  if(inputs.encoder_button.current_val == BUTTON_PRESSED) {
    // set stepper steps
    /* stepper_movement.microstepping_value = inputs.encoder_value.current_val; */
    //TODO:

    // Go back to step control page
    current_page = STEP_CONTROL_PAGE;
    encoder_count_reset();
    display_step_control_page_first_time();

    return;

  }

  /* Display */
  display_step_control_set_microstepping_option(inputs.encoder_value.current_val);
}
void application_process_step_control_set_steps(void) {

  /* Read inputs */
  inputs.encoder_button.current_val = inputs.encoder_button.get_func();
  inputs.encoder_value.current_val = inputs.encoder_value.get_func();

  /* Process inputs */
  // constraining the encoder value
  if(inputs.encoder_value.current_val < 0) { 
    encoder_count_set(9);
    inputs.encoder_value.current_val = inputs.encoder_value.get_func();
  } else if (inputs.encoder_value.current_val >= 10) {
    encoder_count_reset();
    inputs.encoder_value.current_val = inputs.encoder_value.get_func();
  }
  float_digits[float_digit_pointer] = inputs.encoder_value.current_val;

  // Processing current digit
  if(inputs.encoder_button.current_val == BUTTON_PRESSED) {

    encoder_count_reset();
    if(float_digit_pointer >= FLOAT_POINTER_MAX) {

        float_digit_pointer = 0;

        // set stepper steps
        stepper_set_steps_from_float_digits(&stepper_movement, float_digits);

        // Go back to step control page
        current_page = STEP_CONTROL_PAGE;
        encoder_count_reset();
        display_step_control_page_first_time();

        return;
    }
    float_digit_pointer++;
  }

  /* Display */
  display_step_control_set_steps_option(float_digits, float_digit_pointer);

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

