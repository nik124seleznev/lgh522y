/*
 * Copyright 2010 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

#ifndef _ASM_TILE_UACCESS_H
#define _ASM_TILE_UACCESS_H

/*
                                     
 */
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm-generic/uaccess-unaligned.h>
#include <asm/processor.h>
#include <asm/page.h>

#define VERIFY_READ	0
#define VERIFY_WRITE	1

/*
                                                                       
                                                                         
                                               
  
                                                             
 */
#define MAKE_MM_SEG(a)  ((mm_segment_t) { (a) })

#define KERNEL_DS	MAKE_MM_SEG(-1UL)
#define USER_DS		MAKE_MM_SEG(PAGE_OFFSET)

#define get_ds()	(KERNEL_DS)
#define get_fs()	(current_thread_info()->addr_limit)
#define set_fs(x)	(current_thread_info()->addr_limit = (x))

#define segment_eq(a, b) ((a).seg == (b).seg)

#ifndef __tilegx__
/*
                                                                  
                                                                         
                                                                   
                                                                           
 */
static inline int is_arch_mappable_range(unsigned long addr,
					 unsigned long size)
{
	return (addr >= MEM_USER_INTRPT &&
		addr < (MEM_USER_INTRPT + INTRPT_SIZE) &&
		size <= (MEM_USER_INTRPT + INTRPT_SIZE) - addr);
}
#define is_arch_mappable_range is_arch_mappable_range
#else
#define is_arch_mappable_range(addr, size) 0
#endif

/*
                                                                
                                                      
 */
int __range_ok(unsigned long addr, unsigned long size);

/* 
                                                       
                                                                   
                                                                     
                                                                 
                                                       
                                
  
                                                        
  
                                                                   
  
                                                                        
                               
  
                                                                    
                                                                     
                                                                   
 */
#define access_ok(type, addr, size) ({ \
	__chk_user_ptr(addr); \
	likely(__range_ok((unsigned long)(addr), (size)) == 0);	\
})

/*
                                                                       
                                                                        
                                                                      
                                                                        
              
  
                                                                     
                                                                       
                                                                      
                               
 */

struct exception_table_entry {
	unsigned long insn, fixup;
};

extern int fixup_exception(struct pt_regs *regs);

/*
                                   
  
                                                                    
                                                                     
                                                                     
                                                               
                                                                
  
                                                                   
 */

#ifdef __LP64__
#define _ASM_PTR	".quad"
#else
#define _ASM_PTR	".long"
#endif

#define __get_user_asm(OP, x, ptr, ret)					\
	asm volatile("1: {" #OP " %1, %2; movei %0, 0 }\n"		\
		     ".pushsection .fixup,\"ax\"\n"			\
		     "0: { movei %1, 0; movei %0, %3 }\n"		\
		     "j 9f\n"						\
		     ".section __ex_table,\"a\"\n"			\
		     _ASM_PTR " 1b, 0b\n"				\
		     ".popsection\n"					\
		     "9:"						\
		     : "=r" (ret), "=r" (x)				\
		     : "r" (ptr), "i" (-EFAULT))

#ifdef __tilegx__
#define __get_user_1(x, ptr, ret) __get_user_asm(ld1u, x, ptr, ret)
#define __get_user_2(x, ptr, ret) __get_user_asm(ld2u, x, ptr, ret)
#define __get_user_4(x, ptr, ret) __get_user_asm(ld4s, x, ptr, ret)
#define __get_user_8(x, ptr, ret) __get_user_asm(ld, x, ptr, ret)
#else
#define __get_user_1(x, ptr, ret) __get_user_asm(lb_u, x, ptr, ret)
#define __get_user_2(x, ptr, ret) __get_user_asm(lh_u, x, ptr, ret)
#define __get_user_4(x, ptr, ret) __get_user_asm(lw, x, ptr, ret)
#ifdef __LITTLE_ENDIAN
#define __lo32(a, b) a
#define __hi32(a, b) b
#else
#define __lo32(a, b) b
#define __hi32(a, b) a
#endif
#define __get_user_8(x, ptr, ret)					\
	({								\
		unsigned int __a, __b;					\
		asm volatile("1: { lw %1, %3; addi %2, %3, 4 }\n"	\
			     "2: { lw %2, %2; movei %0, 0 }\n"		\
			     ".pushsection .fixup,\"ax\"\n"		\
			     "0: { movei %1, 0; movei %2, 0 }\n"	\
			     "{ movei %0, %4; j 9f }\n"			\
			     ".section __ex_table,\"a\"\n"		\
			     ".word 1b, 0b\n"				\
			     ".word 2b, 0b\n"				\
			     ".popsection\n"				\
			     "9:"					\
			     : "=r" (ret), "=r" (__a), "=&r" (__b)	\
			     : "r" (ptr), "i" (-EFAULT));		\
		(x) = (__typeof(x))(__typeof((x)-(x)))			\
			(((u64)__hi32(__a, __b) << 32) |		\
			 __lo32(__a, __b));				\
	})
#endif

extern int __get_user_bad(void)
  __attribute__((warning("sizeof __get_user argument not 1, 2, 4 or 8")));

/* 
                                                                           
                                  
                                       
  
                                                        
  
                                                                       
                                                                     
                                        
  
                                                                    
                                                              
  
                                                
                                            
  
                                                                     
            
 */
#define __get_user(x, ptr)						\
	({								\
		int __ret;						\
		__chk_user_ptr(ptr);					\
		switch (sizeof(*(ptr))) {				\
		case 1: __get_user_1(x, ptr, __ret); break;		\
		case 2: __get_user_2(x, ptr, __ret); break;		\
		case 4: __get_user_4(x, ptr, __ret); break;		\
		case 8: __get_user_8(x, ptr, __ret); break;		\
		default: __ret = __get_user_bad(); break;		\
		}							\
		__ret;							\
	})

/*                                  */

#define __put_user_asm(OP, x, ptr, ret)			\
	asm volatile("1: {" #OP " %1, %2; movei %0, 0 }\n"		\
		     ".pushsection .fixup,\"ax\"\n"			\
		     "0: { movei %0, %3; j 9f }\n"			\
		     ".section __ex_table,\"a\"\n"			\
		     _ASM_PTR " 1b, 0b\n"				\
		     ".popsection\n"					\
		     "9:"						\
		     : "=r" (ret)					\
		     : "r" (ptr), "r" (x), "i" (-EFAULT))

#ifdef __tilegx__
#define __put_user_1(x, ptr, ret) __put_user_asm(st1, x, ptr, ret)
#define __put_user_2(x, ptr, ret) __put_user_asm(st2, x, ptr, ret)
#define __put_user_4(x, ptr, ret) __put_user_asm(st4, x, ptr, ret)
#define __put_user_8(x, ptr, ret) __put_user_asm(st, x, ptr, ret)
#else
#define __put_user_1(x, ptr, ret) __put_user_asm(sb, x, ptr, ret)
#define __put_user_2(x, ptr, ret) __put_user_asm(sh, x, ptr, ret)
#define __put_user_4(x, ptr, ret) __put_user_asm(sw, x, ptr, ret)
#define __put_user_8(x, ptr, ret)					\
	({								\
		u64 __x = (__typeof((x)-(x)))(x);			\
		int __lo = (int) __x, __hi = (int) (__x >> 32);		\
		asm volatile("1: { sw %1, %2; addi %0, %1, 4 }\n"	\
			     "2: { sw %0, %3; movei %0, 0 }\n"		\
			     ".pushsection .fixup,\"ax\"\n"		\
			     "0: { movei %0, %4; j 9f }\n"		\
			     ".section __ex_table,\"a\"\n"		\
			     ".word 1b, 0b\n"				\
			     ".word 2b, 0b\n"				\
			     ".popsection\n"				\
			     "9:"					\
			     : "=&r" (ret)				\
			     : "r" (ptr), "r" (__lo32(__lo, __hi)),	\
			     "r" (__hi32(__lo, __hi)), "i" (-EFAULT));	\
	})
#endif

extern int __put_user_bad(void)
  __attribute__((warning("sizeof __put_user argument not 1, 2, 4 or 8")));

/* 
                                                                          
                                     
                                            
  
                                                        
  
                                                                    
                                                                     
                                        
  
                                                                            
                                       
  
                                                                     
            
  
                                                
 */
#define __put_user(x, ptr)						\
({									\
	int __ret;							\
	__chk_user_ptr(ptr);						\
	switch (sizeof(*(ptr))) {					\
	case 1: __put_user_1(x, ptr, __ret); break;			\
	case 2: __put_user_2(x, ptr, __ret); break;			\
	case 4: __put_user_4(x, ptr, __ret); break;			\
	case 8: __put_user_8(x, ptr, __ret); break;			\
	default: __ret = __put_user_bad(); break;			\
	}								\
	__ret;								\
})

/*
                                                                    
                                                                 
                   
 */
#define put_user(x, ptr)						\
({									\
	__typeof__(*(ptr)) __user *__Pu_addr = (ptr);			\
	access_ok(VERIFY_WRITE, (__Pu_addr), sizeof(*(__Pu_addr))) ?	\
		__put_user((x), (__Pu_addr)) :				\
		-EFAULT;						\
})

#define get_user(x, ptr)						\
({									\
	__typeof__(*(ptr)) const __user *__Gu_addr = (ptr);		\
	access_ok(VERIFY_READ, (__Gu_addr), sizeof(*(__Gu_addr))) ?	\
		__get_user((x), (__Gu_addr)) :				\
		((x) = 0, -EFAULT);					\
})

/* 
                                                                    
                                             
                                          
                                  
  
                                                        
  
                                                                
                                                                     
  
                                                    
                                 
  
                                                                 
                                                                 
                                                 
 */
extern unsigned long __must_check __copy_to_user_inatomic(
	void __user *to, const void *from, unsigned long n);

static inline unsigned long __must_check
__copy_to_user(void __user *to, const void *from, unsigned long n)
{
	might_fault();
	return __copy_to_user_inatomic(to, from, n);
}

static inline unsigned long __must_check
copy_to_user(void __user *to, const void *from, unsigned long n)
{
	if (access_ok(VERIFY_WRITE, to, n))
		n = __copy_to_user(to, from, n);
	return n;
}

/* 
                                                                      
                                               
                                        
                                  
  
                                                        
  
                                                                
                                                                     
  
                                                    
                                 
  
                                                                      
                                               
  
                                                                   
                                                                 
                                                               
                             
 */
extern unsigned long __must_check __copy_from_user_inatomic(
	void *to, const void __user *from, unsigned long n);
extern unsigned long __must_check __copy_from_user_zeroing(
	void *to, const void __user *from, unsigned long n);

static inline unsigned long __must_check
__copy_from_user(void *to, const void __user *from, unsigned long n)
{
       might_fault();
       return __copy_from_user_zeroing(to, from, n);
}

static inline unsigned long __must_check
_copy_from_user(void *to, const void __user *from, unsigned long n)
{
	if (access_ok(VERIFY_READ, from, n))
		n = __copy_from_user(to, from, n);
	else
		memset(to, 0, n);
	return n;
}

#ifdef CONFIG_DEBUG_STRICT_USER_COPY_CHECKS
/*
                                                                              
                                                                        
           
 */
extern void copy_from_user_overflow(void)
	__compiletime_warning("copy_from_user() size is not provably correct");

static inline unsigned long __must_check copy_from_user(void *to,
					  const void __user *from,
					  unsigned long n)
{
	int sz = __compiletime_object_size(to);

	if (likely(sz == -1 || sz >= n))
		n = _copy_from_user(to, from, n);
	else
		copy_from_user_overflow();

	return n;
}
#else
#define copy_from_user _copy_from_user
#endif

#ifdef __tilegx__
/* 
                                                                      
                                             
                                        
                                  
  
                                                        
  
                                                              
                                                                      
  
                                                    
                                 
 */
extern unsigned long __copy_in_user_inatomic(
	void __user *to, const void __user *from, unsigned long n);

static inline unsigned long __must_check
__copy_in_user(void __user *to, const void __user *from, unsigned long n)
{
	might_sleep();
	return __copy_in_user_inatomic(to, from, n);
}

static inline unsigned long __must_check
copy_in_user(void __user *to, const void __user *from, unsigned long n)
{
	if (access_ok(VERIFY_WRITE, to, n) && access_ok(VERIFY_READ, from, n))
		n = __copy_in_user(to, from, n);
	return n;
}
#endif


/* 
                                                         
                               
  
                                                        
  
                                                         
  
                                                                
                           
  
                                                                       
                                         
 */
extern long strnlen_user_asm(const char __user *str, long n);
static inline long __must_check strnlen_user(const char __user *str, long n)
{
	might_fault();
	return strnlen_user_asm(str, n);
}
#define strlen_user(str) strnlen_user(str, LONG_MAX)

/* 
                                                                                        
                                                                        
                                   
                                         
                                                                       
  
                                                                 
                                                                        
                 
  
                                                                           
        
  
                                                                         
           
  
                                                                          
                      
 */
extern long strncpy_from_user_asm(char *dst, const char __user *src, long);
static inline long __must_check __strncpy_from_user(
	char *dst, const char __user *src, long count)
{
	might_fault();
	return strncpy_from_user_asm(dst, src, count);
}
static inline long __must_check strncpy_from_user(
	char *dst, const char __user *src, long count)
{
	if (access_ok(VERIFY_READ, src, 1))
		return __strncpy_from_user(dst, src, count);
	return -EFAULT;
}

/* 
                                                      
                                              
                                   
  
                                        
  
                                                     
                                 
 */
extern unsigned long clear_user_asm(void __user *mem, unsigned long len);
static inline unsigned long __must_check __clear_user(
	void __user *mem, unsigned long len)
{
	might_fault();
	return clear_user_asm(mem, len);
}
static inline unsigned long __must_check clear_user(
	void __user *mem, unsigned long len)
{
	if (access_ok(VERIFY_WRITE, mem, len))
		return __clear_user(mem, len);
	return len;
}

/* 
                                                                  
                                              
                                    
  
                                                     
                                 
 */
extern unsigned long flush_user_asm(void __user *mem, unsigned long len);
static inline unsigned long __must_check __flush_user(
	void __user *mem, unsigned long len)
{
	int retval;

	might_fault();
	retval = flush_user_asm(mem, len);
	mb_incoherent();
	return retval;
}

static inline unsigned long __must_check flush_user(
	void __user *mem, unsigned long len)
{
	if (access_ok(VERIFY_WRITE, mem, len))
		return __flush_user(mem, len);
	return len;
}

/* 
                                                                     
                                              
                                         
  
                                                         
                                 
  
                                                        
                                                                
                                     
 */
extern unsigned long inv_user_asm(void __user *mem, unsigned long len);
static inline unsigned long __must_check __inv_user(
	void __user *mem, unsigned long len)
{
	int retval;

	might_fault();
	retval = inv_user_asm(mem, len);
	mb_incoherent();
	return retval;
}
static inline unsigned long __must_check inv_user(
	void __user *mem, unsigned long len)
{
	if (access_ok(VERIFY_WRITE, mem, len))
		return __inv_user(mem, len);
	return len;
}

/* 
                                                                       
                                              
                                         
  
                                                               
                                 
 */
extern unsigned long finv_user_asm(void __user *mem, unsigned long len);
static inline unsigned long __must_check __finv_user(
	void __user *mem, unsigned long len)
{
	int retval;

	might_fault();
	retval = finv_user_asm(mem, len);
	mb_incoherent();
	return retval;
}
static inline unsigned long __must_check finv_user(
	void __user *mem, unsigned long len)
{
	if (access_ok(VERIFY_WRITE, mem, len))
		return __finv_user(mem, len);
	return len;
}

#endif /*                     */
