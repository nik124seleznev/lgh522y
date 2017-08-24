/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _ASM_BITOPS_H
#define _ASM_BITOPS_H

#ifndef _LINUX_BITOPS_H
#error only <linux/bitops.h> can be included directly
#endif

#ifdef __KERNEL__

#ifndef __ASSEMBLY__

#include <linux/types.h>
#include <linux/compiler.h>

/*
                                                                      
                                                                          
                                                   
 */
#if defined(CONFIG_ARC_HAS_LLSC)

static inline void set_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned int temp;

	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	__asm__ __volatile__(
	"1:	llock   %0, [%1]	\n"
	"	bset    %0, %0, %2	\n"
	"	scond   %0, [%1]	\n"
	"	bnz     1b	\n"
	: "=&r"(temp)
	: "r"(m), "ir"(nr)
	: "cc");
}

static inline void clear_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned int temp;

	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	__asm__ __volatile__(
	"1:	llock   %0, [%1]	\n"
	"	bclr    %0, %0, %2	\n"
	"	scond   %0, [%1]	\n"
	"	bnz     1b	\n"
	: "=&r"(temp)
	: "r"(m), "ir"(nr)
	: "cc");
}

static inline void change_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned int temp;

	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	__asm__ __volatile__(
	"1:	llock   %0, [%1]	\n"
	"	bxor    %0, %0, %2	\n"
	"	scond   %0, [%1]	\n"
	"	bnz     1b		\n"
	: "=&r"(temp)
	: "r"(m), "ir"(nr)
	: "cc");
}

/*
                
                  
              
                                         
          
                               
  
                                                                             
                                       
 */
static inline int test_and_set_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long old, temp;

	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	__asm__ __volatile__(
	"1:	llock   %0, [%2]	\n"
	"	bset    %1, %0, %3	\n"
	"	scond   %1, [%2]	\n"
	"	bnz     1b		\n"
	: "=&r"(old), "=&r"(temp)
	: "r"(m), "ir"(nr)
	: "cc");

	return (old & (1 << nr)) != 0;
}

static inline int
test_and_clear_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned int old, temp;

	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	__asm__ __volatile__(
	"1:	llock   %0, [%2]	\n"
	"	bclr    %1, %0, %3	\n"
	"	scond   %1, [%2]	\n"
	"	bnz     1b		\n"
	: "=&r"(old), "=&r"(temp)
	: "r"(m), "ir"(nr)
	: "cc");

	return (old & (1 << nr)) != 0;
}

static inline int
test_and_change_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned int old, temp;

	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	__asm__ __volatile__(
	"1:	llock   %0, [%2]	\n"
	"	bxor    %1, %0, %3	\n"
	"	scond   %1, [%2]	\n"
	"	bnz     1b		\n"
	: "=&r"(old), "=&r"(temp)
	: "r"(m), "ir"(nr)
	: "cc");

	return (old & (1 << nr)) != 0;
}

#else	/*                      */

#include <asm/smp.h>

/*
                                     
                                                                      
  
                                                                          
                                 
  
                                                                     
                                                                           
                                          
                                                                           
                                                                       
                                                                           
                               
 */

static inline void set_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long temp, flags;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	bitops_lock(flags);

	temp = *m;
	*m = temp | (1UL << nr);

	bitops_unlock(flags);
}

static inline void clear_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long temp, flags;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	bitops_lock(flags);

	temp = *m;
	*m = temp & ~(1UL << nr);

	bitops_unlock(flags);
}

static inline void change_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long temp, flags;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	bitops_lock(flags);

	temp = *m;
	*m = temp ^ (1UL << nr);

	bitops_unlock(flags);
}

static inline int test_and_set_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long old, flags;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	bitops_lock(flags);

	old = *m;
	*m = old | (1 << nr);

	bitops_unlock(flags);

	return (old & (1 << nr)) != 0;
}

static inline int
test_and_clear_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long old, flags;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	bitops_lock(flags);

	old = *m;
	*m = old & ~(1 << nr);

	bitops_unlock(flags);

	return (old & (1 << nr)) != 0;
}

static inline int
test_and_change_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long old, flags;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	bitops_lock(flags);

	old = *m;
	*m = old ^ (1 << nr);

	bitops_unlock(flags);

	return (old & (1 << nr)) != 0;
}

#endif /*                     */

/*                                      
                      
                                      */

static inline void __set_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long temp;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	temp = *m;
	*m = temp | (1UL << nr);
}

static inline void __clear_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long temp;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	temp = *m;
	*m = temp & ~(1UL << nr);
}

static inline void __change_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long temp;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	temp = *m;
	*m = temp ^ (1UL << nr);
}

static inline int
__test_and_set_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long old;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	old = *m;
	*m = old | (1 << nr);

	return (old & (1 << nr)) != 0;
}

static inline int
__test_and_clear_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long old;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	old = *m;
	*m = old & ~(1 << nr);

	return (old & (1 << nr)) != 0;
}

static inline int
__test_and_change_bit(unsigned long nr, volatile unsigned long *m)
{
	unsigned long old;
	m += nr >> 5;

	if (__builtin_constant_p(nr))
		nr &= 0x1f;

	old = *m;
	*m = old ^ (1 << nr);

	return (old & (1 << nr)) != 0;
}

/*
                                          
 */
static inline int
__constant_test_bit(unsigned int nr, const volatile unsigned long *addr)
{
	return ((1UL << (nr & 31)) &
		(((const volatile unsigned int *)addr)[nr >> 5])) != 0;
}

static inline int
__test_bit(unsigned int nr, const volatile unsigned long *addr)
{
	unsigned long mask;

	addr += nr >> 5;

	/*                                                   */
	mask = 1 << nr;

	return ((mask & *addr) != 0);
}

#define test_bit(nr, addr)	(__builtin_constant_p(nr) ? \
					__constant_test_bit((nr), (addr)) : \
					__test_bit((nr), (addr)))

/*
                                               
                            
                                                          
                                                    
                                                                     
 */
static inline __attribute__ ((const)) int clz(unsigned int x)
{
	unsigned int res;

	__asm__ __volatile__(
	"	norm.f  %0, %1		\n"
	"	mov.n   %0, 0		\n"
	"	add.p   %0, %0, 1	\n"
	: "=r"(res)
	: "r"(x)
	: "cc");

	return res;
}

static inline int constant_fls(int x)
{
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}

/*
                              
                  
                                               
 */
static inline __attribute__ ((const)) int fls(unsigned long x)
{
	if (__builtin_constant_p(x))
	       return constant_fls(x);

	return 32 - clz(x);
}

/*
                                               
 */
static inline __attribute__ ((const)) int __fls(unsigned long x)
{
	if (!x)
		return 0;
	else
		return fls(x) - 1;
}

/*
                                            
                                
 */
#define ffs(x)	({ unsigned long __t = (x); fls(__t & -__t); })

/*
                                               
 */
static inline __attribute__ ((const)) int __ffs(unsigned long word)
{
	if (!word)
		return word;

	return ffs(word) - 1;
}

/*
                                 
                                
 */
#define ffz(x)	__ffs(~(x))

/*                                          */
#define smp_mb__before_clear_bit()  barrier()
#define smp_mb__after_clear_bit()   barrier()

#include <asm-generic/bitops/hweight.h>
#include <asm-generic/bitops/fls64.h>
#include <asm-generic/bitops/sched.h>
#include <asm-generic/bitops/lock.h>

#include <asm-generic/bitops/find.h>
#include <asm-generic/bitops/le.h>
#include <asm-generic/bitops/ext2-atomic-setbit.h>

#endif /*               */

#endif /*            */

#endif
