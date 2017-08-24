/*
 *  arch/arm/include/asm/ptrace.h
 *
 *  Copyright (C) 1996-2003 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ASM_ARM_PTRACE_H
#define __ASM_ARM_PTRACE_H

#include <uapi/asm/ptrace.h>

#ifndef __ASSEMBLY__
struct pt_regs {
	unsigned long uregs[18];
};

#define user_mode(regs)	\
	(((regs)->ARM_cpsr & 0xf) == 0)

#ifdef CONFIG_ARM_THUMB
#define thumb_mode(regs) \
	(((regs)->ARM_cpsr & PSR_T_BIT))
#else
#define thumb_mode(regs) (0)
#endif

#define isa_mode(regs) \
	((((regs)->ARM_cpsr & PSR_J_BIT) >> 23) | \
	 (((regs)->ARM_cpsr & PSR_T_BIT) >> 5))

#define processor_mode(regs) \
	((regs)->ARM_cpsr & MODE_MASK)

#define interrupts_enabled(regs) \
	(!((regs)->ARM_cpsr & PSR_I_BIT))

#define fast_interrupts_enabled(regs) \
	(!((regs)->ARM_cpsr & PSR_F_BIT))

/*                                                  
                                                 
 */
static inline int valid_user_regs(struct pt_regs *regs)
{
	unsigned long mode = regs->ARM_cpsr & MODE_MASK;

	/*
                                                       
  */
	regs->ARM_cpsr &= ~(PSR_F_BIT | PSR_A_BIT);

	if ((regs->ARM_cpsr & PSR_I_BIT) == 0) {
		if (mode == USR_MODE)
			return 1;
		if (elf_hwcap & HWCAP_26BIT && mode == USR26_MODE)
			return 1;
	}

	/*
                                      
  */
	regs->ARM_cpsr &= PSR_f | PSR_s | PSR_x | PSR_T_BIT | MODE32_BIT;
	if (!(elf_hwcap & HWCAP_26BIT))
		regs->ARM_cpsr |= USR_MODE;

	return 0;
}

static inline long regs_return_value(struct pt_regs *regs)
{
	return regs->ARM_r0;
}

#define instruction_pointer(regs)	(regs)->ARM_pc

#ifdef CONFIG_SMP
extern unsigned long profile_pc(struct pt_regs *regs);
#else
#define profile_pc(regs) instruction_pointer(regs)
#endif

#define predicate(x)		((x) & 0xf0000000)
#define PREDICATE_ALWAYS	0xe0000000

/*
                                                                   
                                                                       
                                                                     
                                             
 */
#define is_wide_instruction(instr)	((unsigned)(instr) >= 0xe800)

/*
                                    
 */
#include <linux/stddef.h>
#include <linux/types.h>
#define MAX_REG_OFFSET (offsetof(struct pt_regs, ARM_ORIG_r0))

extern int regs_query_register_offset(const char *name);
extern const char *regs_query_register_name(unsigned int offset);
extern bool regs_within_kernel_stack(struct pt_regs *regs, unsigned long addr);
extern unsigned long regs_get_kernel_stack_nth(struct pt_regs *regs,
					       unsigned int n);

/* 
                                                           
                                                        
                                             
  
                                                                             
                                                               
                                                            
 */
static inline unsigned long regs_get_register(struct pt_regs *regs,
					      unsigned int offset)
{
	if (unlikely(offset > MAX_REG_OFFSET))
		return 0;
	return *(unsigned long *)((unsigned long)regs + offset);
}

/*                                   */
static inline unsigned long kernel_stack_pointer(struct pt_regs *regs)
{
	return regs->ARM_sp;
}

static inline unsigned long user_stack_pointer(struct pt_regs *regs)
{
	return regs->ARM_sp;
}

#define current_pt_regs(void) ({				\
	register unsigned long sp asm ("sp");			\
	(struct pt_regs *)((sp | (THREAD_SIZE - 1)) - 7) - 1;	\
})

#endif /*              */
#endif
