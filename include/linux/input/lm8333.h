/*
 * public include for LM8333 keypad driver - same license as driver
 * Copyright (C) 2012 Wolfram Sang, Pengutronix <w.sang@pengutronix.de>
 */

#ifndef _LM8333_H
#define _LM8333_H

struct lm8333;

struct lm8333_platform_data {
	/*             */
	const struct matrix_keymap_data *matrix_data;
	/*                                                       */
	unsigned active_time;
	/*                                   */
	unsigned debounce_time;
};

extern int lm8333_read8(struct lm8333 *lm8333, u8 cmd);
extern int lm8333_write8(struct lm8333 *lm8333, u8 cmd, u8 val);
extern int lm8333_read_block(struct lm8333 *lm8333, u8 cmd, u8 len, u8 *buf);

#endif /*           */
