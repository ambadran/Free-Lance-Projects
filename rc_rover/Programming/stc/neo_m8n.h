#ifndef NEO_M8N_H
#define NEO_M8N_H

#define NEO_M8N_BUFFER_SIZE 100

typedef enum {
  CONNECTED,
  SEARCHING,
} neo_m8n_status_t;

void neo_m8n_init(void);
neo_m8n_status_t neo_m8n_get_position(void);
char* neo_get_string(void);

#endif
