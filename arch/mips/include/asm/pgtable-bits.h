/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994 - 2002 by Ralf Baechle
 * Copyright (C) 1999, 2000, 2001 Silicon Graphics, Inc.
 * Copyright (C) 2002  Maciej W. Rozycki
 */
#ifndef _ASM_PGTABLE_BITS_H
#define _ASM_PGTABLE_BITS_H


/*
                                                                
                                                               
                                                                     
                                                               
                   
  
                                                              
                                                                   
                                                                  
                                                                   
                                                                 
                                                                   
             
  
                                                                 
                                                                      
                                                             
                                                                    
                                                                   
 */
#if defined(CONFIG_64BIT_PHYS_ADDR) && defined(CONFIG_CPU_MIPS32)

/*
                                                           
 */
#define _PAGE_R4KBUG		(1 << 0)  /*                         */
#define _PAGE_GLOBAL		(1 << 0)
#define _PAGE_VALID_SHIFT	1
#define _PAGE_VALID		(1 << _PAGE_VALID_SHIFT)
#define _PAGE_SILENT_READ	(1 << 1)  /*               */
#define _PAGE_DIRTY_SHIFT	2
#define _PAGE_DIRTY		(1 << _PAGE_DIRTY_SHIFT)  /*                         */
#define _PAGE_SILENT_WRITE	(1 << 2)
#define _CACHE_SHIFT		3
#define _CACHE_MASK		(7 << 3)

/*
                                                 
  
                                                 
 */
#define _PAGE_PRESENT_SHIFT	6
#define _PAGE_PRESENT		(1 << _PAGE_PRESENT_SHIFT)
#define _PAGE_READ_SHIFT	7
#define _PAGE_READ		(1 << _PAGE_READ_SHIFT)
#define _PAGE_WRITE_SHIFT	8
#define _PAGE_WRITE		(1 << _PAGE_WRITE_SHIFT)
#define _PAGE_ACCESSED_SHIFT	9
#define _PAGE_ACCESSED		(1 << _PAGE_ACCESSED_SHIFT)
#define _PAGE_MODIFIED_SHIFT	10
#define _PAGE_MODIFIED		(1 << _PAGE_MODIFIED_SHIFT)

#define _PAGE_FILE		(1 << 10)

#elif defined(CONFIG_CPU_R3000) || defined(CONFIG_CPU_TX39XX)

/*
                                            
  
                                                 
 */
#define _PAGE_PRESENT_SHIFT	0
#define _PAGE_PRESENT		(1 <<  _PAGE_PRESENT_SHIFT)
#define _PAGE_READ_SHIFT	1
#define _PAGE_READ		(1 <<  _PAGE_READ_SHIFT)
#define _PAGE_WRITE_SHIFT	2
#define _PAGE_WRITE		(1 <<  _PAGE_WRITE_SHIFT)
#define _PAGE_ACCESSED_SHIFT	3
#define _PAGE_ACCESSED		(1 <<  _PAGE_ACCESSED_SHIFT)
#define _PAGE_MODIFIED_SHIFT	4
#define _PAGE_MODIFIED		(1 <<  _PAGE_MODIFIED_SHIFT)
#define _PAGE_FILE_SHIFT	4
#define _PAGE_FILE		(1 <<  _PAGE_FILE_SHIFT)

/*
                                      
 */
#define _PAGE_GLOBAL_SHIFT	8
#define _PAGE_GLOBAL		(1 <<  _PAGE_GLOBAL_SHIFT)
#define _PAGE_VALID_SHIFT	9
#define _PAGE_VALID		(1 <<  _PAGE_VALID_SHIFT)
#define _PAGE_SILENT_READ	(1 <<  _PAGE_VALID_SHIFT)	/*          */
#define _PAGE_DIRTY_SHIFT	10
#define _PAGE_DIRTY		(1 << _PAGE_DIRTY_SHIFT)
#define _PAGE_SILENT_WRITE	(1 << _PAGE_DIRTY_SHIFT)
#define _CACHE_UNCACHED_SHIFT	11
#define _CACHE_UNCACHED		(1 << _CACHE_UNCACHED_SHIFT)
#define _CACHE_MASK		(1 << _CACHE_UNCACHED_SHIFT)

#else /*                   */
/*
                                                                   
                                                                    
                                                                    
                                                                   
                                        
 */

/*
                                                 
  
                                                                  
                                                 
 */
#define _PAGE_PRESENT_SHIFT	(0)
#define _PAGE_PRESENT		(1 << _PAGE_PRESENT_SHIFT)
#define _PAGE_READ_SHIFT	(cpu_has_rixi ? _PAGE_PRESENT_SHIFT : _PAGE_PRESENT_SHIFT + 1)
#define _PAGE_READ ({BUG_ON(cpu_has_rixi); 1 << _PAGE_READ_SHIFT; })
#define _PAGE_WRITE_SHIFT	(_PAGE_READ_SHIFT + 1)
#define _PAGE_WRITE		(1 << _PAGE_WRITE_SHIFT)
#define _PAGE_ACCESSED_SHIFT	(_PAGE_WRITE_SHIFT + 1)
#define _PAGE_ACCESSED		(1 << _PAGE_ACCESSED_SHIFT)
#define _PAGE_MODIFIED_SHIFT	(_PAGE_ACCESSED_SHIFT + 1)
#define _PAGE_MODIFIED		(1 << _PAGE_MODIFIED_SHIFT)
#define _PAGE_FILE		(_PAGE_MODIFIED)

#ifdef CONFIG_MIPS_HUGE_TLB_SUPPORT
/*               */
#define _PAGE_HUGE_SHIFT	(_PAGE_MODIFIED_SHIFT + 1)
#define _PAGE_HUGE		(1 << _PAGE_HUGE_SHIFT)
#else
#define _PAGE_HUGE_SHIFT	(_PAGE_MODIFIED_SHIFT)
#define _PAGE_HUGE		({BUG(); 1; })	/*             */
#endif

#ifdef CONFIG_MIPS_HUGE_TLB_SUPPORT
/*               */
#define _PAGE_SPLITTING_SHIFT	(_PAGE_HUGE_SHIFT + 1)
#define _PAGE_SPLITTING		(1 << _PAGE_SPLITTING_SHIFT)
#else
#define _PAGE_SPLITTING_SHIFT	(_PAGE_HUGE_SHIFT)
#define _PAGE_SPLITTING		({BUG(); 1; })	/*             */
#endif

/*                         */
#define _PAGE_NO_EXEC_SHIFT	(cpu_has_rixi ? _PAGE_SPLITTING_SHIFT + 1 : _PAGE_SPLITTING_SHIFT)
#define _PAGE_NO_EXEC		({BUG_ON(!cpu_has_rixi); 1 << _PAGE_NO_EXEC_SHIFT; })

/*                     */
#define _PAGE_NO_READ_SHIFT	(cpu_has_rixi ? _PAGE_NO_EXEC_SHIFT + 1 : _PAGE_NO_EXEC_SHIFT)
#define _PAGE_NO_READ		({BUG_ON(!cpu_has_rixi); 1 << _PAGE_NO_READ_SHIFT; })

#define _PAGE_GLOBAL_SHIFT	(_PAGE_NO_READ_SHIFT + 1)
#define _PAGE_GLOBAL		(1 << _PAGE_GLOBAL_SHIFT)

#define _PAGE_VALID_SHIFT	(_PAGE_GLOBAL_SHIFT + 1)
#define _PAGE_VALID		(1 << _PAGE_VALID_SHIFT)
/*             */
#define _PAGE_SILENT_READ	(_PAGE_VALID)

/*                       */
#define _PAGE_DIRTY_SHIFT	(_PAGE_VALID_SHIFT + 1)
#define _PAGE_DIRTY		(1 << _PAGE_DIRTY_SHIFT)
#define _PAGE_SILENT_WRITE	(_PAGE_DIRTY)

#define _CACHE_SHIFT		(_PAGE_DIRTY_SHIFT + 1)
#define _CACHE_MASK		(7 << _CACHE_SHIFT)

#define _PFN_SHIFT		(PAGE_SHIFT - 12 + _CACHE_SHIFT + 3)

#endif /*                                                              */

#ifndef _PFN_SHIFT
#define _PFN_SHIFT		    PAGE_SHIFT
#endif
#define _PFN_MASK		(~((1 << (_PFN_SHIFT)) - 1))

#ifndef _PAGE_NO_READ
#define _PAGE_NO_READ ({BUG(); 0; })
#define _PAGE_NO_READ_SHIFT ({BUG(); 0; })
#endif
#ifndef _PAGE_NO_EXEC
#define _PAGE_NO_EXEC ({BUG(); 0; })
#endif
#ifndef _PAGE_GLOBAL_SHIFT
#define _PAGE_GLOBAL_SHIFT ilog2(_PAGE_GLOBAL)
#endif


#ifndef __ASSEMBLY__
/*
                                                               
                    
 */
static inline uint64_t pte_to_entrylo(unsigned long pte_val)
{
	if (cpu_has_rixi) {
		int sa;
#ifdef CONFIG_32BIT
		sa = 31 - _PAGE_NO_READ_SHIFT;
#else
		sa = 63 - _PAGE_NO_READ_SHIFT;
#endif
		/*
                                                
                                                       
                                              
   */
		return (pte_val >> _PAGE_GLOBAL_SHIFT) |
			((pte_val & (_PAGE_NO_EXEC | _PAGE_NO_READ)) << sa);
	}

	return pte_val >> _PAGE_GLOBAL_SHIFT;
}
#endif

/*
                   
 */
#if defined(CONFIG_CPU_R3000) || defined(CONFIG_CPU_TX39XX)

#define _CACHE_CACHABLE_NONCOHERENT 0

#elif defined(CONFIG_CPU_SB1)

/*                                                  
                                                          */

#define _CACHE_UNCACHED		    (2<<_CACHE_SHIFT)
#define _CACHE_CACHABLE_COW	    (5<<_CACHE_SHIFT)
#define _CACHE_CACHABLE_NONCOHERENT (5<<_CACHE_SHIFT)
#define _CACHE_UNCACHED_ACCELERATED (7<<_CACHE_SHIFT)

#else

#define _CACHE_CACHABLE_NO_WA	    (0<<_CACHE_SHIFT)  /*              */
#define _CACHE_CACHABLE_WA	    (1<<_CACHE_SHIFT)  /*              */
#define _CACHE_UNCACHED		    (2<<_CACHE_SHIFT)  /*              */
#define _CACHE_CACHABLE_NONCOHERENT (3<<_CACHE_SHIFT)  /*              */
#define _CACHE_CACHABLE_CE	    (4<<_CACHE_SHIFT)  /*                 */
#define _CACHE_CACHABLE_COW	    (5<<_CACHE_SHIFT)  /*                 */
#define _CACHE_CACHABLE_COHERENT    (5<<_CACHE_SHIFT)  /*                */
#define _CACHE_CACHABLE_CUW	    (6<<_CACHE_SHIFT)  /*                 */
#define _CACHE_UNCACHED_ACCELERATED (7<<_CACHE_SHIFT)  /*               */

#endif

#define __READABLE	(_PAGE_SILENT_READ | _PAGE_ACCESSED | (cpu_has_rixi ? 0 : _PAGE_READ))
#define __WRITEABLE	(_PAGE_WRITE | _PAGE_SILENT_WRITE | _PAGE_MODIFIED)

#define _PAGE_CHG_MASK	(_PFN_MASK | _PAGE_ACCESSED | _PAGE_MODIFIED | _CACHE_MASK)

#endif /*                     */
