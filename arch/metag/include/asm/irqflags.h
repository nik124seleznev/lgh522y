/*
                     
  
                                                                    
                                                           
                                                         
 */
#ifndef _ASM_IRQFLAGS_H
#define _ASM_IRQFLAGS_H

#ifndef __ASSEMBLY__

#include <asm/core_reg.h>
#include <asm/metag_regs.h>

#define INTS_OFF_MASK TXSTATI_BGNDHALT_BIT

#ifdef CONFIG_SMP
extern unsigned int get_trigger_mask(void);
#else

extern unsigned int global_trigger_mask;

static inline unsigned int get_trigger_mask(void)
{
	return global_trigger_mask;
}
#endif

static inline unsigned long arch_local_save_flags(void)
{
	return __core_reg_get(TXMASKI);
}

static inline int arch_irqs_disabled_flags(unsigned long flags)
{
	return (flags & ~INTS_OFF_MASK) == 0;
}

static inline int arch_irqs_disabled(void)
{
	unsigned long flags = arch_local_save_flags();

	return arch_irqs_disabled_flags(flags);
}

static inline unsigned long __irqs_disabled(void)
{
	/*
                                                          
                                                              
  */
	return arch_local_save_flags() & INTS_OFF_MASK;
}

/*
                      
 */
static inline unsigned long arch_local_irq_save(void)
{
	unsigned long flags = __irqs_disabled();

	asm volatile("SWAP %0,TXMASKI\n" : "=r" (flags) : "0" (flags)
		     : "memory");

	return flags;
}

static inline void arch_local_irq_restore(unsigned long flags)
{
	asm volatile("MOV TXMASKI,%0\n" : : "r" (flags) : "memory");
}

static inline void arch_local_irq_disable(void)
{
	unsigned long flags = __irqs_disabled();

	asm volatile("MOV TXMASKI,%0\n" : : "r" (flags) : "memory");
}

#ifdef CONFIG_SMP
/*                                                               */
void arch_local_irq_enable(void);
#else
static inline void arch_local_irq_enable(void)
{
	arch_local_irq_restore(get_trigger_mask());
}
#endif

#endif /*                */

#endif /*                    */
