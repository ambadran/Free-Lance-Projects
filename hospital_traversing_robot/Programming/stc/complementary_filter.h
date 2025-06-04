#ifndef COMPLEMENTARY_FILTER
#define COMPLEMENTARY_FILTER

void complementary_filter_init(void);
void complementary_filter_process(void);
int16_t get_compl_pitch(void);
int16_t get_compl_roll(void);
int16_t get_compl_yaw(void);

#endif
