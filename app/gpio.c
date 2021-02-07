#include "conf_target.h"
#include "gpio.h"
#include "reg.h"

#include <extint.h>
#include <extint_callback.h>
#include <port.h>
#include <stdio.h>

static gpio_callback gpio_cb;

/**
 * Since the extint callback doesn't specify which pin triggered it,
 * we create a callback per pin and pass the pin to the main gpio callback.
 */
#define EXTINT_CALLBACK(bit) \
static void extint_callback ## bit(void) \
{ \
	if (gpio_cb) \
		gpio_cb(PIN_GPIO ## bit, bit); \
}

#ifdef PIN_GPIO0
	EXTINT_CALLBACK(0)
#endif

#ifdef PIN_GPIO1
	EXTINT_CALLBACK(1)
#endif

#ifdef PIN_GPIO2
	EXTINT_CALLBACK(2)
#endif

#ifdef PIN_GPIO3
	EXTINT_CALLBACK(3)
#endif

#ifdef PIN_GPIO4
	EXTINT_CALLBACK(4)
#endif

#ifdef PIN_GPIO5
	EXTINT_CALLBACK(5)
#endif

#ifdef PIN_GPIO6
	EXTINT_CALLBACK(6)
#endif

#ifdef PIN_GPIO7
	EXTINT_CALLBACK(7)
#endif

static void gpio_set_dir(uint8_t gpio, uint8_t gpio_idx, uint8_t int_chan, extint_callback_t int_cb, uint8_t dir)
{
#ifdef DEBUG
	printf("%s: gpio: %d, gpio_idx: %d, int_ch: %d, dir: %d\r\n", __func__, gpio, gpio_idx, int_chan, dir);
#endif

	if (dir == DIR_INPUT) {
		struct extint_chan_conf config_extint_chan;
		extint_chan_get_config_defaults(&config_extint_chan);

		enum extint_pull pull = EXTINT_PULL_NONE;
		if (reg_is_bit_set(REG_ID_PUE, (1 << gpio_idx))) {
			if (reg_is_bit_set(REG_ID_PUD, (1 << gpio_idx)) == PUD_UP) {
				pull = EXTINT_PULL_UP;
			} else {
				pull = EXTINT_PULL_DOWN;
			}
		}

		config_extint_chan.gpio_pin = gpio;
		config_extint_chan.gpio_pin_pull = pull;
		config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;

		extint_chan_set_config(int_chan, &config_extint_chan);
		extint_register_callback(int_cb, int_chan, EXTINT_CALLBACK_TYPE_DETECT);
		extint_chan_enable_callback(10, EXTINT_CALLBACK_TYPE_DETECT);

		reg_set_bit(REG_ID_DIR, (1 << gpio_idx));
	} else {
		struct port_config port_cfg;
		port_get_config_defaults(&port_cfg);

		port_cfg.direction = PORT_PIN_DIR_OUTPUT;

		port_pin_set_config(gpio, &port_cfg);

		reg_clear_bit(REG_ID_DIR, (1 << gpio_idx));
	}
}

void gpio_update_dir(uint8_t new_dir)
{
#ifdef DEBUG
	printf("%s: dir: 0x%02X\r\n", __func__, new_dir);
#endif

	const uint8_t old_dir = reg_get_value(REG_ID_DIR);

	(void)old_dir; // Shut up warning in case no GPIOs configured

#define UPDATE_DIR(bit) \
	if ((old_dir & (1 << bit)) != (new_dir & (1 << bit))) \
		gpio_set_dir(PIN_GPIO ## bit, bit, EIC_GPIO ## bit, extint_callback ## bit, (new_dir & (1 << bit)) != 0);

#ifdef PIN_GPIO0
	UPDATE_DIR(0)
#endif
#ifdef PIN_GPIO1
	UPDATE_DIR(1)
#endif
#ifdef PIN_GPIO2
	UPDATE_DIR(2)
#endif
#ifdef PIN_GPIO3
	UPDATE_DIR(3)
#endif
#ifdef PIN_GPIO4
	UPDATE_DIR(4)
#endif
#ifdef PIN_GPIO5
	UPDATE_DIR(5)
#endif
#ifdef PIN_GPIO6
	UPDATE_DIR(6)
#endif
#ifdef PIN_GPIO7
	UPDATE_DIR(7)
#endif
}

void gpio_update_pue_pud(uint8_t new_pue, uint8_t new_pud)
{
#ifdef DEBUG
	printf("%s: pue: 0x%02X, pud: 0x%02X\r\n", __func__, new_pue, new_pud);
#endif

	const uint8_t old_pue = reg_get_value(REG_ID_PUE);
	const uint8_t old_pud = reg_get_value(REG_ID_PUD);

	// Shut up warnings in case no GPIOs configured
	(void)old_pue;
	(void)old_pud;

	reg_set_value(REG_ID_PUE, new_pue);
	reg_set_value(REG_ID_PUD, new_pud);

#define UPDATE_PULL(bit) \
	if (((old_pue & (1 << bit)) != (new_pue & (1 << bit))) || \
		((old_pud & (1 << bit)) != (new_pud & (1 << bit)))) { \
		gpio_set_dir(PIN_GPIO ## bit, bit, EIC_GPIO ## bit, extint_callback ## bit, reg_is_bit_set(REG_ID_DIR, (1 << bit))); \
	}

#ifdef PIN_GPIO0
	UPDATE_PULL(0)
#endif
#ifdef PIN_GPIO1
	UPDATE_PULL(1)
#endif
#ifdef PIN_GPIO2
	UPDATE_PULL(2)
#endif
#ifdef PIN_GPIO3
	UPDATE_PULL(3)
#endif
#ifdef PIN_GPIO4
	UPDATE_PULL(4)
#endif
#ifdef PIN_GPIO5
	UPDATE_PULL(5)
#endif
#ifdef PIN_GPIO6
	UPDATE_PULL(6)
#endif
#ifdef PIN_GPIO7
	UPDATE_PULL(7)
#endif
}

void gpio_set_value(uint8_t value)
{
#ifdef DEBUG
	printf("%s: value: 0x%02X\r\n", __func__, value);
#endif

#define SET_VALUE(bit) \
	if (reg_is_bit_set(REG_ID_DIR, (1 << bit)) == DIR_OUTPUT) { \
		port_pin_set_output_level(PIN_GPIO ## bit, value & (1 << bit)); \
	}

#ifdef PIN_GPIO0
	SET_VALUE(0)
#endif
#ifdef PIN_GPIO1
	SET_VALUE(1)
#endif
#ifdef PIN_GPIO2
	SET_VALUE(2)
#endif
#ifdef PIN_GPIO3
	SET_VALUE(3)
#endif
#ifdef PIN_GPIO4
	SET_VALUE(4)
#endif
#ifdef PIN_GPIO5
	SET_VALUE(5)
#endif
#ifdef PIN_GPIO6
	SET_VALUE(6)
#endif
#ifdef PIN_GPIO7
	SET_VALUE(7)
#endif
}

uint8_t gpio_get_value(void)
{
	uint8_t value = 0;

#define GET_VALUE(bit) \
	if (reg_is_bit_set(REG_ID_DIR, (1 << bit)) == DIR_INPUT) { \
		value |= (port_pin_get_input_level(PIN_GPIO ## bit) << bit); \
	} else { \
		value |= (port_pin_get_output_level(PIN_GPIO ## bit) << bit); \
	}

#ifdef PIN_GPIO0
	GET_VALUE(0)
#endif
#ifdef PIN_GPIO1
	GET_VALUE(1)
#endif
#ifdef PIN_GPIO2
	GET_VALUE(2)
#endif
#ifdef PIN_GPIO3
	GET_VALUE(3)
#endif
#ifdef PIN_GPIO4
	GET_VALUE(4)
#endif
#ifdef PIN_GPIO5
	GET_VALUE(5)
#endif
#ifdef PIN_GPIO6
	GET_VALUE(6)
#endif
#ifdef PIN_GPIO7
	GET_VALUE(7)
#endif

	return value;
}

void gpio_set_int_callback(gpio_callback cb)
{
	gpio_cb = cb;
}

void gpio_init(void)
{
	// Configure all to inputs
	gpio_update_dir(0xFF);
}
