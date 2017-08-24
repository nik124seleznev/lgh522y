/*
 * Kernel traps/events for Hexagon processor
 *
 * Copyright (c) 2010-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/kdebug.h>
#include <linux/syscalls.h>
#include <linux/signal.h>
#include <linux/tracehook.h>
#include <asm/traps.h>
#include <asm/vm_fault.h>
#include <asm/syscall.h>
#include <asm/registers.h>
#include <asm/unistd.h>
#include <asm/sections.h>
#ifdef CONFIG_KGDB
# include <linux/kgdb.h>
#endif

#define TRAP_SYSCALL	1
#define TRAP_DEBUG	0xdb

void __init trap_init(void)
{
}

#ifdef CONFIG_GENERIC_BUG
/*                                                 */
int is_valid_bugaddr(unsigned long addr)
{
	return 1;
}
#endif /*                    */

static const char *ex_name(int ex)
{
	switch (ex) {
	case HVM_GE_C_XPROT:
	case HVM_GE_C_XUSER:
		return "Execute protection fault";
	case HVM_GE_C_RPROT:
	case HVM_GE_C_RUSER:
		return "Read protection fault";
	case HVM_GE_C_WPROT:
	case HVM_GE_C_WUSER:
		return "Write protection fault";
	case HVM_GE_C_XMAL:
		return "Misaligned instruction";
	case HVM_GE_C_WREG:
		return "Multiple writes to same register in packet";
	case HVM_GE_C_PCAL:
		return "Program counter values that are not properly aligned";
	case HVM_GE_C_RMAL:
		return "Misaligned data load";
	case HVM_GE_C_WMAL:
		return "Misaligned data store";
	case HVM_GE_C_INVI:
	case HVM_GE_C_PRIVI:
		return "Illegal instruction";
	case HVM_GE_C_BUS:
		return "Precise bus error";
	case HVM_GE_C_CACHE:
		return "Cache error";

	case 0xdb:
		return "Debugger trap";

	default:
		return "Unrecognized exception";
	}
}

static void do_show_stack(struct task_struct *task, unsigned long *fp,
			  unsigned long ip)
{
	int kstack_depth_to_print = 24;
	unsigned long offset, size;
	const char *name = NULL;
	unsigned long *newfp;
	unsigned long low, high;
	char tmpstr[128];
	char *modname;
	int i;

	if (task == NULL)
		task = current;

	printk(KERN_INFO "CPU#%d, %s/%d, Call Trace:\n",
	       raw_smp_processor_id(), task->comm,
	       task_pid_nr(task));

	if (fp == NULL) {
		if (task == current) {
			asm("%0 = r30" : "=r" (fp));
		} else {
			fp = (unsigned long *)
			     ((struct hexagon_switch_stack *)
			     task->thread.switch_sp)->fp;
		}
	}

	if ((((unsigned long) fp) & 0x3) || ((unsigned long) fp < 0x1000)) {
		printk(KERN_INFO "-- Corrupt frame pointer %p\n", fp);
		return;
	}

	/*                                     */
	if (!ip)
		ip = *(fp+1);

	/*                                     */
	low = (unsigned long)task_stack_page(task);
	high = low + THREAD_SIZE - 8;
	low += sizeof(struct thread_info);

	for (i = 0; i < kstack_depth_to_print; i++) {

		name = kallsyms_lookup(ip, &size, &offset, &modname, tmpstr);

		printk(KERN_INFO "[%p] 0x%lx: %s + 0x%lx", fp, ip, name,
			offset);
		if (((unsigned long) fp < low) || (high < (unsigned long) fp))
			printk(KERN_CONT " (FP out of bounds!)");
		if (modname)
			printk(KERN_CONT " [%s] ", modname);
		printk(KERN_CONT "\n");

		newfp = (unsigned long *) *fp;

		if (((unsigned long) newfp) & 0x3) {
			printk(KERN_INFO "-- Corrupt frame pointer %p\n",
				newfp);
			break;
		}

		/*                                     */
		if (0 == newfp) {
			struct pt_regs *regs = (struct pt_regs *) (((void *)fp)
						+ 8);

			if (regs->syscall_nr != -1) {
				printk(KERN_INFO "-- trap0 -- syscall_nr: %ld",
					regs->syscall_nr);
				printk(KERN_CONT "  psp: %lx  elr: %lx\n",
					 pt_psp(regs), pt_elr(regs));
				break;
			} else {
				/*                             */
				kstack_depth_to_print += 6;
				printk(KERN_INFO "-- %s (0x%lx)  badva: %lx\n",
					ex_name(pt_cause(regs)), pt_cause(regs),
					pt_badva(regs));
			}

			newfp = (unsigned long *) regs->r30;
			ip = pt_elr(regs);
		} else {
			ip = *(newfp + 1);
		}

		/*                                   */
		if (ip == 0x0)
			break;

		/*                                               */
		if (newfp > fp)
			fp = newfp;
		else
			break;
	}
}

void show_stack(struct task_struct *task, unsigned long *fp)
{
	/*                                     */
	do_show_stack(task, fp, 0);
}

int die(const char *str, struct pt_regs *regs, long err)
{
	static struct {
		spinlock_t lock;
		int counter;
	} die = {
		.lock = __SPIN_LOCK_UNLOCKED(die.lock),
		.counter = 0
	};

	console_verbose();
	oops_enter();

	spin_lock_irq(&die.lock);
	bust_spinlocks(1);
	printk(KERN_EMERG "Oops: %s[#%d]:\n", str, ++die.counter);

	if (notify_die(DIE_OOPS, str, regs, err, pt_cause(regs), SIGSEGV) ==
	    NOTIFY_STOP)
		return 1;

	print_modules();
	show_regs(regs);
	do_show_stack(current, &regs->r30, pt_elr(regs));

	bust_spinlocks(0);
	add_taint(TAINT_DIE, LOCKDEP_NOW_UNRELIABLE);

	spin_unlock_irq(&die.lock);

	if (in_interrupt())
		panic("Fatal exception in interrupt");

	if (panic_on_oops)
		panic("Fatal exception");

	oops_exit();
	do_exit(err);
	return 0;
}

int die_if_kernel(char *str, struct pt_regs *regs, long err)
{
	if (!user_mode(regs))
		return die(str, regs, err);
	else
		return 0;
}

/*
                                                               
 */
static void misaligned_instruction(struct pt_regs *regs)
{
	die_if_kernel("Misaligned Instruction", regs, 0);
	force_sig(SIGBUS, current);
}

/*
                                                         
                                                           
                                 
 */
static void misaligned_data_load(struct pt_regs *regs)
{
	die_if_kernel("Misaligned Data Load", regs, 0);
	force_sig(SIGBUS, current);
}

static void misaligned_data_store(struct pt_regs *regs)
{
	die_if_kernel("Misaligned Data Store", regs, 0);
	force_sig(SIGBUS, current);
}

static void illegal_instruction(struct pt_regs *regs)
{
	die_if_kernel("Illegal Instruction", regs, 0);
	force_sig(SIGILL, current);
}

/*
                                                        
                                            
 */
static void precise_bus_error(struct pt_regs *regs)
{
	die_if_kernel("Precise Bus Error", regs, 0);
	force_sig(SIGBUS, current);
}

/*
                                                   
                                                     
                                     
 */
static void cache_error(struct pt_regs *regs)
{
	die("Cache Error", regs, 0);
}

/*
                            
 */
void do_genex(struct pt_regs *regs)
{
	/*
                             
  */
	switch (pt_cause(regs)) {
	case HVM_GE_C_XPROT:
	case HVM_GE_C_XUSER:
		execute_protection_fault(regs);
		break;
	case HVM_GE_C_RPROT:
	case HVM_GE_C_RUSER:
		read_protection_fault(regs);
		break;
	case HVM_GE_C_WPROT:
	case HVM_GE_C_WUSER:
		write_protection_fault(regs);
		break;
	case HVM_GE_C_XMAL:
		misaligned_instruction(regs);
		break;
	case HVM_GE_C_WREG:
		illegal_instruction(regs);
		break;
	case HVM_GE_C_PCAL:
		misaligned_instruction(regs);
		break;
	case HVM_GE_C_RMAL:
		misaligned_data_load(regs);
		break;
	case HVM_GE_C_WMAL:
		misaligned_data_store(regs);
		break;
	case HVM_GE_C_INVI:
	case HVM_GE_C_PRIVI:
		illegal_instruction(regs);
		break;
	case HVM_GE_C_BUS:
		precise_bus_error(regs);
		break;
	case HVM_GE_C_CACHE:
		cache_error(regs);
		break;
	default:
		/*                     */
		panic("Unrecognized exception 0x%lx\n", pt_cause(regs));
		break;
	}
}

/*                               */
long sys_syscall(void)
{
	printk(KERN_ERR "sys_syscall invoked!\n");
	return -ENOSYS;
}

void do_trap0(struct pt_regs *regs)
{
	syscall_fn syscall;

	switch (pt_cause(regs)) {
	case TRAP_SYSCALL:
		/*                         */

		/*                                     */
		if (unlikely(test_thread_flag(TIF_SYSCALL_TRACE) &&
			tracehook_report_syscall_entry(regs)))
			return;  /*                             */

		/*                                                        */
		__vmsetie(VM_INT_ENABLE);

		/*
                                                      
                                                             
                                                           
                                                            
                                                        
                                                          
                                                      
                                    
   */
		regs->syscall_nr = regs->r06;

		/*
                                                         
                                                     
                                                          
                        
   */
		regs->restart_r0 = regs->r00;

		if ((unsigned long) regs->syscall_nr >= __NR_syscalls) {
			regs->r00 = -1;
		} else {
			syscall = (syscall_fn)
				  (sys_call_table[regs->syscall_nr]);
			regs->r00 = syscall(regs->r00, regs->r01,
				   regs->r02, regs->r03,
				   regs->r04, regs->r05);
		}

		/*                                               */
		if (unlikely(test_thread_flag(TIF_SYSCALL_TRACE)))
			tracehook_report_syscall_exit(regs, 0);

		break;
	case TRAP_DEBUG:
		/*                                */
		if (user_mode(regs)) {
			struct siginfo info;

			info.si_signo = SIGTRAP;
			info.si_errno = 0;
			/*
                                                 
                                                 
                                               
                                                
                                               
    */
			info.si_code = TRAP_BRKPT;
			info.si_addr = (void __user *) pt_elr(regs);
			send_sig_info(SIGTRAP, &info, current);
		} else {
#ifdef CONFIG_KGDB
			kgdb_handle_exception(pt_cause(regs), SIGTRAP,
					      TRAP_BRKPT, regs);
#endif
		}
		break;
	}
	/*                                                              */
}

/*
                                  
 */
void do_machcheck(struct pt_regs *regs)
{
	/*                     */
	__vmstop();
}

/*
                                     
 */

void do_debug_exception(struct pt_regs *regs)
{
	regs->hvmer.vmest &= ~HVM_VMEST_CAUSE_MSK;
	regs->hvmer.vmest |= (TRAP_DEBUG << HVM_VMEST_CAUSE_SFT);
	do_trap0(regs);
}