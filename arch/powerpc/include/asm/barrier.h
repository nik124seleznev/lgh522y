/*
 * Copyright (C) 1999 Cort Dougan <cort@cs.nmt.edu>
 */
#ifndef _ASM_POWERPC_BARRIER_H
#define _ASM_POWERPC_BARRIER_H

/*
                  
                                                                     
                                                                   
                                                                      
                                                                      
                                                               
  
                                                                    
                                                          
                                                           
                                                                       
                                  
  
                                                                    
                                                                       
              
  
                                                                     
                                                                       
                                                                      
                                    
  
                                                                        
                                                                 
 */
#define mb()   __asm__ __volatile__ ("sync" : : : "memory")
#define rmb()  __asm__ __volatile__ ("sync" : : : "memory")
#define wmb()  __asm__ __volatile__ ("sync" : : : "memory")
#define read_barrier_depends()  do { } while(0)

#define set_mb(var, value)	do { var = value; mb(); } while (0)

#ifdef CONFIG_SMP

#ifdef __SUBARCH_HAS_LWSYNC
#    define SMPWMB      LWSYNC
#else
#    define SMPWMB      eieio
#endif

#define __lwsync()	__asm__ __volatile__ (stringify_in_c(LWSYNC) : : :"memory")

#define smp_mb()	mb()
#define smp_rmb()	__lwsync()
#define smp_wmb()	__asm__ __volatile__ (stringify_in_c(SMPWMB) : : :"memory")
#define smp_read_barrier_depends()	read_barrier_depends()
#else
#define __lwsync()	barrier()

#define smp_mb()	barrier()
#define smp_rmb()	barrier()
#define smp_wmb()	barrier()
#define smp_read_barrier_depends()	do { } while(0)
#endif /*            */

/*
                                                                     
                                                                       
                                                              
                                                                      
 */
#define data_barrier(x)	\
	asm volatile("twi 0,%0,0; isync" : : "r" (x) : "memory");

#define smp_store_release(p, v)						\
do {									\
	compiletime_assert_atomic_type(*p);				\
	__lwsync();							\
	ACCESS_ONCE(*p) = (v);						\
} while (0)

#define smp_load_acquire(p)						\
({									\
	typeof(*p) ___p1 = ACCESS_ONCE(*p);				\
	compiletime_assert_atomic_type(*p);				\
	__lwsync();							\
	___p1;								\
})

#endif /*                        */
