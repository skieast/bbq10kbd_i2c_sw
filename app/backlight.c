#include "backlight.h"
#include "conf_target.h"
#include "reg.h"

#include <assert.h>
#include <tc.h>

static struct tc_module instance;

void backlight_update(void)
{
#ifdef PWM1_CHANNEL
	tc_set_compare_value(&instance, PWM1_CHANNEL, reg_get_value(REG_ID_BKL)  * 0xFF);
#endif

#ifdef PWM2_CHANNEL
	tc_set_compare_value(&instance, PWM2_CHANNEL, reg_get_value(REG_ID_BK2) * 0xFF);
#endif
}

void backlight_init(void)
{
	struct tc_config config;
	tc_get_config_defaults(&config);

	config.counter_size    = TC_COUNTER_SIZE_16BIT;
	config.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;

#ifdef PWM1_CHANNEL
	config.counter_16_bit.compare_capture_channel[PWM1_CHANNEL] = reg_get_value(REG_ID_BKL) * 0xFF;

	config.pwm_channel[PWM1_CHANNEL].enabled = true;
	config.pwm_channel[PWM1_CHANNEL].pin_out = PWM1_OUT_PIN;
	config.pwm_channel[PWM1_CHANNEL].pin_mux = PWM1_OUT_MUX;
#endif

#ifdef PWM2_MODULE
	config.counter_16_bit.compare_capture_channel[PWM2_CHANNEL] = reg_get_value(REG_ID_BK2) * 0xFF;

	config.pwm_channel[PWM2_CHANNEL].enabled = true;
	config.pwm_channel[PWM2_CHANNEL].pin_out = PWM2_OUT_PIN;
	config.pwm_channel[PWM2_CHANNEL].pin_mux = PWM2_OUT_MUX;
#endif

	// FIXME: This assumes both PWMs are on the same module!!
	while (tc_init(&instance, PWM1_MODULE, &config) != STATUS_OK) { }
	tc_enable(&instance);
}
