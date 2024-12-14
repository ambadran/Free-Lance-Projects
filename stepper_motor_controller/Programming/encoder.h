#ifndef ENCODER_H
#define ENCODER_H

// For PWM_COUNTER_A, we use PWM1P and PWM2P on P1.0 and P5.4 (pin switch = 0).
#define ENCODER_COUNTER PWM_COUNTER_A
#define ENCODER_SWITCH  0
#define ENCODER_CHANNEL PWM_Channel0

void encoder_init(void);
void encoder_process(void);
int16_t get_encoder_count(void);
int8_t get_encoder_dir(void);
void encoder_count_reset(void);
void encoder_count_set(int16_t value);

#endif
