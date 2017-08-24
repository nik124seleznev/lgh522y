#ifndef _ASM_S390_DMA_H
#define _ASM_S390_DMA_H

#include <asm/io.h>

/*
                                                                        
                                                                    
                          
 */
#define MAX_DMA_ADDRESS         0x80000000

#ifdef CONFIG_PCI
extern int isa_dma_bridge_buggy;
#else
#define isa_dma_bridge_buggy	(0)
#endif

#endif /*                 */
