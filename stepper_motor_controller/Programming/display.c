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

    } 
    SSD1306_UpdateScreen();
  }
}

void display_step_control_page_first_time(void) {

    SSD1306_ClearScreen();
    SSD1306_SetPosition(1, 0);
    SSD1306_DrawString("     Step Control");
    SSD1306_SetPosition(1, 1);
    SSD1306_DrawString("Dir: CW          HOLD");
    SSD1306_SetPosition(1, 2);
    SSD1306_DrawString("> Frequency");
    SSD1306_SetPosition(1, 3);
    SSD1306_DrawString("  Microstepping");
    SSD1306_SetPosition(1, 4);
    SSD1306_DrawString("  Steps");
    SSD1306_SetPosition(1, 5);
    SSD1306_DrawString("  Go Back");


    SSD1306_SetPosition(1, 7);
    SSD1306_DrawString("         Run");

    SSD1306_UpdateScreen();
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


