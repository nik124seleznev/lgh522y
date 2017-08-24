#ifndef _MIPS_SPARSEMEM_H
#define _MIPS_SPARSEMEM_H
#ifdef CONFIG_SPARSEMEM

/*
                                                       
                                                                   
 */
#if defined(CONFIG_MIPS_HUGE_TLB_SUPPORT) && defined(CONFIG_PAGE_SIZE_64KB)
# define SECTION_SIZE_BITS	29
#else
# define SECTION_SIZE_BITS	28
#endif
#define MAX_PHYSMEM_BITS	35

#endif /*                  */
#endif /*                   */
