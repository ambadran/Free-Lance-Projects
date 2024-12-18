#include "project-defs.h"


void display_welcome_page(void) {
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
}

//TODO: this variable should be in heap and should be deleted after use
menu_page_selected_page_t menu_page_selected_page = NOTHING_SELECTED;
void display_menu_page(menu_page_selected_page_t selected_page) {
  if(menu_page_selected_page != selected_page) {
    menu_page_selected_page = selected_page;

    SSD1306_ClearScreen();
    SSD1306_SetPosition(1, 0);
    SSD1306_DrawString("      Main Menu");

    switch(selected_page) {

      case STEP_CONTROL_PAGE_SELECTED:
        SSD1306_SetPosition(1, 2);
        SSD1306_DrawString("> Step Control");
        SSD1306_SetPosition(1, 3);
        SSD1306_DrawString("  Distance Control");
        SSD1306_SetPosition(1, 4);
        SSD1306_DrawString("  Encoder Control");
        break;

      case DISTANCE_CONTROL_PAGE_SELECTED:
        SSD1306_SetPosition(1, 2);
        SSD1306_DrawString("  Step Control");
        SSD1306_SetPosition(1, 3);
        SSD1306_DrawString("> Distance Control");
        SSD1306_SetPosition(1, 4);
        SSD1306_DrawString("  Encoder Control");
        break;

      case ENCODER_CONTROL_PAGE_SELECTED:
        SSD1306_SetPosition(1, 2);
        SSD1306_DrawString("  Step Control");
        SSD1306_SetPosition(1, 3);
        SSD1306_DrawString("  Distance Control");
        SSD1306_SetPosition(1, 4);
        SSD1306_DrawString("> Encoder Control");
        break;

      default:
        SSD1306_SetPosition(1, 2);
        SSD1306_DrawString("  Step Control");
        SSD1306_SetPosition(1, 3);
        SSD1306_DrawString("  Distance Control");
        SSD1306_SetPosition(1, 4);
        SSD1306_DrawString("  Encoder Control");
        break;



    } 
    SSD1306_UpdateScreen();
  }
}

void display_step_control_page_first_time(void) {

    SSD1306_ClearScreen();
    SSD1306_SetPosition(1, 0);
    SSD1306_DrawString("     Step Control");
    SSD1306_SetPosition(1, 1);
    SSD1306_DrawString("Dir: CCW         FREE");
    SSD1306_SetPosition(1, 2);
    SSD1306_DrawString("> Frequency");
    SSD1306_SetPosition(1, 3);
    SSD1306_DrawString("  Microstepping");
    SSD1306_SetPosition(1, 4);
    SSD1306_DrawString("  Steps");
    SSD1306_SetPosition(1, 5);
    SSD1306_DrawString("  Go Back");


    SSD1306_SetPosition(1, 7);
    SSD1306_DrawString("         Run         ");

    SSD1306_UpdateScreen();
}

__bit stepper_state_last = 0;
step_control_page_options_t step_control_page_options_last = SET_NO_OPTION;
void display_step_control_page(step_control_page_options_t step_control_page_options) {
  if(step_control_page_options_last != step_control_page_options) {
    step_control_page_options_last = step_control_page_options;
    switch(step_control_page_options) {

      case SET_FREQUENCY_OPTION:
        SSD1306_SetPosition(1, 2);
        SSD1306_DrawString("> Frequency");
        SSD1306_SetPosition(1, 3);
        SSD1306_DrawString("  Microstepping");
        SSD1306_SetPosition(1, 4);
        SSD1306_DrawString("  Steps");
        SSD1306_SetPosition(1, 5);
        SSD1306_DrawString("  Go Back");
        SSD1306_SetPosition(1, 7);
        SSD1306_DrawString("         Run         ");
        break;

      case SET_MICROSTEPPING_OPTION:
        SSD1306_SetPosition(1, 2);
        SSD1306_DrawString("  Frequency");
        SSD1306_SetPosition(1, 3);
        SSD1306_DrawString("> Microstepping");
        SSD1306_SetPosition(1, 4);
        SSD1306_DrawString("  Steps");
        SSD1306_SetPosition(1, 5);
        SSD1306_DrawString("  Go Back");
        SSD1306_SetPosition(1, 7);
        SSD1306_DrawString("         Run         ");
        break;

      case SET_STEPS:
        SSD1306_SetPosition(1, 2);
        SSD1306_DrawString("  Frequency");
        SSD1306_SetPosition(1, 3);
        SSD1306_DrawString("  Microstepping");
        SSD1306_SetPosition(1, 4);
        SSD1306_DrawString("> Steps");
        SSD1306_SetPosition(1, 5);
        SSD1306_DrawString("  Go Back");
        SSD1306_SetPosition(1, 7);
        SSD1306_DrawString("         Run         ");

        break;

      case GO_BACK:
        SSD1306_SetPosition(1, 2);
        SSD1306_DrawString("  Frequency");
        SSD1306_SetPosition(1, 3);
        SSD1306_DrawString("  Microstepping");
        SSD1306_SetPosition(1, 4);
        SSD1306_DrawString("  Steps");
        SSD1306_SetPosition(1, 5);
        SSD1306_DrawString("> Go Back");
        SSD1306_SetPosition(1, 7);
        SSD1306_DrawString("         Run         ");
        break;

      case RUN:
        SSD1306_SetPosition(1, 2);
        SSD1306_DrawString("  Frequency");
        SSD1306_SetPosition(1, 3);
        SSD1306_DrawString("  Microstepping");
        SSD1306_SetPosition(1, 4);
        SSD1306_DrawString("  Steps");
        SSD1306_SetPosition(1, 5);
        SSD1306_DrawString("  Go Back");
        SSD1306_SetPosition(1, 7);
        SSD1306_DrawString(">        Run         ");
        break;

      default:
        SSD1306_SetPosition(1, 2);
        SSD1306_DrawString("  Frequency");
        SSD1306_SetPosition(1, 3);
        SSD1306_DrawString("  Microstepping");
        SSD1306_SetPosition(1, 4);
        SSD1306_DrawString("  Steps");
        SSD1306_SetPosition(1, 5);
        SSD1306_DrawString("  Go Back");
        SSD1306_SetPosition(1, 7);
        SSD1306_DrawString("         Run         ");
        break;



    }
    SSD1306_UpdateScreen();
  }

  // Checking Run
  __bit stepper_state_now = get_stepper_state();
  if(stepper_state_last != stepper_state_now) {
    printf("hapnd\n");
    stepper_state_last = stepper_state_now;
    SSD1306_SetPosition(1, 7);

    if(stepper_state_now) {
      SSD1306_DrawString("*********Run*********");

    } else if (step_control_page_options == RUN){
      SSD1306_DrawString(">        Run         ");

    } else {
      SSD1306_DrawString("         Run         ");
    }
    SSD1306_UpdateScreen();
  } 
}

void display_distance_control_page_first_time(void) {
    SSD1306_ClearScreen();
    SSD1306_SetPosition(1, 3);
    SSD1306_DrawString("   Not Implemented");

    SSD1306_UpdateScreen();
}

void display_encoder_control_page_first_time(void) {
    SSD1306_ClearScreen();
    SSD1306_SetPosition(1, 3);
    SSD1306_DrawString("   Not Implemented");

    SSD1306_UpdateScreen();
}

void display_update_stepper_dir(stepper_direction_t stepper_direction) {
  SSD1306_SetPosition(31, 1);
  if (stepper_direction) {
    SSD1306_DrawString("CW ");
  } else {
    SSD1306_DrawString("CCW");
  }
  SSD1306_UpdateScreen();
}

void display_update_stepper_enable(stepper_enable_status_t stepper_enable_status) {
  SSD1306_SetPosition(103, 1);
  if (stepper_enable_status) {
    SSD1306_DrawString("FREE");
  } else {
    SSD1306_DrawString("HOLD");
  }
  SSD1306_UpdateScreen();

}

void display_step_control_set_steps_option(uint8_t* float_digits, float_digit_pointer_t float_digit_pointer) {
    SSD1306_ClearScreen();
    SSD1306_SetPosition(1, 2);
    SSD1306_DrawString("Setting Steps:");

    // Updating float value for user
    char buffer[20];
    sprintf(buffer, "      %d%d%d.%d%d", float_digits[0], float_digits[1], float_digits[2], float_digits[3], float_digits[4]);
    SSD1306_SetPosition(1, 5);
    SSD1306_DrawString(buffer);

    // Updating float pointer for user
    switch (float_digit_pointer) {
      case HUNDREDS:
        sprintf(buffer, "      ^");
        break;

      case TENS:
        sprintf(buffer, "       ^");
        break;

      case ONES:
        sprintf(buffer, "        ^");
        break;

      case TENTH:
        sprintf(buffer, "          ^");
        break;

      case HUNDREDTH:
        sprintf(buffer, "           ^");
        break;

    }
    SSD1306_SetPosition(1, 6);
    SSD1306_DrawString(buffer);

    SSD1306_UpdateScreen();
}

void display_step_control_set_microstepping_option(int16_t encoder_value) {
    SSD1306_ClearScreen();
    SSD1306_SetPosition(1, 2);
    SSD1306_DrawString("Setting Microstepping:");

    char buffer[5];
    sprintf(buffer,    "      %d", encoder_value);

    SSD1306_SetPosition(1, 5);
    SSD1306_DrawString(buffer);

    SSD1306_UpdateScreen();
}

void display_step_control_set_frequency_option(int16_t encoder_value) {
    SSD1306_ClearScreen();
    SSD1306_SetPosition(1, 2);
    SSD1306_DrawString("Setting frequency:");

    char buffer[5];
    sprintf(buffer,    "      %d", encoder_value);

    SSD1306_SetPosition(1, 5);
    SSD1306_DrawString(buffer);

    SSD1306_UpdateScreen();
}

