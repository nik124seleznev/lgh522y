#ifndef __ASM_ARM_WORD_AT_A_TIME_H
#define __ASM_ARM_WORD_AT_A_TIME_H

#ifndef __ARMEB__

/*
                                                   
                                      
 */
#include <linux/kernel.h>

struct word_at_a_time {
	const unsigned long one_bits, high_bits;
};

#define WORD_AT_A_TIME_CONSTANTS { REPEAT_BYTE(0x01), REPEAT_BYTE(0x80) }

static inline unsigned long has_zero(unsigned long a, unsigned long *bits,
				     const struct word_at_a_time *c)
{
	unsigned long mask = ((a - c->one_bits) & ~a) & c->high_bits;
	*bits = mask;
	return mask;
}

#define prep_zero_mask(a, bits, c) (bits)

static inline unsigned long create_zero_mask(unsigned long bits)
{
	bits = (bits - 1) & ~bits;
	return bits >> 7;
}

static inline unsigned long find_zero(unsigned long mask)
{
	unsigned long ret;

#if __LINUX_ARM_ARCH__ >= 5
	/*                        */
	ret = fls(mask) >> 3;
#else
	/*                                              */
	ret = (0x0ff0001 + mask) >> 23;
	/*                       */
	ret &= mask;
#endif

	return ret;
}

#ifdef CONFIG_DCACHE_WORD_ACCESS

#define zero_bytemask(mask) (mask)

/*
                                            
  
                                                               
                                                             
                                          
 */
static inline unsigned long load_unaligned_zeropad(const void *addr)
{
	unsigned long ret, offset;

	/*                                       */
	asm(
	"1:	ldr	%0, [%2]\n"
	"2:\n"
	"	.pushsection .fixup,\"ax\"\n"
	"	.align 2\n"
	"3:	and	%1, %2, #0x3\n"
	"	bic	%2, %2, #0x3\n"
	"	ldr	%0, [%2]\n"
	"	lsl	%1, %1, #0x3\n"
	"	lsr	%0, %0, %1\n"
	"	b	2b\n"
	"	.popsection\n"
	"	.pushsection __ex_table,\"a\"\n"
	"	.align	3\n"
	"	.long	1b, 3b\n"
	"	.popsection"
	: "=&r" (ret), "=&r" (offset)
	: "r" (addr), "Qo" (*(unsigned long *)addr));

	return ret;
}


#endif	/*                    */

#else	/*           */
#include <asm-generic/word-at-a-time.h>
#endif

#endif /*                            */
