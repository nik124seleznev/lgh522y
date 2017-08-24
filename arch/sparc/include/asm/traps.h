/*
 * traps.h:  Format of entries for the Sparc trap table.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 */
#ifndef _SPARC_TRAPS_H
#define _SPARC_TRAPS_H

#include <uapi/asm/traps.h>

#ifndef __ASSEMBLY__
/*                                     */
struct tt_entry {
	unsigned long inst_one;
	unsigned long inst_two;
	unsigned long inst_three;
	unsigned long inst_four;
};

/*                                        */
extern struct tt_entry *sparc_ttable;

#endif /*                 */
#endif /*                   */
