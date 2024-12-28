#ifndef NEO_M8N_H
#define NEO_M8N_H

#define NMEA_STATEMENT_SIZE UART2_RX_BUFFER_SIZE

void neo_m8n_init(void);
void neo_m8n_read_statement(void);

#endif
