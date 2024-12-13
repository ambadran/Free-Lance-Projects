#include "project-defs.h"

all_inputs_t inputs = {
    .button1 = {BUTTON_IDLE, get_button1_status},
    .button2 = {BUTTON_IDLE, get_button2_status},
    .encoder_button = {BUTTON_IDLE, get_encoder_button_status},
    .switch_ = {SWITCH_OFF, get_switch_status},
    .encoder_value = {0, get_encoder_count}
};

application_page_t current_page = WELCOME_PAGE;

void application_init(void) {

  // Welcome Window
  SSD1306_ClearScreen();
  SSD1306_SetPosition(1, 1);
  SSD1306_DrawString("      Bipolar");
  SSD1306_SetPosition(1, 2);
  SSD1306_DrawString("   Stepper Motor");
  SSD1306_SetPosition(1, 3);
  SSD1306_DrawString("     Controller");
  SSD1306_SetPosition(1, 4);
  SSD1306_DrawString("        V1.0");

  SSD1306_SetPosition(1, 6);
  SSD1306_DrawString("       Press..");

  SSD1306_UpdateScreen();

  delay1ms(1500);
  
}

void application_update_input_values(void) {
  inputs.button1.current_val = inputs.button1.get_func();
  inputs.button2.current_val = inputs.button2.get_func();
  inputs.encoder_button.current_val = inputs.encoder_button.get_func();
  inputs.switch_.current_val = inputs.switch_.get_func();
  inputs.encoder_value.current_val = inputs.encoder_value.get_func();
}

void application_process_welcome_page(void) {
  if (inputs.button1.current_val || inputs.button2.current_val || inputs.encoder_button.current_val) {
    current_page = CHOOSE_CONTROL_MODE;
  }
}

void application_process_inputs(void) {

  switch (current_page) {

    case WELCOME_PAGE:
      application_process_welcome_page();
      break;

    case CHOOSE_CONTROL_MODE:
      application_process_choose_control_mode_page();

  }
      

}
