/*
 *	Precise Delay Loops for Meta
 *
 *	Copyright (C) 1993 Linus Torvalds
 *	Copyright (C) 1997 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *	Copyright (C) 2007,2009 Imagination Technologies Ltd.
 *
 */

#include <linux/export.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include <asm/core_reg.h>
#include <asm/processor.h>

/*
                                                                       
                                                                          
                                                   
  
                                                                     
                           
 */
#define rdtimer(t) t = __core_reg_get(TXTACTCYC)

void __delay(unsigned long loops)
{
	unsigned long bclock, now;

	rdtimer(bclock);
	do {
		asm("NOP");
		rdtimer(now);
	} while ((now-bclock) < loops);
}
EXPORT_SYMBOL(__delay);

inline void __const_udelay(unsigned long xloops)
{
	u64 loops = (u64)xloops * (u64)loops_per_jiffy * HZ;
	__delay(loops >> 32);
}
EXPORT_SYMBOL(__const_udelay);

void __udelay(unsigned long usecs)
{
	__const_udelay(usecs * 0x000010c7); /*                              */
}
EXPORT_SYMBOL(__udelay);

void __ndelay(unsigned long nsecs)
{
	__const_udelay(nsecs * 0x00005); /*                                 */
}
EXPORT_SYMBOL(__ndelay);
