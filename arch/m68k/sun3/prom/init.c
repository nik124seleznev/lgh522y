/*
 * init.c:  Initialize internal variables used by the PROM
 *          library functions.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 */

#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/openprom.h>
#include <asm/oplib.h>

struct linux_romvec *romvec;
enum prom_major_version prom_vers;
unsigned int prom_rev, prom_prev;

/*                                        */
int prom_root_node;

/*                                                  */
struct linux_nodeops *prom_nodeops;

/*                                                               
                                
                                                 
 */

void __init prom_init(struct linux_romvec *rp)
{
	romvec = rp;

	/*                            */
	return;
}
