/*
 * psr.h: This file holds the macros for masking off various parts of
 *        the processor status register on the Sparc. This is valid
 *        for Version 8. On the V9 this is renamed to the PSTATE
 *        register and its members are accessed as fields like
 *        PSTATE.PRIV for the current CPU privilege level.
 *
 * Copyright (C) 1994 David S. Miller (davem@caip.rutgers.edu)
 */
#ifndef __LINUX_SPARC_PSR_H
#define __LINUX_SPARC_PSR_H

#include <uapi/asm/psr.h>


#ifndef __ASSEMBLY__
/*                        */
static inline unsigned int get_psr(void)
{
	unsigned int psr;
	__asm__ __volatile__(
		"rd	%%psr, %0\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	: "=r" (psr)
	: /*           */
	: "memory");

	return psr;
}

static inline void put_psr(unsigned int new_psr)
{
	__asm__ __volatile__(
		"wr	%0, 0x0, %%psr\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	: /*            */
	: "r" (new_psr)
	: "memory", "cc");
}

/*                                                                 
                                                                  
                
 */

extern unsigned int fsr_storage;

static inline unsigned int get_fsr(void)
{
	unsigned int fsr = 0;

	__asm__ __volatile__(
		"st	%%fsr, %1\n\t"
		"ld	%1, %0\n\t"
	: "=r" (fsr)
	: "m" (fsr_storage));

	return fsr;
}

#endif /*                 */

#endif /*                        */
