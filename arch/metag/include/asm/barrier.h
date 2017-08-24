#ifndef _ASM_METAG_BARRIER_H
#define _ASM_METAG_BARRIER_H

#include <asm/metag_mem.h>

#define nop()		asm volatile ("NOP")
#define mb()		wmb()
#define rmb()		barrier()

#ifdef CONFIG_METAG_META21

/*                                                   */
static inline void wr_fence(void)
{
	volatile int *flushptr = (volatile int *) LINSYSEVENT_WR_FENCE;
	barrier();
	*flushptr = 0;
	barrier();
}

#else /*                     */

/*
                                                                             
                                                                               
                              
                                                                          
                                                                               
            
 */
static inline void wr_fence(void)
{
	volatile int *flushptr = (volatile int *) LINSYSEVENT_WR_COMBINE_FLUSH;
	barrier();
	*flushptr = 0;
	*flushptr = 0;
	*flushptr = 0;
	*flushptr = 0;
	barrier();
}

#endif /*                      */

static inline void wmb(void)
{
	/*                                         */
	wr_fence();
}

#define read_barrier_depends()  do { } while (0)

#ifndef CONFIG_SMP
#define fence()		do { } while (0)
#define smp_mb()        barrier()
#define smp_rmb()       barrier()
#define smp_wmb()       barrier()
#else

#ifdef CONFIG_METAG_SMP_WRITE_REORDERING
/*
                                                                               
                                                                           
                                                                              
                                                                          
                                                                          
                     
 */
static inline void fence(void)
{
	volatile int *flushptr = (volatile int *) LINSYSEVENT_WR_ATOMIC_UNLOCK;
	barrier();
	*flushptr = 0;
	barrier();
}
#define smp_mb()        fence()
#define smp_rmb()       fence()
#define smp_wmb()       barrier()
#else
#define fence()		do { } while (0)
#define smp_mb()        barrier()
#define smp_rmb()       barrier()
#define smp_wmb()       barrier()
#endif
#endif
#define smp_read_barrier_depends()     do { } while (0)
#define set_mb(var, value) do { var = value; smp_mb(); } while (0)

#define smp_store_release(p, v)						\
do {									\
	compiletime_assert_atomic_type(*p);				\
	smp_mb();							\
	ACCESS_ONCE(*p) = (v);						\
} while (0)

#define smp_load_acquire(p)						\
({									\
	typeof(*p) ___p1 = ACCESS_ONCE(*p);				\
	compiletime_assert_atomic_type(*p);				\
	smp_mb();							\
	___p1;								\
})

#endif /*                      */
