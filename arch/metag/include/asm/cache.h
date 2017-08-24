#ifndef __ASM_METAG_CACHE_H
#define __ASM_METAG_CACHE_H

/*                               */
#define L1_CACHE_SHIFT		6
#define L1_CACHE_BYTES		(1 << L1_CACHE_SHIFT)

/*                                                      */
#define ARCH_SLAB_MINALIGN	8

/*
                                                                            
                                     
 */
#ifdef CONFIG_METAG_L2C
#define ARCH_DMA_MINALIGN	L1_CACHE_BYTES
#else
#define ARCH_DMA_MINALIGN	8
#endif

#define __read_mostly __attribute__((__section__(".data..read_mostly")))

#endif
