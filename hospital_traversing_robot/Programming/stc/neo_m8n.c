#include "project-defs.h"

static GpioConfig neo_m8n_tx_pin = GPIO_PIN_CONFIG(NEO_M8N_PORT, NEO_M8N_TX_PIN, GPIO_BIDIRECTIONAL_MODE);
static GpioConfig neo_m8n_rx_pin = GPIO_PIN_CONFIG(NEO_M8N_PORT, NEO_M8N_RX_PIN, GPIO_BIDIRECTIONAL_MODE);

static volatile uint8_t temp;
static uint8_t nmea_statement[UART2_RX_BUFFER_SIZE];
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

  /* memset(gps_data, 0, sizeof(gps_data)); */

}

void neo_m8n_extract(uint8_t* raw_statement, uint8_t start_ind, uint8_t end_ind, uint8_t* value) {

  // validating indices
  if(start_ind > end_ind) {
    printf("Wrong values in statement parsing\n");
    return;
  }

  // Calculate the number of characters to copy
  uint8_t length = end_ind - start_ind + 1;

  // Copy the substring from raw_statement to value
  for (uint8_t i = 0; i < length; i++) {
      value[i] = raw_statement[start_ind + i];
  }

  // Null-terminate the value array
  value[length] = '\0';

}

void neo_m8n_read_statement(gps_data_t* gps_data) {

  // Getting the raw statement
  while(temp = uartGetCharacter(NEO_M8N_UART, BLOCKING)) {
    nmea_statement[nmea_statement_pointer++] = temp;

    if (nmea_statement_pointer == NMEA_STATEMENT_SIZE) { 
      nmea_statement_pointer = 0; 
      break;
    }
  }
  /* printf(nmea_statement); */
  /* printf("\n\nEnd nmea statement\n"); */

  // extracting wanted values
  //TODO: check if latitude data exists
  neo_m8n_extract(nmea_statement, LATITUDE_START_IND, LATITUDE_END_IND, gps_data->latitude);
  
  //TODO: check if longitude data exists
  neo_m8n_extract(nmea_statement, LONGITUDE_START_IND, LONGITUDE_END_IND, gps_data->longitude);

  //TODO: check if heading data exists
  neo_m8n_extract(nmea_statement, HEADING_START_IND, HEADING_END_IND, gps_data->heading);

  //TODO: check if time data exists
  neo_m8n_extract(nmea_statement, TIME_START_IND, TIME_END_IND, gps_data->time);

}
