/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (c) 1994-1997, 99, 2000, 06, 07 Ralf Baechle (ralf@linux-mips.org)
 * Copyright (c) 1999, 2000  Silicon Graphics, Inc.
 */
#include <linux/bitops.h>
#include <linux/irqflags.h>
#include <linux/export.h>


/* 
                                                                      
                                                 
                      
                                            
 */
void __mips_set_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long *a = (unsigned long *)addr;
	unsigned bit = nr & SZLONG_MASK;
	unsigned long mask;
	unsigned long flags;

	a += nr >> SZLONG_LOG;
	mask = 1UL << bit;
	raw_local_irq_save(flags);
	*a |= mask;
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL(__mips_set_bit);


/* 
                                                                               
                                    
                    
                                        
 */
void __mips_clear_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long *a = (unsigned long *)addr;
	unsigned bit = nr & SZLONG_MASK;
	unsigned long mask;
	unsigned long flags;

	a += nr >> SZLONG_LOG;
	mask = 1UL << bit;
	raw_local_irq_save(flags);
	*a &= ~mask;
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL(__mips_clear_bit);


/* 
                                                                             
                                       
                     
                                        
 */
void __mips_change_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long *a = (unsigned long *)addr;
	unsigned bit = nr & SZLONG_MASK;
	unsigned long mask;
	unsigned long flags;

	a += nr >> SZLONG_LOG;
	mask = 1UL << bit;
	raw_local_irq_save(flags);
	*a ^= mask;
	raw_local_irq_restore(flags);
}
EXPORT_SYMBOL(__mips_change_bit);


/* 
                                                                         
                                                                    
                  
                               
 */
int __mips_test_and_set_bit(unsigned long nr,
			    volatile unsigned long *addr)
{
	unsigned long *a = (unsigned long *)addr;
	unsigned bit = nr & SZLONG_MASK;
	unsigned long mask;
	unsigned long flags;
	int res;

	a += nr >> SZLONG_LOG;
	mask = 1UL << bit;
	raw_local_irq_save(flags);
	res = (mask & *a) != 0;
	*a |= mask;
	raw_local_irq_restore(flags);
	return res;
}
EXPORT_SYMBOL(__mips_test_and_set_bit);


/* 
                                                                              
                                                                         
                  
                               
 */
int __mips_test_and_set_bit_lock(unsigned long nr,
				 volatile unsigned long *addr)
{
	unsigned long *a = (unsigned long *)addr;
	unsigned bit = nr & SZLONG_MASK;
	unsigned long mask;
	unsigned long flags;
	int res;

	a += nr >> SZLONG_LOG;
	mask = 1UL << bit;
	raw_local_irq_save(flags);
	res = (mask & *a) != 0;
	*a |= mask;
	raw_local_irq_restore(flags);
	return res;
}
EXPORT_SYMBOL(__mips_test_and_set_bit_lock);


/* 
                                                                             
                                                                      
                    
                               
 */
int __mips_test_and_clear_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long *a = (unsigned long *)addr;
	unsigned bit = nr & SZLONG_MASK;
	unsigned long mask;
	unsigned long flags;
	int res;

	a += nr >> SZLONG_LOG;
	mask = 1UL << bit;
	raw_local_irq_save(flags);
	res = (mask & *a) != 0;
	*a &= ~mask;
	raw_local_irq_restore(flags);
	return res;
}
EXPORT_SYMBOL(__mips_test_and_clear_bit);


/* 
                                                                              
                                                                       
                     
                               
 */
int __mips_test_and_change_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long *a = (unsigned long *)addr;
	unsigned bit = nr & SZLONG_MASK;
	unsigned long mask;
	unsigned long flags;
	int res;

	a += nr >> SZLONG_LOG;
	mask = 1UL << bit;
	raw_local_irq_save(flags);
	res = (mask & *a) != 0;
	*a ^= mask;
	raw_local_irq_restore(flags);
	return res;
}
EXPORT_SYMBOL(__mips_test_and_change_bit);
