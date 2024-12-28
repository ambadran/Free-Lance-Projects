#include "project-defs.h"

static GpioConfig neo_m8n_tx_pin = GPIO_PIN_CONFIG(NEO_M8N_PORT, NEO_M8N_TX_PIN, GPIO_BIDIRECTIONAL_MODE);
static GpioConfig neo_m8n_rx_pin = GPIO_PIN_CONFIG(NEO_M8N_PORT, NEO_M8N_RX_PIN, GPIO_BIDIRECTIONAL_MODE);

static volatile uint8_t temp;
static uint8_t* nmea_statement;
static volatile uint8_t nmea_statement_pointer = 0;

void neo_m8n_init(void) {

  uartInitialise(
      NEO_M8N_UART,
      NEO_M8N_UART_SPEED,
      UART_USE_OWN_TIMER,
      UART_8N1,
      NEO_M8N_UART_PIN_CONFIG
      );

  gpioConfigure(&neo_m8n_tx_pin);
  gpioConfigure(&neo_m8n_rx_pin);

}

void neo_m8n_read_statement(void) {

  while(temp = uartGetCharacter(NEO_M8N_UART, BLOCKING)) {
    nmea_statement[nmea_statement_pointer++] = temp;

    /* printf("Counter: %d, char: %c\n", nmea_statement_pointer, temp); */

    if (nmea_statement_pointer == NMEA_STATEMENT_SIZE) { 
      nmea_statement_pointer = 0; 
      printf("%s", nmea_statement);
    }

  }
  printf("\n\n");

}
