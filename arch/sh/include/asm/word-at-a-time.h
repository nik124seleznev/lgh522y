#ifndef __ASM_SH_WORD_AT_A_TIME_H
#define __ASM_SH_WORD_AT_A_TIME_H

#ifdef CONFIG_CPU_BIG_ENDIAN
# include <asm-generic/word-at-a-time.h>
#else
/*
                                          
 */
struct word_at_a_time {
	const unsigned long one_bits, high_bits;
};

#define WORD_AT_A_TIME_CONSTANTS { REPEAT_BYTE(0x01), REPEAT_BYTE(0x80) }

/*                                                      */
static inline long count_masked_bytes(long mask)
{
	/*                                              */
	long a = (0x0ff0001+mask) >> 23;
	/*                       */
	return a & mask;
}

/*                                 */
static inline unsigned long has_zero(unsigned long a, unsigned long *bits, const struct word_at_a_time *c)
{
	unsigned long mask = ((a - c->one_bits) & ~a) & c->high_bits;
	*bits = mask;
	return mask;
}

static inline unsigned long prep_zero_mask(unsigned long a, unsigned long bits, const struct word_at_a_time *c)
{
	return bits;
}

static inline unsigned long create_zero_mask(unsigned long bits)
{
	bits = (bits - 1) & ~bits;
	return bits >> 7;
}

/*                                                      */
#define zero_bytemask(mask) (mask)

static inline unsigned long find_zero(unsigned long mask)
{
	return count_masked_bytes(mask);
}
#endif

#endif
