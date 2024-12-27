#include "project-defs.h"

static uint8_t* neo_m8n_string;

void neo_m8n_init(void) {

	uartInitialise(
     NEO_M8N_UART, 
     NEO_M8N_UART_SPEED, 
     NEO_M8N_TIMER, 
     UART_8N1, 
     NEO_M8N_UART_PIN_CONFIG);

}

neo_m8n_status_t neo_m8n_get_position(void) {

  if (uartGetBlock(NEO_M8N_UART, neo_m8n_string, NEO_M8N_BUFFER_SIZE, NON_BLOCKING)) { 
    printf("Got\n%s\n", neo_m8n_string);
    /* printf("test\n"); */
  } else {
    printf("Nothing\r");
  }
  return SEARCHING;

}
