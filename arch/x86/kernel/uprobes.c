/*
 * User-space Probes (UProbes) for x86
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
 * Copyright (C) IBM Corporation, 2008-2011
 * Authors:
 *	Srikar Dronamraju
 *	Jim Keniston
 */
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/uprobes.h>
#include <linux/uaccess.h>

#include <linux/kdebug.h>
#include <asm/processor.h>
#include <asm/insn.h>

/*                        */

/*                 */
#define UPROBE_FIX_NONE		0x0

/*                                           */
#define UPROBE_FIX_IP		0x1

/*                                          */
#define UPROBE_FIX_CALL	0x2

/*                                             */
#define UPROBE_FIX_SETF	0x4

#define UPROBE_FIX_RIP_AX	0x8000
#define UPROBE_FIX_RIP_CX	0x4000

#define	UPROBE_TRAP_NR		UINT_MAX

/*                                           */
#define OPCODE1(insn)		((insn)->opcode.bytes[0])
#define OPCODE2(insn)		((insn)->opcode.bytes[1])
#define OPCODE3(insn)		((insn)->opcode.bytes[2])
#define MODRM_REG(insn)		X86_MODRM_REG(insn->modrm.value)

#define W(row, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, ba, bb, bc, bd, be, bf)\
	(((b0##UL << 0x0)|(b1##UL << 0x1)|(b2##UL << 0x2)|(b3##UL << 0x3) |   \
	  (b4##UL << 0x4)|(b5##UL << 0x5)|(b6##UL << 0x6)|(b7##UL << 0x7) |   \
	  (b8##UL << 0x8)|(b9##UL << 0x9)|(ba##UL << 0xa)|(bb##UL << 0xb) |   \
	  (bc##UL << 0xc)|(bd##UL << 0xd)|(be##UL << 0xe)|(bf##UL << 0xf))    \
	 << (row % 32))

/*
                                                                           
                                                                            
                                                                
 */
static volatile u32 good_insns_32[256 / 32] = {
	/*                                                             */
	/*                                                             */
	W(0x00, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0) | /*    */
	W(0x10, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0) , /*    */
	W(0x20, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1) | /*    */
	W(0x30, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1) , /*    */
	W(0x40, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0x50, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0x60, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0) | /*    */
	W(0x70, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0x80, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0x90, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0xa0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0xb0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0xc0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0) | /*    */
	W(0xd0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0xe0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0) | /*    */
	W(0xf0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1)   /*    */
	/*                                                             */
	/*                                                             */
};

/*                                            */
static volatile u32 good_2byte_insns[256 / 32] = {
	/*                                                             */
	/*                                                             */
	W(0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1) | /*    */
	W(0x10, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0x20, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0x30, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) , /*    */
	W(0x40, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0x50, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0x60, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0x70, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1) , /*    */
	W(0x80, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0x90, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0xa0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1) | /*    */
	W(0xb0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0xc0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0xd0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0xe0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0xf0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)   /*    */
	/*                                                             */
	/*                                                             */
};

#ifdef CONFIG_X86_64
/*                                         */
static volatile u32 good_insns_64[256 / 32] = {
	/*                                                             */
	/*                                                             */
	W(0x00, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0) | /*    */
	W(0x10, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0) , /*    */
	W(0x20, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0) | /*    */
	W(0x30, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0) , /*    */
	W(0x40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) | /*    */
	W(0x50, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0x60, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0) | /*    */
	W(0x70, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0x80, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0x90, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0xa0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) | /*    */
	W(0xb0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0xc0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0) | /*    */
	W(0xd0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1) , /*    */
	W(0xe0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0) | /*    */
	W(0xf0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1)   /*    */
	/*                                                             */
	/*                                                             */
};
#endif
#undef W

/*
                                        
  
                            
                             
                      
             
                            
                   
            
                     
                                                                              
  
                                  
  
                                                           
                                                          
  
                                             
  
                                                                          
                                                                         
                                                                       
                      
  
                                     
                                         
                                                 
                                                                     
                                                       
  
                                          
                                       
  
                                         
  
                                                
                                       
                                                           
                                                                  
                      
             
                    
 */

/*
        
                                                                              
                                                
 */

static bool is_prefix_bad(struct insn *insn)
{
	int i;

	for (i = 0; i < insn->prefixes.nbytes; i++) {
		switch (insn->prefixes.bytes[i]) {
		case 0x26:	/*               */
		case 0x2E:	/*               */
		case 0x36:	/*               */
		case 0x3E:	/*               */
		case 0xF0:	/*               */
			return true;
		}
	}
	return false;
}

static int validate_insn_32bits(struct arch_uprobe *auprobe, struct insn *insn)
{
	insn_init(insn, auprobe->insn, false);

	/*                                                    */
	insn_get_opcode(insn);
	if (is_prefix_bad(insn))
		return -ENOTSUPP;

	if (test_bit(OPCODE1(insn), (unsigned long *)good_insns_32))
		return 0;

	if (insn->opcode.nbytes == 2) {
		if (test_bit(OPCODE2(insn), (unsigned long *)good_2byte_insns))
			return 0;
	}

	return -ENOTSUPP;
}

/*
                                                                           
                                                            
                                                                        
 */
static void prepare_fixups(struct arch_uprobe *auprobe, struct insn *insn)
{
	bool fix_ip = true, fix_call = false;	/*          */
	int reg;

	insn_get_opcode(insn);	/*                 */

	switch (OPCODE1(insn)) {
	case 0x9d:
		/*      */
		auprobe->fixups |= UPROBE_FIX_SETF;
		break;
	case 0xc3:		/*          */
	case 0xcb:
	case 0xc2:
	case 0xca:
		/*               */
		fix_ip = false;
		break;
	case 0xe8:		/*                                 */
		fix_call = true;
		break;
	case 0x9a:		/*                                         */
		fix_call = true;
		fix_ip = false;
		break;
	case 0xff:
		insn_get_modrm(insn);
		reg = MODRM_REG(insn);
		if (reg == 2 || reg == 3) {
			/*                         */
			/*                                 */
			fix_call = true;
			fix_ip = false;
		} else if (reg == 4 || reg == 5) {
			/*                       */
			/*                */
			fix_ip = false;
		}
		break;
	case 0xea:		/*                               */
		fix_ip = false;
		break;
	default:
		break;
	}
	if (fix_ip)
		auprobe->fixups |= UPROBE_FIX_IP;
	if (fix_call)
		auprobe->fixups |= UPROBE_FIX_CALL;
}

#ifdef CONFIG_X86_64
/*
                                                                   
                                                                       
                                                                 
                                                               
                                                                    
                                                               
              
  
                                                                  
                                                                     
                                                                   
                                                                  
                                                                    
              
  
                                                     
  
                                  
                               
                                         
 */
static void
handle_riprel_insn(struct arch_uprobe *auprobe, struct mm_struct *mm, struct insn *insn)
{
	u8 *cursor;
	u8 reg;

	if (mm->context.ia32_compat)
		return;

	auprobe->rip_rela_target_address = 0x0;
	if (!insn_rip_relative(insn))
		return;

	/*
                                                             
                                                  
                                         
  */
	if (insn->rex_prefix.nbytes) {
		cursor = auprobe->insn + insn_offset_rex_prefix(insn);
		*cursor &= 0xfe;	/*                    */
	}

	/*
                                                             
                                                                  
                             
  */
	cursor = auprobe->insn + insn_offset_modrm(insn);
	insn_get_length(insn);

	/*
                                                               
                                                                 
                                                                  
  */
	reg = MODRM_REG(insn);
	if (reg == 0) {
		/*
                                                           
                                                        
                                                          
                                                          
                                  
   */
		auprobe->fixups = UPROBE_FIX_RIP_CX;
		/*                                             */
		*cursor = 0x1;
	} else {
		/*                                                  */
		auprobe->fixups = UPROBE_FIX_RIP_AX;
		/*                                            */
		*cursor = (reg << 3);
	}

	/*                                                                */
	auprobe->rip_rela_target_address = (long)insn->length + insn->displacement.value;

	/*                                                                  */
	if (insn->immediate.nbytes) {
		cursor++;
		memmove(cursor, cursor + insn->displacement.nbytes, insn->immediate.nbytes);
	}
	return;
}

static int validate_insn_64bits(struct arch_uprobe *auprobe, struct insn *insn)
{
	insn_init(insn, auprobe->insn, true);

	/*                                                    */
	insn_get_opcode(insn);
	if (is_prefix_bad(insn))
		return -ENOTSUPP;

	if (test_bit(OPCODE1(insn), (unsigned long *)good_insns_64))
		return 0;

	if (insn->opcode.nbytes == 2) {
		if (test_bit(OPCODE2(insn), (unsigned long *)good_2byte_insns))
			return 0;
	}
	return -ENOTSUPP;
}

static int validate_insn_bits(struct arch_uprobe *auprobe, struct mm_struct *mm, struct insn *insn)
{
	if (mm->context.ia32_compat)
		return validate_insn_32bits(auprobe, insn);
	return validate_insn_64bits(auprobe, insn);
}
#else /*         */
static void handle_riprel_insn(struct arch_uprobe *auprobe, struct mm_struct *mm, struct insn *insn)
{
	/*                                      */
}

static int validate_insn_bits(struct arch_uprobe *auprobe, struct mm_struct *mm,  struct insn *insn)
{
	return validate_insn_32bits(auprobe, insn);
}
#endif /*               */

/* 
                                                                                 
                                 
                                            
                                                            
                                                
 */
int arch_uprobe_analyze_insn(struct arch_uprobe *auprobe, struct mm_struct *mm, unsigned long addr)
{
	int ret;
	struct insn insn;

	auprobe->fixups = 0;
	ret = validate_insn_bits(auprobe, mm, &insn);
	if (ret != 0)
		return ret;

	handle_riprel_insn(auprobe, mm, &insn);
	prepare_fixups(auprobe, &insn);

	return 0;
}

#ifdef CONFIG_X86_64
/*
                                                                   
                                                                         
 */
static void
pre_xol_rip_insn(struct arch_uprobe *auprobe, struct pt_regs *regs,
				struct arch_uprobe_task *autask)
{
	if (auprobe->fixups & UPROBE_FIX_RIP_AX) {
		autask->saved_scratch_register = regs->ax;
		regs->ax = current->utask->vaddr;
		regs->ax += auprobe->rip_rela_target_address;
	} else if (auprobe->fixups & UPROBE_FIX_RIP_CX) {
		autask->saved_scratch_register = regs->cx;
		regs->cx = current->utask->vaddr;
		regs->cx += auprobe->rip_rela_target_address;
	}
}
#else
static void
pre_xol_rip_insn(struct arch_uprobe *auprobe, struct pt_regs *regs,
				struct arch_uprobe_task *autask)
{
	/*                                      */
}
#endif

/*
                                                        
                                        
                                                        
 */
int arch_uprobe_pre_xol(struct arch_uprobe *auprobe, struct pt_regs *regs)
{
	struct arch_uprobe_task *autask;

	autask = &current->utask->autask;
	autask->saved_trap_nr = current->thread.trap_nr;
	current->thread.trap_nr = UPROBE_TRAP_NR;
	regs->ip = current->utask->xol_vaddr;
	pre_xol_rip_insn(auprobe, regs, autask);

	autask->saved_tf = !!(regs->flags & X86_EFLAGS_TF);
	regs->flags |= X86_EFLAGS_TF;
	if (test_tsk_thread_flag(current, TIF_BLOCKSTEP))
		set_task_blockstep(current, false);

	return 0;
}

/*
                                                                         
                                                             
 */
static int adjust_ret_addr(unsigned long sp, long correction)
{
	int rasize, ncopied;
	long ra = 0;

	if (is_ia32_task())
		rasize = 4;
	else
		rasize = 8;

	ncopied = copy_from_user(&ra, (void __user *)sp, rasize);
	if (unlikely(ncopied))
		return -EFAULT;

	ra += correction;
	ncopied = copy_to_user((void __user *)sp, &ra, rasize);
	if (unlikely(ncopied))
		return -EFAULT;

	return 0;
}

#ifdef CONFIG_X86_64
static bool is_riprel_insn(struct arch_uprobe *auprobe)
{
	return ((auprobe->fixups & (UPROBE_FIX_RIP_AX | UPROBE_FIX_RIP_CX)) != 0);
}

static void
handle_riprel_post_xol(struct arch_uprobe *auprobe, struct pt_regs *regs, long *correction)
{
	if (is_riprel_insn(auprobe)) {
		struct arch_uprobe_task *autask;

		autask = &current->utask->autask;
		if (auprobe->fixups & UPROBE_FIX_RIP_AX)
			regs->ax = autask->saved_scratch_register;
		else
			regs->cx = autask->saved_scratch_register;

		/*
                                                             
                                                           
                                                            
                        
   */
		if (correction)
			*correction += 4;
	}
}
#else
static void
handle_riprel_post_xol(struct arch_uprobe *auprobe, struct pt_regs *regs, long *correction)
{
	/*                                      */
}
#endif

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
	struct uprobe_task *utask;
	long correction;
	int result = 0;

	WARN_ON_ONCE(current->thread.trap_nr != UPROBE_TRAP_NR);

	utask = current->utask;
	current->thread.trap_nr = utask->autask.saved_trap_nr;
	correction = (long)(utask->vaddr - utask->xol_vaddr);
	handle_riprel_post_xol(auprobe, regs, &correction);
	if (auprobe->fixups & UPROBE_FIX_IP)
		regs->ip += correction;

	if (auprobe->fixups & UPROBE_FIX_CALL)
		result = adjust_ret_addr(regs->sp, correction);

	/*
                                                                 
                                                                 
                                           
  */
	if (utask->autask.saved_tf)
		send_sig(SIGTRAP, current, 0);
	else if (!(auprobe->fixups & UPROBE_FIX_SETF))
		regs->flags &= ~X86_EFLAGS_TF;

	return result;
}

/*                                           */
int arch_uprobe_exception_notify(struct notifier_block *self, unsigned long val, void *data)
{
	struct die_args *args = data;
	struct pt_regs *regs = args->regs;
	int ret = NOTIFY_DONE;

	/*                                           */
	if (regs && !user_mode_vm(regs))
		return NOTIFY_DONE;

	switch (val) {
	case DIE_INT3:
		if (uprobe_pre_sstep_notifier(regs))
			ret = NOTIFY_STOP;

		break;

	case DIE_DEBUG:
		if (uprobe_post_sstep_notifier(regs))
			ret = NOTIFY_STOP;

	default:
		break;
	}

	return ret;
}

/*
                                                                        
                                                                         
                  
 */
void arch_uprobe_abort_xol(struct arch_uprobe *auprobe, struct pt_regs *regs)
{
	struct uprobe_task *utask = current->utask;

	current->thread.trap_nr = utask->autask.saved_trap_nr;
	handle_riprel_post_xol(auprobe, regs, NULL);
	instruction_pointer_set(regs, utask->vaddr);

	/*                                                       */
	if (!utask->autask.saved_tf)
		regs->flags &= ~X86_EFLAGS_TF;
}

/*
                                                              
                      
 */
static bool __skip_sstep(struct arch_uprobe *auprobe, struct pt_regs *regs)
{
	int i;

	for (i = 0; i < MAX_UINSN_BYTES; i++) {
		if (auprobe->insn[i] == 0x66)
			continue;

		if (auprobe->insn[i] == 0x90) {
			regs->ip += i + 1;
			return true;
		}

		break;
	}
	return false;
}

bool arch_uprobe_skip_sstep(struct arch_uprobe *auprobe, struct pt_regs *regs)
{
	bool ret = __skip_sstep(auprobe, regs);
	if (ret && (regs->flags & X86_EFLAGS_TF))
		send_sig(SIGTRAP, current, 0);
	return ret;
}

unsigned long
arch_uretprobe_hijack_return_addr(unsigned long trampoline_vaddr, struct pt_regs *regs)
{
	int rasize, ncopied;
	unsigned long orig_ret_vaddr = 0; /*                                 */

	rasize = is_ia32_task() ? 4 : 8;
	ncopied = copy_from_user(&orig_ret_vaddr, (void __user *)regs->sp, rasize);
	if (unlikely(ncopied))
		return -1;

	/*                                                 */
	if (orig_ret_vaddr == trampoline_vaddr)
		return orig_ret_vaddr;

	ncopied = copy_to_user((void __user *)regs->sp, &trampoline_vaddr, rasize);
	if (likely(!ncopied))
		return orig_ret_vaddr;

	if (ncopied != rasize) {
		pr_err("uprobe: return address clobbered: pid=%d, %%sp=%#lx, "
			"%%ip=%#lx\n", current->pid, regs->sp, regs->ip);

		force_sig_info(SIGSEGV, SEND_SIG_FORCED, current);
	}

	return -1;
}
