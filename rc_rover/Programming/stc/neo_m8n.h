#ifndef NEO_M8N_H
#define NEO_M8N_H

#define NMEA_STATEMENT_SIZE UART2_RX_BUFFER_SIZE
#define GPS_NUM_DATA 4 // the number of data read by GPS, note that 0 means return all data

#define LATITUDE_START_IND 1
#define LATITUDE_END_IND 3
#define LONGITUDE_START_IND 4
#define LONGITUDE_END_IND 6
#define HEADING_START_IND 7
#define HEADING_END_IND 9
#define TIME_START_IND 10
#define TIME_END_IND 12
                       
typedef struct {
  uint8_t latitude[10];
  uint8_t longitude[10];
  uint8_t heading[10];
  uint8_t time[10];
} gps_data_t;

void neo_m8n_init(void);
void neo_m8n_extract(uint8_t* raw_statement, uint8_t start_ind, uint8_t end_ind, uint8_t* value);
void neo_m8n_read_statement(gps_data_t* gps_data);

#endif
