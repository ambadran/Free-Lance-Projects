#ifndef COMPLEMENTARY_FILTER
#define COMPLEMENTARY_FILTER

void complementary_filter_init(void);
void complementary_filter_process(void);
int16_t get_compl_pitch_deg(void);
int16_t get_compl_roll_deg(void);
int16_t get_compl_yaw_deg(void);

#endif
