/* MN10300 Architecture time management specifications
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */
#ifndef _ASM_TIMEX_H
#define _ASM_TIMEX_H

#include <unit/timex.h>

#define TICK_SIZE (tick_nsec / 1000)

#define CLOCK_TICK_RATE MN10300_JCCLK /*               */

#ifdef __KERNEL__

extern cycles_t cacheflush_time;

static inline cycles_t get_cycles(void)
{
	return read_timestamp_counter();
}

extern int init_clockevents(void);
extern int init_clocksource(void);

#endif /*            */

#endif /*              */