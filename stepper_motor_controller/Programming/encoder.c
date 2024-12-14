#include "project-defs.h"

static volatile __bit direction = 0; // 0 = count up, 1 = count down
static volatile __bit ready = 0;
static int8_t BIT_TO_INCREMENT[] = {-1, 1};
static int16_t count = 0;
static int16_t prev_count = 0;


/* **IMPORTANT:** You MUST define both pwmOnCounterInterrupt() and
 * pwmOnChannelInterrupt(), even if you don't use them. */
#pragma save
// Suppress warning "unreferenced function argument"
#pragma disable_warning 85
void pwmOnCounterInterrupt(PWM_Counter counter, PWM_CounterInterrupt HAL_PWM_SEGMENT event) {}
void pwmOnChannelInterrupt(PWM_Channel channel, uint16_t HAL_PWM_SEGMENT counterValue) {
  // In quadrature encoder mode, counterValue is 0 when
  // counting up (encoder rotates clockwise), and non-zero
  // when counting down (encoder rotates counter-clockwise).
  if (channel == ENCODER_CHANNEL) {
		// In quadrature encoder mode, counterValue is 0 when
		// counting up (encoder rotates clockwise), and non-zero
		// when counting down (encoder rotates counter-clockwise).
		direction = counterValue;
		ready = 1;
	}

}
#pragma restore

void encoder_init(void) {
  pwmInitialiseQuadratureEncoder(
		ENCODER_COUNTER,
		ENCODER_SWITCH,
		PWM_CAPTURE_ON_RISING_EDGE,
		PWM_FILTER_4_CLOCKS
	);
}

void encoder_process(void) {
  if (ready) {
    prev_count = count;
    count += BIT_TO_INCREMENT[direction];
    ready = 0;
  }
}

int16_t get_encoder_count(void) { return count; }
int8_t get_encoder_dir(void) {  return (count-prev_count); }
void encoder_count_reset(void) { count = 0; }
void encoder_count_set(int16_t value) { count = value; }

