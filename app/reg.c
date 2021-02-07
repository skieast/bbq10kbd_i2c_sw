#include "reg.h"

#include <stdio.h>

static uint8_t regs[REG_ID_LAST];

uint8_t reg_get_value(enum reg_id reg)
{
	return regs[reg];
}

void reg_set_value(enum reg_id reg, uint8_t value)
{
#ifdef DEBUG
	printf("%s: reg: 0x%02X, val: 0x%02X\r\n", __func__, reg, value);
#endif

	regs[reg] = value;
}

bool reg_is_bit_set(enum reg_id reg, uint8_t bit)
{
	return regs[reg] & bit;
}

void reg_set_bit(enum reg_id reg, uint8_t bit)
{
#ifdef DEBUG
	printf("%s: reg: 0x%02X, bit: %d\r\n", __func__, reg, bit);
#endif

	regs[reg] |= bit;
}

void reg_clear_bit(enum reg_id reg, uint8_t bit)
{
#ifdef DEBUG
	printf("%s: reg: 0x%02X, bit: %d\r\n", __func__, reg, bit);
#endif

	regs[reg] &= ~bit;
}

void reg_init(void)
{
	regs[REG_ID_CFG] = CFG_OVERFLOW_INT | CFG_KEY_INT | CFG_USE_MODS;
	regs[REG_ID_DEB] = 10;
	regs[REG_ID_FRQ] = 5;
	regs[REG_ID_BKL] = 255;
	regs[REG_ID_BK2] = 255;
	regs[REG_ID_PUD] = 0xFF;
}

