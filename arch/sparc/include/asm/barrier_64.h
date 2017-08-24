#ifndef __SPARC64_BARRIER_H
#define __SPARC64_BARRIER_H

/*                                                                      
                                                                          
                                                                        
                                                                    
  
                                                                        
                                                                             
                                                                           
                                                                       
                      
  
                           
                      
                      
                
                   
  
                                                                             
                                                                        
                                        
 */
#define membar_safe(type) \
do {	__asm__ __volatile__("ba,pt	%%xcc, 1f\n\t" \
			     " membar	" type "\n" \
			     "1:\n" \
			     : : : "memory"); \
} while (0)

/*                                                           
                                                              
                                                       
 */
#define mb()	membar_safe("#StoreLoad")
#define rmb()	__asm__ __volatile__("":::"memory")
#define wmb()	__asm__ __volatile__("":::"memory")

#define read_barrier_depends()		do { } while(0)
#define set_mb(__var, __value) \
	do { __var = __value; membar_safe("#StoreLoad"); } while(0)

#ifdef CONFIG_SMP
#define smp_mb()	mb()
#define smp_rmb()	rmb()
#define smp_wmb()	wmb()
#else
#define smp_mb()	__asm__ __volatile__("":::"memory")
#define smp_rmb()	__asm__ __volatile__("":::"memory")
#define smp_wmb()	__asm__ __volatile__("":::"memory")
#endif

#define smp_read_barrier_depends()	do { } while(0)

#define smp_store_release(p, v)						\
do {									\
	compiletime_assert_atomic_type(*p);				\
	barrier();							\
	ACCESS_ONCE(*p) = (v);						\
} while (0)

#define smp_load_acquire(p)						\
({									\
	typeof(*p) ___p1 = ACCESS_ONCE(*p);				\
	compiletime_assert_atomic_type(*p);				\
	barrier();							\
	___p1;								\
})

#endif /*                        */
