#ifndef APPLICATION_H
#define APPLICATION_H

typedef struct {
  button_status_t current_val;
  button_status_t (*get_func)();
} button_input_t;

typedef struct {
  switch_status_t current_val;
  switch_status_t (*get_func)();
} switch_input_t;

typedef struct {
  int8_t current_val;
  int8_t (*get_func)();
} encoder_input_t;

//TODO: could implement a void* input_t and store all inputs in array
// then make another array holding the correct function sequence to 
// cast the void* to their respective values when needed.
// this will allow to access using loops instead of call every input
typedef struct {
    button_input_t button1;
    button_input_t button2;
    button_input_t encoder_button;
    switch_input_t switch_;
    encoder_input_t encoder_value;
} all_inputs_t;

typedef enum {
  WELCOME_PAGE,
  CHOOSE_CONTROL_MODE,
  RELAY_CONTROL_PAGE,
  ABSOLUTE_CONTROL_PAGE
} application_page_t;

void application_init(void);
void application_update_input_values(void);
void application_process_welcome_page(void);
void application_process_inputs(void);


#endif
