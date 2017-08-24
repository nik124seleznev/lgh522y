/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/kprobes.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kdebug.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/cacheflush.h>
#include <asm/current.h>
#include <asm/disasm.h>

#define MIN_STACK_SIZE(addr)	min((unsigned long)MAX_STACK_SIZE, \
		(unsigned long)current_thread_info() + THREAD_SIZE - (addr))

DEFINE_PER_CPU(struct kprobe *, current_kprobe) = NULL;
DEFINE_PER_CPU(struct kprobe_ctlblk, kprobe_ctlblk);

int __kprobes arch_prepare_kprobe(struct kprobe *p)
{
	/*                                       */
	if ((unsigned long)p->addr & 0x01)
		return -EINVAL;

	/*                                                  */

	p->ainsn.is_short = is_short_instr((unsigned long)p->addr);
	p->opcode = *p->addr;

	return 0;
}

void __kprobes arch_arm_kprobe(struct kprobe *p)
{
	*p->addr = UNIMP_S_INSTRUCTION;

	flush_icache_range((unsigned long)p->addr,
			   (unsigned long)p->addr + sizeof(kprobe_opcode_t));
}

void __kprobes arch_disarm_kprobe(struct kprobe *p)
{
	*p->addr = p->opcode;

	flush_icache_range((unsigned long)p->addr,
			   (unsigned long)p->addr + sizeof(kprobe_opcode_t));
}

void __kprobes arch_remove_kprobe(struct kprobe *p)
{
	arch_disarm_kprobe(p);

	/*                                                            */
	if (p->ainsn.t1_addr) {
		*(p->ainsn.t1_addr) = p->ainsn.t1_opcode;

		flush_icache_range((unsigned long)p->ainsn.t1_addr,
				   (unsigned long)p->ainsn.t1_addr +
				   sizeof(kprobe_opcode_t));

		p->ainsn.t1_addr = NULL;
	}

	if (p->ainsn.t2_addr) {
		*(p->ainsn.t2_addr) = p->ainsn.t2_opcode;

		flush_icache_range((unsigned long)p->ainsn.t2_addr,
				   (unsigned long)p->ainsn.t2_addr +
				   sizeof(kprobe_opcode_t));

		p->ainsn.t2_addr = NULL;
	}
}

static void __kprobes save_previous_kprobe(struct kprobe_ctlblk *kcb)
{
	kcb->prev_kprobe.kp = kprobe_running();
	kcb->prev_kprobe.status = kcb->kprobe_status;
}

static void __kprobes restore_previous_kprobe(struct kprobe_ctlblk *kcb)
{
	__get_cpu_var(current_kprobe) = kcb->prev_kprobe.kp;
	kcb->kprobe_status = kcb->prev_kprobe.status;
}

static inline void __kprobes set_current_kprobe(struct kprobe *p)
{
	__get_cpu_var(current_kprobe) = p;
}

static void __kprobes resume_execution(struct kprobe *p, unsigned long addr,
				       struct pt_regs *regs)
{
	/*                                                          
                                     
  */
	if (p->ainsn.t1_addr) {
		*(p->ainsn.t1_addr) = p->ainsn.t1_opcode;

		flush_icache_range((unsigned long)p->ainsn.t1_addr,
				   (unsigned long)p->ainsn.t1_addr +
				   sizeof(kprobe_opcode_t));

		p->ainsn.t1_addr = NULL;
	}

	if (p->ainsn.t2_addr) {
		*(p->ainsn.t2_addr) = p->ainsn.t2_opcode;

		flush_icache_range((unsigned long)p->ainsn.t2_addr,
				   (unsigned long)p->ainsn.t2_addr +
				   sizeof(kprobe_opcode_t));

		p->ainsn.t2_addr = NULL;
	}

	return;
}

static void __kprobes setup_singlestep(struct kprobe *p, struct pt_regs *regs)
{
	unsigned long next_pc;
	unsigned long tgt_if_br = 0;
	int is_branch;
	unsigned long bta;

	/*                                                            
                                                                    
                                         
  */
	*(p->addr) = p->opcode;

	flush_icache_range((unsigned long)p->addr,
			   (unsigned long)p->addr + sizeof(kprobe_opcode_t));

	/*                                                                      
                                                                        
           
  */

	bta = regs->bta;

	if (regs->status32 & 0x40) {
		/*                                              */

		next_pc = bta & ~0x01;

		if (!p->ainsn.is_short) {
			if (bta & 0x01)
				regs->blink += 2;
			else {
				/*                  */
				next_pc += 2;

				/*                                              
                             
     */
				regs->bta += 2;
			}
		}

		is_branch = 0;
	} else
		is_branch =
		    disasm_next_pc((unsigned long)p->addr, regs,
			(struct callee_regs *) current->thread.callee_reg,
			&next_pc, &tgt_if_br);

	p->ainsn.t1_addr = (kprobe_opcode_t *) next_pc;
	p->ainsn.t1_opcode = *(p->ainsn.t1_addr);
	*(p->ainsn.t1_addr) = TRAP_S_2_INSTRUCTION;

	flush_icache_range((unsigned long)p->ainsn.t1_addr,
			   (unsigned long)p->ainsn.t1_addr +
			   sizeof(kprobe_opcode_t));

	if (is_branch) {
		p->ainsn.t2_addr = (kprobe_opcode_t *) tgt_if_br;
		p->ainsn.t2_opcode = *(p->ainsn.t2_addr);
		*(p->ainsn.t2_addr) = TRAP_S_2_INSTRUCTION;

		flush_icache_range((unsigned long)p->ainsn.t2_addr,
				   (unsigned long)p->ainsn.t2_addr +
				   sizeof(kprobe_opcode_t));
	}
}

int __kprobes arc_kprobe_handler(unsigned long addr, struct pt_regs *regs)
{
	struct kprobe *p;
	struct kprobe_ctlblk *kcb;

	preempt_disable();

	kcb = get_kprobe_ctlblk();
	p = get_kprobe((unsigned long *)addr);

	if (p) {
		/*
                                                               
                                                           
                                                                
                                                         
             
   */
		if (kprobe_running()) {
			save_previous_kprobe(kcb);
			set_current_kprobe(p);
			kprobes_inc_nmissed_count(p);
			setup_singlestep(p, regs);
			kcb->kprobe_status = KPROBE_REENTER;
			return 1;
		}

		set_current_kprobe(p);
		kcb->kprobe_status = KPROBE_HIT_ACTIVE;

		/*                                                             
                                                                
                                                             
                                                                
                                                        
                  
   */
		if (!p->pre_handler || !p->pre_handler(p, regs)) {
			setup_singlestep(p, regs);
			kcb->kprobe_status = KPROBE_HIT_SS;
		}

		return 1;
	} else if (kprobe_running()) {
		p = __get_cpu_var(current_kprobe);
		if (p->break_handler && p->break_handler(p, regs)) {
			setup_singlestep(p, regs);
			kcb->kprobe_status = KPROBE_HIT_SS;
			return 1;
		}
	}

	/*            */
	preempt_enable_no_resched();
	return 0;
}

static int __kprobes arc_post_kprobe_handler(unsigned long addr,
					 struct pt_regs *regs)
{
	struct kprobe *cur = kprobe_running();
	struct kprobe_ctlblk *kcb = get_kprobe_ctlblk();

	if (!cur)
		return 0;

	resume_execution(cur, addr, regs);

	/*                  */
	arch_arm_kprobe(cur);

	/*
                                                                      
                                                                     
                                             
  */
	regs->ret = addr;

	if ((kcb->kprobe_status != KPROBE_REENTER) && cur->post_handler) {
		kcb->kprobe_status = KPROBE_HIT_SSDONE;
		cur->post_handler(cur, regs, 0);
	}

	if (kcb->kprobe_status == KPROBE_REENTER) {
		restore_previous_kprobe(kcb);
		goto out;
	}

	reset_current_kprobe();

out:
	preempt_enable_no_resched();
	return 1;
}

/*
                                                                   
                                   
                                                                          
                                                     
 */

int __kprobes kprobe_fault_handler(struct pt_regs *regs, unsigned long trapnr)
{
	struct kprobe *cur = kprobe_running();
	struct kprobe_ctlblk *kcb = get_kprobe_ctlblk();

	switch (kcb->kprobe_status) {
	case KPROBE_HIT_SS:
	case KPROBE_REENTER:
		/*
                                                             
                                                                
                                                            
                                                             
   */
		resume_execution(cur, (unsigned long)cur->addr, regs);

		if (kcb->kprobe_status == KPROBE_REENTER)
			restore_previous_kprobe(kcb);
		else
			reset_current_kprobe();

		preempt_enable_no_resched();
		break;

	case KPROBE_HIT_ACTIVE:
	case KPROBE_HIT_SSDONE:
		/*
                                                                 
                      
   */

		/*                                               
                                                        
                                
   */
		kprobes_inc_nmissed_count(cur);

		/*
                                                      
                                                     
                                             
                                              
                                                
   */
		if (cur->fault_handler && cur->fault_handler(cur, regs, trapnr))
			return 1;

		/*
                                                            
                   
   */
		if (fixup_exception(regs))
			return 1;

		/*
                                           
                                
   */
		break;

	default:
		break;
	}
	return 0;
}

int __kprobes kprobe_exceptions_notify(struct notifier_block *self,
				       unsigned long val, void *data)
{
	struct die_args *args = data;
	unsigned long addr = args->err;
	int ret = NOTIFY_DONE;

	switch (val) {
	case DIE_IERR:
		if (arc_kprobe_handler(addr, args->regs))
			return NOTIFY_STOP;
		break;

	case DIE_TRAP:
		if (arc_post_kprobe_handler(addr, args->regs))
			return NOTIFY_STOP;
		break;

	default:
		break;
	}

	return ret;
}

int __kprobes setjmp_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
	struct jprobe *jp = container_of(p, struct jprobe, kp);
	struct kprobe_ctlblk *kcb = get_kprobe_ctlblk();
	unsigned long sp_addr = regs->sp;

	kcb->jprobe_saved_regs = *regs;
	memcpy(kcb->jprobes_stack, (void *)sp_addr, MIN_STACK_SIZE(sp_addr));
	regs->ret = (unsigned long)(jp->entry);

	return 1;
}

void __kprobes jprobe_return(void)
{
	__asm__ __volatile__("unimp_s");
	return;
}

int __kprobes longjmp_break_handler(struct kprobe *p, struct pt_regs *regs)
{
	struct kprobe_ctlblk *kcb = get_kprobe_ctlblk();
	unsigned long sp_addr;

	*regs = kcb->jprobe_saved_regs;
	sp_addr = regs->sp;
	memcpy((void *)sp_addr, kcb->jprobes_stack, MIN_STACK_SIZE(sp_addr));
	preempt_enable_no_resched();

	return 1;
}

static void __used kretprobe_trampoline_holder(void)
{
	__asm__ __volatile__(".global kretprobe_trampoline\n"
			     "kretprobe_trampoline:\n" "nop\n");
}

void __kprobes arch_prepare_kretprobe(struct kretprobe_instance *ri,
				      struct pt_regs *regs)
{

	ri->ret_addr = (kprobe_opcode_t *) regs->blink;

	/*                                              */
	regs->blink = (unsigned long)&kretprobe_trampoline;
}

static int __kprobes trampoline_probe_handler(struct kprobe *p,
					      struct pt_regs *regs)
{
	struct kretprobe_instance *ri = NULL;
	struct hlist_head *head, empty_rp;
	struct hlist_node *tmp;
	unsigned long flags, orig_ret_address = 0;
	unsigned long trampoline_address = (unsigned long)&kretprobe_trampoline;

	INIT_HLIST_HEAD(&empty_rp);
	kretprobe_hash_lock(current, &head, &flags);

	/*
                                                                     
                                                              
                                                                      
                                                      
   
                               
                                                               
                                                                 
                                                                   
                                                             
                              
  */
	hlist_for_each_entry_safe(ri, tmp, head, hlist) {
		if (ri->task != current)
			/*                                         */
			continue;

		if (ri->rp && ri->rp->handler)
			ri->rp->handler(ri, regs);

		orig_ret_address = (unsigned long)ri->ret_addr;
		recycle_rp_inst(ri, &empty_rp);

		if (orig_ret_address != trampoline_address) {
			/*
                                                
                                                 
                                          
    */
			break;
		}
	}

	kretprobe_assert(ri, orig_ret_address, trampoline_address);
	regs->ret = orig_ret_address;

	reset_current_kprobe();
	kretprobe_hash_unlock(current, &flags);
	preempt_enable_no_resched();

	hlist_for_each_entry_safe(ri, tmp, &empty_rp, hlist) {
		hlist_del(&ri->hlist);
		kfree(ri);
	}

	/*                                                                 
                                              
  */
	return 1;
}

static struct kprobe trampoline_p = {
	.addr = (kprobe_opcode_t *) &kretprobe_trampoline,
	.pre_handler = trampoline_probe_handler
};

int __init arch_init_kprobes(void)
{
	/*                                                    */
	return register_kprobe(&trampoline_p);
}

int __kprobes arch_trampoline_kprobe(struct kprobe *p)
{
	if (p->addr == (kprobe_opcode_t *) &kretprobe_trampoline)
		return 1;

	return 0;
}

void trap_is_kprobe(unsigned long cause, unsigned long address,
		    struct pt_regs *regs)
{
	notify_die(DIE_TRAP, "kprobe_trap", regs, address, cause, SIGTRAP);
}
