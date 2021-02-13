#pragma once

#include <stdint.h>

typedef void (*gpio_callback)(uint8_t gpio, uint8_t gpio_idx);

void gpio_update_dir(uint8_t dir);
void gpio_update_pue_pud(uint8_t pue, uint8_t pud);

void gpio_set_value(uint8_t value);
uint8_t gpio_get_value(void);

void gpio_set_int_callback(gpio_callback cb);
void gpio_init(void);
