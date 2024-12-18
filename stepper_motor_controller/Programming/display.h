#ifndef DISPLAY_H
#define DISPLAY_H

typedef enum {
  HUNDREDS = 0,
  TENS,
  ONES,
  TENTH,
  HUNDREDTH,
} float_digit_pointer_t;

#define CONTROL_PAGE_NUM 3
#define STEP_CONTROL_PAGE_OPTIONS_NUM 5

typedef enum {
  WELCOME_PAGE = 0,
  MENU_PAGE,
  STEP_CONTROL_PAGE,
  DISTANCE_CONTROL_PAGE,
  ENCODER_CONTROL_PAGE,
  STEP_CONTROL_SET_FREQUENCY_OPTION,
  STEP_CONTROL_SET_MICROSTEPPING_OPTION,
  STEP_CONTROL_SET_STEPS_OPTION,
  BLANK_PAGE
} page_t;

typedef enum {
  STEP_CONTROL_PAGE_SELECTED = 0,
  DISTANCE_CONTROL_PAGE_SELECTED = 1,
  ENCODER_CONTROL_PAGE_SELECTED = 2,
  NOTHING_SELECTED  // the process menu page should prevent encoder value of >CONTROL_PAGE_NUM
} menu_page_selected_page_t;

typedef enum {
  SET_FREQUENCY_OPTION = 0,
  SET_MICROSTEPPING_OPTION,
  SET_STEPS,
  GO_BACK,
  RUN,
  SET_NO_OPTION
} step_control_page_options_t;

void display_welcome_page(void);
void display_menu_page(menu_page_selected_page_t selected_page);
void display_step_control_page_first_time(void);
void display_step_control_page(step_control_page_options_t step_control_page_options);
void display_step_control_set_steps_option(uint8_t* float_digits, float_digit_pointer_t float_digit_pointer);
void display_step_control_set_microstepping_option(int16_t encoder_value);
void display_step_control_set_frequency_option(int16_t encoder_value);
void display_distance_control_page_first_time(void);
void display_encoder_control_page_first_time(void);
void display_update_stepper_dir(stepper_direction_t stepper_direction);
void display_update_stepper_enable(stepper_enable_status_t stepper_enable_status);

#endif
