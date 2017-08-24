/*
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 2000, 2001, 2002 Andi Kleen, SuSE Labs
 *  Copyright (C) 2011	Don Zickus Red Hat, Inc.
 *
 *  Pentium III FXSR, SSE support
 *	Gareth Hughes <gareth@valinux.com>, May 2000
 */

/*
                                    
 */
#include <linux/spinlock.h>
#include <linux/kprobes.h>
#include <linux/kdebug.h>
#include <linux/nmi.h>
#include <linux/delay.h>
#include <linux/hardirq.h>
#include <linux/slab.h>
#include <linux/export.h>

#if defined(CONFIG_EDAC)
#include <linux/edac.h>
#endif

#include <linux/atomic.h>
#include <asm/traps.h>
#include <asm/mach_traps.h>
#include <asm/nmi.h>
#include <asm/x86_init.h>

struct nmi_desc {
	spinlock_t lock;
	struct list_head head;
};

static struct nmi_desc nmi_desc[NMI_MAX] = 
{
	{
		.lock = __SPIN_LOCK_UNLOCKED(&nmi_desc[0].lock),
		.head = LIST_HEAD_INIT(nmi_desc[0].head),
	},
	{
		.lock = __SPIN_LOCK_UNLOCKED(&nmi_desc[1].lock),
		.head = LIST_HEAD_INIT(nmi_desc[1].head),
	},
	{
		.lock = __SPIN_LOCK_UNLOCKED(&nmi_desc[2].lock),
		.head = LIST_HEAD_INIT(nmi_desc[2].head),
	},
	{
		.lock = __SPIN_LOCK_UNLOCKED(&nmi_desc[3].lock),
		.head = LIST_HEAD_INIT(nmi_desc[3].head),
	},

};

struct nmi_stats {
	unsigned int normal;
	unsigned int unknown;
	unsigned int external;
	unsigned int swallow;
};

static DEFINE_PER_CPU(struct nmi_stats, nmi_stats);

static int ignore_nmis;

int unknown_nmi_panic;
/*
                                                                    
                               
 */
static DEFINE_RAW_SPINLOCK(nmi_reason_lock);

static int __init setup_unknown_nmi_panic(char *str)
{
	unknown_nmi_panic = 1;
	return 1;
}
__setup("unknown_nmi_panic", setup_unknown_nmi_panic);

#define nmi_to_desc(type) (&nmi_desc[type])

static int __kprobes nmi_handle(unsigned int type, struct pt_regs *regs, bool b2b)
{
	struct nmi_desc *desc = nmi_to_desc(type);
	struct nmiaction *a;
	int handled=0;

	rcu_read_lock();

	/*
                                                           
                                                            
                                                          
                               
  */
	list_for_each_entry_rcu(a, &desc->head, list)
		handled += a->handler(type, regs);

	rcu_read_unlock();

	/*                                           */
	return handled;
}

int __register_nmi_handler(unsigned int type, struct nmiaction *action)
{
	struct nmi_desc *desc = nmi_to_desc(type);
	unsigned long flags;

	if (!action->handler)
		return -EINVAL;

	spin_lock_irqsave(&desc->lock, flags);

	/*
                                                          
                                                            
                          
  */
	WARN_ON_ONCE(type == NMI_UNKNOWN && !list_empty(&desc->head));
	WARN_ON_ONCE(type == NMI_SERR && !list_empty(&desc->head));
	WARN_ON_ONCE(type == NMI_IO_CHECK && !list_empty(&desc->head));

	/*
                                                            
                                                       
  */
	if (action->flags & NMI_FLAG_FIRST)
		list_add_rcu(&action->list, &desc->head);
	else
		list_add_tail_rcu(&action->list, &desc->head);
	
	spin_unlock_irqrestore(&desc->lock, flags);
	return 0;
}
EXPORT_SYMBOL(__register_nmi_handler);

void unregister_nmi_handler(unsigned int type, const char *name)
{
	struct nmi_desc *desc = nmi_to_desc(type);
	struct nmiaction *n;
	unsigned long flags;

	spin_lock_irqsave(&desc->lock, flags);

	list_for_each_entry_rcu(n, &desc->head, list) {
		/*
                                                   
                              
   */
		if (!strcmp(n->name, name)) {
			WARN(in_nmi(),
				"Trying to free NMI (%s) from NMI context!\n", n->name);
			list_del_rcu(&n->list);
			break;
		}
	}

	spin_unlock_irqrestore(&desc->lock, flags);
	synchronize_rcu();
}
EXPORT_SYMBOL_GPL(unregister_nmi_handler);

static __kprobes void
pci_serr_error(unsigned char reason, struct pt_regs *regs)
{
	/*                                                                 */
	if (nmi_handle(NMI_SERR, regs, false))
		return;

	pr_emerg("NMI: PCI system error (SERR) for reason %02x on CPU %d.\n",
		 reason, smp_processor_id());

	/*
                                                            
                                 
  */
#if defined(CONFIG_EDAC)
	if (edac_handler_set()) {
		edac_atomic_assert_error();
		return;
	}
#endif

	if (panic_on_unrecovered_nmi)
		panic("NMI: Not continuing");

	pr_emerg("Dazed and confused, but trying to continue\n");

	/*                                            */
	reason = (reason & NMI_REASON_CLEAR_MASK) | NMI_REASON_CLEAR_SERR;
	outb(reason, NMI_REASON_PORT);
}

static __kprobes void
io_check_error(unsigned char reason, struct pt_regs *regs)
{
	unsigned long i;

	/*                                                                 */
	if (nmi_handle(NMI_IO_CHECK, regs, false))
		return;

	pr_emerg(
	"NMI: IOCK error (debug interrupt?) for reason %02x on CPU %d.\n",
		 reason, smp_processor_id());
	show_regs(regs);

	if (panic_on_io_nmi)
		panic("NMI IOCK error: Not continuing");

	/*                                                 */
	reason = (reason & NMI_REASON_CLEAR_MASK) | NMI_REASON_CLEAR_IOCHK;
	outb(reason, NMI_REASON_PORT);

	i = 20000;
	while (--i) {
		touch_nmi_watchdog();
		udelay(100);
	}

	reason &= ~NMI_REASON_CLEAR_IOCHK;
	outb(reason, NMI_REASON_PORT);
}

static __kprobes void
unknown_nmi_error(unsigned char reason, struct pt_regs *regs)
{
	int handled;

	/*
                                                                 
                                                                   
                                                                       
                         
  */
	handled = nmi_handle(NMI_UNKNOWN, regs, false);
	if (handled) {
		__this_cpu_add(nmi_stats.unknown, handled);
		return;
	}

	__this_cpu_add(nmi_stats.unknown, 1);

	pr_emerg("Uhhuh. NMI received for unknown reason %02x on CPU %d.\n",
		 reason, smp_processor_id());

	pr_emerg("Do you have a strange power saving mode enabled?\n");
	if (unknown_nmi_panic || panic_on_unrecovered_nmi)
		panic("NMI: Not continuing");

	pr_emerg("Dazed and confused, but trying to continue\n");
}

static DEFINE_PER_CPU(bool, swallow_nmi);
static DEFINE_PER_CPU(unsigned long, last_nmi_rip);

static __kprobes void default_do_nmi(struct pt_regs *regs)
{
	unsigned char reason = 0;
	int handled;
	bool b2b = false;

	/*
                                                              
                                                           
                                                    
  */

	/*
                                                             
                                                                   
                                                                 
                                                                 
                                                               
  */
	if (regs->ip == __this_cpu_read(last_nmi_rip))
		b2b = true;
	else
		__this_cpu_write(swallow_nmi, false);

	__this_cpu_write(last_nmi_rip, regs->ip);

	handled = nmi_handle(NMI_LOCAL, regs, b2b);
	__this_cpu_add(nmi_stats.normal, handled);
	if (handled) {
		/*
                                                        
                                                        
                                                         
                                                            
                                                        
             
   */
		if (handled > 1)
			__this_cpu_write(swallow_nmi, true);
		return;
	}

	/*                                                               */
	raw_spin_lock(&nmi_reason_lock);
	reason = x86_platform.get_nmi_reason();

	if (reason & NMI_REASON_MASK) {
		if (reason & NMI_REASON_SERR)
			pci_serr_error(reason, regs);
		else if (reason & NMI_REASON_IOCHK)
			io_check_error(reason, regs);
#ifdef CONFIG_X86_32
		/*
                                          
                                      
   */
		reassert_nmi();
#endif
		__this_cpu_add(nmi_stats.external, 1);
		raw_spin_unlock(&nmi_reason_lock);
		return;
	}
	raw_spin_unlock(&nmi_reason_lock);

	/*
                                                     
                                                        
                                                         
                                                          
                                                           
                                                         
   
                                                         
                                                         
                                                          
                                                         
   
                                                         
                                                         
              
   
                                                         
                                                        
                                                     
                                                         
                                                    
                                                         
                                                            
                                                          
                                                       
                                                         
                                                       
            
  */
	if (b2b && __this_cpu_read(swallow_nmi))
		__this_cpu_add(nmi_stats.swallow, 1);
	else
		unknown_nmi_error(reason, regs);
}

/*
                                                           
                                                             
 */
#ifdef CONFIG_X86_32
/*
                                                              
                                                               
                                                             
             
  
                  
                
              
  
                                                                
                                                            
                                                               
                                                               
                                                          
                                                             
                                                                 
  
                                                             
                                                           
                                                                 
                                                                    
                                                                  
                                                              
                                                                
                                                             
                                                                 
  
                                                                       
                                                                         
                                                                   
                 
  
                                                                  
                                                                      
                                                                     
                                                                      
                                                                        
                                                                   
                                                                      
 */
enum nmi_states {
	NMI_NOT_RUNNING = 0,
	NMI_EXECUTING,
	NMI_LATCHED,
};
static DEFINE_PER_CPU(enum nmi_states, nmi_state);
static DEFINE_PER_CPU(unsigned long, nmi_cr2);

#define nmi_nesting_preprocess(regs)					\
	do {								\
		if (this_cpu_read(nmi_state) != NMI_NOT_RUNNING) {	\
			this_cpu_write(nmi_state, NMI_LATCHED);		\
			return;						\
		}							\
		this_cpu_write(nmi_state, NMI_EXECUTING);		\
		this_cpu_write(nmi_cr2, read_cr2());			\
	} while (0);							\
	nmi_restart:

#define nmi_nesting_postprocess()					\
	do {								\
		if (unlikely(this_cpu_read(nmi_cr2) != read_cr2()))	\
			write_cr2(this_cpu_read(nmi_cr2));		\
		if (this_cpu_dec_return(nmi_state))			\
			goto nmi_restart;				\
	} while (0)
#else /*        */
/*
                                                                       
                                                                    
                                                                    
                                                                    
                                                                        
                                                                      
                                                                      
                                                                         
                        
  
                                                                         
                                                                   
                                                                   
                                                                        
                                                                     
                                                                        
                                                                         
                       
 */
static DEFINE_PER_CPU(int, update_debug_stack);

static inline void nmi_nesting_preprocess(struct pt_regs *regs)
{
	/*
                                                       
                                                         
                                                         
                                  
  */
	if (unlikely(is_debug_stack(regs->sp))) {
		debug_stack_set_zero();
		this_cpu_write(update_debug_stack, 1);
	}
}

static inline void nmi_nesting_postprocess(void)
{
	if (unlikely(this_cpu_read(update_debug_stack))) {
		debug_stack_reset();
		this_cpu_write(update_debug_stack, 0);
	}
}
#endif

dotraplinkage notrace __kprobes void
do_nmi(struct pt_regs *regs, long error_code)
{
	nmi_nesting_preprocess(regs);

	nmi_enter();

	inc_irq_stat(__nmi_count);

	if (!ignore_nmis)
		default_do_nmi(regs);

	nmi_exit();

	/*                                      */
	nmi_nesting_postprocess();
}

void stop_nmi(void)
{
	ignore_nmis++;
}

void restart_nmi(void)
{
	ignore_nmis--;
}

/*                                  */
void local_touch_nmi(void)
{
	__this_cpu_write(last_nmi_rip, 0);
}
EXPORT_SYMBOL_GPL(local_touch_nmi);
