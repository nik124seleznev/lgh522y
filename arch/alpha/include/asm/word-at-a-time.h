#ifndef _ASM_WORD_AT_A_TIME_H
#define _ASM_WORD_AT_A_TIME_H

#include <asm/compiler.h>

/*
                                      
 */

/*
                                                                       
                                          
 */
struct word_at_a_time {
	const unsigned long unused;
};

#define WORD_AT_A_TIME_CONSTANTS { 0 }

/*                                  */
static inline unsigned long has_zero(unsigned long val, unsigned long *bits, const struct word_at_a_time *c)
{
	unsigned long zero_locations = __kernel_cmpbge(0, val);
	*bits = zero_locations;
	return zero_locations;
}

static inline unsigned long prep_zero_mask(unsigned long val, unsigned long bits, const struct word_at_a_time *c)
{
	return bits;
}

#define create_zero_mask(bits) (bits)

static inline unsigned long find_zero(unsigned long bits)
{
#if defined(CONFIG_ALPHA_EV6) && defined(CONFIG_ALPHA_EV67)
	/*                                 */
	return __kernel_cttz(bits);
#else
	unsigned long t1, t2, t3;
	/*                            */
	bits &= -bits;
	/*                                  */
	t1 = bits & 0xf0;
	t2 = bits & 0xcc;
	t3 = bits & 0xaa;
	if (t1) t1 = 4;
	if (t2) t2 = 2;
	if (t3) t3 = 1;
	return t1 + t2 + t3;
#endif
}

#endif /*                       */
