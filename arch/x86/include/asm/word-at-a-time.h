#ifndef _ASM_WORD_AT_A_TIME_H
#define _ASM_WORD_AT_A_TIME_H

#include <linux/kernel.h>

/*
                                                              
                                                               
                                                             
                                                                
                          
 */
struct word_at_a_time {
	const unsigned long one_bits, high_bits;
};

#define WORD_AT_A_TIME_CONSTANTS { REPEAT_BYTE(0x01), REPEAT_BYTE(0x80) }

#ifdef CONFIG_64BIT

/*
                                                 
                                                   
                                                 
                   
 */
static inline long count_masked_bytes(unsigned long mask)
{
	return mask*0x0001020304050608ul >> 56;
}

#else	/*             */

/*                                                      */
static inline long count_masked_bytes(long mask)
{
	/*                                              */
	long a = (0x0ff0001+mask) >> 23;
	/*                       */
	return a & mask;
}

#endif

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

/*
                                            
  
                                                               
                                                             
                                          
 */
static inline unsigned long load_unaligned_zeropad(const void *addr)
{
	unsigned long ret, dummy;

	asm(
		"1:\tmov %2,%0\n"
		"2:\n"
		".section .fixup,\"ax\"\n"
		"3:\t"
		"lea %2,%1\n\t"
		"and %3,%1\n\t"
		"mov (%1),%0\n\t"
		"leal %2,%%ecx\n\t"
		"andl %4,%%ecx\n\t"
		"shll $3,%%ecx\n\t"
		"shr %%cl,%0\n\t"
		"jmp 2b\n"
		".previous\n"
		_ASM_EXTABLE(1b, 3b)
		:"=&r" (ret),"=&c" (dummy)
		:"m" (*(unsigned long *)addr),
		 "i" (-sizeof(unsigned long)),
		 "i" (sizeof(unsigned long)-1));
	return ret;
}

#endif /*                       */
