/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_ARC_CMPXCHG_H
#define __ASM_ARC_CMPXCHG_H

#include <linux/types.h>
#include <asm/smp.h>

#ifdef CONFIG_ARC_HAS_LLSC

static inline unsigned long
__cmpxchg(volatile void *ptr, unsigned long expected, unsigned long new)
{
	unsigned long prev;

	__asm__ __volatile__(
	"1:	llock   %0, [%1]	\n"
	"	brne    %0, %2, 2f	\n"
	"	scond   %3, [%1]	\n"
	"	bnz     1b		\n"
	"2:				\n"
	: "=&r"(prev)
	: "r"(ptr), "ir"(expected),
	  "r"(new) /*                                              */
	: "cc");

	return prev;
}

#else

static inline unsigned long
__cmpxchg(volatile void *ptr, unsigned long expected, unsigned long new)
{
	unsigned long flags;
	int prev;
	volatile unsigned long *p = ptr;

	atomic_ops_lock(flags);
	prev = *p;
	if (prev == expected)
		*p = new;
	atomic_ops_unlock(flags);
	return prev;
}

#endif /*                     */

#define cmpxchg(ptr, o, n) ((typeof(*(ptr)))__cmpxchg((ptr), \
				(unsigned long)(o), (unsigned long)(n)))

/*
                                                                            
                               
                                                                             
                                            
  
                                                                                
                        
 */
#define atomic_cmpxchg(v, o, n) ((int)cmpxchg(&((v)->counter), (o), (n)))


/*
                                                          
 */
static inline unsigned long __xchg(unsigned long val, volatile void *ptr,
				   int size)
{
	extern unsigned long __xchg_bad_pointer(void);

	switch (size) {
	case 4:
		__asm__ __volatile__(
		"	ex  %0, [%1]	\n"
		: "+r"(val)
		: "r"(ptr)
		: "memory");

		return val;
	}
	return __xchg_bad_pointer();
}

#define _xchg(ptr, with) ((typeof(*(ptr)))__xchg((unsigned long)(with), (ptr), \
						 sizeof(*(ptr))))

/*
                                                                               
                                                    
                                                                                
                                                                       
                                                                             
                                                                               
  
                                                             
                    
                                       
                                                                      
                                
 */

#if !defined(CONFIG_ARC_HAS_LLSC) && defined(CONFIG_SMP)

#define xchg(ptr, with)			\
({					\
	unsigned long flags;		\
	typeof(*(ptr)) old_val;		\
					\
	atomic_ops_lock(flags);		\
	old_val = _xchg(ptr, with);	\
	atomic_ops_unlock(flags);	\
	old_val;			\
})

#else

#define xchg(ptr, with)  _xchg(ptr, with)

#endif

/*
                             
                                                                             
                                                                              
                                          
                                                        
                                                                           
                                                              
                                                                          
                                                                          
                                                                            
                                   
 */
#define atomic_xchg(v, new) (xchg(&((v)->counter), new))

#endif
