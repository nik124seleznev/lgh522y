#ifndef _ASM_X86_CMPXCHG_32_H
#define _ASM_X86_CMPXCHG_32_H

/*
                                                                      
                                                           
 */

/*
                                                             
                                                                   
                                                                  
                                                                     
                                     
  
                                                                    
                                                 
  
                                                                
                                                                     
                                        
 */
static inline void set_64bit(volatile u64 *ptr, u64 value)
{
	u32 low  = value;
	u32 high = value >> 32;
	u64 prev = *ptr;

	asm volatile("\n1:\t"
		     LOCK_PREFIX "cmpxchg8b %0\n\t"
		     "jnz 1b"
		     : "=m" (*ptr), "+A" (prev)
		     : "b" (low), "c" (high)
		     : "memory");
}

#define __HAVE_ARCH_CMPXCHG 1

#ifdef CONFIG_X86_CMPXCHG64
#define cmpxchg64(ptr, o, n)						\
	((__typeof__(*(ptr)))__cmpxchg64((ptr), (unsigned long long)(o), \
					 (unsigned long long)(n)))
#define cmpxchg64_local(ptr, o, n)					\
	((__typeof__(*(ptr)))__cmpxchg64_local((ptr), (unsigned long long)(o), \
					       (unsigned long long)(n)))
#endif

static inline u64 __cmpxchg64(volatile u64 *ptr, u64 old, u64 new)
{
	u64 prev;
	asm volatile(LOCK_PREFIX "cmpxchg8b %1"
		     : "=A" (prev),
		       "+m" (*ptr)
		     : "b" ((u32)new),
		       "c" ((u32)(new >> 32)),
		       "0" (old)
		     : "memory");
	return prev;
}

static inline u64 __cmpxchg64_local(volatile u64 *ptr, u64 old, u64 new)
{
	u64 prev;
	asm volatile("cmpxchg8b %1"
		     : "=A" (prev),
		       "+m" (*ptr)
		     : "b" ((u32)new),
		       "c" ((u32)(new >> 32)),
		       "0" (old)
		     : "memory");
	return prev;
}

#ifndef CONFIG_X86_CMPXCHG64
/*
                                                                            
                                                        
 */

#define cmpxchg64(ptr, o, n)					\
({								\
	__typeof__(*(ptr)) __ret;				\
	__typeof__(*(ptr)) __old = (o);				\
	__typeof__(*(ptr)) __new = (n);				\
	alternative_io(LOCK_PREFIX_HERE				\
			"call cmpxchg8b_emu",			\
			"lock; cmpxchg8b (%%esi)" ,		\
		       X86_FEATURE_CX8,				\
		       "=A" (__ret),				\
		       "S" ((ptr)), "0" (__old),		\
		       "b" ((unsigned int)__new),		\
		       "c" ((unsigned int)(__new>>32))		\
		       : "memory");				\
	__ret; })


#define cmpxchg64_local(ptr, o, n)				\
({								\
	__typeof__(*(ptr)) __ret;				\
	__typeof__(*(ptr)) __old = (o);				\
	__typeof__(*(ptr)) __new = (n);				\
	alternative_io("call cmpxchg8b_emu",			\
		       "cmpxchg8b (%%esi)" ,			\
		       X86_FEATURE_CX8,				\
		       "=A" (__ret),				\
		       "S" ((ptr)), "0" (__old),		\
		       "b" ((unsigned int)__new),		\
		       "c" ((unsigned int)(__new>>32))		\
		       : "memory");				\
	__ret; })

#endif

#define system_has_cmpxchg_double() cpu_has_cx8

#endif /*                       */