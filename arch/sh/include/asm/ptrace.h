/*
 * Copyright (C) 1999, 2000  Niibe Yutaka
 */
#ifndef __ASM_SH_PTRACE_H
#define __ASM_SH_PTRACE_H


#include <linux/stringify.h>
#include <linux/stddef.h>
#include <linux/thread_info.h>
#include <asm/addrspace.h>
#include <asm/page.h>
#include <uapi/asm/ptrace.h>

#define user_mode(regs)			(((regs)->sr & 0x40000000)==0)
#define kernel_stack_pointer(_regs)	((unsigned long)(_regs)->regs[15])

#define GET_FP(regs)	((regs)->regs[14])
#define GET_USP(regs)	((regs)->regs[15])

#define arch_has_single_step()	(1)

/*
                                    
 */
struct pt_regs_offset {
	const char *name;
	int offset;
};

#define REG_OFFSET_NAME(r) {.name = #r, .offset = offsetof(struct pt_regs, r)}
#define REGS_OFFSET_NAME(num)	\
	{.name = __stringify(r##num), .offset = offsetof(struct pt_regs, regs[num])}
#define TREGS_OFFSET_NAME(num)	\
	{.name = __stringify(tr##num), .offset = offsetof(struct pt_regs, tregs[num])}
#define REG_OFFSET_END {.name = NULL, .offset = 0}

/*                                                    */
extern int regs_query_register_offset(const char *name);
extern const char *regs_query_register_name(unsigned int offset);

extern const struct pt_regs_offset regoffset_table[];

/* 
                                                           
                                                      
                                          
  
                                                                        
                                                                             
                                                            
 */
static inline unsigned long regs_get_register(struct pt_regs *regs,
					      unsigned int offset)
{
	if (unlikely(offset > MAX_REG_OFFSET))
		return 0;
	return *(unsigned long *)((unsigned long)regs + offset);
}

/* 
                                                              
                                                      
                                   
  
                                                                              
                                                                               
 */
static inline int regs_within_kernel_stack(struct pt_regs *regs,
					   unsigned long addr)
{
	return ((addr & ~(THREAD_SIZE - 1))  ==
		(kernel_stack_pointer(regs) & ~(THREAD_SIZE - 1)));
}

/* 
                                                           
                                                      
                           
  
                                                                            
                                                                        
                  
 */
static inline unsigned long regs_get_kernel_stack_nth(struct pt_regs *regs,
						      unsigned int n)
{
	unsigned long *addr = (unsigned long *)kernel_stack_pointer(regs);
	addr += n;
	if (regs_within_kernel_stack(regs, (unsigned long)addr))
		return *addr;
	else
		return 0;
}

struct perf_event;
struct perf_sample_data;

extern void ptrace_triggered(struct perf_event *bp,
		      struct perf_sample_data *data, struct pt_regs *regs);

#define task_pt_regs(task) \
	((struct pt_regs *) (task_stack_page(task) + THREAD_SIZE) - 1)

static inline unsigned long profile_pc(struct pt_regs *regs)
{
	unsigned long pc = regs->pc;

	if (virt_addr_uncached(pc))
		return CAC_ADDR(pc);

	return pc;
}
#define profile_pc profile_pc

#include <asm-generic/ptrace.h>
#endif /*                   */
