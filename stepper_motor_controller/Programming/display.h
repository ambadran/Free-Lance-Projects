#ifndef DISPLAY_H
#define DISPLAY_H

typedef enum {
  WELCOME_PAGE = 0,
  MENU_PAGE,
  STEP_CONTROL_PAGE,
  DISTANCE_CONTROL_PAGE,
  ENCODER_CONTROL_PAGE,
  BLANK_PAGE
} page_t;

#define CONTROL_PAGE_NUM 3
typedef enum {
  STEP_CONTROL_PAGE_SELECTED = 0,
  DISTANCE_CONTROL_PAGE_SELECTED = 1,
  ENCODER_CONTROL_PAGE_SELECTED = 2,
  NOTHING_SELECTED  // the process menu page should prevent encoder value of >CONTROL_PAGE_NUM
} menu_page_selected_page_t;

void display_welcome_page(void);
void display_menu_page(menu_page_selected_page_t selected_page);
void display_step_control_page_first_time(void);
void display_distance_control_page_first_time(void);
void display_encoder_control_page_first_time(void);

#endif
