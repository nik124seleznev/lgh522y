/*
                   
 */
#include <asm/trace_clock.h>
#include <asm/barrier.h>
#include <asm/msr.h>

/*
                                                                 
  
                                                       
 */
u64 notrace trace_clock_x86_tsc(void)
{
	u64 ret;

	rdtsc_barrier();
	rdtscll(ret);

	return ret;
}
