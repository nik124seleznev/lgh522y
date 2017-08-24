/*
 * User-space Probes (UProbes) for powerpc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright IBM Corporation, 2007-2012
 *
 * Adapted from the x86 port by Ananth N Mavinakayanahalli <ananth@in.ibm.com>
 */
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/uprobes.h>
#include <linux/uaccess.h>
#include <linux/kdebug.h>

#include <asm/sstep.h>

#define UPROBE_TRAP_NR	UINT_MAX

/* 
                                                            
                                    
                                           
 */
bool is_trap_insn(uprobe_opcode_t *insn)
{
	return (is_trap(*insn));
}

/* 
                           
                                 
                                            
                         
                                                
 */
int arch_uprobe_analyze_insn(struct arch_uprobe *auprobe,
		struct mm_struct *mm, unsigned long addr)
{
	if (addr & 0x03)
		return -EINVAL;

	return 0;
}

/*
                                                        
                                        
                                                        
 */
int arch_uprobe_pre_xol(struct arch_uprobe *auprobe, struct pt_regs *regs)
{
	struct arch_uprobe_task *autask = &current->utask->autask;

	autask->saved_trap_nr = current->thread.trap_nr;
	current->thread.trap_nr = UPROBE_TRAP_NR;
	regs->nip = current->utask->xol_vaddr;

	user_enable_single_step(current);
	return 0;
}

/* 
                                                                      
                                                                            
               
                                                    
 */
unsigned long uprobe_get_swbp_addr(struct pt_regs *regs)
{
	return instruction_pointer(regs);
}

/*
                                                                        
                                                                           
                                                                          
                                   
  
                                                                        
                                                                             
                                                           
 */
bool arch_uprobe_xol_was_trapped(struct task_struct *t)
{
	if (t->thread.trap_nr != UPROBE_TRAP_NR)
		return true;

	return false;
}

/*
                                                                   
                                                            
                                                            
  
                                                                    
 */
int arch_uprobe_post_xol(struct arch_uprobe *auprobe, struct pt_regs *regs)
{
	struct uprobe_task *utask = current->utask;

	WARN_ON_ONCE(current->thread.trap_nr != UPROBE_TRAP_NR);

	current->thread.trap_nr = utask->autask.saved_trap_nr;

	/*
                                                              
                                                                  
                                                                 
                                                                 
                   
  */
	regs->nip = utask->vaddr + MAX_UINSN_BYTES;

	user_disable_single_step(current);
	return 0;
}

/*                                           */
int arch_uprobe_exception_notify(struct notifier_block *self,
				unsigned long val, void *data)
{
	struct die_args *args = data;
	struct pt_regs *regs = args->regs;

	/*                              */
	if (WARN_ON(!regs))
		return NOTIFY_DONE;

	/*                                           */
	if (!user_mode(regs))
		return NOTIFY_DONE;

	switch (val) {
	case DIE_BPT:
		if (uprobe_pre_sstep_notifier(regs))
			return NOTIFY_STOP;
		break;
	case DIE_SSTEP:
		if (uprobe_post_sstep_notifier(regs))
			return NOTIFY_STOP;
	default:
		break;
	}
	return NOTIFY_DONE;
}

/*
                                                                        
                                                                         
                  
 */
void arch_uprobe_abort_xol(struct arch_uprobe *auprobe, struct pt_regs *regs)
{
	struct uprobe_task *utask = current->utask;

	current->thread.trap_nr = utask->autask.saved_trap_nr;
	instruction_pointer_set(regs, utask->vaddr);

	user_disable_single_step(current);
}

/*
                                          
                                                             
 */
bool arch_uprobe_skip_sstep(struct arch_uprobe *auprobe, struct pt_regs *regs)
{
	int ret;

	/*
                                                                   
                                                            
  */
	ret = emulate_step(regs, auprobe->ainsn);
	if (ret > 0)
		return true;

	return false;
}

unsigned long
arch_uretprobe_hijack_return_addr(unsigned long trampoline_vaddr, struct pt_regs *regs)
{
	unsigned long orig_ret_vaddr;

	orig_ret_vaddr = regs->link;

	/*                                              */
	regs->link = trampoline_vaddr;

	return orig_ret_vaddr;
}
