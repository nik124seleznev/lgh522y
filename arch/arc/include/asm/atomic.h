/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _ASM_ARC_ATOMIC_H
#define _ASM_ARC_ATOMIC_H

#ifdef __KERNEL__

#ifndef __ASSEMBLY__

#include <linux/types.h>
#include <linux/compiler.h>
#include <asm/cmpxchg.h>
#include <asm/barrier.h>
#include <asm/smp.h>

#define atomic_read(v)  ((v)->counter)

#ifdef CONFIG_ARC_HAS_LLSC

#define atomic_set(v, i) (((v)->counter) = (i))

static inline void atomic_add(int i, atomic_t *v)
{
	unsigned int temp;

	__asm__ __volatile__(
	"1:	llock   %0, [%1]	\n"
	"	add     %0, %0, %2	\n"
	"	scond   %0, [%1]	\n"
	"	bnz     1b		\n"
	: "=&r"(temp)	/*                                     */
	: "r"(&v->counter), "ir"(i)
	: "cc");
}

static inline void atomic_sub(int i, atomic_t *v)
{
	unsigned int temp;

	__asm__ __volatile__(
	"1:	llock   %0, [%1]	\n"
	"	sub     %0, %0, %2	\n"
	"	scond   %0, [%1]	\n"
	"	bnz     1b		\n"
	: "=&r"(temp)
	: "r"(&v->counter), "ir"(i)
	: "cc");
}

/*                                   */
static inline int atomic_add_return(int i, atomic_t *v)
{
	unsigned int temp;

	__asm__ __volatile__(
	"1:	llock   %0, [%1]	\n"
	"	add     %0, %0, %2	\n"
	"	scond   %0, [%1]	\n"
	"	bnz     1b		\n"
	: "=&r"(temp)
	: "r"(&v->counter), "ir"(i)
	: "cc");

	return temp;
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
	unsigned int temp;

	__asm__ __volatile__(
	"1:	llock   %0, [%1]	\n"
	"	sub     %0, %0, %2	\n"
	"	scond   %0, [%1]	\n"
	"	bnz     1b		\n"
	: "=&r"(temp)
	: "r"(&v->counter), "ir"(i)
	: "cc");

	return temp;
}

static inline void atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
	unsigned int temp;

	__asm__ __volatile__(
	"1:	llock   %0, [%1]	\n"
	"	bic     %0, %0, %2	\n"
	"	scond   %0, [%1]	\n"
	"	bnz     1b		\n"
	: "=&r"(temp)
	: "r"(addr), "ir"(mask)
	: "cc");
}

#else	/*                      */

#ifndef CONFIG_SMP

 /*                                                                       */
#define atomic_set(v, i) (((v)->counter) = (i))

#else

static inline void atomic_set(atomic_t *v, int i)
{
	/*
                                                                      
                                                                   
                                                                   
            
   
                                                                 
                         
  */
	unsigned long flags;

	atomic_ops_lock(flags);
	v->counter = i;
	atomic_ops_unlock(flags);
}
#endif

/*
                                     
                                                                      
 */

static inline void atomic_add(int i, atomic_t *v)
{
	unsigned long flags;

	atomic_ops_lock(flags);
	v->counter += i;
	atomic_ops_unlock(flags);
}

static inline void atomic_sub(int i, atomic_t *v)
{
	unsigned long flags;

	atomic_ops_lock(flags);
	v->counter -= i;
	atomic_ops_unlock(flags);
}

static inline int atomic_add_return(int i, atomic_t *v)
{
	unsigned long flags;
	unsigned long temp;

	atomic_ops_lock(flags);
	temp = v->counter;
	temp += i;
	v->counter = temp;
	atomic_ops_unlock(flags);

	return temp;
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
	unsigned long flags;
	unsigned long temp;

	atomic_ops_lock(flags);
	temp = v->counter;
	temp -= i;
	v->counter = temp;
	atomic_ops_unlock(flags);

	return temp;
}

static inline void atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
	unsigned long flags;

	atomic_ops_lock(flags);
	*addr &= ~mask;
	atomic_ops_unlock(flags);
}

#endif /*                      */

/* 
                                                               
                               
                                
                                 
  
                                                      
                              
 */
#define __atomic_add_unless(v, a, u)					\
({									\
	int c, old;							\
	c = atomic_read(v);						\
	while (c != (u) && (old = atomic_cmpxchg((v), c, c + (a))) != c)\
		c = old;						\
	c;								\
})

#define atomic_inc_not_zero(v)		atomic_add_unless((v), 1, 0)

#define atomic_inc(v)			atomic_add(1, v)
#define atomic_dec(v)			atomic_sub(1, v)

#define atomic_inc_and_test(v)		(atomic_add_return(1, v) == 0)
#define atomic_dec_and_test(v)		(atomic_sub_return(1, v) == 0)
#define atomic_inc_return(v)		atomic_add_return(1, (v))
#define atomic_dec_return(v)		atomic_sub_return(1, (v))
#define atomic_sub_and_test(i, v)	(atomic_sub_return(i, v) == 0)

#define atomic_add_negative(i, v)	(atomic_add_return(i, v) < 0)

#define ATOMIC_INIT(i)			{ (i) }

#include <asm-generic/atomic64.h>

#endif

#endif

#endif