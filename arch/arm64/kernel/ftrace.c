/*
 * arch/arm64/kernel/ftrace.c
 *
 * Copyright (C) 2013 Linaro Limited
 * Author: AKASHI Takahiro <takahiro.akashi@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/ftrace.h>
#include <linux/swab.h>
#include <linux/uaccess.h>

#include <asm/cacheflush.h>
#include <asm/ftrace.h>
#include <asm/insn.h>

#ifdef CONFIG_DYNAMIC_FTRACE
/*
                                                              
                                                                         
 */
static int ftrace_modify_code(unsigned long pc, u32 old, u32 new,
			      bool validate)
{
	u32 replaced;

	/*
         
                                                             
                                                                 
                                                                  
   
                                                              
                           
  */
	if (validate) {
		if (aarch64_insn_read((void *)pc, &replaced))
			return -EFAULT;

		if (replaced != old)
			return -EINVAL;
	}
	if (aarch64_insn_patch_text_nosync((void *)pc, new))
		return -EPERM;

	return 0;
}

/*
                                             
 */
int ftrace_update_ftrace_func(ftrace_func_t func)
{
	unsigned long pc;
	u32 new;

	pc = (unsigned long)&ftrace_call;
	new = aarch64_insn_gen_branch_imm(pc, (unsigned long)func, true);

	return ftrace_modify_code(pc, 0, new, false);
}

/*
                                                               
 */
int ftrace_make_call(struct dyn_ftrace *rec, unsigned long addr)
{
	unsigned long pc = rec->ip;
	u32 old, new;

	old = aarch64_insn_gen_nop();
	new = aarch64_insn_gen_branch_imm(pc, addr, true);

	return ftrace_modify_code(pc, old, new, true);
}

/*
                                                                
 */
int ftrace_make_nop(struct module *mod, struct dyn_ftrace *rec,
		    unsigned long addr)
{
	unsigned long pc = rec->ip;
	u32 old, new;

	old = aarch64_insn_gen_branch_imm(pc, addr, true);
	new = aarch64_insn_gen_nop();

	return ftrace_modify_code(pc, old, new, true);
}

int __init ftrace_dyn_arch_init(void)
{
	return 0;
}
#endif /*                       */

#ifdef CONFIG_FUNCTION_GRAPH_TRACER
/*
                                                                        
                                                                       
                                                                         
                                       
  
                                                                
 */
void prepare_ftrace_return(unsigned long *parent, unsigned long self_addr,
			   unsigned long frame_pointer)
{
	unsigned long return_hooker = (unsigned long)&return_to_handler;
	unsigned long old;
	struct ftrace_graph_ent trace;
	int err;

	if (unlikely(atomic_read(&current->tracing_graph_pause)))
		return;

	/*
         
                                                                
                                             
  */
	old = *parent;
	*parent = return_hooker;

	trace.func = self_addr;
	trace.depth = current->curr_ret_stack + 1;

	/*                                               */
	if (!ftrace_graph_entry(&trace)) {
		*parent = old;
		return;
	}

	err = ftrace_push_return_trace(old, self_addr, &trace.depth,
				       frame_pointer);
	if (err == -EBUSY) {
		*parent = old;
		return;
	}
}

#ifdef CONFIG_DYNAMIC_FTRACE
/*
                                                                   
                        
 */
static int ftrace_modify_graph_caller(bool enable)
{
	unsigned long pc = (unsigned long)&ftrace_graph_call;
	u32 branch, nop;

	branch = aarch64_insn_gen_branch_imm(pc,
			(unsigned long)ftrace_graph_caller, false);
	nop = aarch64_insn_gen_nop();

	if (enable)
		return ftrace_modify_code(pc, nop, branch, true);
	else
		return ftrace_modify_code(pc, branch, nop, true);
}

int ftrace_enable_ftrace_graph_caller(void)
{
	return ftrace_modify_graph_caller(true);
}

int ftrace_disable_ftrace_graph_caller(void)
{
	return ftrace_modify_graph_caller(false);
}
#endif /*                       */
#endif /*                              */
